#include "include/Application.h"
#include "include/Request.h"
#include "include/TaskPerformer.h"

// system libs
#include <curl/curl.h>
#include <locale>

#include <iostream>
#include <chrono>
#include <conio.h>
#include <thread>

namespace
{
	constexpr std::chrono::milliseconds kSleepInterval = std::chrono::milliseconds{1000};
}

Application::Application()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::setlocale(LC_ALL, "en_US.UTF-8");
}

Application::~Application()
{
	Logout();
	curl_global_cleanup();
}

bool Application::Initialize()
{
	if (!m_Configs.Read("config.json"))
	{
		std::cerr << "ERROR: Can't read settings, please sownload config.json from admin panel" << std::endl;
		return false;
	}

	bool ret = true;
	const std::vector<std::string> requiredConfigs = { "api", "username", "password", "checkername" };
	for (const std::string& key : requiredConfigs)
	{
		if (!m_Configs.Has(key))
		{
			std::cerr << "ERROR: missing required key in config.json \"" << key << "\"" << std::endl;
			ret = false;
		}
	}

	return ret;
}

bool Application::Register()
{
	Request request{ m_Configs["api"] + "/register", RequestType::Post};
	request.Post()["username"] = m_Configs["username"];
	request.Post()["password"] = m_Configs["password"];
	request.Post()["checkername"] = m_Configs["checkername"];

	const Response response = request.Perform();
	if (!response)
	{
		std::cerr << "ERROR: Register request failed with the following error: " << response.m_Error << std::endl;
		return false;
	}

	const Json::Value json = response.ToJson();
	if (json["checkertoken"].isNull())
	{
		std::cerr << "ERROR: Registration failed, unexpected return value" << std::endl;
		return false;
	}

	m_Configs.Set("checkertoken", json["checkertoken"].asString());
	m_bRegistered = true;
	return true;
}

void Application::Run()
{
	assert(m_bRegistered && "Running messages without registering");

	int dotsCount = 0;
	auto fnPrintDots = [&dotsCount]()
	{
		// do nothing, only output
		constexpr int kNumDots = 4;
		dotsCount = (dotsCount + 1) % kNumDots;
		for (int i = 0; i < kNumDots; i++)
			std::cout << (i < dotsCount ? "." : " ");
	};

	do
	{
		Request request{ m_Configs["api"] + "/message", RequestType::Post };
		request.Post()["checkername"] = m_Configs["checkername"];
		request.Post()["checkertoken"] = m_Configs["checkertoken"];
		request.Post()["limit"] = "1"; //TODO: change to count of cores?
		if (const Response response = request.Perform())
		{
			const Json::Value json = response.ToJson();
			const std::string message = json.get("message", "idle").asString();
			if (message == "idle")
			{
				// do nothing, only output
				std::cout << "\rIdle";
				fnPrintDots();
			}
			else if (message == "authentification")
			{
				// do nothing, only output
				std::cout << "\rAuthentification";
				fnPrintDots();
			}
			else if (message == "logout")
			{
				std::cout << std::endl << "Logout message has bees received" << std::endl;
				return;
			}
			else if (message == "task")
			{
				std::cout << std::endl << "Task message has bees received" << std::endl;
				PerformTasks(json.get("tasks", {}));
			}
			else
			{
				std::cerr << "WARNING: unknown message type: " << message << std::endl;
			}
		}
		else
		{
			std::cerr << "WARNING: Message request failed" << std::endl;
		}

		// sleep for 1s
		std::this_thread::sleep_for(kSleepInterval);
	} while (!_kbhit() || _getch() != 27);
}

bool Application::Logout()
{
	// do not logout if we are not registered
	if (!m_bRegistered)
		return true;

	Request request{ m_Configs["api"] + "/logout", RequestType::Post };
	request.Post()["checkername"] = m_Configs["checkername"];
	request.Post()["checkertoken"] = m_Configs["checkertoken"];

	const Response response = request.Perform();
	if (!response)
	{
		std::cerr << "ERROR: Logout request failed with the following error: " << response.m_Error << std::endl;
		return false;
	}

	m_bRegistered = false;
	return true;
}

void Application::PerformTasks(const Json::Value& tasks)
{
	//TODO: get settings, code and run the task
}
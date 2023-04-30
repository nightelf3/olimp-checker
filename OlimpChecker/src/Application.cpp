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

	const std::vector<std::string> requiredCompilers = {
		"Compilers/MinGW32/bin/gcc.exe",
		"Compilers/MinGW32/bin/g++.exe",
		"Compilers/FPC/bin/i386-win32/fpc.exe",
		"Compilers/Python36/python.exe",
		"Compilers/Java/bin/javac.exe",
		"Compilers/Mono/bin/csc.bat"
	};
	for (const std::string& compiler : requiredCompilers)
	{
		if (!std::filesystem::exists(std::filesystem::absolute({ compiler })))
		{
			std::cerr << "ERROR: missing required compiler: \"" << compiler << "\"" << std::endl;
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
	bool dotsPrinted = false;
	auto fnPrintDots = [&dotsCount, &dotsPrinted]()
	{
		// do nothing, only output
		constexpr int kNumDots = 4;
		dotsCount = (dotsCount + 1) % kNumDots;
		for (int i = 0; i < kNumDots; i++)
			std::cout << (i < dotsCount ? "." : " ");
		dotsPrinted = true;
	};

	do
	{
		//TODO: use 'limit' to get more tasks, default: 1
		if (const Response response = MakeRequestWithCredentials("/message").Perform())
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
			else
			{
				if (dotsPrinted)
				{
					dotsPrinted = false;
					std::cout << std::endl;
				}

				if (message == "logout")
				{
					std::cout << "Logout message has been received" << std::endl;
					return;
				}
				else if (message == "task")
				{
					std::cout << "Task message has been received" << std::endl;
					PerformTasks(json.get("tasks", {}));
				}
				else
				{
					std::cerr << "WARNING: unknown message type: " << message << std::endl;
				}
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

	const Response response = MakeRequestWithCredentials("/logout").Perform();
	if (!response)
	{
		std::cerr << "ERROR: Logout request failed with the following error: " << response.m_Error << std::endl;
		return false;
	}

	m_bRegistered = false;
	return true;
}

Request Application::MakeRequestWithCredentials(std::string path) const
{
	Request request{ m_Configs["api"] + path, RequestType::Post };
	request.Post()["checkername"] = m_Configs["checkername"];
	request.Post()["checkertoken"] = m_Configs["checkertoken"];
	return request;
}

void Application::PerformTasks(const Json::Value& tasks)
{
	//TODO: process tasks on different threads
	const Json::ArrayIndex count = tasks.isArray() ? tasks.size() : 0;
	assert(count > 0 && "Tasks value should not be empty");
	for (Json::ArrayIndex i = 0; i < count; i++)
	{
		TaskPerformer task(tasks.get(i, {}));
		task.Run(MakeRequestWithCredentials("/results"));
	}
}
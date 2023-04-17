#include "include/Application.h"

#include <curl/curl.h>
#include <locale>

#include <iostream>
#include "include/Request.h"

Application::Application()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::setlocale(LC_ALL, "en_US.UTF-8");
}

Application::~Application()
{
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
	return true;
}
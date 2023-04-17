#include <curl/curl.h>
#include <locale>

#include <iostream>
#include "include/Request.h"
#include "include/Config.h"

bool CheckConfigs(const Config& config)
{
	bool ret = true;
	const std::vector<std::string> requiredConfigs = { "api", "username", "password", "checkername" };
	for (const std::string& key : requiredConfigs)
	{
		if (!config.Has(key))
		{
			std::cerr << "ERROR: missing required key in config.json \"" << key << "\"" << std::endl;
			ret = false;
		}
	}
	return ret;
}

int main(int argc, char** argv)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::setlocale(LC_ALL, "en_US.UTF-8");

	Config config;
	if (!config.Read("config.json"))
	{
		std::cerr << "ERROR: Can't read settings, please sownload config.json from admin panel" << std::endl;
		return 1;
	}

	if (!CheckConfigs(config))
		return -1;

	if (const Response response = Request{ config["api"], RequestType::Post}.Perform())
		std::cout << response.m_Data << std::endl;
	else
		std::cout << "curl_easy_perform() failed: " << response.m_Error << std::endl;

	curl_global_cleanup();
	return 0;
}
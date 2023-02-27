#include "include/Application.h"

#include <curl/curl.h>
#include <locale>

Application::Application()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::setlocale(LC_ALL, "en_US.UTF-8");
}

Application::~Application()
{
	curl_global_cleanup();
}
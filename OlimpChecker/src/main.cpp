#include <curl/curl.h>
#include <locale>

#include <iostream>
#include "include/Request.h"

int main(int argc, char** argv)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::setlocale(LC_ALL, "en_US.UTF-8");

	if (const Response response = Request{ "https://en.wikipedia.org/wiki/HTTPS" }.Perform())
		std::cout << response.m_Data << std::endl;
	else
		std::cout << "curl_easy_perform() failed: " << response.m_Error << std::endl;

	curl_global_cleanup();
	return 0;
}
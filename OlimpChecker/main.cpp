#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include "utils.h"

namespace
{
	size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
	{
		if (std::string* result = static_cast<std::string*>(userp))
		{
			result->append(static_cast<char*>(contents), size * nmemb);
			return size * nmemb;
		}
		return 0;
	}
}

int main(int argc, char** argv)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	std::setlocale(LC_ALL, "en_US.UTF-8");

	if (upCURL curl = make_upCURL())
	{
		curl_easy_setopt(curl.get(), CURLOPT_URL, "https://www.google.com.ua/");
		curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L);

		std::string buffer;
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &buffer);

		if (const CURLcode code = curl_easy_perform(curl.get()); CURLE_OK == code)
			std::cout << buffer << std::endl;
		else
			std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(code) << std::endl;
	}

	curl_global_cleanup();
	return 0;
}
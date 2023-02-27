#pragma once

#include <curl/curl.h>
#include <memory>

struct CUrlDeleter
{
	void operator()(CURL* curl)
	{
		if (!curl)
			return;
		curl_easy_cleanup(curl);
	}
};

using upCURL = std::unique_ptr<CURL, CUrlDeleter>;
inline upCURL make_upCURL()
{
	return upCURL{ curl_easy_init() };
}
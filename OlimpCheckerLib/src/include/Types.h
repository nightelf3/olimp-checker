#pragma once

#include <curl/curl.h>
#include <memory>

struct CUrlDeleter
{
	void operator()(CURL* curl);
};

using upCURL = std::unique_ptr<CURL, CUrlDeleter>;
upCURL make_upCURL();

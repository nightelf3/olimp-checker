#pragma once

#include "DllExport.h"

#include <curl/curl.h>
#include <memory>

struct DLL_EXPORT CUrlDeleter
{
	void operator()(CURL* curl);
};

using upCURL = std::unique_ptr<CURL, CUrlDeleter>;
DLL_EXPORT upCURL make_upCURL();

#pragma once

#include "DllExport.h"

#include <curl/curl.h>
#include <string>

struct DLL_EXPORT Response
{
	CURLcode m_Code = CURLE_OK;
	std::string m_Data;

	operator bool() const { return CURLE_OK == m_Code; }
	std::string GetError() const;
};
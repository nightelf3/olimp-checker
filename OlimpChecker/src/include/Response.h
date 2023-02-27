#pragma once

#include <curl/curl.h>
#include <string>

struct Response
{
	CURLcode m_Code = CURLE_OK;
	std::string m_Data;

	operator bool() const { return CURLE_OK == m_Code; }
	std::string GetError() const { return curl_easy_strerror(m_Code); }
};
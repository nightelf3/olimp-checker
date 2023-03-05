#include "include/Response.h"

std::string Response::GetError() const
{
	return curl_easy_strerror(m_Code);
}

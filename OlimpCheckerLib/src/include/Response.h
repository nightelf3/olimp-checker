#pragma once

#include <string>

struct Response
{
	int m_Code = -1;
	std::string m_Data;
	std::string m_Error;

	operator bool() const { return 200 == m_Code; }
};
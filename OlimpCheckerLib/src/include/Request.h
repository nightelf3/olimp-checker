#pragma once

#include "Types.h"
#include "RequestData.h"
#include "Response.h"

class Request
{
public:
	Request() = default;
	Request(const Request&) = delete;
	Request(Request&&) = default;
	~Request() = default;

	operator CURL* () { return m_CURL.get(); }

	Response GetContent(const RequestData& data);

private:
	upCURL m_CURL = make_upCURL();
};
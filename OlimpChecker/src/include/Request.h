#pragma once

#include "Types.h"
#include "Response.h"
#include <unordered_map>

enum class RequestType
{
	Get,
	Post
};

struct RequestParams
{
	std::string GetParams() const;

	RequestType m_Type = RequestType::Get;
	std::unordered_map<std::string, std::string> m_Params;
};

struct RequestData
{
	RequestData() = default;
	RequestData(const char url[]);
	std::string GetUrl() const { return m_Server + m_Path; }

	std::string m_Server;
	std::string m_Path;
	RequestParams m_Params;
};

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
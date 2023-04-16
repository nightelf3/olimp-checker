#pragma once

#include <string_view>
#include <unordered_map>

enum class RequestType
{
	Get,
	Post
};

using RequestParams = std::unordered_map<std::string, std::string>;

class RequestData
{
public:
	RequestData(const char* url, RequestType type = RequestType::Get);

	const char* GetUrl() const { return m_Url; }
	const std::string_view& GetServer() const { return m_Server; }
	const std::string_view& GetPath() const { return m_Path; }
	const RequestParams& GetParams() const { return m_Params; }

	std::string ParamsToString() const;

private:
	const char* m_Url = "";
	std::string_view m_Server;
	std::string_view m_Path;
	RequestParams m_Params;

	RequestType m_Type = RequestType::Get;
};

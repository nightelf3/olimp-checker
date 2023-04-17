#pragma once

#include "Interfaces/IRequestImpl.h"
#include "Response.h"
#include <cassert>
#include <memory>
#include <string_view>
#include <unordered_map>

enum class RequestType
{
	Get,
	Post
};

using RequestParams = std::unordered_map<std::string, std::string>;

class Request
{
public:
	explicit Request(const char* url, RequestType type = RequestType::Get, std::unique_ptr<IRequestImpl> pImpl = nullptr);
	Request(const std::string& url, RequestType type = RequestType::Get, std::unique_ptr<IRequestImpl> pImpl = nullptr);

	std::string Url() const;
	const std::string& Server() const { return m_Server; }
	const std::string& Path() const { return m_Path; }

	RequestType Type() const { return m_Type; }
	RequestType SetType(RequestType eType) { m_Type = eType; }
	const RequestParams& Get() const { return m_GetParams; }
	RequestParams& Get() { return m_GetParams; }
	const RequestParams& Post() const { assert(m_Type == RequestType::Post); return m_PostParams; }
	RequestParams& Post() { assert(m_Type == RequestType::Post); return m_PostParams; }

	Response Perform() const;

private:
	void Initialize(const std::string& url, RequestType type, std::unique_ptr<IRequestImpl> pImpl = {});
	std::string ParamsToString(const RequestParams& params) const;

private:
	std::unique_ptr<IRequestImpl> m_RequestImpl;
	RequestType m_Type = RequestType::Get;
	std::string m_Server;
	std::string m_Path;
	RequestParams m_GetParams;
	RequestParams m_PostParams;
};

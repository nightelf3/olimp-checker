#include "include/Request.h"
#include <regex>

namespace
{
	size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
	{
		if (Response* response = static_cast<Response*>(userp))
		{
			//TODO: log an error when size != 1
			response->m_Data.append(static_cast<char*>(contents), size * nmemb);
			return size * nmemb;
		}
		return 0;
	}
}

std::string RequestParams::GetParams() const
{
	auto escape = [](const std::string& str)
	{
		// escape & in params
		return std::regex_replace(str, std::regex("&"), "%26");
	};

	auto estimateReserve = [this]()
	{
		constexpr size_t round = 2; // \0 and &
		size_t reserve = 0;
		for (auto& [name, value] : m_Params)
			reserve += name.length() + value.length() + round;
		return reserve;
	};

	std::string result;
	result.reserve(estimateReserve());

	bool separator = false;
	for (auto& [name, value] : m_Params)
	{
		if (separator)
			result.append("&");
		result.append(escape(name) + "=" + escape(value));
		separator = true;
	}

	return result;
}

RequestData::RequestData(const char url[])
{
	m_Server = url;
	//TODO: implement parser
}

Response Request::GetContent(const RequestData& data)
{
	curl_easy_setopt(*this, CURLOPT_URL, data.GetUrl().c_str());
	curl_easy_setopt(*this, CURLOPT_SSL_VERIFYPEER, 0L);

	Response response;
	curl_easy_setopt(*this, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(*this, CURLOPT_WRITEDATA, &response);
	response.m_Code = curl_easy_perform(*this);

	//TODO: log when fail
	return response;
}

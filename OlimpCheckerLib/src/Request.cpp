#include "include/Request.h"
#include "include/Types.h"

#include <regex>
#include <string>

namespace
{
	struct RequestImpl : public IRequestImpl
	{
		void SetPostParams(std::string params) override
		{
			m_PostParams = std::move(params);
			curl_easy_setopt(m_Curl.get(), CURLOPT_POSTFIELDS, m_PostParams.c_str());
		}

		Response Perform(const std::string& url) override
		{
			curl_easy_setopt(m_Curl.get(), CURLOPT_URL, url.c_str());
			if (!m_PostParams.empty())
				curl_easy_setopt(m_Curl.get(), CURLOPT_POSTFIELDS, m_PostParams.c_str());
			curl_easy_setopt(m_Curl.get(), CURLOPT_SSL_VERIFYPEER, 0L);

			Response response;
			curl_easy_setopt(m_Curl.get(), CURLOPT_WRITEFUNCTION, RequestImpl::WriteCallback);
			curl_easy_setopt(m_Curl.get(), CURLOPT_WRITEDATA, &response);
			if (const CURLcode code = curl_easy_perform(m_Curl.get()); code != CURLE_OK)
			{
				response.m_Error = curl_easy_strerror(code);
				response.m_Code = -1;
			}
			else
			{
				curl_easy_getinfo(m_Curl.get(), CURLINFO_RESPONSE_CODE, &response.m_Code);
			}

			return response;
		}

	private:
		static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
		{
			if (Response* response = static_cast<Response*>(userp))
			{
				assert(size == 1 && "function can work with 1b chars only");
				response->m_Data.append(static_cast<char*>(contents), size * nmemb);
				return size * nmemb;
			}
			return 0;
		}

		upCURL m_Curl = make_upCURL();
		std::string m_PostParams;
	};
}

Request::Request(const char* url, RequestType type, std::unique_ptr<IRequestImpl> pImpl)
{
	Initialize({url}, type, std::move(pImpl));
}

Request::Request(const std::string& url, RequestType type, std::unique_ptr<IRequestImpl> pImpl)
{
	Initialize(url, type, std::move(pImpl));
}

std::string Request::Url() const
{
	std::string url = m_Server + m_Path;
	if (!m_GetParams.empty())
		url += std::string{"?"} + ParamsToString(m_GetParams);
	return url;
}

Response Request::Perform() const
{
	if (RequestType::Post == m_Type)
		m_RequestImpl->SetPostParams(ParamsToString(m_PostParams));
	return m_RequestImpl->Perform(Url());
}

void Request::Initialize(const std::string& url, RequestType type, std::unique_ptr<IRequestImpl> pImpl)
{
	if (pImpl)
		m_RequestImpl = std::move(pImpl);
	else
		m_RequestImpl = std::make_unique<RequestImpl>();

	m_Type = type;

	const std::regex rgx("(((https|http):\\/\\/)?([^\\/]+))(\\/[^\\?\\s]*)?\\??");
	std::smatch matches;
	if (std::regex_search(url, matches, rgx))
	{
		m_Server = matches[1].str();
		m_Path = matches[5].str();

		const std::regex rgx("(&)?([^=]*)=?([^&]*)");
		std::smatch::value_type suffix = matches.suffix();
		while (suffix.matched)
		{
			std::smatch matches;
			if (!std::regex_search(suffix.first, suffix.second, matches, rgx))
				break;
			m_GetParams[matches[2].str()] = matches[3].str();
			suffix = matches.suffix();
		}
	}
}

std::string Request::ParamsToString(const RequestParams& params) const
{
	auto escape = [](std::string str)
	{
		// escape & and = in params
		str = std::regex_replace(str, std::regex("&"), "%26");
		return std::regex_replace(str, std::regex("="), "%3D");
	};

	auto estimateReserve = [this, &params]()
	{
		constexpr size_t round = 2; // \0 and &
		size_t reserve = 0;
		for (auto& [name, value] : params)
			reserve += name.length() + value.length() + round;
		return reserve;
	};

	std::string result;
	result.reserve(estimateReserve());

	bool separator = false;
	for (auto& [name, value] : params)
	{
		if (separator)
			result.append("&");
		result.append(escape(name) + "=" + escape(value));
		separator = true;
	}

	return result;
}

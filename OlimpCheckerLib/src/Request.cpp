#include "include/Request.h"

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

Response Request::GetContent(const RequestData& data)
{
	curl_easy_setopt(*this, CURLOPT_URL, data.GetUrl());
	curl_easy_setopt(*this, CURLOPT_SSL_VERIFYPEER, 0L);

	Response response;
	curl_easy_setopt(*this, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(*this, CURLOPT_WRITEDATA, &response);
	response.m_Code = curl_easy_perform(*this);

	//TODO: log when fail
	return response;
}

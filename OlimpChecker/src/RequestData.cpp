#include "include/RequestData.h"

#include <regex>
#include <string>

RequestData::RequestData(const char* url, RequestType type) :
	m_Url(url),
	m_Type(type)
{
	const std::regex rgx("(((https|http):\\/\\/)?([^\\/]+))(\\/[^\\?\\s]*)?\\??");
	std::cmatch matches;
	if (std::regex_search(m_Url, matches, rgx))
	{
		m_Server = std::string_view(matches[1].first, matches[1].length());
		m_Path = std::string_view(matches[5].first, matches[5].length());

		const std::regex rgx("(&)?([^=]*)=?([^&]*)");
		std::cmatch::value_type suffix = matches.suffix();
		while (suffix.matched)
		{
			std::cmatch matches;
			if (!std::regex_search(suffix.first, suffix.second, matches, rgx))
				break;
			m_Params[matches[2].str()] = matches[3].str();
			suffix = matches.suffix();
		}
	}
}

std::string RequestData::ParamsToString() const
{
	auto escape = [](std::string str)
	{
		// escape & and = in params
		str = std::regex_replace(str, std::regex("&"), "%26");
		return std::regex_replace(str, std::regex("="), "%3D");
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

void ParseParams(std::csub_match& params)
{
}

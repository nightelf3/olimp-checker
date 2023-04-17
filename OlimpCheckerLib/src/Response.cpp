#include "include/Response.h"
#include <cassert>
#include <iostream>
#include "json/json.h"

Json::Value Response::ToJson() const
{
	assert(operator bool() && "Getting Json from invalid response");

	std::string errs;
	Json::CharReaderBuilder rbuilder;
	std::stringstream dataStream{m_Data};
	if (Json::Value value; Json::parseFromStream(rbuilder, dataStream, &value, &errs))
		return value;

	std::cerr << "WARNING: can't nake json from response with the following error: " << errs << std::endl;
	return {};
}
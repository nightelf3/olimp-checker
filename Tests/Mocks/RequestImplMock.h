#pragma once

#include "gmock/gmock.h"
#include "include/Interfaces/IRequestImpl.h"

struct RequestImplMock : public IRequestImpl
{
	MOCK_METHOD(void, SetPostParams, (std::string params), (override));
	MOCK_METHOD(Response, Perform, (const std::string& url), (override));
};
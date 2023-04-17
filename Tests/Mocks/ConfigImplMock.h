#pragma once

#include "gmock/gmock.h"
#include "include/Interfaces/IConfigImpl.h"

struct ConfigImplMock : public IConfigImpl
{
	MOCK_METHOD(bool, Read, (const std::string& path), (override));
	MOCK_METHOD(std::optional<std::string>, Get, (const char* key), (const, override));
};
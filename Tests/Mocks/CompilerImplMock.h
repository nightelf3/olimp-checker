#pragma once

#include "gmock/gmock.h"
#include "include/Interfaces/ICompilerImpl.h"

struct CompilerImplMock : public ICompilerImpl
{
	MOCK_METHOD(bool, CreatePath, (const std::filesystem::path& path), (override));
	MOCK_METHOD(bool, CreateFile, (const std::filesystem::path& path, const std::string& text), (override));
	MOCK_METHOD(bool, Compile, (const std::filesystem::path& srcFile, std::filesystem::path& executablePath, std::string& executableParams, std::string& error), (override));
};
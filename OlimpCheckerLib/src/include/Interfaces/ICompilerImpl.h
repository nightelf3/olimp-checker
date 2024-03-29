#pragma once

#include "IProcessImpl.h"
#include <filesystem>

struct ICompilerImpl
{
	virtual ~ICompilerImpl() = default;
	virtual bool CreatePath(const std::filesystem::path& path) = 0;
	virtual bool CreateFile(const std::filesystem::path& path, const std::string& text) = 0;
	virtual bool Compile(const std::filesystem::path& srcFile, ExecutableData& exeData, std::string& error) = 0;
};

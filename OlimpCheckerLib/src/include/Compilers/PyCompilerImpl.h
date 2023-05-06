#pragma once

#include "include/Compilers/BaseCompilerImpl.h"

class PyCompilerImpl : public BaseCompilerImpl
{
protected:
	std::pair<std::filesystem::path, std::string> MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const override;
	std::pair<std::filesystem::path, std::string> MakeExecutableParams(const std::filesystem::path& srcFile) const override;
};

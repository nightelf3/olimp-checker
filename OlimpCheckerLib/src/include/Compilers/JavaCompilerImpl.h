#pragma once

#include "include/Compilers/BaseCompilerImpl.h"

class JavaCompilerImpl : public BaseCompilerImpl
{
protected:
	ExecutableData MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const override;
	ExecutableData MakeExecutableParams(const std::filesystem::path& srcFile) const override;
	std::string CleanupError(std::string error, const std::filesystem::path& path, bool bProcessReturn) const override;
};

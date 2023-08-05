#pragma once

#include "include/Compilers/BaseCompilerImpl.h"

class CSharpCompilerImpl : public BaseCompilerImpl
{
protected:
	ExecutableData MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const override;
	std::string CleanupError(std::string error, const std::filesystem::path& path, bool bProcessReturn) const override;
};

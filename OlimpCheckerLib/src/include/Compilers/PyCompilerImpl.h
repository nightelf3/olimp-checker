#pragma once

#include "include/Compilers/BaseCompilerImpl.h"

class PyCompilerImpl : public BaseCompilerImpl
{
protected:
	ExecutableData MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const override;
	ExecutableData MakeExecutableParams(const std::filesystem::path& srcFile) const override;
};

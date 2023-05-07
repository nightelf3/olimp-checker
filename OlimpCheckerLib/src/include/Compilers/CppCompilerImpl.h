#pragma once

#include "include/Compilers/BaseCompilerImpl.h"

class CppCompilerImpl : public BaseCompilerImpl
{
protected:
	ExecutableData MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const override;
};

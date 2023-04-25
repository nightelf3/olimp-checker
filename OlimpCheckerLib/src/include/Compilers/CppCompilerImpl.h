#pragma once

#include "include/Compilers/BaseCompilerImpl.h"

class CppCompilerImpl : public BaseCompilerImpl
{
public:
	bool Compile(const std::filesystem::path& srcFile, std::filesystem::path& dstFile, std::string& error) override;
};

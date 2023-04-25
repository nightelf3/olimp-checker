#include "stdafx.h"

#include "include/Compilers/CppCompilerImpl.h"

bool CppCompilerImpl::Compile(const std::filesystem::path& srcFile, std::filesystem::path& dstFile, std::string& error)
{
	error.clear();
	dstFile = srcFile;
	dstFile.replace_extension("exe");
	return true;
}

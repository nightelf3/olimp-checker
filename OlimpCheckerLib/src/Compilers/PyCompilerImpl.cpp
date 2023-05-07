#include "stdafx.h"

#include "include/Compilers/PyCompilerImpl.h"
#include "include/Process.h"

ExecutableData PyCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Python36/python" });
	std::string params = std::format("-m py_compile \"{}\"", srcFile.string());
	return { std::move(path), std::move(params) };
}

ExecutableData PyCompilerImpl::MakeExecutableParams(const std::filesystem::path& srcFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Python36/python" });
	std::string params = std::format("\"{}\"", srcFile.string());
	return { std::move(path), std::move(params), srcFile.parent_path() };
}

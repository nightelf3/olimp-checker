#include "stdafx.h"

#include "include/Compilers/PyCompilerImpl.h"
#include "include/Process.h"

std::pair<std::filesystem::path, std::string> PyCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Python36/python" });
	std::string params = std::format("-m py_compile \"{}\"", srcFile.string());
	return { std::move(path), std::move(params) };
}

std::pair<std::filesystem::path, std::string> PyCompilerImpl::MakeExecutableParams(const std::filesystem::path& srcFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Python36/python" });
	std::string params = std::format("\"{}\"", srcFile.string());
	return { std::move(path), std::move(params) };
}

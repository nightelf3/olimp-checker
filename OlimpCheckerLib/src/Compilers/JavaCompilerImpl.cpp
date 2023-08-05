#include "stdafx.h"

#include "include/Compilers/JavaCompilerImpl.h"
#include "include/Process.h"

ExecutableData JavaCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Java/bin/javac" });
	std::string params = std::format("\"{}\"", srcFile.string());
	return { std::move(path), std::move(params) };
}

ExecutableData JavaCompilerImpl::MakeExecutableParams(const std::filesystem::path& srcFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Java/bin/java" });
	std::string params = "Program";
	return { std::move(path), std::move(params), srcFile.parent_path() };
}

std::string JavaCompilerImpl::CleanupError(std::string error, const std::filesystem::path& path, bool bProcessReturn) const
{
	if (bProcessReturn)
		return {};
	return __super::CleanupError(std::move(error), path, bProcessReturn);
}

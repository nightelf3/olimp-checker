#include "stdafx.h"

#include "include/Compilers/CSharpCompilerImpl.h"
#include "include/Process.h"

ExecutableData CSharpCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/Mono/bin/csc.bat" });
	std::string params = std::format("-o+ \"{}\" -out:\"{}\"", srcFile.string(), dstFile.string());
	return { std::move(path), std::move(params) };
}

std::string CSharpCompilerImpl::CleanupError(std::string error, const std::filesystem::path& path, bool bProcessReturn) const
{
	if (bProcessReturn)
		return {};
	return __super::CleanupError(std::move(error), path, bProcessReturn);
}

#include "stdafx.h"

#include "include/Compilers/PasCompilerImpl.h"
#include "include/Process.h"

ExecutableData PasCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/FPC/bin/i386-win32/fpc" });
	std::string params = std::format("-O3 \"{}\" -FE\"{}\"", srcFile.string(), dstFile.parent_path().string());
	return { std::move(path), std::move(params) };
}

std::string PasCompilerImpl::CleanupError(std::string error, const std::filesystem::path& path, bool bProcessReturn) const
{
	if (error.find("Error:") == std::string::npos && error.find("Fatal:") == std::string::npos)
		return {};
	return __super::CleanupError(std::move(error), path, bProcessReturn);
}

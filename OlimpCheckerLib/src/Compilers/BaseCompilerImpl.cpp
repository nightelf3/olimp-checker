#include "stdafx.h"

#include "include/Compilers/BaseCompilerImpl.h"
#include "include/Process.h"

#include <fstream>

bool BaseCompilerImpl::CreatePath(const std::filesystem::path& path)
{
	if (std::filesystem::exists(path))
		return true;
	return std::filesystem::create_directories(path);
}

bool BaseCompilerImpl::CreateFile(const std::filesystem::path& path, const std::string& text)
{
	std::ofstream out(path);
	out << text;
	out.close();
	return !!out;
}

bool BaseCompilerImpl::Compile(const std::filesystem::path& srcFile, std::filesystem::path& executablePath, std::string& executableParams, std::string& error)
{
	error.clear();
	executablePath.clear();
	executableParams.clear();

	// get the compilation string
	auto [exePath, exeParams] = MakeExecutableParams(srcFile);
	auto [path, params] = MakeCompilationParams(srcFile, exePath);

	// run the compilation
	Process process;
	const bool bProcessReturn = process.Run(std::move(path), std::move(params));
	if (bProcessReturn)
	{
		executablePath = std::move(exePath);
		executableParams = std::move(exeParams);
	}
	error = CleanupError(process.MoveOutput(), srcFile);
	return bProcessReturn;
}

std::pair<std::filesystem::path, std::string> BaseCompilerImpl::MakeExecutableParams(const std::filesystem::path& srcFile) const
{
	std::filesystem::path path = srcFile;
	return { path.replace_extension("exe"), std::string{} };
}

std::string BaseCompilerImpl::CleanupError(std::string error, const std::filesystem::path& path)
{
	const std::string repalce = path.parent_path().string();
	for (size_t startPos = 0; (startPos = error.find(repalce)) != std::string::npos;)
		error.replace(startPos, repalce.size() + 1, "");
	return error;
}

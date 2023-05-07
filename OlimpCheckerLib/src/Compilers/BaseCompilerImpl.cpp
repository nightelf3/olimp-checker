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

bool BaseCompilerImpl::Compile(const std::filesystem::path& srcFile, ExecutableData& exeData, std::string& error)
{
	error.clear();
	exeData = {};

	// get the compilation string
	exeData = MakeExecutableParams(srcFile);

	// run the compilation
	Process process;
	const bool bProcessReturn = process.Run(MakeCompilationParams(srcFile, exeData.path));
	error = CleanupError(process.MoveOutput(), srcFile);
	return bProcessReturn;
}

ExecutableData BaseCompilerImpl::MakeExecutableParams(const std::filesystem::path& srcFile) const
{
	std::filesystem::path path = srcFile;
	return { path.replace_extension("exe"), std::string{}, path.parent_path() };
}

std::string BaseCompilerImpl::CleanupError(std::string error, const std::filesystem::path& path)
{
	const std::string repalce = path.parent_path().string();
	for (size_t startPos = 0; (startPos = error.find(repalce)) != std::string::npos;)
		error.replace(startPos, repalce.size() + 1, "");
	return error;
}

#pragma once

#include "include/Interfaces/ICompilerImpl.h"

class BaseCompilerImpl : public ICompilerImpl
{
public:
	bool CreatePath(const std::filesystem::path& path) override;
	bool CreateFile(const std::filesystem::path& path, const std::string& text) override;
	bool Compile(const std::filesystem::path& srcFile, ExecutableData& exeData, std::string& error) override;

protected:
	virtual ExecutableData MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const = 0;
	virtual ExecutableData MakeExecutableParams(const std::filesystem::path& srcFile) const;

	static std::string CleanupError(std::string error, const std::filesystem::path& path);
};

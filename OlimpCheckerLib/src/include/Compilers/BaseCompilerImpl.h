#pragma once

#include "include/Interfaces/ICompilerImpl.h"

class BaseCompilerImpl : public ICompilerImpl
{
public:
	bool CreatePath(const std::filesystem::path& path) override;
	bool CreateFile(const std::filesystem::path& path, const std::string& text) override;
	bool Compile(const std::filesystem::path& srcFile, std::filesystem::path& executablePath, std::string& executableParams, std::string& error) override;

protected:
	virtual std::pair<std::filesystem::path, std::string> MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const = 0;
	virtual std::pair<std::filesystem::path, std::string> MakeExecutableParams(const std::filesystem::path& srcFile) const;

	static std::string CleanupError(std::string error, const std::filesystem::path& path);
};

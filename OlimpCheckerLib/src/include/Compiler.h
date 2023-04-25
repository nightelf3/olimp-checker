#pragma once

#include "Interfaces/ICompilerImpl.h"
#include <string>
#include <memory>
#include <filesystem>

class Compiler
{
public:
	Compiler(std::string text, std::filesystem::path path, std::unique_ptr<ICompilerImpl> pImpl);

	bool Run();
	std::filesystem::path ExecutablePath() const { return m_ExecutablePath; }
	std::string Error() const { return m_Error; }

	static std::unique_ptr<ICompilerImpl> MakeImplFromExtension(const std::string& extension);
	static std::filesystem::path MakeFilePath(const std::string& username, const std::string& taskname, int queueId, const std::string& extension);

private:
	std::string m_Text;
	std::filesystem::path m_FilePath;
	std::unique_ptr<ICompilerImpl> m_pImpl;

	std::filesystem::path m_ExecutablePath;
	std::string m_Error;
};
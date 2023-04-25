#include "stdafx.h"

#include "include/Compiler.h"
#include "include/Compilers/CppCompilerImpl.h"

#include <fstream>
#include <format>

Compiler::Compiler(std::string text, std::filesystem::path path, std::unique_ptr<ICompilerImpl> pImpl)
	: m_Text(std::move(text)),
		m_FilePath(std::move(path)),
		m_pImpl(std::move(pImpl))
{
}

bool Compiler::Run()
{
	if (!m_pImpl)
		return false;

	try
	{
		// create path
		if (!m_pImpl->CreatePath(m_FilePath.parent_path()))
			return false;

		// dump the file
		if (!m_pImpl->CreateFile(m_FilePath, m_Text))
			return false;

		// run the compilation
		if (!m_pImpl->Compile(m_FilePath, m_ExecutablePath, m_Error))
			return false;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception caught in Compiler::Run: " << ex.what() << std::endl;
		return false;
	}

	return true;
}

std::filesystem::path Compiler::MakeFilePath(const std::string& username, const std::string& taskname, int queueId, const std::string& extension)
{
	// "Users" is a root folder which is used for compiled programs
	std::filesystem::path path("Users", std::filesystem::path::native_format);
	path.append(username);
	path.append(taskname);
	path.append(std::to_string(queueId));
	path.append(std::format("{}.{}", queueId, extension));
	return path;
}

std::unique_ptr<ICompilerImpl> Compiler::MakeImplFromExtension(const std::string& extension)
{
	if (extension == "c" || extension == "cpp")
		return std::make_unique<CppCompilerImpl>();
	std::cerr << "Unknown extension: " << extension << std::endl;
	return {};

}

#pragma once

#include <gtest/gtest.h>
#include "include/Compiler.h"
#include <filesystem>

struct FileGuard
{
	FileGuard(std::filesystem::path path) : m_Path(std::move(path)) {}
	~FileGuard()
	{
		try
		{
			if (!m_Path.empty() && std::filesystem::exists(m_Path))
				std::filesystem::remove(m_Path);
		}
		catch (const std::exception& ex)
		{
			std::cerr << "ERROR: can't remove the file with the following error: " << ex.what() << std::endl;
		}
	}

	const std::filesystem::path& Path() const { return m_Path; }
	std::string Extension() const { return m_Path.extension().string().erase(0, 1); }

	operator std::filesystem::path() const { return m_Path; }

private:
	std::filesystem::path m_Path;
};

inline FileGuard CompileCode(std::string code, const FileGuard& path)
{
	Compiler compiler(std::move(code), path, Compiler::MakeImplFromExtension(path.Extension()));
	EXPECT_TRUE(compiler.Run());
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
	return { compiler.ExecutablePath() };
}
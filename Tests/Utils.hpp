#pragma once

#include <gtest/gtest.h>
#include "include/Compiler.h"
#include <filesystem>

struct FileGuard
{
	FileGuard(ExecutableData exeData) : m_Data(std::move(exeData)) {}
	FileGuard(std::filesystem::path path) : m_Data{ std::move(path) } {}

	~FileGuard()
	{
		try
		{
			if (m_bDelete && Exists())
				std::filesystem::remove(m_Data.path);
		}
		catch (const std::exception& ex)
		{
			std::cerr << "ERROR: can't remove the file with the following error: " << ex.what() << std::endl;
		}
	}

	void PreventDeletion() { m_bDelete = false; }

	bool Exists() const { return !m_Data.path.empty() && std::filesystem::exists(m_Data.path); }
	const ExecutableData& ExecutableData() const { return m_Data; }
	const std::filesystem::path& Path() const { return m_Data.path; }
	std::string Extension() const { return m_Data.path.extension().string().erase(0, 1); }
	operator std::filesystem::path() const { return m_Data.path; }
	operator ::ExecutableData() const { return m_Data; }

private:
	::ExecutableData m_Data;
	bool m_bDelete = true;
};

inline FileGuard CompileCode(std::string code, const FileGuard& path)
{
	Compiler compiler(std::move(code), path, Compiler::MakeImplFromExtension(path.Extension()));
	EXPECT_TRUE(compiler.Run());
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
	return compiler.ExecutableData();
}
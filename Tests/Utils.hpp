#pragma once

#include <filesystem>

struct FileGuard
{
	FileGuard(std::filesystem::path path) : m_Path(std::move(path)) {}
	~FileGuard() { if (std::filesystem::exists(m_Path)) std::filesystem::remove(m_Path); }
	std::filesystem::path m_Path;
};
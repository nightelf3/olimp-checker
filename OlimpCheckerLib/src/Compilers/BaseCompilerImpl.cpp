#include "stdafx.h"

#include "include/Compilers/BaseCompilerImpl.h"

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

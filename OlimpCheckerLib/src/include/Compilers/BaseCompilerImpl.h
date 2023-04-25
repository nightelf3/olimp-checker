#pragma once

#include "include/Interfaces/ICompilerImpl.h"

class BaseCompilerImpl : public ICompilerImpl
{
public:
	bool CreatePath(const std::filesystem::path& path) override;
	bool CreateFile(const std::filesystem::path& path, const std::string& text) override;
};

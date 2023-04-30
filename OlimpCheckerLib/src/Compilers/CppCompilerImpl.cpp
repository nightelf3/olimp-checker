#include "stdafx.h"

#include "include/Compilers/CppCompilerImpl.h"
#include "include/Process.h"

std::pair<std::filesystem::path, std::string> CppCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	const std::filesystem::path compiler = { srcFile.extension() == "c" ? "gcc" : "g++" };
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/MinGW32/bin" }) / compiler;
	std::string params = std::format("-g -O3 -static \"{}\" -o \"{}\"", srcFile.string(), dstFile.string());
	return { std::move(path), std::move(params) };
}

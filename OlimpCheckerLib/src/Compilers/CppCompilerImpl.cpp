#include "stdafx.h"

#include "include/Compilers/CppCompilerImpl.h"
#include "include/Process.h"

ExecutableData CppCompilerImpl::MakeCompilationParams(const std::filesystem::path& srcFile, const std::filesystem::path& dstFile) const
{
	const std::filesystem::path compiler = { srcFile.extension() == "c" ? "gcc" : "g++" };
	std::filesystem::path path = std::filesystem::absolute({ "Compilers/MinGW32/bin" }) / compiler;
	std::string params = std::format("-O3 -static -DNDEBUG \"{}\" -o \"{}\"", srcFile.string(), dstFile.string());
	return { std::move(path), std::move(params) };
}

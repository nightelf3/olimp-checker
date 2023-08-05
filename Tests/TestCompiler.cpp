#include "gtest/gtest.h"
#include "gmock/gmock.h" // for EXPECT_THAT

#include "Mocks/CompilerImplMock.h"
#include "Utils.hpp"

#include <fstream>

using namespace ::testing;

TEST(TestCompiler, MakeImplFromExtension)
{
	// supported types
	EXPECT_THAT(Compiler::MakeImplFromExtension("c"), NotNull());
	EXPECT_THAT(Compiler::MakeImplFromExtension("cpp"), NotNull());

	// unsupported types
	EXPECT_THAT(Compiler::MakeImplFromExtension("trololo"), IsNull());
}

TEST(TestCompiler, EmptyImpl)
{
	const std::filesystem::path tempFile = std::filesystem::temp_directory_path() / "EmptyImpl.cpp";
	Compiler compiler({}, tempFile, nullptr);
	EXPECT_FALSE(compiler.Run());
}

TEST(TestCompiler, PathCreation)
{
	const std::filesystem::path tmpFilePath = std::filesystem::temp_directory_path() / "PathCreation.cpp";

	std::unique_ptr<CompilerImplMock> pCompilerMock = std::make_unique<CompilerImplMock>();
	EXPECT_CALL(*pCompilerMock, CreatePath(_)).WillRepeatedly(Return(false));

	Compiler compiler({}, tmpFilePath, std::move(pCompilerMock));
	EXPECT_FALSE(compiler.Run());
}

TEST(TestCompiler, FileCreation)
{
	const std::filesystem::path tmpFilePath = std::filesystem::temp_directory_path() / "FileCreation.cpp";

	std::unique_ptr<CompilerImplMock> pCompilerMock = std::make_unique<CompilerImplMock>();
	EXPECT_CALL(*pCompilerMock, CreatePath(_)).WillRepeatedly(Return(true));
	EXPECT_CALL(*pCompilerMock, CreateFile(_, _)).WillRepeatedly(Return(false));

	Compiler compiler({}, tmpFilePath, std::move(pCompilerMock));
	EXPECT_FALSE(compiler.Run());
}

TEST(TestCompiler, ExceptionHandling)
{
	const std::filesystem::path tmpFilePath = std::filesystem::temp_directory_path() / "ExceptionHandling.cpp";

	std::unique_ptr<CompilerImplMock> pCompilerMock = std::make_unique<CompilerImplMock>();
	EXPECT_CALL(*pCompilerMock, CreatePath(_)).WillRepeatedly(Return(true));
	EXPECT_CALL(*pCompilerMock, CreateFile(_, _)).WillRepeatedly(Return(true));
	EXPECT_CALL(*pCompilerMock, Compile(_, _, _)).WillRepeatedly([]() {
		throw std::exception("Fake exception");
		return false;
	});

	Compiler compiler({}, tmpFilePath, std::move(pCompilerMock));
	EXPECT_FALSE(compiler.Run());
}

TEST(TestCompiler, RunCCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "RunCCompilation.c";
	std::string code = "int main() { return 0; }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_TRUE(compiler.Run());
	const FileGuard exePath = compiler.ExecutableData();
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
}

TEST(TestCompiler, FailCCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "FailCCompilation.c";
	std::string code = "int main() { retrn 0; }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_FALSE(compiler.Run());
	const FileGuard exePath = compiler.ExecutableData();
	EXPECT_THAT(compiler.Error(), HasSubstr("'retrn' was not declared in this scope"));
}

TEST(TestCompiler, RunCppCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "RunCppCompilation.cpp";
	std::string code = "int main() { return 0; }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_TRUE(compiler.Run());
	const FileGuard exePath = compiler.ExecutableData();
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
}

TEST(TestCompiler, FailCppCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "FailCppCompilation.c";
	std::string code = "int man() { return 0; }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_FALSE(compiler.Run());
	const FileGuard exePath = compiler.ExecutableData();
	EXPECT_THAT(compiler.Error(), HasSubstr("undefined reference to"));
}

TEST(TestCompiler, RunPasCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "RunPasCompilation.pas";
	std::string code = R"---(
		Begin
		End.
	)---";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_TRUE(compiler.Run());
	const FileGuard exePath = compiler.ExecutableData();
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
}

TEST(TestCompiler, FailPasCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "FailPasCompilation.pas";
	std::string code = R"---(
		Begin
		End
	)---";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_FALSE(compiler.Run());
	const FileGuard exePath = compiler.ExecutableData();
	EXPECT_THAT(compiler.Error(), HasSubstr("Fatal: Syntax error"));
}

TEST(TestCompiler, RunPyCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "RunPyCompilation.py";
	std::string code = "print(\"Hello World!\")";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_TRUE(compiler.Run());
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
}

TEST(TestCompiler, FailPyCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "FailPyCompilation.py";
	std::string code = "print(\"Hello World!\"";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_FALSE(compiler.Run());
	EXPECT_THAT(compiler.Error(), HasSubstr("was never closed"));
}

TEST(TestCompiler, RunCSharpCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "RunCSharpCompilation.cs";
	std::string code = "class HelloWorld { public static void Main(string[] args) { } }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_TRUE(compiler.Run());
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
}

TEST(TestCompiler, FailCSharpCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "FailCSharpCompilation.cs";
	std::string code = "class HelloWorld { public static void Main(string[] args) { }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_FALSE(compiler.Run());
	EXPECT_THAT(compiler.Error(), HasSubstr("error CS1513: } expected"));
}

TEST(TestCompiler, RunJavaCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "RunJavaCompilation.java";
	std::string code = "class Program { public static void main(String[] args) { } }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_TRUE(compiler.Run());
	EXPECT_EQ(compiler.Error().size(), 0) << "Compilation fails with the following message: " << compiler.Error();
}

TEST(TestCompiler, FailJavaCompilation)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "FailJavaCompilation.java";
	std::string code = "class Program { public static void main(String[] args) { }";
	Compiler compiler(std::move(code), srcPath, Compiler::MakeImplFromExtension(srcPath.Extension()));
	EXPECT_FALSE(compiler.Run());
	EXPECT_THAT(compiler.Error(), HasSubstr("error: reached end of file while parsing"));
}

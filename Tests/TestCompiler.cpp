#include "gtest/gtest.h"
#include "gmock/gmock.h" // for EXPECT_THAT

#include "Mocks/CompilerImplMock.h"
#include "include/Compiler.h"

using namespace ::testing;

namespace
{
	constexpr std::string_view kCppCode = "int main() { return 0; }";
}

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
	Compiler compiler(std::string{ kCppCode }, tempFile, nullptr);
	EXPECT_FALSE(compiler.Run());
}

TEST(TestCompiler, PathCreation)
{
	const std::filesystem::path tmpFilePath = std::filesystem::temp_directory_path() / "PathCreation.cpp";

	std::unique_ptr<CompilerImplMock> pCompilerMock = std::make_unique<CompilerImplMock>();
	EXPECT_CALL(*pCompilerMock, CreatePath(_)).WillRepeatedly(Return(false));

	Compiler compiler(std::string{ kCppCode }, tmpFilePath, std::move(pCompilerMock));
	EXPECT_FALSE(compiler.Run());
}

TEST(TestCompiler, FileCreation)
{
	const std::filesystem::path tmpFilePath = std::filesystem::temp_directory_path() / "FileCreation.cpp";

	std::unique_ptr<CompilerImplMock> pCompilerMock = std::make_unique<CompilerImplMock>();
	EXPECT_CALL(*pCompilerMock, CreatePath(_)).WillRepeatedly(Return(true));
	EXPECT_CALL(*pCompilerMock, CreateFile(_, _)).WillRepeatedly(Return(false));

	Compiler compiler(std::string{ kCppCode }, tmpFilePath, std::move(pCompilerMock));
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

	Compiler compiler(std::string{ kCppCode }, tmpFilePath, std::move(pCompilerMock));
	EXPECT_FALSE(compiler.Run());
}

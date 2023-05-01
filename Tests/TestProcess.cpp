#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "include/Process.h"
#include "Utils.hpp"

using namespace ::testing;

TEST(TestProcess, CustomInput)
{
	Process process;
	process.Input("echo Trololo && exit");
	EXPECT_TRUE(process.Run({ "C:\\Windows\\System32\\cmd.exe" }));
	EXPECT_THAT(process.MoveOutput(), HasSubstr("Trololo"));
}

TEST(TestProcess, CustomParams)
{
	Process process;
	EXPECT_TRUE(process.Run({ "C:\\Windows\\System32\\ping.exe" }, "-n 1 127.0.0.1"));
	EXPECT_THAT(process.MoveOutput(), HasSubstr("127.0.0.1:"));
}

TEST(TestProcess, TimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "TimeLimit.cpp";
	std::string code = "int main() { for (volatile unsigned i = 999999; i >= 0; i--); return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, MemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "MemoryLimit.cpp";
	std::string code = "int main() { volatile long long* mem = new long long[1000000]; return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(2);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, BandwidthLimit)
{
	//TODO: add with Python
}
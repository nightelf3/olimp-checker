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

TEST(TestProcess, SysRestirctions)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "SysRestirctions.cpp";
	std::string code = R"---(
		#include <windows.h>
		int main() { ExitWindowsEx(EWX_LOGOFF, NULL); return 0; }
	)---";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.SysRestrictions(true);
	EXPECT_TRUE(process.Run(exePath));
}

TEST(TestProcess, CppTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppTimeLimit.cpp";
	std::string code = "int main() { for (volatile unsigned i = 999999; i >= 0; i--); return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, CppMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppMemoryLimit.cpp";
	std::string code = "int main() { volatile long long* mem = new long long[1000000]; return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(2);
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, CppRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "CppRuntimeError.cpp";
	std::string code = "int main() { throw 0; return 0; }";
	const FileGuard exePath = CompileCode(std::move(code), srcPath);
	ASSERT_FALSE(exePath.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exePath));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}

TEST(TestProcess, PyTimeLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyTimeLimit.py";
	std::string code = R"---(
x = 0
while True:
    x += 1
	)---";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000);
	EXPECT_FALSE(process.Run(exeParams.Path(), exeParams.Params()));
	EXPECT_EQ(process.Code(), ProcessCode::TimeLimit);
}

TEST(TestProcess, PyMemoryLimit)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyMemoryLimit.py";
	std::string code = "new_list1M = [0] * 1000000";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	process.TimeLimit(1'000); // just in case
	process.MemoryLimit(5);
	EXPECT_FALSE(process.Run(exeParams.Path(), exeParams.Params()));
	EXPECT_EQ(process.Code(), ProcessCode::MemoryLimit);
}

TEST(TestProcess, PyRuntimeError)
{
	const FileGuard srcPath = std::filesystem::temp_directory_path() / "PyRuntimeError.py";
	std::string code = "print(\"Hello World!\");sd";
	FileGuard exeParams = CompileCode(std::move(code), srcPath);
	exeParams.PreventDeletion();
	ASSERT_FALSE(exeParams.Path().empty()) << "Code is not compiled";

	Process process;
	EXPECT_FALSE(process.Run(exeParams.Path(), exeParams.Params()));
	EXPECT_EQ(process.Code(), ProcessCode::RuntimeError);
}
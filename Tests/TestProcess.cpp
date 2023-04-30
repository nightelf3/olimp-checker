#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "include/Process.h"

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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "include/Process.h"

using namespace ::testing;

TEST(TestProcess, ReadOutput)
{
	Process process;
	process.Path({ "C:\\Windows\\System32\\cmd.exe" });
	process.Input("echo Trololo && exit");
	EXPECT_TRUE(process.Run());
	EXPECT_THAT(process.Output(), HasSubstr("Trololo"));
}
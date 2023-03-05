#include "pch.h"

#include "include/Request.h"

TEST(TestRequest, ParseQuery)
{
	RequestData data("https://sub.test.com/path?params=23");
	EXPECT_EQ(data.m_Server, "https://sub.test.com");
	EXPECT_EQ(data.m_Path, "/path?params=23");
}

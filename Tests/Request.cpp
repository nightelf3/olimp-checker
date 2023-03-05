#include "pch.h"

#include "include/Request.h"

TEST(TestRequest, ParseQuery)
{
	constexpr char url[] = "https://sub.test.com/path/test/?param1=23&param2=12";

	RequestData data(url);
	EXPECT_EQ(data.GetServer(), "https://sub.test.com");
	EXPECT_EQ(data.GetPath(), "/path/test/");
	EXPECT_EQ(data.GetUrl(), url);

	auto& params = data.GetParams();
	auto fnCheckParam = [&params](const char key[], const char value[])
	{
		auto it = params.find(key);
		EXPECT_NE(it, params.end()) << "Can't find the " << key << " param";
		EXPECT_EQ(it->second, value);
	};

	fnCheckParam("param1", "23");
	fnCheckParam("param2", "12");
}

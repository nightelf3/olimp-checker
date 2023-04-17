#include "gtest/gtest.h"

#include "include/Request.h"
#include "Mocks/RequestImplMock.h"

using namespace ::testing;

TEST(TestRequest, ParseEmptyUrl)
{
	constexpr char url[] = "http://sub.test.com/path/";

	Request data(url);
	EXPECT_EQ(data.Server(), "http://sub.test.com");
	EXPECT_EQ(data.Path(), "/path/");
	EXPECT_EQ(data.Url(), url);
}

TEST(TestRequest, ParseGetParams)
{
	constexpr char url[] = "https://sub.test.com/path/test/?param1=23&param2=12";

	Request data(url);
	EXPECT_EQ(data.Server(), "https://sub.test.com");
	EXPECT_EQ(data.Path(), "/path/test/");
	EXPECT_EQ(data.Url(), url);

	auto& params = data.Get();
	auto fnCheckParam = [&params](const char* key, const char* value)
	{
		auto it = params.find(key);
		EXPECT_NE(it, params.end()) << "Can't find the " << key << " param";
		EXPECT_EQ(it->second, value);
	};

	fnCheckParam("param1", "23");
	fnCheckParam("param2", "12");
}

TEST(TestRequest, GetParams)
{
	std::unique_ptr<RequestImplMock> pRequestMock = std::make_unique<RequestImplMock>();
	EXPECT_CALL(*pRequestMock, Perform(StrEq("https://sub.test.com/path/test/?param1=123")))
		.WillRepeatedly(Return(Response{ 200, "test", "" }));

	Request data("https://sub.test.com/path/test/", RequestType::Get, std::move(pRequestMock));
	data.Get()["param1"] = "123";
	const Response response = data.Perform();
	EXPECT_TRUE(response);
	EXPECT_EQ(response.m_Data, "test");
}

TEST(TestRequest, PostParams)
{
	std::unique_ptr<RequestImplMock> pRequestMock = std::make_unique<RequestImplMock>();
	EXPECT_CALL(*pRequestMock, SetPostParams(StrEq("param2=456"))).Times(1);
	EXPECT_CALL(*pRequestMock, Perform(StrEq("https://sub.test.com/path/test/?param1=123")))
		.WillRepeatedly(Return(Response{ 200, "test", "" }));

	Request data("https://sub.test.com/path/test/", RequestType::Post, std::move(pRequestMock));
	data.Get()["param1"] = "123";
	data.Post()["param2"] = "456";
	const Response response = data.Perform();
	EXPECT_TRUE(response);
	EXPECT_EQ(response.m_Data, "test");
}

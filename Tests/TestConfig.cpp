#include "gtest/gtest.h"
#include "Mocks/ConfigImplMock.h"
#include "include/Config.h"
#include <memory>

using namespace ::testing;

TEST(TestConfig, DefaultValue)
{
	std::unique_ptr<ConfigImplMock> pRequestMock = std::make_unique<ConfigImplMock>();
	EXPECT_CALL(*pRequestMock, Get(StrEq("field1"))).WillRepeatedly(Return("data1"));
	EXPECT_CALL(*pRequestMock, Get(StrEq("field2"))).WillRepeatedly(Return(std::nullopt));

	Config config(std::move(pRequestMock));
	EXPECT_EQ(config.Get("field1"), "data1");
	EXPECT_EQ(config.Get("field1"), "data1");
	EXPECT_EQ(config.Get("field1", "data2"), "data1");
	EXPECT_EQ(config.Get("field2", "data2"), "data2");
}
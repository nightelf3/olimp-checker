#pragma once

#include "include/Response.h"

struct IRequestImpl
{
	virtual ~IRequestImpl() = default;
	virtual void SetPostParams(std::string params) = 0;
	virtual Response Perform(const std::string& url) = 0;
};

#pragma once

#include <optional>
#include <string>

struct IConfigImpl
{
	virtual ~IConfigImpl() = default;
	virtual bool Read(const std::string& path) = 0;
	virtual std::optional<std::string> Get(const char* key) const = 0;
};

#pragma once

#include "Interfaces/IConfigImpl.h"
#include <map>
#include <memory>

class Config
{
public:
	Config(std::unique_ptr<IConfigImpl> impl = nullptr);
	bool Read(const std::string& path);

	bool Has(const char* key) const;
	bool Has(const std::string& key) const;

	std::string Get(const char* key, const std::string& def = {}) const;
	std::string Get(const std::string& key, const std::string& def = {}) const;
	std::string operator[](const char* key) const;
	std::string operator[](const std::string& key) const;

private:
	std::unique_ptr<IConfigImpl> m_Impl;
};
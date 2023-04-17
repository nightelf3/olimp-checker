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

	void Set(const char* key, std::string val);
	void Set(const std::string& key, std::string val);

private:
	std::unique_ptr<IConfigImpl> m_Impl;
};
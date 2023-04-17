#include "stdafx.h"
#include "include/Config.h"

#include <json/json.h>
#include <fstream>

namespace
{
	class ConfigImpl : public IConfigImpl
	{
	public:
		bool Read(const std::string& path) override
		{
			std::ifstream in{ path };
			if (!in)
			{
				std::cerr << "ERROR: Can't open filestream" << std::endl;
				return false;
			}

			std::string errs;
			if (Json::CharReaderBuilder rbuilder; !Json::parseFromStream(rbuilder, in, &m_Value, &errs))
			{
				std::cerr << "ERROR: Config parsing failed with the following error: " << errs << std::endl;
				return false;
			}

			return true;
		}

		std::optional<std::string> Get(const char* key) const
		{
			return m_Value[key].isNull() ? std::optional<std::string>{} : m_Value[key].asString();
		}

		void Set(const char* key, std::string value) override
		{
			m_Value[key] = std::move(value);
		}

	private:
		Json::Value m_Value;
	};
}

Config::Config(std::unique_ptr<IConfigImpl> impl)
{
	if (!impl)
		impl = std::make_unique<ConfigImpl>();
	m_Impl = std::move(impl);
}

bool Config::Read(const std::string& path)
{
	return m_Impl->Read(path);
}

bool Config::Has(const char* key) const
{
	return m_Impl->Get(key) != std::nullopt;
}

bool Config::Has(const std::string& key) const
{
	return Has(key.c_str());
}

std::string Config::Get(const char* key, const std::string& def) const
{
	if (auto value = m_Impl->Get(key))
		return *value;
	return def;
}

std::string Config::Get(const std::string& key, const std::string& def) const
{
	return Get(key.c_str(), def);
}

std::string Config::operator[](const char* key) const
{
	if (auto value = m_Impl->Get(key))
		return *value;
	return std::string{};
}

std::string Config::operator[](const std::string& key) const
{
	return operator[](key.c_str());
}

void Config::Set(const char* key, std::string val)
{
	m_Impl->Set(key, std::move(val));
}

void Config::Set(const std::string& key, std::string val)
{
	Set(key.c_str(), std::move(val));
}
#pragma once

#include "include/Config.h"
#include "include/Request.h"

namespace Json { class Value; }

class Application
{
public:
	Application();
	Application(const Application&) = delete;
	Application(Application&&) = default;
	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = default;
	~Application();

	bool Initialize();
	bool Register();
	void Run();
	bool Logout();

private:
	Request MakeRequestWithCredentials(std::string path) const;
	void PerformTasks(const Json::Value& task);

	Config m_Configs;
	bool m_bRegistered = false;
};
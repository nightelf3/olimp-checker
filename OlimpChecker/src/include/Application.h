#pragma once

#include "include/Config.h"

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
	void PerformTask(const Json::Value& task);
	Config m_Configs;
	bool m_bRegistered = false;
};
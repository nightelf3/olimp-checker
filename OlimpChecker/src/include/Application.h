#pragma once

#include "include/Config.h"

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

private:
	Config m_Configs;
};
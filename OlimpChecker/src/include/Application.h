#pragma once

class Application
{
public:
	Application();
	Application(const Application&) = delete;
	Application(const Application&&) = delete;
	~Application();
};
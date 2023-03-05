#pragma once

#include "DllExport.h"

class DLL_EXPORT Application
{
public:
	Application();
	Application(const Application&) = delete;
	Application(const Application&&) = delete;
	~Application();

	void Run();
};
#include "include/Application.h"
#include <iostream>
#include <signal.h>

namespace
{
	Application app;
	void SignalCallbackHandler(int signum)
	{
		app.Logout();
		exit(signum);
	}
}

int main(int argc, char** argv)
{
	std::cout << "Initialize the application, read configs" << std::endl;
	if (!app.Initialize())
		return 1;

	std::cout << "Register application" << std::endl;
	if (!app.Register())
		return 1;

	// logout on exceptions
	signal(SIGINT, SignalCallbackHandler);
	signal(SIGILL, SignalCallbackHandler);
	signal(SIGFPE, SignalCallbackHandler);
	signal(SIGSEGV, SignalCallbackHandler);
	signal(SIGTERM, SignalCallbackHandler);
	signal(SIGBREAK, SignalCallbackHandler);
	signal(SIGABRT, SignalCallbackHandler);

	std::cout << "Running loop, press ESC to finish" << std::endl;
	app.Run();

	std::cout << "Loop ended, logging out" << std::endl;
	return app.Logout();
}
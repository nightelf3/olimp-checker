#include "include/Application.h"

int main(int argc, char** argv)
{
	Application app;

	// read configs, initialize the applications
	if (!app.Initialize())
		return 1;

	// register throght the API
	if (!app.Register())
		return 1;

	// logout from API
	return app.Logout();
}
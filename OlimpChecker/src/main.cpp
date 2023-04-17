#include "include/Application.h"

int main(int argc, char** argv)
{
	Application app;
	if (!app.Initialize())
		return 1;
	if (!app.Register())
		return 1;

	return 0;
}
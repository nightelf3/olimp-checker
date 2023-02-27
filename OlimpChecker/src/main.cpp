#include <iostream>

#include "include/Application.h"
#include "include/Request.h"

int main(int argc, char** argv)
{
	Application app;

	if (Response response = Request{}.GetContent("https://en.wikipedia.org/wiki/HTTPS"))
		std::cout << response.m_Data << std::endl;
	else
		std::cout << "curl_easy_perform() failed: " << response.GetError() << std::endl;

	return 0;
}
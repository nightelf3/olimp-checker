#pragma once

#include "Request.h"
#include "json/value.h"

class TaskPerformer
{
public:
	TaskPerformer(const Json::Value& task);
	void Run(Request request);

private:
	int m_QueueId = 0;
	bool m_UseFiles = false;
	std::string m_InputFile;
	std::string m_OutputFile;
	std::string m_Extension;
	int m_TimeLimit = 0; // ms
	int m_MemoryLimit = 0; // kB
	std::vector<std::pair<std::string, std::string>> m_Tests;
	std::string m_Text;
};
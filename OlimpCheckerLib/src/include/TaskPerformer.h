#pragma once

#include "Request.h"
#include "Compiler.h"
#include "json/value.h"

enum class TaskState
{
	NoAction = 0,
	InQueue,
	Compiling,
	CompilingError,
	InProgress,
	ResponseError,
	RuntimeError,
	OverMemory,
	OverTime,
	Succeed,
	InvalidOutputStream
};

class TaskPerformer
{
public:
	TaskPerformer(const Json::Value& task);
	void Run(Request request);

private:
	void ChangeState(Request& request, TaskState state);

private:
	int m_QueueId = 0;
	std::unique_ptr<Compiler> m_Compiler;

	bool m_UseFiles = false;
	std::filesystem::path m_InputFile;
	std::filesystem::path m_OutputFile;
	int m_TimeLimit = 0; // ms
	int m_MemoryLimit = 0; // Mb
	std::vector<std::pair<std::string, std::string>> m_Tests;
};
#pragma once

#include <filesystem>
#include <memory>

enum class ProcessCode
{
	Success = 0,
	TimeLimit,
	MemoryLimit,
	RuntimeError,
	Failed
};

struct ProcessResponse
{
	ProcessCode code = ProcessCode::Failed;
	std::string output;
};

struct ProcessData
{
	std::filesystem::path path;
	std::string params;
	std::string input;
	std::optional<size_t> memoryLimit;
	std::optional<size_t> timeLimit;
	bool sysRestrictions = false;
};

struct IProcessImpl
{
	virtual ~IProcessImpl() = default;
	virtual ProcessResponse Run(ProcessData data) = 0;
};

std::unique_ptr<IProcessImpl> MakeProcessImpl();

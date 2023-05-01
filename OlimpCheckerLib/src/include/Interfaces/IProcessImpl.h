#pragma once

#include <filesystem>
#include <memory>

enum class ProcessCode
{
	Success = 0,
	TimeLimit,
	MemoryLimit,
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
	std::optional<size_t> bandwithLimit;
};

struct IProcessImpl
{
	virtual ~IProcessImpl() = default;
	virtual ProcessResponse Run(ProcessData data) = 0;
};

std::unique_ptr<IProcessImpl> MakeProcessImpl();

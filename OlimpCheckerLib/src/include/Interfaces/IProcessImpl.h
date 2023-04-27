#pragma once

#include <filesystem>
#include <memory>

enum class ProcessCode
{
	Success = 0,
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
	std::string input;
};

struct IProcessImpl
{
	virtual ~IProcessImpl() = default;
	virtual ProcessResponse Run(ProcessData data) = 0;
};

std::unique_ptr<IProcessImpl> MakeProcessImpl();

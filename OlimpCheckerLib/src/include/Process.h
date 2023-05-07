#pragma once

#include "Interfaces/IProcessImpl.h"
#include "Interfaces/ICompilerImpl.h"

class Process
{
public:
	Process(std::unique_ptr<IProcessImpl> impl = nullptr);

	void ReadWriteFiles(std::filesystem::path input, std::filesystem::path output);
	void Input(std::string input);
	void TimeLimit(size_t limitInMs);
	void MemoryLimit(size_t limitInMb);
	void SysRestrictions(bool restriction);

	bool Run(ExecutableData data);

	ProcessCode Code() const { return m_Response.code; }
	const std::string& Output() const { return m_Response.output; }
	std::string MoveOutput() { return std::move(m_Response.output); }

private:
	std::unique_ptr<IProcessImpl> m_Impl;
	ProcessData m_Data;
	ProcessResponse m_Response;
	std::optional<std::pair<std::filesystem::path, std::filesystem::path>> m_Files;
};
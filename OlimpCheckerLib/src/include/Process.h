#pragma once

#include "Interfaces/IProcessImpl.h"

class Process
{
public:
	Process(std::unique_ptr<IProcessImpl> impl = nullptr);

	void Input(std::string input);
	void TimeLimit(size_t limitInMs);
	void MemoryLimit(size_t limitInMb);
	void BandwidthLimit(size_t limitInMb);

	bool Run(std::filesystem::path path, std::string params = {});

	ProcessCode Code() const { return m_Response.code; }
	const std::string& Output() const { return m_Response.output; }
	std::string MoveOutput() { return std::move(m_Response.output); }

private:
	std::unique_ptr<IProcessImpl> m_Impl;
	ProcessData m_Data;
	ProcessResponse m_Response;
};
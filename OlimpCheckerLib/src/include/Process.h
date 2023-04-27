#pragma once

#include "Interfaces/IProcessImpl.h"

class Process
{
public:
	Process(std::unique_ptr<IProcessImpl> impl = nullptr);

	void Input(std::string input);
	void Path(std::filesystem::path path) { m_Data.path = std::move(path); }

	bool Run();

	ProcessCode Code() const { return m_Response.code; }
	const std::string& Output() const { return m_Response.output; }
	std::string Output() { return std::move(m_Response.output); }

private:
	std::unique_ptr<IProcessImpl> m_Impl;
	ProcessData m_Data;
	ProcessResponse m_Response;
};
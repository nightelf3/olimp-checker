#include "stdafx.h"
#include "include/Process.h"

Process::Process(std::unique_ptr<IProcessImpl> impl)
{
	if (!impl)
		impl = MakeProcessImpl();
	m_Impl = std::move(impl);
}

void Process::Input(std::string input)
{
	m_Data.input = std::move(input);
	m_Data.input.append(1, '\n');
}

void Process::TimeLimit(size_t limitInMs)
{
	m_Data.timeLimit = limitInMs;
}

void Process::MemoryLimit(size_t limitInMb)
{
	m_Data.memoryLimit = limitInMb;
}

bool Process::Run(std::filesystem::path path, std::string params)
{
	if (!m_Impl)
		return false;

	m_Data.path = std::move(path);
	m_Data.params = std::move(params);

	m_Response = m_Impl->Run(std::move(m_Data));
	return m_Response.code == ProcessCode::Success;
}

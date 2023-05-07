#include "stdafx.h"
#include "include/Process.h"
#include <fstream>

Process::Process(std::unique_ptr<IProcessImpl> impl)
{
	if (!impl)
		impl = MakeProcessImpl();
	m_Impl = std::move(impl);
}

void Process::ReadWriteFiles(std::filesystem::path input, std::filesystem::path output)
{
	m_Files = std::make_pair(std::move(input), std::move(output));
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

void Process::SysRestrictions(bool restriction)
{
	m_Data.sysRestrictions = restriction;
}

bool Process::Run(ExecutableData exeData)
{
	if (!m_Impl)
		return false;

	m_Data.path = std::move(exeData.path);
	m_Data.workingDir = exeData.workingDir.empty() ? m_Data.path.parent_path() : std::move(exeData.workingDir);
	m_Data.params = std::move(exeData.params);

	if (m_Files)
	{
		std::ofstream out(m_Files->first);
		out << m_Data.input;
		m_Data.input.clear();
	}

	m_Response = m_Impl->Run(std::move(m_Data));

	if (m_Files && m_Response.code == ProcessCode::Success)
	{
		std::ostringstream buffer;
		buffer << std::ifstream(m_Files->second).rdbuf();
		m_Response.output = std::move(buffer.str());
	}

	return m_Response.code == ProcessCode::Success;
}

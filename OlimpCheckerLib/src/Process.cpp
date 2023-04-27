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

bool Process::Run()
{
	if (!m_Impl || m_Data.path.empty())
		return false;

	m_Response = m_Impl->Run(std::move(m_Data));
	return m_Response.code == ProcessCode::Success;
}

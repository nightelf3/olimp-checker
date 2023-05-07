#include "stdafx.h"
#include "include/Interfaces/IProcessImpl.h"

#include <array>
#include <Windows.h>

class Process_win : public IProcessImpl
{
public:
	ProcessResponse Run(ProcessData data) override;

private:
	std::pair<HANDLE, HANDLE> MakeInOutPipe(bool out) const;
	void WriteToPipe(HANDLE hPipe, std::string data);
	std::string ReadFromPipe(HANDLE hPipe);
};

ProcessResponse Process_win::Run(ProcessData data)
{
	ProcessResponse response;
	response.code = ProcessCode::Failed;

	// create in/out pipes
	auto [hPipeOutRead, hPipeOutWrite] = MakeInOutPipe(true);
	auto [hPipeInRead, hPipeInWrite] = MakeInOutPipe(false);
	if (!hPipeOutRead || !hPipeOutWrite || !hPipeInRead || !hPipeInWrite)
		return response;

	// Set up members of the STARTUPINFO structure.
	// This structure specifies the STDIN and STDOUT handles for redirection.
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = hPipeOutWrite;
	si.hStdOutput = hPipeOutWrite;
	si.hStdInput = hPipeInRead;
	si.dwFlags = STARTF_USESTDHANDLES;

	HANDLE hJobObject = CreateJobObject(nullptr, nullptr);
	if (data.sysRestrictions)
	{
		JOBOBJECT_BASIC_UI_RESTRICTIONS sysRestrioctions;
		sysRestrioctions.UIRestrictionsClass = JOB_OBJECT_UILIMIT_ALL;
		SetInformationJobObject(hJobObject, JobObjectBasicUIRestrictions, &sysRestrioctions, sizeof(JOBOBJECT_BASIC_UI_RESTRICTIONS));
	}

	// Set up members of the PROCESS_INFORMATION structure.
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	// Create the child process.
	const std::wstring cmd = std::format(L"{} {}", data.path.wstring(), std::wstring{ data.params.begin(), data.params.end()});
	const std::wstring dir = data.path.parent_path().wstring();
	if (!CreateProcess(nullptr, const_cast<LPWSTR>(cmd.c_str()), nullptr, nullptr, TRUE,
		CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED, nullptr, const_cast<LPWSTR>(dir.c_str()), &si, &pi))
	{
		std::cerr << "CreateProcess failed with the following error: " << GetLastError() << std::endl;
		return response;
	}

	// assign process with the job
	if (!AssignProcessToJobObject(hJobObject, pi.hProcess))
		std::cerr << "WARNING: AssignProcessToJobObject failed with the following error:" << GetLastError() << std::endl;

	// Close handles to the stdin and stdout pipes no longer needed by the child process.
	// If they are not explicitly closed, there is no way to recognize that the child process has ended.
	CloseHandle(hPipeOutWrite);
	CloseHandle(hPipeInRead);

	// Write into input pipe
	WriteToPipe(hPipeInWrite, std::move(data.input));

	// start the thread
	ResumeThread(pi.hThread);

	bool bCheckOtherErrors = true;
	switch (WaitForSingleObject(pi.hProcess, (DWORD)data.timeLimit.value_or(INFINITE)))
	{
	case WAIT_OBJECT_0:
		response.code = ProcessCode::Success;
		break;

	case WAIT_TIMEOUT:
		response.code = ProcessCode::TimeLimit;
		break;

	default:
		bCheckOtherErrors = false;
		response.code = ProcessCode::Failed;
		break;
	}

	// read the output only on success
	if (ProcessCode::Success == response.code)
		response.output = ReadFromPipe(hPipeOutRead);

	if (bCheckOtherErrors)
	{
		// check memory limit
		if (data.memoryLimit)
		{
			JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobExtLimit;
			ZeroMemory(&jobExtLimit, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
			QueryInformationJobObject(hJobObject, JobObjectExtendedLimitInformation, &jobExtLimit, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION), nullptr);

			if ((jobExtLimit.PeakProcessMemoryUsed >> 20) > *data.memoryLimit)
			{
				response.code = ProcessCode::MemoryLimit;
				bCheckOtherErrors = false;
			}
		}

		// check exit code to make sure no runtime errors
		if (bCheckOtherErrors)
		{
			DWORD exitCode = 0;
			GetExitCodeProcess(pi.hProcess, &exitCode);
			if (exitCode != 0 && exitCode != STATUS_PENDING)
				response.code = ProcessCode::RuntimeError;
		}
	}

	CloseHandle(hPipeInWrite);
	CloseHandle(hPipeOutRead);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	TerminateJobObject(hJobObject, 0);

	return response;
}

std::pair<HANDLE, HANDLE> Process_win::MakeInOutPipe(bool out) const
{
	HANDLE hRead = NULL, hWrite = NULL;

	// Set the bInheritHandle flag so pipe handles are inherited.
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	// Create a pipe for the child process's STDOUT.
	if (CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		// Ensure the read handle to the pipe for STDOUT is not inherited.
		SetHandleInformation(out ? hRead : hWrite, HANDLE_FLAG_INHERIT, 0);
	}

	return { hRead, hWrite };
}

void Process_win::WriteToPipe(HANDLE hPipe, std::string data)
{
	if (data.empty())
		return;

	if (!WriteFile(hPipe, data.c_str(), (DWORD)data.size(), nullptr, nullptr))
		std::cerr << "Can`t write the data to the process (by pipe)" << std::endl;
}

std::string Process_win::ReadFromPipe(HANDLE hPipe)
{
	constexpr DWORD kBuffSize = 1024;
	std::array<char, kBuffSize> buffer;

	std::string result;
	DWORD nReadBytes = 0;

	for (;;)
	{
		if (!ReadFile(hPipe, buffer.data(), kBuffSize, &nReadBytes, nullptr))
			return result;
		result.append(buffer.data(), nReadBytes);
	}

	return result;
}

std::unique_ptr<IProcessImpl> MakeProcessImpl()
{
	return std::make_unique<Process_win>();
}
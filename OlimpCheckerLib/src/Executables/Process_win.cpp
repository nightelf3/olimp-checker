#include "stdafx.h"
#include "include/Interfaces/IProcessImpl.h"

#include <array>
#include <Windows.h>

namespace
{
	struct HandlerDeleter
	{
		void operator()(PHANDLE handler)
		{
			if (!handler)
				return;
			CloseHandle(*handler);
			delete handler;
		}
	};
	using upHandler = std::unique_ptr<HANDLE, HandlerDeleter>;
}

class Process_win : public IProcessImpl
{
public:
	ProcessResponse Run(ProcessData data) override;

private:
	std::pair<upHandler, upHandler> MakeInOutPipe(bool out) const;
	void WriteToPipe(upHandler hPipe, std::string data);
	std::string ReadFromPipe(upHandler hPipe);
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
	si.hStdError = *hPipeOutWrite;
	si.hStdOutput = *hPipeOutWrite;
	si.hStdInput = *hPipeInRead;
	si.dwFlags = STARTF_USESTDHANDLES;

	// Set up members of the PROCESS_INFORMATION structure.
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	// Create the child process.
	const std::wstring cmd = data.path.wstring();
	const std::wstring dir = data.path.parent_path().wstring();
	if (!CreateProcess(NULL, const_cast<LPWSTR>(cmd.c_str()), nullptr, nullptr, TRUE,
		CREATE_NO_WINDOW, nullptr, const_cast<LPWSTR>(dir.c_str()), &si, &pi))
	{
		return response;
	}

	// Close handles to the child process and its primary thread.
	// Some applications might keep these handles to monitor the status
	// of the child process, for example.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// Close handles to the stdin and stdout pipes no longer needed by the child process.
	// If they are not explicitly closed, there is no way to recognize that the child process has ended.
	hPipeOutWrite.reset();
	hPipeInRead.reset();

	// Write into input pipe
	WriteToPipe(std::move(hPipeInWrite), std::move(data.input));

	response.code = ProcessCode::Success;
	response.output = ReadFromPipe(std::move(hPipeOutRead));
	return response;
}

std::pair<upHandler, upHandler> Process_win::MakeInOutPipe(bool out) const
{
	upHandler hRead{ new HANDLE };
	upHandler hWrite{ new HANDLE };

	// Set the bInheritHandle flag so pipe handles are inherited.
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	// Create a pipe for the child process's STDOUT.
	if (!CreatePipe(hRead.get(), hWrite.get(), &sa, 0))
		return {};

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	SetHandleInformation(out ? hRead.get() : hWrite.get(), HANDLE_FLAG_INHERIT, 0);

	return std::make_pair(std::move(hRead), std::move(hWrite));
}

void Process_win::WriteToPipe(upHandler hPipe, std::string data)
{
	if (data.empty())
		return;

	if (!WriteFile(*hPipe, data.c_str(), (DWORD)data.size(), nullptr, nullptr))
		std::cerr << "Can`t write the data to the process (by pipe)" << std::endl;
}

std::string Process_win::ReadFromPipe(upHandler hPipe)
{
	constexpr DWORD kBuffSize = 1024;
	std::array<char, kBuffSize> buffer;

	std::string result;
	DWORD nReadBytes = 0;

	for (;;)
	{
		if (!ReadFile(*hPipe, buffer.data(), kBuffSize, &nReadBytes, nullptr))
			return result;
		result.append(buffer.data(), nReadBytes);
	}

	return result;
}

std::unique_ptr<IProcessImpl> MakeProcessImpl()
{
	return std::make_unique<Process_win>();
}
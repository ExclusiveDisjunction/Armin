#include "Misc.h"
#include <iostream>

using namespace std;

void LoadExe(PCWSTR Path)
{
	LoadExe(Path, nullptr);
}
void LoadExe(PCWSTR Path, LPWSTR CommandLine)
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(Path, CommandLine, NULL, NULL, FALSE, FALSE, NULL, NULL, &si, &pi))
	{
		std::wcout << L"The process could not complete sucessfully." << std::endl;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
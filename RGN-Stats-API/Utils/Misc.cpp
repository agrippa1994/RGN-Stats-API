#include "Misc.h"
#include "Windows.h"

#include <TlHelp32.h>
#include <Psapi.h>

#include <boost/algorithm/string.hpp>

DWORD GetProcIdByWindowName(const std::string& windName)
{
	DWORD dwPID = 0;
	HWND wHandle = 0;
	wHandle = FindWindow(NULL, windName.c_str());
	if (wHandle == 0 || wHandle == INVALID_HANDLE_VALUE)
		return dwPID;

	GetWindowThreadProcessId(wHandle, &dwPID);
	return dwPID;
}

DWORD GetProcIdByProcName(const std::string& procName)
{
	PROCESSENTRY32 entry;
	DWORD dwPID = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == 0 || hSnapshot == INVALID_HANDLE_VALUE)
		return dwPID;

	entry.dwSize = sizeof(entry);
	Process32First(hSnapshot, &entry);

	while (Process32Next(hSnapshot, &entry))
	{
		if (boost::iequals(entry.szExeFile, procName))
		{
			dwPID = entry.th32ProcessID;
			break;
		}
	}

	CloseHandle(hSnapshot);
	return dwPID;
}

DWORD GetModuleLength(HMODULE hHandle)
{
	MODULEINFO info;
	GetModuleInformation(GetCurrentProcess(), hHandle, &info, sizeof(info));
	return info.SizeOfImage;
}
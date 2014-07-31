#include "Client.h"

#include <dllmain.h>
#include <ShlObj.h>

#include <Shared/PipeMessages.h>
#include <Utils/Misc.h>

#include <string>


bool IsServerAvailable()
{
	CBitStream bsIn, bsOut;

	bsIn << PipeMessages::Ping;

	return CNamedPipeClient(bsIn, bsOut).Success();
}



EXPORT int Init()
{
	char szDLLPath[MAX_PATH + 1] = { 0 };
	BOOL bRetn;

	GetModuleFileName((HMODULE) g_hDllHandle, szDLLPath, sizeof(szDLLPath));
	DWORD dwPId = GetProcIdByWindowName("GTA:SA:MP");

	if (dwPId == 0)
		return 0;

	HANDLE hHandle = OpenProcess((STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF), FALSE, dwPId);
	if (hHandle == 0 || hHandle == INVALID_HANDLE_VALUE)
		return 0;

	void *pAddr = VirtualAllocEx(hHandle, NULL, strlen(szDLLPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAddr == NULL)
	{
		CloseHandle(hHandle);
		return 0;
	}

	bRetn = WriteProcessMemory(hHandle, pAddr, szDLLPath, strlen(szDLLPath) + 1, NULL);
	if (bRetn == FALSE)
	{
		VirtualFreeEx(hHandle, pAddr, strlen(szDLLPath) + 1, MEM_RELEASE);
		CloseHandle(hHandle);
		return 0;
	}

	LPTHREAD_START_ROUTINE pFunc = (LPTHREAD_START_ROUTINE) GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (pFunc == NULL)
	{
		VirtualFreeEx(hHandle, pAddr, strlen(szDLLPath) + 1, MEM_RELEASE);
		CloseHandle(hHandle);
		return 0;
	}

	HANDLE hThread = CreateRemoteThread(hHandle, 0, 0, pFunc, pAddr, 0, 0);
	if (hThread == NULL || hThread == INVALID_HANDLE_VALUE)
	{
		VirtualFreeEx(hHandle, pAddr, strlen(szDLLPath) + 1, MEM_RELEASE);
		CloseHandle(hHandle);
		return 0;
	}

	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hHandle, pAddr, strlen(szDLLPath) + 1, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hHandle);

	return 1;
}

EXPORT int ReadStats(char *& text, size_t allocationSize)
{
	SERVER_CHECK(0);

	CBitStream bsIn, bsOut;

	bsIn << PipeMessages::GetStats;

	if (!CNamedPipeClient(bsIn, bsOut).Success())
		return 0;

	std::string ret;
	bsOut >> ret;

	if (ret.length() == 0)
		return 0;

	return sprintf_s(text, allocationSize, ret.c_str());
}
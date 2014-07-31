#pragma comment(lib, "detours.lib")
#pragma comment(lib,"Psapi.lib")

#include "dllmain.h"

#include <Utils/Windows.h>
#include <Game/Game.h>

HANDLE g_hDllHandle = 0;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	g_hDllHandle = hInstance;

	DisableThreadLibraryCalls((HMODULE) hInstance);

	if (dwReason != DLL_PROCESS_ATTACH)
		return FALSE;

	if (GetModuleHandle("samp.dll"))
		return CreateThread(0, 0, (LPTHREAD_START_ROUTINE) initGame, 0, 0, 0) > 0;

	return TRUE;
}
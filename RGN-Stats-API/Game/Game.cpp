#include <Utils/Windows.h>
#include <Utils/Pattern.h>
#include <Utils/NamedPipeServer.h>

#include <Utils/Pattern.h>
#include <Utils/Misc.h>

#include "Game.h"
#include "Messagehandler.h"

#include <detours.h>

#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#define BIND(T) PaketHandler[PipeMessages::T] = boost::bind(T, _1, _2);


typedef void (__thiscall *showDialogPtr)(void *, int, int, char *, char *, char *, void *, int);


showDialogPtr g_origShowDialog = NULL;

bool g_bShouldReadStats = false;

std::mutex g_synchronizationMutex;
std::condition_variable g_synchronizationCondition;
std::string g_statsText;


void __fastcall hkShowDialog(void * _ecx, void *_edx, int id, int style, char *caption, char *text, char *button, void *unk, int unk2);


void initGame()
{
	HMODULE hHandle;
	while ((hHandle = GetModuleHandle("samp.dll")) == NULL)
		Sleep(200);

	DWORD dwFunction = findPattern((DWORD) hHandle, GetModuleLength(hHandle), (BYTE *)"\x83\xEC\x08\x56\x57\x8B\x7C\x24\x14\x85\xFF\x8B\xF1\x7D\x18", 
		"xxxxxxxxxxxxxxx");

	if (dwFunction)
		g_origShowDialog = (showDialogPtr) DetourFunction((PBYTE) dwFunction, (PBYTE) hkShowDialog);

	typedef std::map<PipeMessages, boost::function<void(CBitStream&, CBitStream&)> > MessagePaketHandler;
	MessagePaketHandler PaketHandler;

	BIND(GetStats);

	new CNamedPipeServer([&](CBitStream& bsIn, CBitStream& bsOut)
	{
		BITSTREAM_READ(bsIn, PipeMessages, eMessage);

		try
		{
			auto it = PaketHandler.find(eMessage);
			if (it == PaketHandler.end())
				return;

			if (!PaketHandler[eMessage])
				return;

			PaketHandler[eMessage](bsIn, bsOut);
		}
		catch (...)
		{
		}
	});

	while (true){
		Sleep(100);
	}
}

bool sendCommand(const char *text)
{
	static BYTE *pattern = (BYTE *) "\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\xA1\x00\x00\x00\x00\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x18\x01\x00\x00";
	static char *mask = "xxxxxxxxx????xx????xxxxxxxxxxxxx";

	HMODULE hModule = GetModuleHandle("samp.dll");
	if (hModule == 0)
		return false;

	DWORD dwFunction = findPattern((DWORD) hModule, GetModuleLength(hModule), pattern, mask);
	if (dwFunction == 0)
		return false;

	__asm push text
	__asm call dwFunction

	return true;
}


void __fastcall hkShowDialog(void * _ecx, void *_edx, int id, int style, char *caption, char *text, char *button, void *unk, int unk2)
{
	if (g_bShouldReadStats && id == 500) // DialogID von den Stats ist 500
	{
		try
		{
			std::unique_lock<std::mutex> lock(g_synchronizationMutex);

			g_statsText = text;
			g_bShouldReadStats = false;

			g_synchronizationCondition.notify_all();
		}
		catch (...)
		{
			return g_origShowDialog(_ecx, id, style, caption, text, button, unk, unk2);
		}
		return;
	}

	return g_origShowDialog(_ecx, id, style, caption, text, button, unk, unk2);
}

std::string getStats()
{
	if (g_origShowDialog == 0)
		return{};

	g_bShouldReadStats = true;
	if (!sendCommand("/stats"))
		return{};

	try
	{
		std::unique_lock<std::mutex> lock(g_synchronizationMutex);

		g_synchronizationCondition.wait_for(lock, std::chrono::milliseconds(500));

		return g_statsText;
	}
	catch (...)
	{
		g_bShouldReadStats = false;
		return{};
	}
}
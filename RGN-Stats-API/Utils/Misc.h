#pragma once
#include "Windows.h"
#include <string>

DWORD GetProcIdByProcName(const std::string&);
DWORD GetProcIdByWindowName(const std::string&);

DWORD GetModuleLength(HMODULE hHandle);
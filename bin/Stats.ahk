#NoEnv 

PATH_STATS_API := RelToAbs(A_ScriptDir, "RGN-Stats-API.dll")

hModule := DllCall("LoadLibrary", Str, PATH_STATS_API)
if(hModule == -1 || hModule == 0)
{
	MsgBox, 48, Error, Die DLL konnte nicht gefunden werden!
	ExitApp
}

ReadStats_func := DllCall("GetProcAddress", UInt, hModule, Str, "ReadStats")

ReadStats(byref string)
{
	global ReadStats_func

	allocationSize := 4096
	VarSetCapacity(string, allocationSize, 0)

	res := DllCall(ReadStats_func, StrP, string, UInt, allocationSize)

	return res
}

RelToAbs(root, dir, s = "\") {
	pr := SubStr(root, 1, len := InStr(root, s, "", InStr(root, s . s) + 2) - 1)
		, root := SubStr(root, len + 1), sk := 0
	If InStr(root, s, "", 0) = StrLen(root)
		StringTrimRight, root, root, 1
	If InStr(dir, s, "", 0) = StrLen(dir)
		StringTrimRight, dir, dir, 1
	Loop, Parse, dir, %s%
	{
		If A_LoopField = ..
			StringLeft, root, root, InStr(root, s, "", 0) - 1
		Else If A_LoopField =
			root =
		Else If A_LoopField != .
			Continue
		StringReplace, dir, dir, %A_LoopField%%s%
	}
	Return, pr . root . s . dir
}
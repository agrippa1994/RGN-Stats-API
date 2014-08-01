#NoEnv 

PATH_STATS_API := RelToAbs(A_ScriptDir, "RGN-Stats-API.dll")

hModule := DllCall("LoadLibrary", Str, PATH_STATS_API)
if(hModule == -1 || hModule == 0)
{
	MsgBox, 48, Error, Die DLL konnte nicht gefunden werden!
	ExitApp
}

g_whenIsAllowedToSend := 0
g_szText := 0

ReadStats_func := DllCall("GetProcAddress", UInt, hModule, Str, "ReadStats")

ReadStats(byref string)
{
	global ReadStats_func, g_szText
	allocationSize := 4096
	
	static once := true
	if(once)
	{
		VarSetCapacity(g_szText, allocationSize, 0)
		once := false
	}
	
	res := DllCall(ReadStats_func, StrP, g_szText, UInt, allocationSize)
	string := g_szText
	
	return res
}

ReadStats_s(byref string)
{	
	global g_whenIsAllowedToSend, g_szText
	
	if(g_whenIsAllowedToSend > A_TickCount)
	{
		string := g_szText
		return true
	}
	
	return ReadStats(string)
}

keyForEntry(text, entry)
{
	RegExMatch(text, "i)" .  entry . "\:\[(.*?)\]", match)
	return match1
}

getChatlogLineCount()
{
	local i := 0
	Loop, read , %A_MyDocuments%\GTA San Andreas User Files\SAMP\chatlog.txt
		i++
	
	return i
}

AntiSpamHandler()
{
	global g_whenIsAllowedToSend
	
	static lastIndex := 0
	
	if(getChatlogLineCount() < lastIndex)
		lastIndex := 0

	Loop, read, %A_MyDocuments%\GTA San Andreas User Files\SAMP\chatlog.txt
	{
		if(A_Index > lastIndex)
		{
			lastIndex := A_Index
			
			if(InStr(A_LoopReadLine, "Antiflood:"))
			{
				if(RegExMatch(A_LoopReadLine, "i)(\d+) Sekunde", match))
				{
					g_whenIsAllowedToSend := A_TickCount + match1*1000 + 500
				}
			}
		}
	}
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

SetTimerF( Function, Period=0, ParmObject=0, dwTime=0 ) { ; Quelle: http://www.autohotkey.com/board/topic/59492-settimerf-settimer-for-functions/
 Static tmrs:=Object()
 If IsFunc( Function ) {
    if IsObject(tmrs[Function])
       ret := DllCall( "KillTimer", "UInt",0, "UInt" ,tmrs[Function,"tmr"])
       , DllCall("GlobalFree", "UInt", tmrs[Function,"CBA"])
       , ObjRelease(tmrs[Function,"REF"])
       , tmrs.remove(Function)
    if (Period = 0 || Period ? "off")
       return ret
    Period := (Period && Period!="On") ? Period : 250
    tmrs[Function]:=Object("func",Function,"OneTime",(Period<0),"params",IsObject(ParmObject)?ParmObject:Object())
    tmrs[Function,"REF"] := &tmrs[Function]
    tmrs[Function,"CBA"] := RegisterCallback(A_ThisFunc,"",4,tmrs[Function,"REF"])
    return !!tmrs[Function,"tmr"] := DllCall("SetTimer", "UInt",0, "UInt",0, "UInt",Abs(Period), "UInt",tmrs[Function,"CBA"])
 }
 tmr := Object(A_EventInfo)
 if IsObject(tmr) {
    if (tmr.OneTime)
       DllCall("KillTimer", "UInt",0, "UInt",tmr.tmr)
       , DllCall("GlobalFree", "UInt",tmr.CBA)
       , ObjRelease(tmr.REF)
       , tmr := tmrs.remove(tmr.func)
    ErrorLevel:=dwTime
    tmr.func(tmr.params*)
 }
}

SetTimerF("AntiSpamHandler", 20)
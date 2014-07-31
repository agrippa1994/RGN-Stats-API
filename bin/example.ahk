#SingleInstance, force
#include Stats.ahk
#include UDF.ahk

keyForEntry(text, entry)
{
	RegExMatch(text, "i)" .  entry . "\:\[(.*?)\]", match)
	return match1
}


#IfWinActive, GTA:SA:MP

1::
if(ReadStats(text))
{
	level := keyForEntry(text, "Level")
	kills := keyForEntry(text, "Kills\(DM\)")
	death := keyForEntry(text, "Tode")
	payday := keyForEntry(text, "Minuten seit Payday")
	bank := keyForEntry(text, "Bank")

	AddChatMessage("{ffffff} Mein Level: " . level . ", Kills: " . kills . ", Tode: " . death . ", Zeit seit Payday: " . payday . ", Bankvermögen: " . bank)
	SendChat("Meine Stats: mein Level: " . level . ", Kills: " . kills . ", Tode: " . death . ", Zeit seit Payday: " . payday . ", Bankvermögen: " . bank)
}
Else
{
	AddChatMessage("{ffffff} Stats konnten nicht gelesen werden")
}
return
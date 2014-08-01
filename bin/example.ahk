#SingleInstance, force
#include UDF.ahk
#include Stats.ahk

#IfWinActive, GTA:SA:MP

1::
if(ReadStats_s(text))
{
	level := keyForEntry(text, "Level")
	kills := keyForEntry(text, "Kills\(DM\)")
	death := keyForEntry(text, "Tode")
	payday := keyForEntry(text, "Minuten seit Payday")
	bank := keyForEntry(text, "Bank")

	AddChatMessage("{ffffff} Mein Level: " . level . ", Kills: " . kills . ", Tode: " . death . ", Zeit seit Payday: " . payday . ", Bankvermögen: " . bank)
}
Else
{
	AddChatMessage("{ffffff} Stats konnten nicht gelesen werden, da der Dialog nicht geoeffnet werden konnte, bzw. der Spam-Schutz aktiv ist!")
}
return

2::
ExitApp
return
engine_require "common/Script/ExportClass"

local ExportTbl = 
{
	{"CCharacterProperties", "character/CharacterProperties"},
	{"CNpc", "activity/npc/Npc"},
	{"CNpcProperties", "activity/npc/NpcProperties"},
	{"CBattleHorseProperties", "activity/npc/HorseServantProperties"},
	{"CIntObjProperties", "activity/int_obj/IntObjProperties"},
	{"CPlayerProperties", "player/PlayerProperties"},
	{"CConsignment", "commerce/consignment/Consignment"},

}

SetExportClass("gac_gas", ExportTbl)
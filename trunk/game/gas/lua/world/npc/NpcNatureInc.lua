CNpcNature = class()

g_Str2Nature = 
{
	["coward"] = "怯懦",
	["fear"] = "恐惧",
	["gregarious"] = "群居",
	["like"] = "喜好",
	["attract"] = "吸引",
	["abhor"] = "厌恶",
	["sleey"] = "嗜睡",
	["hide"] = "躲避",
	["greet"] = "相聚",
}



g_NatureNpcAoi = 
{
	["怯懦"] = EObjectAoiType.EAoi_NatureNpc,
	["喜好"] = EObjectAoiType.EAoi_NatureNpc,
	["吸引"] = EObjectAoiType.EAoi_NatureNpc,
	["厌恶"] = EObjectAoiType.EAoi_NatureNpc,
	["嗜睡"] = EObjectAoiType.EAoi_TaskNpc,		--不涉及到Aoi的个性Npc
	["相聚"] = EObjectAoiType.EAoi_NatureNpc,
}

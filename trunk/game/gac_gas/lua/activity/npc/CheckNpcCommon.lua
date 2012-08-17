cfg_load "npc/Npc_Common"

g_NpcTitleNameTbl = {}
AddCheckLeakFilterObj(g_NpcTitleNameTbl)

function SaveNpcTitleName()
	for _, RealName in pairs(Npc_Common:GetKeys()) do
	--for RealName, tbl in pairs(Npc_Common) do
		if Npc_Common(RealName,"Title") ~= "" then
			local title =	Npc_Common(RealName,"Title")
			if g_NpcTitleNameTbl[title] == nil then
				g_NpcTitleNameTbl[title] = {}
			end
			g_NpcTitleNameTbl[title][RealName] = true
		end
	end
end
SaveNpcTitleName()

function GetNpcTitleName(RealName)
	local Cfg = Npc_Common(RealName)
	if not Cfg then
		return RealName
	end
	if Cfg("Title") ~= "" then
		return Cfg("Title")
	end
	return RealName
end

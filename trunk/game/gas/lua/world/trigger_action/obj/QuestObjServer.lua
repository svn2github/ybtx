gac_gas_require "activity/quest/QuestBase"
gas_require "framework/main_frame/SandBoxDef"
gas_require "activity/NpcTalk"

local IfAcceptQuestArea = IfAcceptQuestArea
local g_GetDistance = g_GetDistance
local ShowFuncNpcOrObjTalkWnd = ShowFuncNpcOrObjTalkWnd

local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn,QuestObj, ObjName, GlobalID)
	--local pos = CPos:new()
	--QuestObj:GetGridPos( pos )
	local Player = g_GetPlayerInfo(Conn.m_Player.m_uID)
	if g_GetDistance(Player,QuestObj) <= 6 then
		local uEntityID	= QuestObj:GetEntityID()
		local uTongID	= QuestObj.m_TongID or 0
		ShowFuncNpcOrObjTalkWnd(Conn, uEntityID, uTongID)
	end
	return
end

return Entry

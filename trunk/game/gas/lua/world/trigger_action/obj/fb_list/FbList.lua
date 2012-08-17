--所在副本的列表
local g_TeamMgr = g_TeamMgr
local g_GetDistance = g_GetDistance
local g_GetPlayerInfo = g_GetPlayerInfo
local IsCppBound = IsCppBound
local CallAccountManualTrans = CallAccountManualTrans


local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, ServerIntObj, ObjName, GlobalID)
	local Player = Conn.m_Player
	
	if g_GetDistance(Player,ServerIntObj) > 6 then --判断距离
		return
	end
	
	local function CallBack(res)
		if res then
			for i = 1, #(res) do
				local SceneName = res[i]["SceneName"]
				local QuestName = res[i]["QuestName"]
				Gas2Gac:RetAreaFbListBegin(Conn,SceneName, QuestName)
			end
		end
		Gas2Gac:RetAreaFbListEnd(Conn)
	end

	local param = {}
	param["charId"] = Player.m_uID
	CallAccountManualTrans(Conn.m_Account, "SceneMgrDB", "GetQuestListFromScene", CallBack, param)
end

return Entry

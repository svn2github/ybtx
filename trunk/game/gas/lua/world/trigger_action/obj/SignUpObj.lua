gas_require "framework/main_frame/SandBoxDef"

local g_GetDistance = g_GetDistance
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, IntObj, ObjName, GlobalID)
	local Player = g_GetPlayerInfo(Conn.m_Player.m_uID)
	local sceneName = Player.m_Scene.m_SceneName
	if g_GetDistance(Player,IntObj) <= 6 then --ÅÐ¶Ï¾àÀë
		Player.m_SignUpId = ObjName
		Gas2Gac:RetRobResourceWnd(Conn, sceneName)
	end
	return 
end

return Entry






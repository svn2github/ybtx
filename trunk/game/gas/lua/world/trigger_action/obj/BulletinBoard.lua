gas_require "framework/main_frame/SandBoxDef"

local g_GetDistance = g_GetDistance
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, BulletinBoard, ObjName, GlobalID)
	if g_GetDistance(Conn.m_Player,BulletinBoard) <= 5 then --ÅÐ¶Ï¾àÀë
		Gas2Gac:RetObjShowContentWnd(Conn, GlobalID)
	end
end

return Entry
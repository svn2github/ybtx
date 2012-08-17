gas_require "framework/main_frame/SandBoxDef"
gas_require "world/common/CommLoadProgress"
local g_GetDistance = g_GetDistance
local StopItemProgressTick = StopItemProgressTick
local CommStopLoadProgress = CommStopLoadProgress
local EProgressBreak = EProgressBreak
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)
function Entry.Exec(Conn,MailBoxObj, ObjName, GlobalID)
	local Player=g_GetPlayerInfo(Conn.m_Player.m_uID)
	if g_GetDistance(Player,MailBoxObj)<=5 then --判断距离
		Gas2Gac:OpenMailWnd(Conn)
		--打断物品使用读条
    	if Conn.m_Player.m_UseItemLoadingTick then
    		StopItemProgressTick(Conn.m_Player)
    	end
		if Conn.m_Player.m_CommLoadProTick then
			CommStopLoadProgress(Conn.m_Player, EProgressBreak.ePB_OpenMailbox)
		end
	end
end
return Entry
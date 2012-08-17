local function Script(Arg, Trigger, Player)

	local funStr = Arg[1]
	
	if not IsCppBound(Player) then
		return
	end
	local function CallBack(res, profferValue)
		if res then
			if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
				MsgToConn(Player.m_Conn, 9448, profferValue)
			end
		end	
	end
	local ProfferFun = g_TriggerMgr:GetExpressions(funStr)
	local data = {}
	local ChannelIDList = {}
	local sceneName	= Player.m_Scene.m_SceneName
	local modifyType = 143
	local PlayerTbl = Player.m_Scene.m_tbl_Player
	for playerId, Char in pairs(PlayerTbl) do
		if Char.m_Properties:GetTongID() == Player.m_Properties:GetTongID() then
			table.insert(data, {playerId, ProfferFun(Char:CppGetLevel()), sceneName, modifyType})
			table.insert(ChannelIDList, Char.m_AccountID)
		end
	end
	CallAccountManualTrans(Player.m_Conn.m_Account, "GasTongBasicDB", "AddProfferPoint", CallBack, data, unpack(ChannelIDList))
end
  
return Script

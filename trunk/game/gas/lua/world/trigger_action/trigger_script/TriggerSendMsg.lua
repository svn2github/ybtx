local function Script(Arg, Trigger, Player)

	local MsgID = Arg[1]
	if IsNumber(MsgID) then
		if Arg[2] == "µØÍ¼" then
			MsgToConn(Trigger.m_Scene.m_CoreScene, MsgID)
		else
			if IsCppBound(Player) then
				MsgToConn(Player.m_Conn, MsgID)
			else
				local id = Trigger.m_OwnerId
				Player = g_GetPlayerInfo(id)
				if IsCppBound(Player) then
					MsgToConn(Player.m_Conn, MsgID)
				end
			end
		end
	end
end

return Script
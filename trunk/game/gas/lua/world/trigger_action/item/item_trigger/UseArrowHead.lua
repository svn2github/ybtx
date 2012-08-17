function GetArrowHeadFromItem(Conn,Type,NpcName,ShowTime)
	local Player = Conn.m_Player
	if not Player then
		return false
	end
	local NpcOrObj = nil
	if Type == "Npc" then
		NpcOrObj = GetOnlyNpc(Player.m_Scene.m_SceneId, NpcName)
	elseif Type == "Object" then
		NpcOrObj = GetOnlyIntObj(Player.m_Scene.m_SceneId, NpcName)
	elseif Type == "Player" then
		NpcOrObj = Player
		local NpcPos = CFPos:new()
		NpcOrObj:GetPixelPos(NpcPos)
		Gas2Gac:SetTickCompassHeadDir(Conn,2,NpcPos.x,NpcPos.y,ShowTime)
		return true
	else
		MsgToConn(Conn,31)
		return false
	end
	if not NpcOrObj then
		MsgToConn(Conn,830)
		return false
	end
	local DistanceNum = g_GetDistance(Player,NpcOrObj)
	if DistanceNum < 2 then
		MsgToConn(Conn,831)
		return false
	end
	
	local NpcPos = CFPos:new()
	NpcOrObj:GetPixelPos(NpcPos)
	
	Gas2Gac:SetTickCompassHeadDir(Conn,2,NpcPos.x,NpcPos.y,ShowTime)
	return true
end

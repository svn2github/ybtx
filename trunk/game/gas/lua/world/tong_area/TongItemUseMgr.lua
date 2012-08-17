CTongItemUseMgr = class()

function CTongItemUseMgr.ChangeSceneByMsg(Conn, posx, posy, sceneId, serverId)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	local Truck = Conn.m_Player:GetServantByType(ENpcType.ENpcType_Truck)
	if IsServerObjValid(Truck) then
		MsgToConn(Conn, 9071)
		return
	end
	local Pos = CPos:new()
	Pos.x, Pos.y = posx, posy
	if sceneId == Conn.m_Player.m_Scene.m_SceneId then
		Conn.m_Player:SetGridPosByTransport(Pos)
	else
		MultiServerChangeScene(Conn, sceneId, serverId, posx, posy)
	end
end

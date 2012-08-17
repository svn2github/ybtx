--管理队伍的副本场景,
--如果以后副本有保留时间, 只需加一个 玩家上线的处理 OnPlayerLogon

local function OnLeaveFbSceneTick(player)
	if IsCppBound(player ) 
		and player.m_LeaveFbScene
		and player.m_Scene.m_SceneId == player.m_LeaveFbScene["oldSceneId"] then -- 玩家通过其他方式离开了副本,不需传送了,
		local sceneName = player.m_MasterSceneName
		local pos = player.m_MasterScenePos
		ChangeSceneByName(player.m_Conn, sceneName, pos.x, pos.y)
	end
	player.m_LeaveFbScene = nil
end

--注意:请不要在其他任何位置对 scene 的 m_OwnerTeamId 属性进行修改, 
--     只会在TeamSceneMgr:AddScene 中赋值 和在TeamSceneMgr:RemoveScene 中删除
--     (OnReleaseTeam比较特殊,为了效率没有对每个scene调用RemoveScene 而是直接将m_OwnerTeamId删除)
function Gac2Gas:AgreedChangeOut(Conn)
	if Conn and Conn.m_Player and Conn.m_Player.m_LeaveFbScene then
		OnLeaveFbSceneTick (Conn.m_Player)
	end
end

--强迫玩家离开副本,传出到之前的主场景(可能以后会变).
--当参数 time 为 nil 或者 0 时候 不会给客服端发确认消息
--当 msg 为 nil 不给客服端发 Gas2Gac:RetIsChangeOut 消息
function NotifyPlayerLeaveFbScene(playerId, time, msgID)
	local  player = g_GetPlayerInfo(playerId)
	if player == nil
		or not IsCppBound(player.m_Conn)
		or player.m_Scene.m_SceneAttr.SceneType == 1 then
		return
	end
	local sceneName = player.m_MasterSceneName
	local pos = player.m_MasterScenePos
	
	if time and time > 0 then
		if player.m_LeaveFbScene  then
			return
		end
		player.m_LeaveFbScene = {}
		player.m_LeaveFbScene["oldSceneId"] = player.m_Scene.m_SceneId
		RegisterOnceTick(player,"LeaveFbSceneTick", OnLeaveFbSceneTick, time * 1000, player)
		if msgID then
			Gas2Gac:RetIsChangeOut(player.m_Conn ,time, msgID)
		end
	else
		ChangeSceneByName(player.m_Conn, sceneName, pos.x, pos.y)
	end
end

--跨服,让退出副本的消息
function Gas2GasDef:NotifyPlayerLeaveFbScene(ServerConn, playerId, time, msgID)
	NotifyPlayerLeaveFbScene(playerId, time, msgID)
end

gac_gas_require "scene_mgr/SceneCfg"
cfg_load "fb_game/FbActionDirect_Common"

local m_FbName = "佣兵团刷怪本"

function Gac2Gas:ShowMercenaryMonsterFbWnd(Conn, NpcID)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	if not CheckAllNpcFunc(Conn.m_Player, NpcID, "佣兵团刷怪本传送处" ) then
		return
	end
	
	if not CheckActionIsBeginFunc(m_FbName) then
		MsgToConn(Conn,3520)--活动还没有开放,不可以报名
		return
	end
	if SearchClosedActionTbl(m_FbName) then
		MsgToConn(Conn,3521)--活动已经关闭,不可以报名
		return
	end
	Gas2Gac:RetShowMercenaryMonsterFbWnd(Conn)
end


function ChangeToMercenaryMonsterFb(Conn, SceneName, LeaderTongId, isInviteTeammate)
	-- 得到玩家等级
	local player = Conn.m_Player
	local playerLev = player:CppGetLevel()
	-- 判断玩家等级是否低于副本要求的等级
	if playerLev < FbActionDirect_Common(SceneName, "MinLevel") then
		MsgToConn(Conn, 190012, SceneName, FbActionDirect_Common(SceneName, "MinLevel"))
		return
	end
	--判断玩家是不是佣兵团成员
	local uTongId = player.m_Properties:GetTongID()
	if uTongId == 0 then 
		MsgToConn(Conn, 191030)	
		return
	end

	--判断非队长玩家所在的佣兵团和队长所在佣兵团是否相同
	if not isInviteTeammate and uTongId ~= LeaderTongId then
		MsgToConn(Conn, 191031)
		return
	end
	
	local teamId = player.m_Properties:GetTeamID()
	local data = {}
	data["PlayerId"] = player.m_uID
	data["charLevel"] = playerLev
	data["teamId"] = teamId
	data["tongId"] = uTongId
	data["SceneName"] = SceneName
	data["InviteTeammate"] = isInviteTeammate
	
	local function CallBack(isSucceed, count, sceneId, serverId, inviteTbl)
		if not (IsCppBound(Conn) and IsCppBound(player)) then
			return
		end
		if isSucceed then
			local x = Scene_Common[SceneName].InitPosX
			local y = Scene_Common[SceneName].InitPosY
			MultiServerChangeScene(Conn, sceneId, serverId, x , y)
			for _, teammateId in pairs(inviteTbl) do
				Gas2GacById:RetIsJoinMercenaryMonsterFb(teammateId, SceneName, uTongId)
			end
		else
			if IsTable(count) then
				MsgToConn(player.m_Conn, unpack(count))
			elseif count < 0 then
				MsgToConn(player.m_Conn, 191037)
			end
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, "SceneMgrDB", "GetMercenaryMonsterFbScene", CallBack, data, teamId)
end

function Gac2Gas:EnterGeneralMercenaryMonsterFb(Conn)
	local player = Conn.m_Player
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
--	if not CheckAllNpcFunc(player, NpcID, "佣兵团刷怪本传送处" )then
--		return
--	end
	ChangeToMercenaryMonsterFb(Conn, m_FbName, nil, true)
end

function Gac2Gas:AgreedJoinMercenaryMonsterFb(Conn, SceneName, LeaderTongId)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	ChangeToMercenaryMonsterFb(Conn, SceneName, LeaderTongId, false)
end

function InviteJoinMercenaryMonsterFb(Conn, SceneName, SceneId, ServerId)
	--判断玩家是不是佣兵团成员
	local uTongId = Conn.m_Player.m_Properties:GetTongID()
	if uTongId == 0 then 
		MsgToConn(Conn, 191030)	
		return
	end
	
	--以后要加一个次数和判断
	local x = Scene_Common[SceneName].InitPosX
	local y = Scene_Common[SceneName].InitPosY
	MultiServerChangeScene(Conn, SceneId, ServerId, x , y)
end

--function InviteJoinMercenaryMonsterFb(Conn, InvitedPlayer, scene)
--	if not CheckActionIsBeginFunc(m_FbName) then
--		MsgToConn(Conn,3520)--活动还没有开放,不可以报名
--		return
--	end
--	if SearchClosedActionTbl(m_FbName) then
--		MsgToConn(Conn,3521)--活动已经关闭,不可以报名
--		return
--	end
--	
--	if not IsCppBound(InvitedPlayer) then
--		return
--	end
--	local SceneName = scene.m_SceneName
--	InvitedPlayer.IsInvited = true
--	-- 得到玩家等级
--	local PlayerLev = InvitedPlayer:CppGetLevel()
--	-- 判断玩家等级是否低于副本要求的等级
--	if playerLev < FbActionDirect_Common(SceneName, "MinLevel") then
--		MsgToConn(Conn, 190012, SceneName, FbActionDirect_Common(SceneNam, "MinLevel"))
--		return
--	end
--	--判断玩家是不是佣兵团成员
--	local tongId = Player.m_Properties:GetTongID()
--	if tongId == 0 then 
--		MsgToConn(Conn, 191030)	
--		return
--	end
--	
--	if InvitedPlayer.m_Scene.m_SceneId == scene.m_SceneId then
--		local PlayerName = InvitedPlayer.m_Properties:GetCharName()
--		MsgToConn(Conn, 10008, PlayerName)
--		return
--	end
--	
--	ChangeToMercenaryMonsterFb(InvitedPlayer.m_Conn, SceneName, IsHard, false, Conn.m_Player)
--end

function IntoMercenaryMonsterFbScene(player)
	local scene = player.m_Scene
	if not scene.m_EnteredCharTbl then
		scene.m_EnteredCharTbl = {}
	end
	if not scene.m_EnteredCharTbl[player.m_uID] then
		scene.m_EnteredCharTbl[player.m_uID] = true		
		local limitType = FbActionDirect_Common("佣兵团刷怪本", "MaxTimes")
		local parameters = {}
		parameters["PlayerId"] = player.m_uID
		parameters["ActivityName"] = limitType
		parameters["SceneId"] = scene.m_SceneId
		CallAccountAutoTrans(player.m_Conn.m_Account, "SceneMgrDB", "EnterMercenaryMonsterFbScene", nil, parameters)
	end
end

function LeaveMercenaryMonsterFbScene(Player)
	--清除临时技能
	Player:RemoveAllTempSkill()
end

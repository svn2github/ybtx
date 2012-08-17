gas_require "world/tong_area/Challenge"
gas_require "log_mgr/CLogMgr"

local function Script(Arg, Trigger, Player)

	local TongId = Trigger.m_TongID
	local scene = Trigger.m_Scene
	scene.m_NotCanEnter = true
	scene.m_ChlgIsOver = true
	if scene.m_SceneName ~= "帮会挑战本" then
		return
	end
	
	scene.m_ChlgerGameCamp = GAME_CAMP_PEACE
	scene.m_DfnderGameCamp = GAME_CAMP_PEACE
	UpdateAllGameCamp(scene)
	
	local HonorChlgerAdd, HonorTgtAdd
	
	local challengerTongId = scene.m_ChlgerTongId
	local targetWarZoneId = scene.m_TgtWzId
	local targetStationId = scene.m_TgtWzsId
	local targetTongId = scene.m_TgtTongId
	if TongId == targetTongId then
		local sceneName = "花语平原" .. targetWarZoneId
		local sceneId,serverId = g_SceneMgr:GetSceneByName(sceneName)
		local playerId = Player and Player.m_uID or 0
		Gas2GasAutoCall:ExchangeStation(GetServerAutoConn(serverId), playerId, challengerTongId, targetWarZoneId, targetStationId)
		scene.m_ChlgKillBuildNum = scene.m_ChlgKillBuildNum + 1
	end
	
	if TongId == challengerTongId then
		scene.m_TgtKillBuildNum = scene.m_TgtKillBuildNum + 1
	end
	
	local ChlgKillBuildNum = scene.m_ChlgKillBuildNum
	local TgtKillBuildNum = scene.m_TgtKillBuildNum
	local ChlgKillPersonNum = scene.m_ChlgKillPersonNum
	local TgtKillPersonNum = scene.m_TgtKillPersonNum
	
	local HaveNotify = {}
	local function CallBack(Flag, ChlgTongName, TgtTongName)
		if Flag then
			for playerId, player in pairs(scene.m_tbl_Player) do
				if not HaveNotify[playerId] then
					HaveNotify[playerId] = 0
					local uTongID = player.m_Properties:GetTongID()
					if uTongID ~= 0 then
						if TongId == targetTongId then
							Gas2Gac:ShowTongChallengeStatisticsWnd(player.m_Conn, ChlgTongName, ChlgKillBuildNum, ChlgKillPersonNum, 9, TgtTongName, TgtKillBuildNum, TgtKillPersonNum, 3)
						elseif TongId == challengerTongId then
							Gas2Gac:ShowTongChallengeStatisticsWnd(player.m_Conn, TgtTongName, TgtKillBuildNum, TgtKillPersonNum, 9, ChlgTongName, ChlgKillBuildNum, ChlgKillPersonNum, 3)
						end
					end
				end
			end
		end
	end
	
	local parameter = {}
	parameter["ChlgerTongId"] = challengerTongId
	parameter["TgtTongId"] = targetTongId
	CallDbTrans("GasTongBasicDB", "GetChlgerNameAndTgtName", CallBack, parameter, challengerTongId, targetTongId)
	
	if TongId == targetTongId then
		HonorChlgerAdd = 9
		HonorTgtAdd = 3
	elseif TongId == challengerTongId then
		HonorChlgerAdd = 3
		HonorTgtAdd = 9
	end
	local parameter = {}
	parameter["uChlgerTongId"] = challengerTongId
	parameter["uChlgerHonor"] = HonorChlgerAdd
	parameter["uTgtTongId"] = targetTongId
	parameter["uTgtHonor"] = HonorTgtAdd
	parameter["uState"] = 4
	parameter["uServerId"] = g_CurServerId
	CallDbTrans("GasTongWarDB", "UpdateHonorAndBattleState", nil, parameter, challengerTongId, targetTongId)
	
	if TongId == targetTongId then
		local function CallBack(flag, result)
			if flag then
				local resTbl = result:GetTableSet()
				for i = 1, result:GetRowNum() do
					local nPlayerId = resTbl(i, 1)
					local nServerId = resTbl(i, 2)
					if nServerId == g_CurServerId then
						local Player = g_GetPlayerInfo(nPlayerId)
						if Player and Player.m_Conn then
							SysMovementNotifyToClient(Player.m_Conn, 1003)
							MsgToConn(Player.m_Conn, 9436)
						end
					else
						Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1003)
					end
				end
			end
		end
		
		CallDbTrans("GasTongWarDB", "AddWinLogAndSendMsg", CallBack, {["uTongID"] = challengerTongId}, challengerTongId)
	end
	
	--写LOG
	local uResult = (TongId == targetTongId) and 1 or 0
	TongChallengeEndLog(challengerTongId,targetTongId,targetWarZoneId,targetStationId,uResult)
end

return Script
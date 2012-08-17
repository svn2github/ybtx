cfg_load "fb_game/JuQingTransmit_Common"
cfg_load "fb_game/FbActionDirect_Common"

CScenarioExploration = class()

local WaitTime = 30
local g_ScenarioMgr = {}

function CScenarioExploration.EnterScene(Conn, ActionName, IsAutoTeam)
	if IsCppBound(Conn.m_Player) then
		if JuQingTransmit_Common(ActionName) then
			local SceneName = JuQingTransmit_Common(ActionName, "SceneName")
			local SceneCommon = Scene_Common[SceneName]
			if SceneCommon and SceneCommon.SceneType == 19 then
				MultiServerJoinAction(Conn, ActionName, IsAutoTeam)
			end
		end
	end
end

local function DeleteScene(SceneId)
	local Scene = g_SceneMgr:GetScene(SceneId)
	if not Scene then
		return
	end
	
	Scene.m_State = g_FbRoomState.GameEnd
	
	if Scene:IsHavePlayer() then
		local WarnInfo = GetCfgTransformValue(true, "FbActionDirect_Common", "剧情探险", "WarnValue")
		--print("剧情探险", Scene.m_matchTeam.m_TempTeamId)
		if (CWarner.GMAllOpen ~= 0 or (WarnInfo and WarnInfo[1])) and Scene.m_matchTeam.m_TempTeamId then
			local WarnTeamTbl = {}
			WarnTeamTbl[ Scene.m_matchTeam.m_TempTeamId ] = Scene.m_PlayerInfo
			CreateWarner(Scene.m_SceneId, WarnInfo, 0, WarnTeamTbl)
		end
		return
	end
	
	Scene:Destroy()
end

function CScenarioExploration.Destroy(Scene)
	g_ScenarioMgr[Scene.m_RoomId] = nil
end

function CScenarioExploration.CreateRoom(Scene, roomId, actionName, roomMembers, captainsTbl)
	Scene.m_State = g_FbRoomState.PlayerCanIn
	
	g_ScenarioMgr[roomId] = {}
	g_ScenarioMgr[roomId].m_SceneId = Scene.m_SceneId
	
	Scene.m_RoomId = roomId
	Scene.m_ActionName = actionName
	Scene.m_PlayerInfo = {}
	
	local NpcLevel = Scene.m_SceneBaseLv or 1
	
	for teamId, v in pairs(roomMembers) do
		local matchTeam = {}
		local captain = captainsTbl[teamId]
		if string.sub(captain, 1, 6) == "{Auto}" then
			matchTeam.m_TempTeamId = g_TemporaryTeamMgr:CreateTeam()
			captain = string.sub(captain, 7, -1)
			matchTeam.m_CaptainName = captain
		end
		Scene.m_matchTeam = matchTeam
		
		for _, charId in pairs(v) do
			Scene.m_PlayerInfo[charId] = {}
			Scene.m_PlayerInfo[charId].m_IsInScene = false
			Gas2GacById:RetIsJoinFbActionAffirm(charId, actionName, WaitTime-10, NpcLevel+10)
		end
	end
	
	RegisterOnceTick(Scene, "ScenarioClearTick", DeleteScene, 1000 * WaitTime, Scene.m_SceneId)
end

function CScenarioExploration.GetTeamEnterPos(roomId, ActionName)
	local SceneName = JuQingTransmit_Common(ActionName, "SceneName")
	local SceneCommon = Scene_Common[SceneName]
	local pos = {SceneCommon.InitPosX, SceneCommon.InitPosY}
	
	if g_ScenarioMgr[roomId] then
		return g_ScenarioMgr[roomId].m_SceneId,pos
	end
end

local function ChangeSceneLeave(Player, roomId)
	local Scene = Player.m_Scene 
	local PlayerInfo = Scene.m_PlayerInfo
	if g_ScenarioMgr[roomId] and g_ScenarioMgr[roomId].m_SceneId == Scene.m_SceneId
		and PlayerInfo and PlayerInfo[Player.m_uID] and PlayerInfo[Player.m_uID].m_IsInScene then
			--说明是在副本中
			
			Scene.m_PlayerInfo[Player.m_uID] = nil

			local sceneName = Player.m_MasterSceneName
			local pos = Player.m_MasterScenePos 
			Player:SetGameCamp(0)
			ChangeSceneByName(Player.m_Conn,sceneName,pos.x,pos.y)
			
	end
end

function CScenarioExploration.ExitGame(roomId, charId)
	local player = g_GetPlayerInfo(charId)
	if IsCppBound(player) then
		ChangeSceneLeave(player, roomId)
	end
end

local function StopCameraPos(Tick, Player)
	if Player.m_ScenarioTick then
		UnRegisterTick(Player.m_ScenarioTick)
		Player.m_ScenarioTick = nil
	end
	if Player.m_Storing then
		Player.m_Storing = nil
		if IsCppBound(Player) then
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, false)
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, false)
			Player:DisableDirectorMoving(false)
			Gas2Gac:RetChangeCameraPos(Player.m_Conn, "", false)
		end
	end
end

local function SetCameraPos(Player)
	local ActionName = Player.m_Scene.m_ActionName
	local RunTime = JuQingTransmit_Common(ActionName,"RunTime")
	local Height = JuQingTransmit_Common(ActionName,"CameraHeight")
	if RunTime and IsNumber(RunTime) and RunTime ~= 0 then 
		if Height and IsNumber(Height) and (Height >= 20 and Height <= 40)then
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, true)
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, true)
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, true)
			Player:DisableDirectorMoving(true)
			Gas2Gac:RetChangeCameraPos(Player.m_Conn, ActionName, true)
			Player.m_Storing = true
			Player.m_ScenarioTick = RegisterTick("ScenarioTick",StopCameraPos,RunTime*1000,Player)
		end
	end
end

function CScenarioExploration.CancelChangeCameraPos(Conn)
	if Conn.m_Player then
		StopCameraPos(nil, Conn.m_Player)
	end
end

function CScenarioExploration.IntoScene(Player)
	MultiServerCancelAllAction(Player.m_uID)
	
	local Scene = Player.m_Scene
	Scene.m_PlayerInfo[Player.m_uID].m_IsInScene = true
	Player:SetGameCamp(1)
	
	Gas2Gac:RetSetJoinBtnEnable(Player.m_Conn, Scene.m_ActionName)
	
	if Scene.m_matchTeam.m_TempTeamId then
		g_TemporaryTeamMgr:AddMember(Player.m_uID, Scene.m_matchTeam.m_TempTeamId)
		local Name = Player.m_Properties:GetCharName()
		if Name == Scene.m_matchTeam.m_CaptainName then
			g_TemporaryTeamMgr:SetCaptain(Player.m_uID,Scene.m_matchTeam.m_TempTeamId)
		end
	end
	
	--调整镜头的距离
	SetCameraPos(Player)
	
	--进剧情本后,加一次计数
	local function CallBack(result, ActionName, Type, IsTimes)
		if result and IsCppBound(Player) then
			local Level = Player:CppGetLevel()
			local MaxTimes = JoinCountLimit_Server(ActionName, "Count")
			if Level < 30 then
				MaxTimes = 2
			elseif Level >= 30 and Level < 40 then
				MaxTimes = 4
			elseif Level >= 40 and Level < 50 then
				MaxTimes = 6
			elseif Level >= 50 and Level < 60 then
				MaxTimes = 8
			else
				MaxTimes = 10
			end
			MsgToConn(Player.m_Conn, 3512, MaxTimes, IsTimes+1)
		end
	end
	
	if not Scene.m_EnteredCharTbl then
		Scene.m_EnteredCharTbl = {}
	end
	if not Scene.m_EnteredCharTbl[Player.m_uID] then
		Scene.m_EnteredCharTbl[Player.m_uID] = true
		local parameters = {}
		parameters["ActionName"] = Scene.m_ActionName
		parameters["PlayerId"] = Player.m_uID
		parameters["ActivityName"] = "剧情本限次"
		parameters["Type"] = 1
		CallAccountManualTrans(Player.m_Conn.m_Account, "SceneMgrDB", "EnterDareQuestFbScene", CallBack, parameters)
	end
end

function CScenarioExploration.LeaveScene(Player, mustDelDb)
	local Scene = Player.m_Scene
	local DPSnum1 = Player:GetStatisticFightHurt("")
	local DPSnum2 = Player:GetStatisticFightHeal("")
	Player:EndStatistic("")
	Player:SetGameCamp(0)
	
	--恢复镜头的位置
	StopCameraPos(nil, Player)
		
	if mustDelDb then
		local data = {}
		data["CharId"] = Player.m_uID
		data["RoomId"] = Scene.m_RoomId
		CallDbTrans("JoinActionDB", "CancelActionByRoom", nil, data, Player.m_uID, Scene.m_ActionName)
	end
	
	Gas2GacById:RetDelQueueFbAction(Player.m_uID, Scene.m_ActionName, true)
	
	if not Scene.m_PlayerDPS then
		Scene.m_PlayerDPS = {}
	end
	if not Scene.m_PlayerDPS[Player.m_uID] then
		Scene.m_PlayerDPS[Player.m_uID] = {["Hurt"]=0,["Heal"]=0}
	end
	
	Scene.m_PlayerDPS[Player.m_uID]["Hurt"] = Scene.m_PlayerDPS[Player.m_uID]["Hurt"] + DPSnum1
	Scene.m_PlayerDPS[Player.m_uID]["Heal"] = Scene.m_PlayerDPS[Player.m_uID]["Heal"] + DPSnum2
	
	if Scene.m_matchTeam.m_TempTeamId then
		g_TemporaryTeamMgr:DelMember(Player.m_uID, Scene.m_matchTeam.m_TempTeamId)
	end
end

function CScenarioExploration.LogIn(Player)
	local Scene = Player.m_Scene
	Player:SetGameCamp(1)
	Gas2Gac:RetInitFbActionQueueWnd(Player.m_Conn, Scene.m_ActionName)
	
	if Scene.m_matchTeam.m_TempTeamId then
		g_TemporaryTeamMgr:AddMember(Player.m_uID, Scene.m_matchTeam.m_TempTeamId)
		local Name = Player.m_Properties:GetCharName()
		if Name == Scene.m_matchTeam.m_CaptainName then
			g_TemporaryTeamMgr:SetCaptain(Player.m_uID,Scene.m_matchTeam.m_TempTeamId)
		end
	end
end

function CScenarioExploration.CheckIsInScene(Scene, charId)
	if Scene.m_PlayerInfo	and Scene.m_PlayerInfo[charId] then 
		return true
	end
end


--技能系数公式为：(1500-(150-X)*9.675)/1500  X为等级
--积分的奖励公式为：【队伍技能系数之和】×100×DPS输出比例
--修改为:  个人技能系数*30+【队伍技能系数之和】×70×DPS输出比例
local function GetScopesPoint(DPSInfo)
	local countPoint = 0
	local skillCofeCount = 0
	for i=1, #(DPSInfo) do
		countPoint = countPoint + DPSInfo[i][3] + DPSInfo[i][4]
		skillCofeCount = skillCofeCount + (1500-(150-DPSInfo[i][2])*9.675)/1500
	end
	
	local PointTbl = {}
	if countPoint > 0 then
		
		for i=1, #(DPSInfo) do
			local DPSnum = DPSInfo[i][3] + DPSInfo[i][4]
			local Cofe = math.floor(DPSnum / countPoint * 1000) / 1000
			
			
			--local Point = math.floor(skillCofeCount * 100 * Cofe)
			local Point = (1500-(150-DPSInfo[i][2])*9.675)/1500 * 30 + skillCofeCount * 70 * Cofe
			Point = math.floor(Point)
			
			Point = Point * DPSInfo[i][5]			--有双倍积分卷轴的,就给双倍积分
			
			--print(DPSInfo[i][1],DPSInfo[i][3], Cofe, Point)
			if Point > 0 then
				PointTbl[DPSInfo[i][1]] = {["Hurt"]=DPSInfo[i][3],["Heal"]=DPSInfo[i][4],["Cofe"]=Cofe * 1000, ["Point"]=Point, ["Rate"]=DPSInfo[i][5]}
			end
		end
		
	end
	return PointTbl
end

--在普通领域本中杀死boss,调出DPS输出窗口并给分
function CScenarioExploration.OutPutDPS(Scene, NpcName)
	if Scene.m_SceneAttr.SceneType == 19 then
		local BossName = nil
		local keys = JuQingTransmit_Common:GetKeys()
		for i=1, #(keys) do
			local iSceneName = JuQingTransmit_Common(keys[i], "SceneName")
			if Scene.m_SceneName == iSceneName then
				BossName = JuQingTransmit_Common(keys[i], "BossName")
				break
			end
		end
		
		if BossName and BossName == NpcName then
			
			local DPSInfo = {}
			local ChannelIDList = {}
			if Scene.m_tbl_Player then
				for id,Target in pairs(Scene.m_tbl_Player) do
					if IsCppBound(Target) then
					--	Target:EndStatistic()
						local DPSnum1 = Target:GetStatisticFightHurt("")
						local DPSnum2 = Target:GetStatisticFightHeal("")
						if Scene.m_PlayerDPS and Scene.m_PlayerDPS[id] then
							DPSnum1 = DPSnum1 + Scene.m_PlayerDPS[id]["Hurt"]
							DPSnum2 = DPSnum2 + Scene.m_PlayerDPS[id]["Heal"]
						end
						
						DPSnum2 = DPSnum2 * 2--治疗加倍
						
						local CofePoint = 1
						if Target:ExistState("双倍积分符buff") then
							Target:PlayerDoFightSkillWithoutLevel("撤销双倍积分符buff")
							CofePoint = 2
						end
						
						table.insert(DPSInfo, {id, Target:CppGetLevel(), DPSnum1, DPSnum2, CofePoint})
						
						table.insert(ChannelIDList, Target.m_AccountID)
					end
				end
			end
			
			local DPSInfo = GetScopesPoint(DPSInfo)
			if not next(DPSInfo) then
				return
			end
			
			local function CallBack(result, NameTbl)
				if result then
					for id, Info in pairs(DPSInfo) do
						local player = g_GetPlayerInfo(id)
						if IsCppBound(player) and NameTbl[id][2] > 0 then
							Gas2Gac:UpdatePlayerPoint(player.m_Conn, 5, NameTbl[id][2])
						end
						Gas2Gac:SendScopesExplorationDPS(Scene.m_CoreScene,NameTbl[id][1],Info["Hurt"],Info["Heal"],Info["Cofe"],NameTbl[id][2],Info["Rate"])
					end
					Gas2Gac:SendScopesExplorationDPSEnd(Scene.m_CoreScene)
				end
			end
			local data = {}
			data["SceneName"] = Scene.m_SceneName
			data["PlayerInfo"] = DPSInfo
			data["NpcName"] = NpcName
			
			CallDbTrans("JuQingFbDB", "AddPointForPlayerTbl", CallBack, data, unpack(ChannelIDList))
		end
	end
end

function CScenarioExploration.ShowJuQingWnd(Conn, EntityID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local PlayerId = Conn.m_Player.m_uID
	if not CheckAllNpcFunc(Conn.m_Player,EntityID,"剧情探险报名处") then
		return
	end
	
	local function CallBack(res, ActionTbl)
		if res and IsCppBound(Conn) then
			for i=1, #(ActionTbl) do
				Gas2Gac:SendScenarioFinishInfo(Conn, ActionTbl[i])
			end
			Gas2Gac:RetShowJuQingWnd(Conn, EntityID)
		end
	end
	
	local param = {}
	param["PlayerId"] = PlayerId
	CallAccountManualTrans(Conn.m_Account, "JuQingFbDB", "GiveFinishAction", CallBack, param)
end

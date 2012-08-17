CLevelUpGas = class()

local FirstTimeSendMsgTbl = {
			[2] = {"等级2",3002},
			[4] = {"等级4",3004},
			[5] = {"等级5",3005},
			[6] = {"等级6",3006},
			[8] = {"等级8",3008},
			[10] = {"等级10",3010},
			[15] = {"等级15",3015},
			[20] = {"等级20",3020},
			[30] = {"等级30",3030},
			[40] = {"等级40",3040},
			}

local function SendPlayerFirstInfo(Player,Level)
	local MsgType = FirstTimeSendMsgTbl[Level]
	if MsgType then
		Player:IsFirstTimeAndSendMsg(MsgType[1], MsgType[2], sMsg)
	end
end


--跟等级需求和完成任务的最高等级有关的操作
local function AddVarNumForLevelQuest(Player, uLevel)
	if not (IsCppBound(Player) and IsCppBound(Player.m_Conn)) then
		return
	end
	
	local sVarName = "等级"..uLevel
	if g_VarQuestMgr[sVarName] == nil then
		sVarName = nil
	end
	
	local function CallBack(LevelQuestInfo)
		if LevelQuestInfo["PlayerAddQuestInfo"] then
			for PlayerId, QuestTbl in pairs(LevelQuestInfo["PlayerAddQuestInfo"]) do 
				local num = table.getn(QuestTbl)
				for i = 1, num do
					local Player = g_GetPlayerInfo(PlayerId)
					if IsCppBound(Player) then
						local res = QuestTbl[i]
						Gas2Gac:RetAddQuestVar(Player.m_Conn, res[1],res[2],res[3])
					end
				end
			end
		end
		
		if LevelQuestInfo["PlayerFailQuestInfo"] and IsCppBound(Player) then
			for i=1, #(LevelQuestInfo["PlayerFailQuestInfo"]) do
				local succ = LevelQuestInfo["PlayerFailQuestInfo"][i][1]
				local QuestName = LevelQuestInfo["PlayerFailQuestInfo"][i][2]
				if succ then
					CRoleQuest.QuestFailure_DB_Ret(Player,QuestName)
				end
			end
		end
	end
	
	--让超过任务等级的所有任务失败
	local data = {}
	data["iNum"] = 1
	data["sVarName"] = sVarName
	data["char_id"] = Player.m_uID
	data["Level"] = uLevel
	data["CanShareTeamMateTbl"] = {}
	
	local ChannelIDList = {}
	local TeamID = Player.m_Properties:GetTeamID()
	if TeamID and TeamID ~= 0 then
		local SceneName = Player.m_Scene.m_SceneName
		local tblMembers = g_TeamMgr:GetMembers(TeamID)
		for i = 1, #(tblMembers) do
			local Member = g_GetPlayerInfo(tblMembers[i])
			if tblMembers[i] ~= Player.m_uID 
				and Member 
				and SceneName == Member.m_Scene.m_SceneName
				and g_AreaMgr:CheckIsInSameArea(SceneName,Player,Member) then
					table.insert(ChannelIDList, Member.m_AccountID)
					table.insert(data["CanShareTeamMateTbl"],tblMembers[i])
			end
		end
	end
	
	CallAccountAutoTrans(Player.m_Conn.m_Account, "RoleQuestDB","AddVarNumForLevelQuests", CallBack, data,unpack(ChannelIDList))
end

local function ClearActionJoinCount(Player, ulevel)
	local Conn = Player.m_Conn 
	if IsCppBound(Conn) then
		if(ulevel == 15) then
			local data = {}
			data.CharId = Player.m_uID
			data.ActivityName = "佣兵训练活动"
--			CallAccountAutoTrans(Conn.m_Account, "ActivityCountDB", "ClearActionJoinCount", nil, data)
			CallDbTrans("ActivityCountDB", "ClearCurCycAndHistoryCount", nil, data, Player.m_uID)
		end
	end
end

local function SetPlayerPkState(Player, ulevel)
	if string.find(Player.m_AreaName, "前线") and not string.find(Player.m_AreaName,"前线山区") then
		if ulevel > 40 then
			Player:CppSetPKState(true)
		else
			Player:CppSetPKState(false)
		end
		Gas2Gac:TransferSwitchState(Player.m_Conn,Player.m_SwitchState)
		Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
		Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
	end
end

function CLevelUpGas.SetLevel(Player,ulevel)
	Player:CppSetLevel(ulevel)
	SendPlayerFirstInfo(Player,ulevel)
	SetPlayerPkState(Player, ulevel)
	CRoleQuest.PlayerSendTempQuestList(Player)
	--加任务计数和让一些超过等级的任务失败
	AddVarNumForLevelQuest(Player, ulevel)
	--更新佣兵等级追踪窗口
	--AddLevelUpdateMercenaryLevelTraceWnd(Player)
	
	-- 玩家升至15级时，重置修行塔计数
	ClearActionJoinCount(Player, ulevel)
	
	-- 升级后，记录升级的时间
	local data = {}
	data.CharID = Player.m_uID
	data.LevelNum = Player:CppGetLevel()
	--角色升级刷新战斗力
	CGasFightingEvaluation.UpdateFightingEvaluationInfo(Player)
	CallAccountAutoTrans(Player.m_Conn.m_Account, "GasRoleLevelUpAttentionDB","SavePlayerLevelUpTime", nil, data)
end

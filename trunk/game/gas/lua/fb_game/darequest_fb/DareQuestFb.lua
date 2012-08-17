CDareQuestFb = class()


local function FbChangeScene(Player, SceneName, x, y, IsSendMsg, QuestName)
	local function CallBack(isSucceed, sceneId, serverId, inviteTbl)
		if not IsCppBound(Player) then
			return
		end
		if isSucceed then
			MultiServerChangeScene(Player.m_Conn, sceneId, serverId, x , y)
			for _, teammateId in pairs(inviteTbl) do
				Gas2GacById:RetShowTakeDareQuest(teammateId, QuestName,SceneName)
			end
		end
	end
	
	local teamId = Player.m_Properties:GetTeamID()
	local data = {}
	data["charId"] = Player.m_uID
	data["teamId"] = teamId
	data["SceneName"] = SceneName
	data["QuestName"] = QuestName
	data["InviteTeammate"] = IsSendMsg
	
	CallAccountManualTrans(Player.m_Conn.m_Account, "SceneMgrDB", "GetDareQuestFbScene", CallBack, data)
end

function CDareQuestFb:UseItemJoinFb(Player, SceneName, QuestName)
	if not IsCppBound(Player) then
		return
	end
	local SceneCommon = Scene_Common[SceneName]
	if SceneCommon and SceneCommon.SceneType == 16 then
		local x  = SceneCommon.InitPosX
		local y = SceneCommon.InitPosY
		FbChangeScene(Player, SceneName, x, y, true, QuestName)
	end
end

function CDareQuestFb.AgreedJoinDareFb(Conn, SceneName, QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local SceneCommon = Scene_Common[SceneName]
	
	local function CallBack(IsSatisfy)
		if IsCppBound(Player) then
			if IsSatisfy then
				local x  = SceneCommon.InitPosX
				local y = SceneCommon.InitPosY
				FbChangeScene(Player, SceneName, x, y, false, QuestName)
			else
				--提示,没有任务,或者是任务已经完成,不可以进
				MsgToConn(Player.m_Conn,3051,QuestName)
			end
		end
	end
	
	if SceneCommon and SceneCommon.SceneType == 16 then
		local param = {}
		param["sQuestName"] = QuestName
		param["uCharId"] = Player.m_uID
		CallAccountManualTrans(Conn.m_Account, "RoleQuestDB","CheckQuestSatisfy", CallBack, param)
	end
end

function CDareQuestFb.LeaveDareQuestFbScene(Player)
	if IsCppBound(Player) then
		Gas2Gac:RetCloseDownTimeWnd(Player.m_Conn)
	end
end

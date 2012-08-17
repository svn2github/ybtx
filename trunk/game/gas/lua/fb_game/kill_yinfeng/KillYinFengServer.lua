CKillYinFengServer = class()

local m_FbName = "杀吟风"--GetStaticTextServer(9403)--杀吟风
local m_QuestName = "莱斯林克的祝福"


local m_FbRunTime = 2*60*60 --单位秒


--=====================================================
--使用物品第一次进入场景
function CKillYinFengServer:ChangeSceneFb(Player, SceneName, x, y)
	local PlayerId = Player.m_uID
	
	local function CallBack(IsSucceed, SceneId, ServerId)
		if not IsCppBound(Player) then
			return
		end
		if IsSucceed then
			MultiServerChangeScene(Player.m_Conn, SceneId, ServerId, x , y)
		end
	end
	
	local ChannelIDList = {}
	local TeamID = Player.m_Properties:GetTeamID()
	if TeamID and TeamID ~= 0 then
		local tblMembers = g_TeamMgr:GetMembers(TeamID)
		for i = 1, #(tblMembers) do
			table.insert(ChannelIDList,tblMembers[i])
		end
	end
	
	local data = {}
	data["charId"] = PlayerId
	data["SceneName"] = SceneName
	data["teamId"] = TeamID
	CallAccountManualTrans(Player.m_Conn.m_Account, "SceneMgrDB", "GetKillYFScene", CallBack, data, unpack(ChannelIDList))
end
--===================================================

local function TimeOver(data)
	local SceneId = data["SceneID"]
	local QuestName = m_QuestName
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerIdTbl"] = {}
	
	local ChannelIDList = {}
	
	--看那些人还在副本中
	local Scene = g_SceneMgr:GetScene(SceneId)
	if Scene.m_tbl_Player then
		for id,Target in pairs(Scene.m_tbl_Player) do
			table.insert(ChannelIDList, Target.m_AccountID)
			table.insert(parameter["PlayerIdTbl"],id)
		end
	end
	
	local function CallBack(result)
		for i=1,#(result) do
			local id,IsSucc = result[i][1], result[i][2]
			local Target = g_GetPlayerInfo(id)
			if IsCppBound(Target) then
				if IsSucc then
					CRoleQuest.QuestFailure_DB_Ret(Target,QuestName)
				end
				Gas2Gac:RetCloseDownTimeWnd(Target.m_Conn)
				NotifyPlayerLeaveFbScene(id, 30, 8802)-- "时间结束.")
			end
		end
	end
	
	if not next(ChannelIDList) then
		ChannelIDList[1] = 1
	end
	CallDbTrans("RoleQuestDB", "AllPlayerQuestFail", CallBack, parameter, unpack(ChannelIDList))
end

--杀死指定NPC后,开始倒计时
function CKillYinFengServer.KillNpc(Player, NpcName)
	local PlayerId = Player.m_uID
	local Scene = Player.m_Scene
	local SceneType = Scene.m_SceneAttr.SceneType
	if SceneType == 15 then--特殊副本(吟风)
		Scene.m_YfTimeMgr = {}
		Scene.m_YfTimeMgr.m_StarTime = os.time()
		
		local data = {}
		data["SceneID"] = Scene.m_SceneId
		RegisterOnceTick(Scene, "KillYinFengTick", TimeOver, m_FbRunTime *1000, data)
		
		local TeamId = Player.m_Properties:GetTeamID()
		if TeamId ~= 0 and TeamId then
			local teamMember = g_TeamMgr:GetMembers(TeamId)
			for i = 1, #(teamMember) do
				local id = teamMember[i]
				if PlayerId ~= id then
					local Target = g_GetPlayerInfo(id)
					if IsCppBound(Target) 
						and Target.m_Scene.m_SceneId == Scene.m_SceneId then
						Gas2Gac:RetShowDownTimeWnd(Target.m_Conn, m_FbRunTime)
					end
				end
			end
		end
		
		Gas2Gac:RetShowDownTimeWnd(Player.m_Conn, m_FbRunTime)
	end
end

--玩家登录时,的判断部分
function CKillYinFengServer.PlayerLoginYF_Fb(Player)
	local	Scene = Player.m_Scene
	local PlayerId = Player.m_uID
	
	Gas2Gac:RetInitFbActionQueueWnd(Player.m_Conn,m_FbName)
	
	local ResidualTime = os.time() - Scene.m_YfTimeMgr.m_StarTime
	
	if m_FbRunTime > ResidualTime then
		Gas2Gac:RetShowDownTimeWnd(Player.m_Conn, m_FbRunTime - ResidualTime)
	else
		NotifyPlayerLeaveFbScene(PlayerId, 30, 8803)--"该副本已经不属于你.")
	end
end

--玩家进入副本
function CKillYinFengServer.IntoKillYinFengFb(Player)
	local Scene = Player.m_Scene
	if not Scene.m_YfTimeMgr then
		Gas2Gac:RetInitFbActionQueueWnd(Player.m_Conn,m_FbName)
		CKillYinFengServer.KillNpc(Player)
	else
		CKillYinFengServer.PlayerLoginYF_Fb(Player)
	end
end

--玩家离开副本
function CKillYinFengServer.LeaveKillYinFengFb(Player)
	Gas2Gac:RetDelQueueFbAction(Player.m_Conn, m_FbName, true)
	Gas2Gac:RetCloseDownTimeWnd(Player.m_Conn)
end


--玩家在副本中掉线
function CKillYinFengServer.PlayerOffLineYF_Fb(Player)
	if not Player:CppIsLive() then
--		local PixelPos = CFPos:new()
--		local PosX = Player.m_Scene.m_SceneAttr.InitPosX
--		local PosY = Player.m_Scene.m_SceneAttr.InitPosY
--		PixelPos.x = PosX * EUnits.eGridSpanForObj
--		PixelPos.y = PosY *EUnits.eGridSpanForObj
--		Player:SetPixelPos(PixelPos)
	end
end

--玩家放弃比赛
function CKillYinFengServer.ExitKillYinFengFb(Conn)
	local Player = Conn.m_Player
	local SceneName = Player.m_MasterSceneName
	local Pos = Player.m_MasterScenePos
	ChangeSceneByName(Conn,SceneName,Pos.x,Pos.y)
end

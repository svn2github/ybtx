cfg_load "liveskill/OreAreaTriggerEvent_Common"
cfg_load "liveskill/OreMapPosition_Common"
gac_gas_require "scene_mgr/SceneCfg"
gas_require "world/npc/OreAreaNpc"
gas_require "world/scene_mgr/SceneMgr"


--给组队成员发送消息 下一层副本或退出 flag标志下一层副本是否可进
local function SendMsgToTeamMembers(Conn, teamId, Scene, flag)
	local teamMembers = g_TeamMgr:GetMembers(teamId)
	local playerId = Conn.m_Player.m_uID
	for index,id in ipairs(teamMembers) do
		local teamMember = g_GetPlayerInfo(id)
		if IsCppBound(teamMember) and id ~= playerId and teamMember.m_Scene == Scene then
			Gas2Gac:IsExitSceneMsgBox(teamMember.m_Conn, flag)
		end
	end
end

--离开副本时跳转场景
function LeaveAreaScene(Conn)											--如果要跳转的场景已经销毁 回到上一个唯一场景
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local sceneName = Player.m_MasterSceneName
	local pos = Player.m_MasterScenePos
	ChangeSceneByName(Conn, sceneName, pos.x, pos.y)
end

--创建初始矿洞副本
local function CreateAreaScene(Conn, SceneName)
	local Player = Conn.m_Player
	
	if not IsCppBound(Conn) or not IsCppBound(Player) then
		return
	end
	if not Player.m_OreMapName then
		return
	end
	
	local MapInfo = OreAreaMap_Common(Player.m_OreMapName)
	Player.m_OreMapName = nil
	
	local DelTbl = {}
	DelTbl["nCharID"] = Player.m_uID     
	DelTbl["ItemId"]  	= Player.OreMapID
	DelTbl["sceneName"] = Player.m_Scene.m_SceneName
	
	local ArgTbl = {}
	ArgTbl["m_SceneMaxLevel"] = MapInfo("MaxLevel")
	ArgTbl["m_SceneRank"] = MapInfo("StartTrigger")
	ArgTbl["m_SceneLevel"] = MapInfo("MinLevel")
	ArgTbl["m_PlayerLevel"] = Player:CppGetLevel()
	ArgTbl["m_CreateSceneId"] = Player.m_Scene.m_SceneId
	ArgTbl["m_CreateServerId"] = g_CurServerId
	ArgTbl["m_SceneBaseLv"] = Player:CppGetLevel() - 10
	if ArgTbl["m_SceneBaseLv"] < 0 then
		ArgTbl["m_SceneBaseLv"] = 0
	end
	local teamId = Player.m_Properties:GetTeamID()
	local function CallBack(SceneId, serverId, res)
		if not IsCppBound(Conn) then
			return
		end
		if not SceneId then
			MsgToConn(Conn, 802)
			return
		end
		if teamId and teamId ~= 0 then
			Gas2Gac:TeamJoinSceneMsgBox(Conn, SceneId, serverId, SceneName, SceneLevel)
		else
			JoinOreScene(Conn, SceneId, serverId, SceneName)
		end
	end

	local data = {}
	data["SceneName"] = SceneName
	data["Proc"] = 0
	data["OtherArg"] = ArgTbl
	data["DelTbl"] = DelTbl
	Conn.m_Player.OreMapID = nil
	if teamId and teamId ~= 0 then
		data["TeamId"] = teamId
	end
	CallAccountManualTrans(Conn.m_Account, "GatherLiveSkillDB", "CreateOreAreaScene", CallBack, data)
end

--创建下一层副本
local function CreateNextAreaScene(Conn, SceneName, SceneLevel, OldScene, flag)

	local teamId = Conn.m_Player.m_Properties:GetTeamID()
	local function CallBack(SceneId, serverId)
		if SceneId == 0 then
			return
		end
		local ArgTbl = {}
		ArgTbl["m_SceneLevel"] = SceneLevel
		ArgTbl["m_SceneRank"] = OldScene["m_SceneRank"] + 1			--矿洞层数+1  下一层
		ArgTbl["m_SceneMaxLevel"] = OldScene["m_SceneMaxLevel"]
--		scene.m_PlayerPostbl = OldScene.m_PlayerPostbl
		ArgTbl["m_SceneBaseLv"] = OldScene["m_SceneBaseLv"]
		ArgTbl["m_PlayerLevel"] = OldScene["m_PlayerLevel"]
		ArgTbl["m_CreateSceneId"] = OldScene["m_CreateSceneId"]
		ArgTbl["m_CreateServerId"] = OldScene["m_CreateServerId"]
		ArgTbl["m_OreAreaBoss"] = flag
		local scene = g_SceneMgr:CreateScene(SceneName, SceneId,ArgTbl)
		if scene == nil then
			return
		end
--		CreateOreAreaNpc(scene)
--		print("创建了"..SceneName.."副本的第"..scene.m_SceneRank.."层,本层为"..SceneLevel.."级矿洞")
		OldScene.ChangeSceneId = SceneId
		OldScene.ChangeServerId = serverId
		OldScene.ChangeSceneName = SceneName
		if teamId and teamId ~= 0 then
			SendMsgToTeamMembers(Conn, teamId, OldScene, true)
		end
		JoinOreScene(Conn, SceneId, serverId, SceneName)
	end

	local data = {}
	data["SceneName"] = SceneName
	data["Proc"] = 0
	data["ServerId"] = g_CurServerId
	if teamId and teamId ~= 0 then
		data["TeamId"] = teamId
	end
	CallAccountAutoTrans(Conn.m_Account, "GatherLiveSkillDB", "CreateOreAreaScene", CallBack, data)
end

--离开副本事件
local function LeaveSceneEvent(Conn, Scene, flag)
	local Scene = Conn.m_Player.m_Scene
	local teamId = Conn.m_Player.m_Properties:GetTeamID()
	if teamId and teamId ~= 0 then
		SendMsgToTeamMembers(Conn, teamId, Scene, false)
	end
	if flag then
		CreateOreBossInScene(Scene, Conn.m_Player.m_Properties:GetCharName())
	end
	LeaveAreaScene(Conn) 
end

--下一层副本
local function GotoNextAreaScene(Conn, scene, flag, SceneName)
	local Scene = Conn.m_Player.m_Scene
	local SceneLevel = scene.m_SceneLevel
	if SceneName == nil then
		SceneName = RandomSceneMap()
	end
	if SceneLevel < scene.m_SceneMaxLevel then
		SceneLevel = SceneLevel + 1
	end
	CreateNextAreaScene(Conn, SceneName, SceneLevel, scene, flag)
end

--客户端回调 确定进入矿洞副本
function Gac2Gas:CreateAreaScene(Conn, SceneName)
	CreateAreaScene(Conn, SceneName)
end

local msgtbl = {
	["矿洞坍塌"] = 10013,
	["矿洞守护兽"] = 10017,
	["进入特殊矿洞"] = 10018,
	["离开矿洞"] = 10014,
	["触动地脉"] = 10015,
}

--触发随机事件
local function OraAreaEventTrigger(Conn, EventName)
--	print(EventName)
	local scene = Conn.m_Player.m_Scene
--	MsgToConn(scene.m_CoreScene, 10006, EventName)
	MsgToConn(scene.m_CoreScene, msgtbl[EventName])
	if EventName == "矿洞坍塌" then
		GotoNextAreaScene(Conn, scene, false)
	elseif EventName == "离开矿洞" then
		LeaveSceneEvent(Conn, scene, false)
	elseif EventName == "触动地脉" then
		LeaveSceneEvent(Conn, scene, true)
	elseif EventName == "矿洞守护兽" then
		GotoNextAreaScene(Conn, scene, true) 
	elseif EventName == "进入特殊矿洞" then
		GotoNextAreaScene(Conn, scene, false, "特殊矿洞")
	end
end

--决定本层随机事件
function RandomEvent(Conn, Obj)
	local scene = Conn.m_Player.m_Scene
	local SceneId = scene.m_SceneId
	local function CallBack(AveLevel)
		if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
			Obj.IsClick = false
			return
		end
		if not AveLevel then
	--		print("您不是这个矿洞的创建者或创建队伍中的一员。")
			MsgToConn(Conn, 10007)
			Obj.IsClick = false
			return
		end
		if scene.m_SceneRank >= 10 then
			OraAreaEventTrigger(Conn, "离开矿洞")
			return "离开矿洞"
		end
		local Rank = "Rank"..scene.m_SceneRank
		local EventName
		local sum = 0
		local eventTbl = {}
		local Keys = OreAreaTriggerEvent_Common:GetKeys()
		for i=1, #Keys do
			local EventName = Keys[i]
			local value = OreAreaTriggerEvent_Common(EventName, Rank)
			if value ~= 0 then
				if EventName == "矿洞坍塌" then
					value = value + (AveLevel / 50)--memo
				end
				sum = sum + (value * 100)
				table.insert(eventTbl,{EventName,sum})
				--print(EventName,sum)
			end
		end
		local n = math.random(sum)
		for i = 1 , table.maxn(eventTbl) do
			if n <= eventTbl[i][2] then
				OraAreaEventTrigger(Conn, eventTbl[i][1])
				return eventTbl[i][1]
			end
		end
	end
	
	local ChannelIDList = {}
	local PlayerTbl = {}
	for id,member in pairs(scene.m_tbl_Player) do
		if id ~= Conn.m_Player.m_uID then
			table.insert(ChannelIDList, member.m_AccountID)
		end
		PlayerTbl[id] = id
	end
	local data = {}
	data["char_id"] = Conn.m_Player.m_uID
	data["sceneId"] = SceneId
	data["PlayerTbl"] = PlayerTbl
	CallAccountAutoTrans(Conn.m_Account, "GatherLiveSkillDB", "GetOreSkillAveLevel", CallBack, data, unpack(ChannelIDList))
	return true
end

--向组队成员发送进入副本邀请
local function TeamMembersJoinScene(Conn, teamId, SceneId, serverId, SceneName, SceneLevel)
	local teamMembers = g_TeamMgr:GetMembers(teamId)
	local playerId = Conn.m_Player.m_uID
	for index,id in pairs(teamMembers) do
		if IsNumber(index) and IsNumber(id) and id ~= playerId then
			Gas2GacById:JoinAreaSceneMsgBox(id, SceneId, serverId, SceneName, SceneLevel)
		end
	end
end

function Gac2Gas:TeamJoinScene(Conn, SceneId, serverId, SceneName, SceneLevel)
	JoinOreScene(Conn, SceneId, serverId, SceneName)
	local teamId = Conn.m_Player.m_Properties:GetTeamID()
	if teamId and teamId ~= 0 then
		TeamMembersJoinScene(Conn, teamId, SceneId, serverId, SceneName, SceneLevel)
	end
end

--获得Scene_Common中地图的InitPos
local function getSceneGoToPos(SceneName)
	local SceneInfo = Scene_Common[SceneName]
	if SceneInfo ~= nil then
		return SceneInfo.InitPosX,SceneInfo.InitPosY
	end
end

--邀请进入副本
function Gac2Gas:SendInviteJoinSceneMsg(Conn, CharID)
	if not IsCppBound(Conn.m_Player) then
		MsgToConn(Conn, 191028)
		return
	end
	local Player = Conn.m_Player
	local Scene = Conn.m_Player.m_Scene
	local SceneType = Scene.m_SceneAttr.SceneType
	local InvitedPlayer = g_GetPlayerInfo(CharID)
	if IsCppBound(InvitedPlayer) then
		if InvitedPlayer.m_Scene == Scene then
			local PlayerName = InvitedPlayer.m_Properties:GetCharName()
			MsgToConn(Conn, 10008, PlayerName)
			return
		end
	end
--	local QuestName = Scene.m_QuestName or ""
--	Gas2GacById:InviteJoinSceneMsgBox(CharID, Scene.m_SceneId, g_CurServerId, Scene.m_SceneName, Player.m_Properties:GetCharName(), SceneType, QuestName)
--	local function CallBack(res,strMsg)
--		if IsTable(res) then
--			if IsCppBound(Conn) and IsCppBound(Player) then
--				for n=1,#res do
--					for j=1,#res[n] do
--						Gas2Gac:RetDelItem(Conn,res[n].m_nRoomIndex,res[n].m_nPos,res[n][j])
--					end
--					Gas2Gac:RetDelItemEnd(Conn,res[n].m_nRoomIndex,res[n].m_nPos)
--				end
--				Gas2Gac:RetDelItemAllEnd(Conn)
--			end
--			local QuestName = Scene.m_QuestName or ""
--			Gas2GacById:InviteJoinSceneMsgBox(CharID, Scene.m_SceneId, g_CurServerId, Scene.m_SceneName, Player.m_Properties:GetCharName(), SceneType, QuestName)
--		else
--			if strMsg then
--				MsgToConn(Conn,res,unpack(strMsg))
--			else
--				MsgToConn(Conn,res)
--			end
--		end
--	end	
--	local data = {}
--	data["nCharID"]	= Player.m_uID    
--	data["InvitedID"] = CharID
--	data["QuestName"] = Scene.m_QuestName
--	data["SceneName"] = Scene.m_SceneName
--	data["SceneType"] = SceneType
--	CallAccountManualTrans(Conn.m_Account, "CharacterMediatorDB", "DelInviteItem", CallBack, data)
	local function CallBack(res,MsgID,strMsg)
		if res then
			local QuestName = Scene.m_QuestName or ""
			Gas2GacById:InviteJoinSceneMsgBox(CharID, Scene.m_SceneId, g_CurServerId, Scene.m_SceneName, Player.m_Properties:GetCharName(), SceneType, QuestName)
		else
			if strMsg then
				MsgToConn(Conn,MsgID,unpack(strMsg))
			elseif MsgID then
				MsgToConn(Conn,MsgID)
			end
		end
	end
	
	local data = {}
	data["nCharID"]	= Player.m_uID    
	data["InvitedID"] = CharID
	data["QuestName"] = Scene.m_QuestName
	data["SceneName"] = Scene.m_SceneName
	data["SceneType"] = SceneType
	data["SceneID"]		= Scene.m_SceneId
	CallAccountManualTrans(Conn.m_Account, "CharacterMediatorDB", "CheckIsCanInvite", CallBack, data)
end

function Gac2Gas:InviteChangeScene(Conn, SceneId, ServerId, SceneName, SceneType, OtherParam)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	if SceneType == 5 then
		CAreaFbServer.InviteJoinAreaFb(Conn, SceneName, SceneId, ServerId)
	elseif SceneType == 16 then
		Gac2Gas:AgreedJoinDareFb(Conn, SceneName, OtherParam)
	elseif SceneType == 21 then
		InviteJoinMercenaryMonsterFb(Conn, SceneName, SceneId, ServerId)
	elseif SceneType == 25 then
		InviteJoinPublicAreaFb(Conn, SceneId, ServerId)
	end
end

--客户端调用的跳转
function Gac2Gas:JoinOreScene(Conn, SceneId, serverId, SceneName)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	if g_CurServerId == serverId then
		local function CallBack(res)
			if res then
				JoinOreScene(Conn, SceneId, serverId, SceneName)
			else
				MsgToConn(Conn, 10016)
			end
		end
		local data = {}
		data["char_id"] = Conn.m_Player.m_uID
		data["sceneId"] = SceneId
		CallAccountAutoTrans(Conn.m_Account, "TeamSceneMgrDB", "CheckInFbScene", CallBack, data, SceneId)
	else
		Gas2GasCall:CheckJoinOreScene(GetServerAutoConn(serverId), SceneId, g_CurServerId, SceneName, Conn.m_Player.m_uID)
	end
end

function Gas2GasDef:RetCheckJoinOreScene(Conn, SceneId, serverId, SceneName, CharID)
	local Player = g_GetPlayerInfo(CharID)
	if IsCppBound(Player) then
		JoinOreScene(Player.m_Conn, SceneId, serverId, SceneName)
	end
end

function Gas2GasDef:CheckJoinOreScene(Conn, SceneId, serverId, SceneName, CharID)
	local function CallBack(res)
		Gas2GasCall:RetCheckJoinOreScene(GetServerAutoConn(serverId), SceneId, g_CurServerId, SceneName, CharID)
	end
	local data = {}
	data["char_id"] = CharID
	data["sceneId"] = SceneId
	CallDbTrans("TeamSceneMgrDB", "CheckInFbScene", CallBack, data, CharID, SceneId)
end

function Gac2Gas:JoinOreNextScene(Conn)
	if not IsCppBound(Conn) then
		return
	end
	local SceneId = Conn.m_Player.m_Scene.ChangeSceneId
	local serverId = Conn.m_Player.m_Scene.ChangeServerId
	local SceneName = Conn.m_Player.m_Scene.ChangeSceneName
	if SceneId and serverId and SceneName then
		JoinOreScene(Conn, SceneId, serverId, SceneName)
	end
end

function JoinOreScene(Conn, SceneId, serverId, SceneName)
	if SceneId == 0 then
		return
	end
	local x, y = getSceneGoToPos(SceneName)
	if x and y then
		MultiServerChangeScene(Conn, SceneId, serverId, x , y)
	end
end

function Gac2Gas:LeaveAreaScene(Conn)		--memo
	LeaveAreaScene(Conn)
end

function RandomSceneMap()
	return "矿洞"..math.random(4)
end

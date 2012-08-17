g_JFSPlayerCmpTick = nil

local m_FbName = "竞技场"
local m_RoomName = "竞技场副本"

g_JiFenSaiRoomTbl = {}

local WaitEnterSceneTime = 30 --50 	--玩家可以进场的时间
local BeginGameTime = 60 --60  			--从场景创建到开始的时间(秒)

local PlayerGameState ={}
PlayerGameState.InGame = 1	--表示在游戏中(接收消息)
PlayerGameState.InGameDead = 2	--表示在游戏中死亡
PlayerGameState.Running = 3	--表示逃跑
PlayerGameState.LeaveGame = 4	--表示正常退出(但不接收窗口消息)

local TeamCamp ={}
TeamCamp.One = 1	--表示队伍1
TeamCamp.Two = 2	--表示队伍2

local function FindPlayer(jfsInfo, charId)
	local charInfo = jfsInfo.m_PlayerIndex[charId]
	if charInfo then
		return jfsInfo.m_TeamTbl[charInfo.m_TeamId], charInfo
	end
end

local function DestroyRoom(RoomId)
	local Scene = g_SceneMgr:GetScene(RoomId)
	if not Scene then
		return
	end

	if Scene:IsHavePlayer() then
		return
	end
	Scene:Destroy()
end

local function PlayerExitFb(PlayerId,SpareTimes,MatchInfo)
	if SpareTimes < 0 then
		SpareTimes = 0
	end
	local Player = g_GetPlayerInfo(PlayerId)
	if IsCppBound(Player) then
		Gas2Gac:RetCloseFbActionWnd(Player.m_Conn,m_FbName)
		Gas2Gac:RetDelQueueFbAction(Player.m_Conn, m_FbName, true)
		MatchInfo = MatchInfo or ""
		Gas2Gac:RetOpenCountScoreWnd(Player.m_Conn,m_FbName,SpareTimes,MatchInfo)
	end
end

local function addexp_callback(PlayerId,LevelInfo)
	--加经验
	if LevelInfo then
		local Player = g_GetPlayerInfo(PlayerId)
		if IsCppBound(Player) then
			local AddExpTbl = {}
			AddExpTbl["Level"] = LevelInfo["CurLevel"]
			AddExpTbl["Exp"] = LevelInfo["LevelExp"]
			AddExpTbl["AddExp"] = LevelInfo["addExp"]
			AddExpTbl["uInspirationExp"] = 0
			CRoleQuest.AddPlayerExp_DB_Ret(Player, AddExpTbl)
		end
	end
end

local function AllPlayerExitFb(jfsInfo, DeadTeamID, IsAllExit)
	--进入退出状态后,就不用再进了
	if jfsInfo.m_State == g_FbRoomState.GameEnd then
		return
	end
	--改变当前的游戏状态
	jfsInfo.m_State = g_FbRoomState.GameEnd
	
	--首先算出要加或者减的积分数
	local TeamType = jfsInfo.m_GameType
	
	local ChannelIDList = {}
	local Member = {}
	local DeadRunAwayTbl = {}
	local PlayerKillNumTbl = {}
	local PlayerDeadNumTbl = {}
	for charId,charInfo in pairs(jfsInfo.m_PlayerIndex) do
		PlayerKillNumTbl[charId] = charInfo.m_KillCount
		PlayerDeadNumTbl[charId] = charInfo.m_BekillCount
		if not DeadRunAwayTbl[charInfo.m_TeamId] then
			DeadRunAwayTbl[charInfo.m_TeamId] = {}
		end
		
		DeadRunAwayTbl[charInfo.m_TeamId][charId] = charInfo.m_GameState
		
		if charInfo.m_TeamId ~= DeadTeamID and not IsAllExit then
			MsgToConnById(charId, 3422)
		else
			MsgToConnById(charId, 3423)
		end
		
		local Target = g_GetPlayerInfo(charId)
		if IsCppBound(Target) then
			table.insert(ChannelIDList, Target.m_AccountID)
			table.insert(Member, Target)
		else
			table.insert(ChannelIDList, charId)
		end
	end
	
	local function CallBack(result)
		if result then
			for index,_ in pairs(result) do
				local PlayerId = result[index]["PlayerId"]
				
				if result[index]["MatchPoint"] and result[index]["MatchPoint"] ~= 0 then
					MsgToConnById(PlayerId, 3402, result[index]["MatchPoint"])
					MsgToConnById(PlayerId, 3403, result[index]["CountPoint"])
				end
				
				if result[index]["PlayerState"] == 1 or result[index]["PlayerState"] == 2 then
					PlayerExitFb(PlayerId,result[index]["MatchTimes"],result[index]["MatchInfo"])
				end
				
				if result[index]["MatchTimes"] > 0 and result[index]["GloryPoint"] ~= 0 then
					Gas2GacById:UpdatePlayerPoint(PlayerId, 5, result[index]["GloryPoint"])
				end
				
				--加经验
				if result[index]["LevelInfo"] then
					addexp_callback(PlayerId, result[index]["LevelInfo"])
				end		
				
			end
		end
	end
	
	local parameters = {}
	parameters["DeadRunAwayTbl"] = DeadRunAwayTbl
	parameters["PlayerKillNumTbl"] = PlayerKillNumTbl
	parameters["PlayerDeadNumTbl"] = PlayerDeadNumTbl
	parameters["TeamType"] = TeamType
	parameters["time"] = os.time()
	parameters["DeadTeamID"] = DeadTeamID
	parameters["IsAllExit"] = IsAllExit
	OnSavePlayerExpFunc(Member)
	CallDbTrans("JiFenSaiFbDB", "UpdateDataInfo", CallBack, parameters, unpack(ChannelIDList))
end

local function GetTeamNum(jfsInfo,charId)
	local charInfo = jfsInfo.m_PlayerIndex[charId]
	
	local FriendNum = 0
	local EnemyNum = 0
	for teamid,teaminfo in pairs(jfsInfo.m_TeamTbl) do
		if charInfo.m_TeamId == teamid then
			FriendNum = teaminfo.m_PlayerNum
		else
			EnemyNum = teaminfo.m_PlayerNum
		end
	end
	return FriendNum,EnemyNum
end

--更新客户端的人数消息
local function UpdateAllClientInfo(jfsInfo)
	for charId,charInfo in pairs(jfsInfo.m_PlayerIndex) do
		local player = g_GetPlayerInfo(charId)
		if player and charInfo.m_IsInScene then --未进场的不更新
			if charInfo.m_GameState == PlayerGameState.InGame then
				--or charInfo.m_GameState == PlayerGameState.InGameDead then
				
				local FriendNum,EnemyNum = GetTeamNum(jfsInfo,charId)
				Gas2Gac:RetJFSUpdateMemberInfo(player.m_Conn,FriendNum,EnemyNum)
				
			end
		end
	end
end

--检测队伍中的人数,看还够不够
local function CheckTeamPlayer(jfsInfo, DeadTeamID)
	for charId,charInfo in pairs(jfsInfo.m_TeamTbl[DeadTeamID].m_PlayerTbl) do
		if charInfo.m_GameState == PlayerGameState.InGame then
			--or charInfo.m_GameState == PlayerGameState.InGameDead then
				
			return true--表示还有人
				
		end
	end
	
	return false--表示没有人了
end


local function PlayerRunGame(jfsInfo,charInfo,charId)
	if jfsInfo.m_State == g_FbRoomState.GameEnd then
		return
	end
	
	for TargetId,_ in pairs(jfsInfo.m_TeamTbl[charInfo.m_TeamId].m_PlayerTbl) do
		if charId ~= TargetId then
			MsgToConnById(TargetId, 3424, charInfo.m_Name)
--			local Target = g_GetPlayerInfo(TargetId)
--			if IsCppBound(Target) then
--				MsgToConn(Target.m_Conn, 3424)--RunningName)
--			end
		end
	end
end

--第三个参数是退出的状态
local function RemovePlayer(jfsInfo, charId, GameState, IsExit)
	local teamInfo, charInfo = FindPlayer(jfsInfo, charId)
	if charInfo then
		local char = g_GetPlayerInfo(charId)
		--改变玩家相关数据状态
		if charInfo.m_IsInScene then
			if IsCppBound(char) then
				Gas2Gac:RetCloseFbActionWnd(char.m_Conn, m_FbName)
			end
		end
		
		--队伍中少一个人
		if charInfo.m_GameState == PlayerGameState.InGame then
			if charInfo.m_IsInScene then
				teamInfo.m_PlayerNum = teamInfo.m_PlayerNum - 1
			end
			
			charInfo.m_GameState = GameState
		elseif charInfo.m_GameState == PlayerGameState.InGameDead then
			charInfo.m_GameState = PlayerGameState.LeaveGame
		end
		
		--发消息说,有人逃跑
		if GameState == PlayerGameState.Running then
			PlayerRunGame(jfsInfo,charInfo,charId)
		end
		
		UpdateAllClientInfo(jfsInfo)
		
		--如果是还没有比赛就退出的话,不用给所有的人记分
		if IsExit or jfsInfo.m_State == g_FbRoomState.PlayerCanIn then
			return
		end
		
		--判断看是不是还有两只队伍在比赛
		if not CheckTeamPlayer(jfsInfo, charInfo.m_TeamId) then
			AllPlayerExitFb(jfsInfo, charInfo.m_TeamId)
		end
	end
end

--有人死掉更新
function DeadUpdateJiFenSaiInfo(killer, deader)
	local jfsInfo = deader.m_Scene.m_JfsInfo
	if IsCppBound(killer) then
		local killerId = killer.m_uID
		local killerTeam, killerInfo = FindPlayer(jfsInfo, killerId)
		if killerInfo then
			killerInfo.m_KillCount = killerInfo.m_KillCount + 1
		end
		local deaderId = deader.m_uID
		local deaderTeam, deaderInfo = FindPlayer(jfsInfo, deaderId)
		if deaderInfo then
			deaderInfo.m_BekillCount = deaderInfo.m_BekillCount + 1
		end
		
		CallAccountAutoTrans(killer.m_Conn.m_Account, "JiFenSaiFbDB", "UpdateKillNumSort", nil, {killerId})
	end
	
	--修改当前玩家的状态(还是在游戏中)............
	RemovePlayer(jfsInfo, deader.m_uID, PlayerGameState.InGameDead)
	
--	--死掉后加经验
--	if IsCppBound(deader) then
--		local data = {}
--		data["charId"] = deader.m_uID
--		local function CallBack(resExpInfo)
--			addexp_callback(data["charId"], resExpInfo)
--		end
--		OnSavePlayerExpFunc({deader})
--		CallAccountAutoTrans(deader.m_Conn.m_Account, "JiFenSaiFbDB", "JfsAddExp", CallBack, data)
--	end
	
	if jfsInfo.m_State == g_FbRoomState.GameEnd then
		deader.m_NotShowJFSMsgWnd = true--要全退出的时候,就不变显示是否要进入观察模式窗口
		if IsCppBound(killer) then
			killer.m_NotShowJFSMsgWnd = true--为了防止两个人同归于尽(有一个人会弹窗口出来)
		end
	end
	
end

--死亡后弹出提示窗口,选择退出副本
function MsgSelExitIntegralMatch(Player)
	if IsCppBound(Player) then
		local jfsInfo = Player.m_Scene.m_JfsInfo
		--修改当前玩家的状态(是逃跑)............
		ExitJiFenSaiGame(jfsInfo, Player.m_uID, PlayerGameState.LeaveGame, true)		
	end
end

--掉线时调用
function PlayerOffLineJiFenSaiFb(Player)
	local jfsInfo = Player.m_Scene.m_JfsInfo
	if jfsInfo then
		ExitJiFenSaiGame(jfsInfo, Player.m_uID, PlayerGameState.Running, true)
	end
end

local function CheckAllPlayerInFb(jfsInfo)
	--1表示没有人点确定;2表示只有一队的人进入了副本;3表示进入副本中有两个队伍
	local TeamInScene = {}
	TeamInScene[TeamCamp.One] = {}
	TeamInScene[TeamCamp.Two] = {}
	TeamInScene[TeamCamp.One].HavePeople = false
	TeamInScene[TeamCamp.One].TeamId = 0
	TeamInScene[TeamCamp.Two].HavePeople = false
	TeamInScene[TeamCamp.Two].TeamId = 0
	for teamid,teaminfo in pairs(jfsInfo.m_TeamTbl) do
		TeamInScene[teaminfo.m_TeamNum].TeamId = teamid
		for charId,charInfo in pairs(teaminfo.m_PlayerTbl) do
			local player = g_GetPlayerInfo(charId)
			if player and charInfo.m_IsInScene and charInfo.m_GameState == PlayerGameState.InGame then
				TeamInScene[teaminfo.m_TeamNum].HavePeople = true
				break
			end
		end
	end
	
	if not TeamInScene[TeamCamp.One].HavePeople and not TeamInScene[TeamCamp.Two].HavePeople then
		AllPlayerExitFb(jfsInfo,nil,true)
		DestroyRoom(jfsInfo.m_Scene.m_SceneId)
	elseif not TeamInScene[TeamCamp.One].HavePeople then
		AllPlayerExitFb(jfsInfo,TeamInScene[TeamCamp.One].TeamId)
	elseif not TeamInScene[TeamCamp.Two].HavePeople then
		AllPlayerExitFb(jfsInfo,TeamInScene[TeamCamp.Two].TeamId)
	else
		return true
	end
end

local function BeginJiFenSaiTick(jfsInfo)
	jfsInfo.m_AllTeamCount = jfsInfo.m_LiveTeamCount
	
	if jfsInfo.m_State == g_FbRoomState.GameEnd then
		return
	end
	
	--检测现在还用不用开比赛,还是比赛结束
	if not CheckAllPlayerInFb(jfsInfo) then
		return
	end
	
	--改变游戏中的状态
	for charId,charInfo in pairs(jfsInfo.m_PlayerIndex) do
		local player = g_GetPlayerInfo(charId)
		if player and charInfo.m_IsInScene then
			local TeamNum = jfsInfo.m_TeamTbl[charInfo.m_TeamId].m_TeamNum
			player:SetGameCamp(TeamNum)
		end
	end
	for charId,charInfo in pairs(jfsInfo.m_PlayerIndex) do
		local player = g_GetPlayerInfo(charId)
		if player and charInfo.m_IsInScene then
			Gas2Gac:RetSetJFSPlayerCamp(player.m_Conn)
		end
	end
	
	jfsInfo.m_State = g_FbRoomState.BeginGame
end

local function ClearNotInFbTick(jfsInfo)
	if jfsInfo.m_State == g_FbRoomState.GameEnd then
		return
	end
	
	--清理未入场的玩家
	for _, teamInfo in pairs(jfsInfo.m_TeamTbl) do
		for charId, charInfo in pairs(teamInfo.m_PlayerTbl) do
			local player = g_GetPlayerInfo(charId)
			if not (player and charInfo.m_IsInScene) then
				if charInfo.m_GameState ~= PlayerGameState.Running then
					ExitJiFenSaiGame(jfsInfo, charId, PlayerGameState.Running, true)
				end
			end
		end
	end
	
	--检测现在还用不用开比赛,还是比赛结束
	if not CheckAllPlayerInFb(jfsInfo) then
		return
	end
	
	UpdateAllClientInfo(jfsInfo)
	
	jfsInfo.m_State = g_FbRoomState.CountDown
	RegisterOnceTick(jfsInfo.m_Scene, "BeginJiFenSaiTick", BeginJiFenSaiTick, 1000 * (BeginGameTime-WaitEnterSceneTime), jfsInfo)
end

local EnterPos = {--玩家进入副本后,1队和2队的坐标
		[1] = {59,76},
		[2] = {59,33},
}

--创建房间,让对抗的两个队伍进入房间
function OnJiFenSaiCreateRoom(scene, roomId, actionName, roomMembers, JiFenSaiInfo)
	local jfsInfo = {}
	jfsInfo.m_RoomId = roomId
	jfsInfo.m_Scene = scene
	jfsInfo.m_GameType = JiFenSaiInfo["RoomType"]--比赛的类型(2V2,3V3,5V5);存数为1,2,3
	jfsInfo.m_PlayerIndex = {}	--通过charId 索引的玩家表
	jfsInfo.m_TeamTbl = {}			--队伍表
	
	local index = 0
	for teamId, v in pairs(roomMembers) do
		index = index + 1
		jfsInfo.m_TeamTbl[teamId] = {}
		jfsInfo.m_TeamTbl[teamId].m_PlayerTbl = {}
		jfsInfo.m_TeamTbl[teamId].m_PlayerNum = 0
		jfsInfo.m_TeamTbl[teamId].m_TeamNum = index
		jfsInfo.m_TeamTbl[teamId].m_EnterPos = EnterPos[index]
		
		for _, charId in pairs(v) do
			local charInfo = {}
			charInfo.m_TeamId = teamId
			charInfo.m_KillCount = 0
			charInfo.m_BekillCount = 0
			charInfo.m_GameState = PlayerGameState.InGame
			charInfo.m_Name = JiFenSaiInfo["NameTbl"][charId]
			
			jfsInfo.m_PlayerIndex[charId] = charInfo
			jfsInfo.m_TeamTbl[teamId].m_PlayerTbl[charId] = charInfo
			
			Gas2GacById:RetIsJoinFbActionAffirm(charId, actionName, WaitEnterSceneTime-10, 0)
		end
	end
	
	scene.m_JfsInfo = jfsInfo
	g_JiFenSaiRoomTbl[roomId] = jfsInfo
	jfsInfo.m_State = g_FbRoomState.PlayerCanIn
	RegisterOnceTick(jfsInfo.m_Scene, "JiFenSaiClearTick", ClearNotInFbTick, 1000 * WaitEnterSceneTime, jfsInfo)
	jfsInfo.m_CreateSceneTime = os.time()
end

function GetJiFenSaiEnterPos(roomId, charId)
	local jfsInfo = g_JiFenSaiRoomTbl[roomId]
	if jfsInfo then
		local teamInfo, charInfo = FindPlayer(jfsInfo, charId)
		if charInfo then
			return jfsInfo.m_Scene.m_SceneId, teamInfo.m_EnterPos
		end
	end
end

function Gac2Gas:JoinJiFenSai(Conn, NpcID)
	if SearchClosedActionTbl(m_FbName) then
		MsgToConn(Conn,3521)--活动已经关闭,不可以报名
		return
	end
	
	local function CallBack(suc,result,msg)
		if IsCppBound(Conn) then
			if suc then
				MultiServerJoinAction(Conn, m_FbName)
			else
				if msg then
					MsgToConn(Conn,result,unpack(msg))
				elseif result then
					MsgToConn(Conn,result)
				end
			end
		end
	end
	
	local parameters = {}
	parameters["charID"] = Conn.m_Player.m_uID
	parameters["ActionName"] = m_FbName
	parameters["TeamNum"] = g_JfsTeamNum
	CallAccountManualTrans(Conn.m_Account, "JiFenSaiFbDB", "CheckTeamMemberNum", CallBack, parameters)
end

local function ChangeSceneLeave(Player)
	if Player.m_Scene.m_JfsInfo then --说明是在副本中
		local _, charInfo = FindPlayer(Player.m_Scene.m_JfsInfo, Player.m_uID)
		if charInfo and charInfo.m_IsInScene then
			charInfo.m_IsInScene = nil
			
			local sceneName = Player.m_MasterSceneName
			local pos = Player.m_MasterScenePos 
			if Player.m_EnterObserverMode then
				Player:LeaveObserverMode()
			end
			Player:SetGameCamp(0)
			ChangeSceneByName(Player.m_Conn,sceneName,pos.x,pos.y)
			
		end
	end
end

function Gac2Gas:ExitJFSFbAction(Conn)
	local player = Conn.m_Player
	if IsCppBound(player) then
		ChangeSceneLeave(player)
	end
end

function OnDestroyJiFenSaiScene(scene)
	local jfsInfo = scene.m_JfsInfo
	g_JiFenSaiRoomTbl[jfsInfo.m_RoomId] = nil
end

function IsCanEnterJiFenSaiScene(scene, charId)
	local jfsInfo = scene.m_JfsInfo
	if jfsInfo.m_State == g_FbRoomState.PlayerCanIn and FindPlayer(jfsInfo, charId) then
		return true
	end
end

function ExitJiFenSaiGame(jfsInfo, charId, GameState, mustDelDb)
	if mustDelDb then
		RemovePlayer(jfsInfo, charId, GameState)
		Gas2GacById:RetDelQueueFbAction(charId, m_FbName, true)
		
		local data = {}
		data["CharId"] = charId
		data["RoomId"] = jfsInfo.m_RoomId
		CallDbTrans("JoinActionDB", "CancelActionByRoom", nil, data, charId, m_FbName)
	else
		RemovePlayer(jfsInfo, charId, GameState, true)
	end
	local player = g_GetPlayerInfo(charId)
	if IsCppBound(player) then
		ChangeSceneLeave(player)
	end
end

function Gac2Gas:ShowJiFenSaiWnd(Conn, NpcID)
	if not CheckAllNpcFunc(Conn.m_Player,NpcID,"竞技场报名处") then
		return
	end
	
	if not CheckActionIsBeginFunc(m_FbName) then
		MsgToConn(Conn,3520)--活动还没有开放,不可以报名
		return
	end
	
	Gas2Gac:RetShowJiFenSaiWnd(Conn, NpcID)
end

function IntoJFSScene(player)
	local jfsInfo = player.m_Scene.m_JfsInfo
	local teamInfo, charInfo = FindPlayer(jfsInfo, player.m_uID)
	assert(teamInfo and charInfo, "玩家非法进入积分赛场景")
	charInfo.m_IsInScene = true
	teamInfo.m_PlayerNum = teamInfo.m_PlayerNum + 1
	
	player:SetGameCamp(1)--设置为统一的玩法阵营
	
	local time = BeginGameTime - (os.time() - jfsInfo.m_CreateSceneTime)
	Gas2Gac:RetSetJoinBtnEnable(player.m_Conn,m_FbName)
	Gas2Gac:RetOpenFbActionWnd(player.m_Conn, m_FbName, time)
	UpdateAllClientInfo(jfsInfo)
	
	--进入副本计一次数
	local limitName = FbActionDirect_Common(m_FbName, "MaxTimes")
	if limitName and limitName ~= "" then
		local parameters = {}
		parameters["ActionName"] = m_FbName
		parameters["PlayerId"] = player.m_uID
		parameters["ActivityName"] = limitName
		CallAccountAutoTrans(player.m_Conn.m_Account, "JoinActionDB", "OnEnterRoom", nil, parameters)
	end
end

function LeaveJFSScene(Player, sceneId)
	local scene = g_SceneMgr:GetScene(sceneId)
	local jfsInfo = scene.m_JfsInfo
	Gas2GacById:RetCloseCountScoreWnd(Player.m_uID, m_FbName)
	Player:PlayerDoFightSkillWithoutLevel("玩法出场恢复")
	ExitJiFenSaiGame(jfsInfo, Player.m_uID, PlayerGameState.Running, true)
end

function Gac2Gas:ShowJiFenSaiInfoWnd(Conn, NpcID)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	if not CheckAllNpcFunc(Conn.m_Player,NpcID,"积分赛面板测试") then
		return
	end
	
	local function CallBack(InfoTbl)
		local tbl = InfoTbl[3]
		Gas2Gac:RetShowJiFenSaiInfoWnd(Conn, tbl.WinNum, tbl.LoseNum, tbl.RunNum, tbl.WinPro, tbl.KillNum, tbl.DeadNum, tbl.Point)
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	CallDbTrans("JiFenSaiFbDB", "GetPlayerPanelInfo", CallBack, data)
end
--对于大逃杀 原来 room 的概念变成 area,  现在的room 指活动场景(为了跟数据库的概念统一), 
--为了和数据库统一, 以及将来的扩展性 玩家多了一层Team的概念,虽然现在一个team内只有一个人
--所有大逃杀数据都挂在scene.m_DtsInfo 上, (为了避免其他人使用重名 而不直接挂到 scene 上),  dtsInfo还挂在了 g_DaTaoShaRoomTbl 里

local _SceneName = "角斗场副本"
local _ActionName = "角斗场"

local WaitJointTime = 35    		--可以继续报名的时间
local WaitEnterSceneTime = 50 	--玩家可以进场的时间
local BeginGameTime = 60  			--从场景创建到开始的时间(秒)

local FightingTime = 60

g_DaTaoShaRoomTbl = {} 

local function TestPrint(...)
	--print("----大逃杀测试输出:  ", ...)
end

local function OnDtsLogErr(dtsInfo, err, msg)
	local str = msg.."#r"..table.concat(dtsInfo.m_LogTbl, "#r")
	--print(str)
	LogErr(err, str)
end

local function BeginGame(dtsInfo)
	--TestPrint("开始比赛")
	--清理未入场的玩家
	table.insert(dtsInfo.m_LogTbl, "BeginGame, time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	for _, teamInfo in pairs(dtsInfo.m_TeamTbl) do
		for charId, charInfo in pairs(teamInfo.m_PlayerTbl) do
			local player = g_GetPlayerInfo(charId)
			if not (player and charInfo.m_IsInScene) then
				ExitDaTaoShaGame(dtsInfo, charId, 0, true)
			else
				Gas2Gac:OpenGameCountdownWnd(player.m_Conn, FightingTime-2, 0)	-- -2消息延迟时间差
				
				table.insert(dtsInfo.m_LogTbl, "playerId:"..charId..", areaId:"..charInfo.m_AreaId)
			end
		end
	end
	
	for _, area in pairs(dtsInfo.m_AreaIndex) do
		AddCountDownTime(dtsInfo, area, FightingTime)
	end
	
	table.insert(dtsInfo.m_LogTbl, "teamCount:"..dtsInfo.m_LiveTeamCount)
	
	dtsInfo.m_AllTeamCount = dtsInfo.m_LiveTeamCount
	dtsInfo.m_State = g_FbRoomState.BeginGame
	if dtsInfo.m_LiveTeamCount <= 1 then
		--TestPrint("比赛开始人数不足直接结束活动, 当前人数:" .. dtsInfo.m_LiveTeamCount)
		DaTaoShaGameOver(dtsInfo)
	end
end

local function WaitPlayerInEnd(dtsInfo)
	dtsInfo.m_State = g_FbRoomState.CountDown
	
	table.insert(dtsInfo.m_LogTbl, "WaitPlayerInEnd, state:"..dtsInfo.m_State..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	RegisterOnceTick(dtsInfo.m_Scene, "DaTaoShaBeginTick", BeginGame, 1000 * (BeginGameTime - WaitEnterSceneTime), dtsInfo)
end

local function WaitPlayerJoinEnd(dtsInfo)
	table.insert(dtsInfo.m_LogTbl, "WaitPlayerJoinEnd, state:"..dtsInfo.m_State..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	RegisterOnceTick(dtsInfo.m_Scene, "DaTaoShaWaitInTick", WaitPlayerInEnd, 1000 * (WaitEnterSceneTime - WaitJointTime), dtsInfo)
	
	CallDbTrans("JoinActionDB","WaitPlayerEnd", nil, {["RoomId"] = dtsInfo.m_RoomId}, _ActionName)
end

local function FindPlayer(dtsInfo, charId)
	local charInfo = dtsInfo.m_PlayerIndex[charId]
	if charInfo then
		return dtsInfo.m_TeamTbl[charInfo.m_TeamId], charInfo
	end
end

local GloryCoef = {1.2, 1.8, 2.4, 2.9, 3.5, 4.1, 4.6, 5.2, 5.8, 6.3, 6.9, 7.5, 8, 8.6, 9.2, 9.2}
local function GetPlayerGloryPoint(level, killCount, leaveCount, isWin)
	local point = 0
	if isWin then
	 point = (leaveCount + 1) * 2 + (killCount*10)
	else
		point = leaveCount + (killCount*10)
	end
	local	index = math.floor(level/10)+1
	return math.floor(point * GloryCoef[index]), math.floor(point * GloryCoef[index])--point
end

local function GetMaxPoint(Level)
	local	index = math.floor(Level/10)+1
	return math.floor(30 * GloryCoef[index])
end

--仅用于比赛中的更新
local function UpdateAllClientInfo(dtsInfo)
	if dtsInfo.m_State == g_FbRoomState.PlayerCanIn or dtsInfo.m_State == g_FbRoomState.CountDown then
		local liveTeamCount = dtsInfo.m_LiveTeamCount
		
		for _, teamInfo in pairs(dtsInfo.m_TeamTbl) do
			for charId, charInfo in pairs(teamInfo.m_PlayerTbl) do
				local player = g_GetPlayerInfo(charId)
				if player and charInfo.m_IsInScene then --未进场的不更新
					Gas2Gac:DtsUpdateLivePlayerNum(player.m_Conn, liveTeamCount)
				end
			end
		end
		
	elseif dtsInfo.m_State == g_FbRoomState.BeginGame then
		local liveTeamCount = dtsInfo.m_LiveTeamCount
		local maxKillCount = dtsInfo.m_MaxKillCount
		local leaveCount = dtsInfo.m_AllTeamCount - liveTeamCount
		local isWin = liveTeamCount <= 1
		
		for _, teamInfo in pairs(dtsInfo.m_TeamTbl) do
			for charId, charInfo in pairs(teamInfo.m_PlayerTbl) do
				local player = g_GetPlayerInfo(charId)
				if player then
					local gloryPoint = GetPlayerGloryPoint(player:CppGetLevel(), charInfo.m_KillCount, leaveCount, isWin)
					Gas2Gac:DtsUpdateAllInfo(player.m_Conn, charInfo.m_KillCount, liveTeamCount, gloryPoint, maxKillCount)
				end
			end
		end
		
	end
end

--erea采用双向链表,并有一个索引table    --节点都统一插到尾部,
local function AreaListInsertNode(dtsInfo, areaId)
	assert(dtsInfo.m_AreaIndex[areaId] == nil)
	local node = {}
	node.m_AreaId = areaId
	node.m_PlayerCount = 0  --记录该区域当前有几个玩家在里面
	
	if dtsInfo.m_AreaListHead == nil then
		node.m_FrontNode = node
		node.m_NextNode = node
		dtsInfo.m_AreaListHead = node
	else
		local headNode = dtsInfo.m_AreaListHead
		local tailNode = dtsInfo.m_AreaListHead.m_FrontNode
		node.m_FrontNode = tailNode
		node.m_NextNode = headNode
		tailNode.m_NextNode = node
		headNode.m_FrontNode = node
	end
	dtsInfo.m_AreaIndex[areaId] = node
	dtsInfo.m_AreaCount = dtsInfo.m_AreaCount + 1
end

local function AreaListDelNode(dtsInfo, areaId)
	assert(dtsInfo.m_AreaIndex[areaId])
	local node = dtsInfo.m_AreaIndex[areaId]
	if node.m_NextNode == node or node.m_FrontNode == node then --剩最后一个节点(其实两个条件取其一就可, 不过为了断言更全面才这样写)
		assert(dtsInfo.m_AreaListHead == node and dtsInfo.m_AreaCount == 1 and node.m_FrontNode == node.m_NextNode)
		dtsInfo.m_AreaListHead = nil
	else
		node.m_NextNode.m_FrontNode = node.m_FrontNode
		node.m_FrontNode.m_NextNode = node.m_NextNode
		if node == dtsInfo.m_AreaListHead then
			dtsInfo.m_AreaListHead = node.m_NextNode
		end
	end
	if dtsInfo.m_AreaIndex[areaId].FightTick then
		UnRegisterTick(dtsInfo.m_AreaIndex[areaId].FightTick)
		dtsInfo.m_AreaIndex[areaId].FightTick = nil
	end
	dtsInfo.m_AreaIndex[areaId] = nil
	dtsInfo.m_AreaCount = dtsInfo.m_AreaCount - 1
end

local function GetUnUsedAreaId(dtsInfo)
	local areaId = 1
	while true do
		if not dtsInfo.m_AreaIndex[areaId] then
			return areaId
		end
		areaId = areaId + 1
	end
end

local function GetEndGiveItem(PlayerLevel)
	if g_MercenaryAwardItemMgr[_ActionName] then
		for i=1, #(g_MercenaryAwardItemMgr[_ActionName]) do
			local ItemMgr = g_MercenaryAwardItemMgr[_ActionName][i]
			if ItemMgr[1] and ItemMgr[2] then
				local LevelTbl = ItemMgr[1]
				local ItemTbl = ItemMgr[2]
				if LevelTbl[1] and LevelTbl[2] then
					if PlayerLevel >= LevelTbl[1] and PlayerLevel <= LevelTbl[2] then
						return ItemTbl
					end
				elseif LevelTbl[1] and PlayerLevel == LevelTbl[1] then
					return ItemTbl
				end
			end
		end
	end
end

local function AddPlayerPoint(charId, sceneId, stillNum, killNum, allNum, exitState)
	--TestPrint("给玩家加积分:::::::::::")
	--TestPrint("stillNum", stillNum)
	--TestPrint("killNum", killNum)
	--TestPrint("allNum", allNum)
	--TestPrint("exitState", exitState)
	
	local player = g_GetPlayerInfo(charId)
	if not IsCppBound(player) then
		return
	end
	local level = player:CppGetLevel()
	local gloryPoint, matchPoint = GetPlayerGloryPoint(level, killNum, allNum - stillNum, exitState == 1)
	local addExp = 0
	if exitState ~= 0 then --给比赛的玩家加经验奖励(逃跑的不给)
		addExp = gloryPoint * 160
	end
	
	gloryPoint = 0   --临时屏蔽大逃杀奖励积分

	local function CallBack(succ,spareTimes,matchInfo,curLevel,levelExp,AwardInfo)
		--TestPrint("给玩家加积分CallBack :::::::::::::")
		--TestPrint("spareTimes", spareTimes)
		--TestPrint("matchInfo", matchInfo)
		--TestPrint("curLevel", curLevel)
		--TestPrint("levelExp", levelExp)
		
		if not IsCppBound(player) then
			return
		end
		if not succ then
			if IsNumber(spareTimes) then
				MsgToConn(player.m_Conn,spareTimes)
			end
		elseif player.m_Scene.m_SceneId == sceneId then
			if matchInfo then
				matchInfo = stillNum..","..killNum..","..addExp..","..matchInfo
			else
				matchInfo = ""
			end
			if curLevel then
				local AddExpTbl = {}
				AddExpTbl["Level"] = curLevel
				AddExpTbl["Exp"] = levelExp
				AddExpTbl["AddExp"] = addExp
				AddExpTbl["uInspirationExp"] = 0
				CRoleQuest.AddPlayerExp_DB_Ret(player, AddExpTbl)
			end
			if spareTimes > 0 then
				if gloryPoint > 0 then
					Gas2Gac:UpdatePlayerPoint(player.m_Conn, 4, gloryPoint)
				end
			else
				spareTimes = 0
			end
			if exitState ~= 0 then
				Gas2Gac:RetOpenCountScoreWnd(player.m_Conn,_ActionName,spareTimes,matchInfo)
			end
			
			if AwardInfo and AwardInfo[charId] and AwardInfo[charId][3] then
				local itemResult = AwardInfo[charId][3]
				if next(itemResult) then
					for i=1,#(itemResult) do
						if IsNumber(itemResult[i][4]) then
							if itemResult[i][4] == 3 then
								--,'包裹空间不足'
								MsgToConn(player.m_Conn,160000)
							elseif itemResult[i][4] == 0 then
								MsgToConn(player.m_Conn,8003,'CreateFightItem error')
							end
						else
							--MsgToConn(player.m_Conn,3603,itemResult[i][3],itemResult[i][2])
							--CRoleQuest.add_item_DB_Ret(player, itemResult[i][1], itemResult[i][2], itemResult[i][3],itemResult[i][4])
						end
					end
				end
			end
			
		end
		
	end
	
	if exitState == 1 then
		local PlayerName = player.m_Properties:GetCharName()
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:SendActionRollMsg(GetServerAutoConn(server_id), PlayerName, _ActionName)
		end
	end
	
	local parameters = {}
	parameters["PlayerId"] = charId
	parameters["MatchPoint"] = matchPoint
	parameters["GloryPoint"] = gloryPoint
	parameters["MaxPoint"] = GetMaxPoint(level)
	parameters["AddExp"] = addExp
	parameters["State"] = exitState
	parameters["CurrTime"] = os.time()
	parameters["AwardItem"] = GetEndGiveItem(level)
	
	OnSavePlayerExpFunc({player})
	CallDbTrans("DaTaoShaFbDB", "updateDaTaoShaPoint", CallBack, parameters, player.m_AccountID)
end

function DaTaoShaGameOver(dtsInfo)
	table.insert(dtsInfo.m_LogTbl, "DaTaoShaGameOver, time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	local teamId, teamInfo = next(dtsInfo.m_TeamTbl)
	if teamInfo then
		assert(next(dtsInfo.m_TeamTbl, teamId) == nil , "大逃杀结束时候人数超过2个")
		local charId = next(teamInfo.m_PlayerTbl)
		ExitDaTaoShaGame(dtsInfo, charId, 1, true)
	end
	
	--OnDtsLogErr(dtsInfo, "内网测试，非bug", "")
	
	local scene = dtsInfo.m_Scene
	if scene.m_tbl_Player and next(scene.m_tbl_Player) then
		return
	end
	scene:Destroy()
end

--exitState 0:逃跑, 1:胜利, 2:失败
local function RemovePlayer(dtsInfo, charId, exitState)
	local teamInfo, charInfo = FindPlayer(dtsInfo, charId)
	if charInfo then
		--TestPrint("删除玩家 charId: ".. charId .. "  exitState: " .. exitState)
		local char = g_GetPlayerInfo(charId)
		local areaNode = dtsInfo.m_AreaIndex[charInfo.m_AreaId]
		--删除玩家相关数据
		if charInfo.m_IsInScene then
			areaNode.m_PlayerCount = areaNode.m_PlayerCount - 1
			assert(areaNode.m_PlayerCount >= 0, "大逃杀区域内人数统计有误")
			if char then
				Gas2Gac:RetCloseFbActionWnd(char.m_Conn, _ActionName)
				Gas2Gac:RetDelQueueFbAction(char.m_Conn, _ActionName, true)
				--发一个消息,让客户端取消头顶文字的特殊显示
				Gas2Gac:RetSetSceneStateForClient(char.m_Conn, 0)--表示设置显示,0表示取消显示
			end
			
			if areaNode.FightTick then
				--if areaNode.m_PlayerCount <= 1 then
				--	StopCountDown(dtsInfo, areaNode)
				--else
					Gas2Gac:CloseGameCountdownWnd(char.m_Conn)
				--end
			end
		end
		
		if areaNode.m_PlayerCount == 0 then
			AreaListDelNode(dtsInfo, areaNode.m_AreaId)
		end
		teamInfo.m_PlayerTbl[charId] = nil
		dtsInfo.m_PlayerIndex[charId] = nil
		
		--删除队伍相关数据
		if not next(teamInfo.m_PlayerTbl) then
			if teamInfo.m_IsLive then
				dtsInfo.m_LiveTeamCount = dtsInfo.m_LiveTeamCount - 1
			end
			dtsInfo.m_TeamTbl[charInfo.m_TeamId] = nil
			UpdateAllClientInfo(dtsInfo)
		else
			assert(false, "目前大逃杀队伍里只要一个玩家,删除一个必然要为空")
		end
		
		if charInfo.m_IsInScene then
			local sceneId = dtsInfo.m_Scene.m_SceneId
			if dtsInfo.m_State == g_FbRoomState.BeginGame  then
				AddPlayerPoint(charId, sceneId, dtsInfo.m_LiveTeamCount, charInfo.m_KillCount, dtsInfo.m_AllTeamCount, exitState)
			else
				AddPlayerPoint(charId, sceneId, 1, 0, 1, exitState)
			end
		end
		
		if not next(teamInfo.m_PlayerTbl) then
			if dtsInfo.m_LiveTeamCount == 1 and dtsInfo.m_State == g_FbRoomState.BeginGame then
				DaTaoShaGameOver(dtsInfo)
			end
		end
		
		return true
	end
	return false
end

function OnDaTaoShaCreateRoom(scene, roomId, actionName, roomMembers)
	local dtsInfo = {}
	dtsInfo.m_RoomId = roomId
	dtsInfo.m_Scene = scene
	dtsInfo.m_PlayerIndex = {}	--通过charId 索引的玩家表
	dtsInfo.m_TeamTbl = {}			--队伍表
	dtsInfo.m_AreaIndex = {}		--通过areaId 索引的area表
	dtsInfo.m_AreaCount = 0			--当前使用的area数
	dtsInfo.m_LiveTeamCount = 0	--存活的队伍数
	dtsInfo.m_MaxKillCount = 0		--最多击杀数
	
	dtsInfo.m_ProcessTime = GetProcessTime()
	dtsInfo.m_LogTbl = {}
	table.insert(dtsInfo.m_LogTbl, "OnDaTaoShaCreateRoom, roomId:"..roomId..", sceneId:"..scene.m_SceneId..", serverId:"..g_CurServerId..", now:"..os.date("%c")..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	local areaId = 1
	for teamId, v in pairs(roomMembers) do
		dtsInfo.m_TeamTbl[teamId] = {}
		dtsInfo.m_TeamTbl[teamId].m_PlayerTbl = {}
		for _, charId in pairs(v) do
			
			table.insert(dtsInfo.m_LogTbl, "playerId:"..charId..", teamId:"..teamId)
			
			local charInfo = {}
			dtsInfo.m_PlayerIndex[charId] = charInfo
			dtsInfo.m_TeamTbl[teamId].m_PlayerTbl[charId] = charInfo
			charInfo.m_TeamId = teamId
			charInfo.m_AreaId = areaId
			charInfo.m_KillCount = 0
			
			Gas2GacById:RetIsJoinFbActionAffirm(charId, actionName, WaitEnterSceneTime, 0)
		end
		AreaListInsertNode(dtsInfo, areaId)
		areaId = areaId + 1
	end
	
	--创建传送trap
	local trapPos = CPos:new()
	for id, v in pairs(g_DaTaoShaPointMgr) do
		trapPos.x, trapPos.y = v["OutRoomPoint"][1], v["OutRoomPoint"][2]
		local trap = CreateServerTrap(scene, trapPos, "大逃杀传送点")
		trap.m_AreaId = id	
	end
--	for id, v in pairs(DaTaoShaTransport_Server) do
--		trapPos.x, trapPos.y = v["OutRoomPoint"][1], v["OutRoomPoint"][2]
--		local trap = CreateServerTrap(scene, trapPos, "大逃杀传送点")
--		trap.m_AreaId = id
--	end
	
	scene.m_DtsInfo = dtsInfo
	g_DaTaoShaRoomTbl[roomId] = dtsInfo
	dtsInfo.m_State = g_FbRoomState.PlayerCanIn
	RegisterOnceTick(dtsInfo.m_Scene, "DaTaoShaWaitJoinTick", WaitPlayerJoinEnd, 1000 * WaitJointTime, dtsInfo)
	dtsInfo.m_OpenTime = os.time()
end

function OnDestroyDaTaoShaScene(scene)
	local dtsInfo = scene.m_DtsInfo
	for i, area in pairs(dtsInfo.m_AreaIndex) do
		if area.FightTick then
			UnRegisterTick(area.FightTick)
			area.FightTick = nil
		end
	end
	g_DaTaoShaRoomTbl[dtsInfo.m_RoomId] = nil
end

function IsCanEnterDaTaShaScene(scene, charId)
	local dtsInfo = scene.m_DtsInfo
	if (dtsInfo.m_State == g_FbRoomState.PlayerCanIn or dtsInfo.m_State == g_FbRoomState.CountDown)
		and FindPlayer(dtsInfo, charId) then
		return true
	end
end

function OnDaTaoShaAddTeam(actionName, roomId, teamId, memberTbl)
	local dtsInfo = g_DaTaoShaRoomTbl[roomId]
	
	if dtsInfo then
		table.insert(dtsInfo.m_LogTbl, "OnDaTaoShaAddTeam, state:"..dtsInfo.m_State..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	end
	
	if dtsInfo and dtsInfo.m_State == g_FbRoomState.PlayerCanIn then
		--TestPrint("添加一支队伍 ")
		assert(dtsInfo[teamId] == nil)
		dtsInfo[teamId] = {}
		dtsInfo.m_TeamTbl[teamId] = {}
		dtsInfo.m_TeamTbl[teamId].m_PlayerTbl = {}
		local areaId = GetUnUsedAreaId(dtsInfo)
		local time = WaitEnterSceneTime - (os.time() - dtsInfo.m_OpenTime)
		if time < 5 then
			time = 5
		end
		for _, charId in pairs(memberTbl) do
			
			table.insert(dtsInfo.m_LogTbl, "playerId:"..charId..", teamId:"..teamId..", areaId:"..areaId)
			
			assert(dtsInfo.m_PlayerIndex[charId] == nil)
			local charInfo = {}
			dtsInfo.m_PlayerIndex[charId] = charInfo
			dtsInfo.m_TeamTbl[teamId].m_PlayerTbl[charId] = charInfo
			charInfo.m_TeamId = teamId
			charInfo.m_AreaId = areaId
			charInfo.m_KillCount = 0

			Gas2GacById:RetIsJoinFbActionAffirm(charId, actionName, time, 0)
		end
		AreaListInsertNode(dtsInfo, areaId)
	end
end

function GetDaTaoShaoEnterPos(roomId, charId)
	local dtsInfo = g_DaTaoShaRoomTbl[roomId]
	if dtsInfo then
		local teamInfo, charInfo = FindPlayer(dtsInfo, charId)
		if charInfo then
			
			table.insert(dtsInfo.m_LogTbl, "GetDaTaoShaoEnterPos, playerId:"..charId..", areaId:"..tostring(charInfo.m_AreaId)..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
			if not g_DaTaoShaPointMgr[charInfo.m_AreaId] then
				OnDtsLogErr(dtsInfo, "g_DaTaoShaPointMgr异常", "playerId:"..charId)
				--LogErr("g_DaTaoShaPointMgr异常 ".. charId.." "..tostring(charInfo.m_AreaId))
			end
			
			return dtsInfo.m_Scene.m_SceneId, g_DaTaoShaPointMgr[charInfo.m_AreaId]["BornPoint"]
		end
	end
end

function OnPlayerIntoDaTaoShaScene(player)
	MultiServerCancelAllAction(player.m_uID)
	local dtsInfo = player.m_Scene.m_DtsInfo
	local teamInfo, charInfo = FindPlayer(dtsInfo, player.m_uID)
	assert(teamInfo and charInfo, "玩家非法进入大逃杀场景")
	
	table.insert(dtsInfo.m_LogTbl, "OnPlayerIntoDaTaoShaScene, playerId:"..player.m_uID..", areaId:"..charInfo.m_AreaId..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	local areaNode = dtsInfo.m_AreaIndex[charInfo.m_AreaId]
	areaNode.m_PlayerCount = areaNode.m_PlayerCount + 1
	charInfo.m_IsInScene = true
	if not teamInfo.m_IsLive then
		teamInfo.m_IsLive = true
		dtsInfo.m_LiveTeamCount = dtsInfo.m_LiveTeamCount + 1
	end
	
	player:SetGameCamp(charInfo.m_AreaId)
	player:PlayerDoFightSkillWithoutLevel("大逃杀改变恢复速度")
	--TestPrint("玩家进入场景, 当前场景剩余人数:", dtsInfo.m_LiveTeamCount)
	local time = BeginGameTime - (os.time() - dtsInfo.m_OpenTime)
	Gas2Gac:RetSetJoinBtnEnable(player.m_Conn,_ActionName)
	Gas2Gac:RetOpenFbActionWnd(player.m_Conn, _ActionName, time)
	--发一个消息,让客户端所有的显示都为角斗士
	Gas2Gac:RetSetSceneStateForClient(player.m_Conn, 1)--1表示为大逃杀副本,0表示取消显示
	UpdateAllClientInfo(dtsInfo)
	
	local data = {}
	data["PlayerId"] = player.m_uID
	data["ActionName"] = _ActionName
	CallAccountAutoTrans(player.m_Conn.m_Account, "JoinActionDB", "OnEnterRoom", nil, data, _ActionName)
end

function OnPlayerChangeOutDaTaoShaScene(player, sceneId)
	local scene = g_SceneMgr:GetScene(sceneId)
	local dtsInfo = scene.m_DtsInfo
	player:SetGameCamp(0)
	player:PlayerDoFightSkillWithoutLevel("清除大逃杀状态")
	player:PlayerDoFightSkillWithoutLevel("大逃杀撤销改变恢复速度")
	Gas2GacById:RetCloseCountScoreWnd(player.m_uID, _ActionName)
	--TestPrint("玩家离开场景, 当前场景剩余人数:", dtsInfo.m_LiveTeamCount)
	ExitDaTaoShaGame(dtsInfo, player.m_uID, 0, true)
end

function PlayerOffLineDaTaoShaFb(player)
	local dtsInfo = player.m_Scene.m_DtsInfo
	
	table.insert(dtsInfo.m_LogTbl, "PlayerOffLineDaTaoShaFb, playerId:"..player.m_uID..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	ExitDaTaoShaGame(dtsInfo, player.m_uID, 0, true)
end

function GetTransToArea(dtsInfo, curAreaNode)
	--return curAreaNode.m_NextNode.m_AreaId   --连续传送 现在改成随机传送
	local step = math.random(1, dtsInfo.m_AreaCount - 1)
	local temp = curAreaNode
	for i = 1, step do
		temp = temp.m_NextNode
	end
	return temp
end

function DaTaoShaTransport(trap, player)
	local dtsInfo = player.m_Scene.m_DtsInfo
	local _, charInfo = FindPlayer(dtsInfo, player.m_uID)
	if not charInfo then
		table.insert(dtsInfo.m_LogTbl, "DaTaoShaTransport, return because charInfo:false, time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
		return
	end
	
	local curAreaNode = dtsInfo.m_AreaIndex[trap.m_AreaId]
	if curAreaNode == nil then
		OnDtsLogErr(dtsInfo, "大逃杀curAreaNode异常", "playerId:"..player.m_uID.." areaId:"..trap.m_AreaId)
	end
	if dtsInfo.m_State == g_FbRoomState.BeginGame and dtsInfo.m_AreaCount > 1 and curAreaNode.m_PlayerCount <= 1 then
		assert(trap.m_AreaId == charInfo.m_AreaId, "大逃杀中玩家所在区域信息和实际区域不一致")
		
		local transToArea = GetTransToArea(dtsInfo, curAreaNode)
		local pos = CPos:new()
		pos.x = g_DaTaoShaPointMgr[transToArea.m_AreaId]["IntoRoomPoint"][1]
		pos.y = g_DaTaoShaPointMgr[transToArea.m_AreaId]["IntoRoomPoint"][2]
		if player:SetGridPosByTransport(pos) == 0 then
			
			table.insert(dtsInfo.m_LogTbl, "DaTaoShaTransport, playerId:"..player.m_uID..", oldAreaId:"..charInfo.m_AreaId..", tarAreaId:"..transToArea.m_AreaId..", areaPlayerCount:"..transToArea.m_PlayerCount..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
			
			curAreaNode.m_PlayerCount = curAreaNode.m_PlayerCount - 1
			transToArea.m_PlayerCount = transToArea.m_PlayerCount + 1
			if curAreaNode.m_PlayerCount == 0 then
				AreaListDelNode(dtsInfo, charInfo.m_AreaId)
			end
			
			charInfo.m_AreaId = transToArea.m_AreaId
			if transToArea.m_PlayerCount >= 2 then
				AddCountDownTime(dtsInfo, transToArea, FightingTime)
			end
		end
	else
		if dtsInfo.m_State ~= g_FbRoomState.BeginGame then
			MsgToConn(player.m_Conn,3312)
		elseif dtsInfo.m_AreaCount <= 1 then
			MsgToConn(player.m_Conn,3310)
		else
			MsgToConn(player.m_Conn,3313)
		end
	end
end

function PlayerDeadInDaTaoSha(killer, dead)
	local dtsInfo = dead.m_Scene.m_DtsInfo
	local deadTeam, deadInfo = FindPlayer(dtsInfo, dead.m_uID)
	
	if killer and (killer.m_uID ~= dead.m_uID) then
		local killerTeam, killerInfo = FindPlayer(dtsInfo, killer.m_uID)
		if killerInfo then
			local skillLv = 1
			local rNum = math.random(1,100)
			if rNum <= 40 then
				skillLv = 1
			elseif rNum <= 70 then
				skillLv = 2
			elseif rNum <= 90 then
				skillLv = 3
			else
				skillLv = 4
			end
			local buffSkillName = "大逃杀状态"..skillLv
			killer:PlayerDoFightSkillWithoutLevel(buffSkillName)
			killerInfo.m_KillCount = killerInfo.m_KillCount + 1
			if killerInfo.m_KillCount > dtsInfo.m_MaxKillCount then
				dtsInfo.m_MaxKillCount = killerInfo.m_KillCount
			end
		end
	end
	RemovePlayer(dtsInfo, dead.m_uID, 2)
end

local function LeaveDaTaoShaScene(player)
	--TestPrint("通知玩家切出场景")
	if player.m_Scene.m_DtsInfo then --说明是在大逃杀副本中
		local sceneName = player.m_MasterSceneName
		local pos = player.m_MasterScenePos 
		--TestPrint(sceneName, pos.x, pos.y)
		ChangeSceneByName(player.m_Conn,sceneName,pos.x,pos.y)
	end
end

function ExitDaTaoShaGame(dtsInfo, charId, exitState, mustDelDb)
	--TestPrint("ExitDaTaoShaGame", charId, exitState, mustDelDb)
	table.insert(dtsInfo.m_LogTbl, "ExitDaTaoShaGame, playerId:"..charId..", time:"..GetProcessTime()-dtsInfo.m_ProcessTime)
	
	local IsSucc = RemovePlayer(dtsInfo, charId, exitState)
	if mustDelDb then
		Gas2GacById:RetDelQueueFbAction(charId, _ActionName, true)
		
		local data = {}
		data["CharId"] = charId
		data["RoomId"] = dtsInfo.m_RoomId
		CallDbTrans("JoinActionDB", "CancelActionByRoom", nil, data, charId, _ActionName)
	end
	local player = g_GetPlayerInfo(charId)
	if IsCppBound(player) and exitState == 0 and IsSucc then
		LeaveDaTaoShaScene(player)
	end
end

function GetCharTblInArea(dtsInfo, area)
	local charTbl = {}
	for charId, charInfo in pairs(dtsInfo.m_PlayerIndex) do
		if charInfo.m_IsInScene and charInfo.m_AreaId == area.m_AreaId then
			table.insert(charTbl, charId)
		end
	end
	return charTbl
end

function OnFightCountDownTick(tick, dtsInfo, area)
	local charTbl = GetCharTblInArea(dtsInfo, area)
	local fun = function(id1, id2)
		local player1 = g_GetPlayerInfo(id1)
		local player2 = g_GetPlayerInfo(id2)
		return player1:CppGetHP() < player2:CppGetHP()
	end
	
	table.sort(charTbl, fun)
	
	for _, charId in ipairs(charTbl) do
		local player = g_GetPlayerInfo(charId)
		if IsCppBound(player) then
			player:CppSuicide()
			--RemovePlayer(dtsInfo, charId, 2)
		end
	end
end

function AddCountDownTime(dtsInfo, area, time)
	if area.FightTick then
		UnRegisterTick(area.FightTick)
		area.FightTick = nil
		area.FightingTime = area.FightingTime + time
	else
		area.FightingTime = time
		area.StartFightTime = os.time()
	end
	local tickTime = area.FightingTime - (os.time() - area.StartFightTime)
	area.FightTick = RegisterTick("DaTaoShaFightTick", OnFightCountDownTick, tickTime * 1000, dtsInfo, area)
	
	local charTbl = GetCharTblInArea(dtsInfo, area)
	for _, charId in pairs(charTbl) do
		local player = g_GetPlayerInfo(charId)
		if IsCppBound(player) then
			Gas2Gac:SetCountdownTime(player.m_Conn, tickTime)
		end
	end
end

function StopCountDown(dtsInfo, area)
	if area.FightTick then
		UnRegisterTick(area.FightTick)
		area.FightTick = nil
	end
	local charTbl = GetCharTblInArea(dtsInfo, area)
	for _, charId in pairs(charTbl) do
		local player = g_GetPlayerInfo(charId)
		if IsCppBound(player) then
			Gas2Gac:CloseGameCountdownWnd(player.m_Conn)
		end
	end
	area.FightingTime = nil
	area.StartFightTime = nil
end


function Gac2Gas:JoinDaTaoSha(Conn)
	if SearchClosedActionTbl(_ActionName) then
		MsgToConn(Conn,3521)--活动已经关闭,不可以报名
		return
	end
	MultiServerJoinAction(Conn, _ActionName)
end

function Gac2Gas:ShowDaTaoShaWnd(Conn, NpcID)
	if not CheckAllNpcFunc(Conn.m_Player,NpcID,"角斗场报名处") then
		return
	end
	if not CheckActionIsBeginFunc(_ActionName) then
		MsgToConn(Conn,3520)--活动还没有开放,不可以报名
		return
	end
	Gas2Gac:RetShowDaTaoShaWnd(Conn, NpcID)
end

function Gac2Gas:ExitDTSFbAction(Conn)
	LeaveDaTaoShaScene(Conn.m_Player)
end

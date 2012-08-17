cfg_load "fb_game/FbActionDirect_Common"

g_AllActionOpenTimeTbl = {}

--从类似 "9:24" 的字符串中获取相对当天 0:00 点经过的秒数
local function GetOffsetSecond(strTime)
	local index1 = string.find(strTime, ":")
	local index2 = string.find(strTime, ":", index1 + 1) or 0
	local hour = tonumber(string.sub(strTime, 1, index1 - 1))
	local min = tonumber(string.sub(strTime, index1 + 1, index2 -1))
	local sec = 0
	if index2 ~= 0 then
		sec = tonumber(string.sub(strTime, index2 + 1, -1))
	end
	assert(hour and min and sec, "时间格式错误")
	return (hour * 60 + min) * 60  + sec
end


--配置表时间转换
--for actionName, v in pairs(FbActionDirect_Common)  do
for _, actionName in pairs(FbActionDirect_Common:GetKeys()) do
	g_AllActionOpenTimeTbl[actionName] = {}
	local DateTbl = GetCfgTransformValue(true, "FbActionDirect_Common", actionName, "StartTime")

	if not DateTbl.wday then
		g_AllActionOpenTimeTbl[actionName].wdayTbl = {1,2,3,4,5,6,7}
	else
		g_AllActionOpenTimeTbl[actionName].wdayTbl = DateTbl.wday
	end
	if not DateTbl.time or #DateTbl.time == 0 then
		g_AllActionOpenTimeTbl[actionName].timeTbl = {0}
		g_AllActionOpenTimeTbl[actionName].length = 24*60*60
	else
		local timeTbl = {}
		for _, timeStr in pairs(DateTbl.time) do
			table.insert(timeTbl, GetOffsetSecond(timeStr))
		end
		g_AllActionOpenTimeTbl[actionName].timeTbl = timeTbl
		g_AllActionOpenTimeTbl[actionName].length =  FbActionDirect_Common(actionName, "ContinualMin") * 60
	end
end


--判断是否在开放时间里
local function IsInOpenTime(wdayTbl, timeTbl, length)
	local curDate =  os.date("*t")
	local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
	local curWday = ((curDate.wday == 1) and 7 or curDate.wday-1)  --将系统星期换成填表星期
	
	for _, wday in pairs(wdayTbl) do
		for _, offset in pairs(timeTbl) do
			local endOffset = offset + length
			local day = (curWday >= wday and curWday - wday) or curWday + 7 - wday --跨越天数
			local curOffsetSecond = todayOffsetSecond + day * 86400
			
			if curOffsetSecond >= offset and curOffsetSecond < endOffset then
					return true, endOffset - curOffsetSecond
			end
		end
	end
	return false
end

function CheckActionIsBeginFunc(actionName)
	if g_FreeActionTime then
		return true
	end
	local openData = g_AllActionOpenTimeTbl[actionName]
	if not openData then
		return true
	end
 	return IsInOpenTime(openData.wdayTbl, openData.timeTbl, openData.length)
--	return true
end



function MultiServerJoinAction(Conn, actionName, isAutoTeam)
	if not CheckActionIsBeginFunc(actionName) then
		MsgToConn(Conn, 191002, actionName)
		return
	end
	
	local function CallBack(args)  --如前 只有报名失败才有返回值
		if args and next(args) then
			MsgToConn(Conn, unpack(args))
		end
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	data["ActionName"] = actionName
	data["IsAutoTeam"] = isAutoTeam
	CallAccountManualTrans(Conn.m_Account, "JoinActionDB", "JoinAction", CallBack, data, actionName)
end

function MultiServerCancelAction(Conn, actionName)
	if Conn.m_Player.m_IsChangeSceneing then
		return
	end
--print("MultiServerCancelAction", Conn.m_Player.m_uID)
	local charId = Conn.m_Player.m_uID
	local charName = Conn.m_Player.m_Properties:GetCharName()
	
	local data = {}
	data["CharId"] = charId
	data["ActionName"] = actionName
	data["CharName"] = charName
	CallAccountManualTrans(Conn.m_Account, "JoinActionDB", "CancelAction", nil, data, actionName)
end



--玩家掉线时,取消所有的报名
function MultiServerCancelAllAction(charId)
	local data = {}
	data["CharId"] = charId
	local function CancelAllActionFun(ActionTbl)
		data["ActionTbl"] = ActionTbl
		CallDbTrans("JoinActionDB", "CancelAllAction", nil, data, unpack(ActionTbl))
	end
	CallDbTrans("JoinActionDB", "GetAllActionName", CancelAllActionFun, data, charId)
end

function MultiServerEnterActionScene(Conn, actionName)
	local charId = Conn.m_Player.m_uID
	local data = {}
	data["CharId"] = charId
	data["ActionName"] = actionName
	CallAccountManualTrans(Conn.m_Account, "JoinActionDB", "EnterAction", CallBack, data, actionName)
end





-----------------------------------------------------------------------
function Db2Gas:CreateActionRoom(result)
	--print("Db2Gas:CreateActionRoom")
	local actionName = result["actionName"]
	local roomId = result["roomId"]
	local sceneId =	result["sceneId"]
	local sceneName =	result["sceneName"]
	local otherArg =	result["otherArg"]
	local roomMembers =	result["roomMembers"]
	local captainsTbl =	result["captainsTbl"]
	local JiFenSaiInfo = result["JiFenSaiInfo"]
	local teamIndexTbl = result["teamIndexTbl"]
	
	if result["PlayerInfo"] then
		for _,Info in pairs(result["PlayerInfo"]) do
			CCharacterMediator.SendPlayerInfoToGac( CreateDbResultShellByTable(Info["Res"]) )
			CCharacterMediator.SendFightingEvaluationToGac( CreateDbResultShellByTable(Info["FightingEvaluation"]) )
		end
	end
	
	local scene = g_SceneMgr:CreateScene(sceneName, sceneId, otherArg)
	if MatchGame_Common(actionName) then
		OnMatchGameCreateRoom(scene, roomId, actionName, roomMembers, captainsTbl, teamIndexTbl)
	elseif JuQingTransmit_Common(actionName) then
		CScenarioExploration.CreateRoom(scene, roomId, actionName, roomMembers, captainsTbl)
	elseif actionName == "角斗场" then
		OnDaTaoShaCreateRoom(scene, roomId, actionName, roomMembers)
	elseif actionName == "竞技场" then
		OnJiFenSaiCreateRoom(scene, roomId, actionName, roomMembers, JiFenSaiInfo)
	end
end


function Db2Gas:AddActionTeam(result)
	--print("AddActionTeam")
	local actionName = result["actionName"]
	local roomId = result["roomId"]
	local actionTeamId = result["actionTeamId"]
	local teamIndex = result["teamIndex"]
	local captainName = result["captainName"]
	local memberTbl = result["memberTbl"]

	if MatchGame_Common(actionName) then
		OnMatchGameAddTeam(actionName, roomId, actionTeamId, teamIndex, memberTbl, captainName)
	elseif actionName == "角斗场" then
		OnDaTaoShaAddTeam(actionName, roomId, actionTeamId, memberTbl)
	end
end


function Gas2GasDef:PlayerEnterActionScene(Conn, charId, roomId, actionName)
	if MatchGame_Common(actionName) then
		local game = g_MatchGameMgr.m_GameTbl[actionName]
		local room = game and game:GetRoom(roomId)
		if not (room and room:IsCanEnterScene(charId)) then
			return
		end
		local teamTbl = room:FindPlayer(charId)
		if not teamTbl then
			return
		end
		local pos = room:GetTeamEnterPos(teamTbl)
		Gas2GasAutoCall:RetSrcServerChangeScene(Conn, charId, room.m_SceneId, pos[1], pos[2])
	elseif JuQingTransmit_Common(actionName) then
		local sceneId, pos = CScenarioExploration.GetTeamEnterPos(roomId, actionName)
		if sceneId then
			Gas2GasAutoCall:RetSrcServerChangeScene(Conn, charId, sceneId, pos[1], pos[2])
		end
	elseif actionName == "角斗场" then
		local sceneId, pos = GetDaTaoShaoEnterPos(roomId, charId)
		if sceneId then
			Gas2GasAutoCall:RetSrcServerChangeScene(Conn, charId, sceneId, pos[1], pos[2])
		end
	elseif actionName == "竞技场" then
		local sceneId, pos = GetJiFenSaiEnterPos(roomId, charId)
		if sceneId then
			Gas2GasAutoCall:RetSrcServerChangeScene(Conn, charId, sceneId, pos[1], pos[2])
		end
--	elseif actionName == "抢夺资源点" then
--		GetRobResEnterPos(Conn, roomId, charId)
	end
end


local function PlayerCancelAction(charId, roomId, actionName)
	if MatchGame_Common(actionName) then
		local game = g_MatchGameMgr.m_GameTbl[actionName]
		local room = game and game:GetRoom(roomId)
		if room then
			room:ExitGame(charId, false)
		end
	elseif JuQingTransmit_Common(actionName) then
		CScenarioExploration.ExitGame(roomId, charId)
	elseif actionName == "角斗场" then
		local dtsInfo = g_DaTaoShaRoomTbl[roomId]
		if dtsInfo then
			ExitDaTaoShaGame(dtsInfo, charId, 0, false)
		end
	elseif actionName == "竞技场" then
		local jfsInfo = g_JiFenSaiRoomTbl[roomId]
		if jfsInfo then
			ExitJiFenSaiGame(jfsInfo, charId, 3, false)--3表示逃跑
		end
	end
end

function Db2Gas:PlayerCancelAction(result)
	--print("PlayerCancelAction")
	local charId = result["charId"]
	local roomId = result["roomId"]
	local actionName = result["actionName"]
	PlayerCancelAction(charId, roomId, actionName)
end

function Db2Gas:TeamCancelAction(result)
	--print("TeamCancelAction")
	local roomId = result["roomId"]
	local actionName = result["actionName"]
	local cancelTbl = result["cancelTbl"]
	for _,charId in pairs(cancelTbl) do
		PlayerCancelAction(charId, roomId, actionName)
	end
end

function DbCallBack:OnLeaveTeamCancelAction(charId, charName, ActionTbl)
	for actionName, tbl in pairs(ActionTbl) do
		local memberTbl = tbl[1]
		local charGroupMembers = tbl[2]
		local roomId = tbl[3]
		local room = g_MatchGameMgr.m_GameTbl[actionName]:GetRoom(roomId)
		local team = room and room:FindPlayer(charId)
		
		for _, id in pairs(memberTbl) do
			if room and team then
				room:RemovePlayer(team, id)
			end
			if id ~= charId then
				MsgToConnById(id, 191023, charName, actionName)
			else
				MsgToConnById(id, 191024, actionName)
			end
			Gas2GacById:RetDelQueueFbAction(id, actionName, id == charId or charGroupMembers[id])
		end
	end
end

function DbCallBack:OnReleaseTeamCancelAction(charId, ActionTbl)
	for actionName, tbl in pairs(ActionTbl) do
		local memberTbl = tbl[1]
		local charGroupMembers = tbl[2]
		local roomId = tbl[3]
		local room = g_MatchGameMgr.m_GameTbl[actionName]:GetRoom(roomId)
		local team = room and room:FindPlayer(charId)
		
		for _,id in pairs(memberTbl) do
			if room and team then
				room:RemovePlayer(team, id)
			end
			MsgToConnById(id, 191036, actionName)
			Gas2GacById:RetDelQueueFbAction(id, actionName, id == charId or charGroupMembers[id])
		end
	end
end

----进副本后,通知删除所需的门票
--function DbCallBack:OnDeleteActionItem(charId, resItemTbl)
--	local type,name,num,res = resItemTbl[1],resItemTbl[2],resItemTbl[3],resItemTbl[4]
--	local Player = g_GetPlayerInfo(charId)
--	CRoleQuest.delete_item_DB_Ret(Player,type,name,num,res)
--end

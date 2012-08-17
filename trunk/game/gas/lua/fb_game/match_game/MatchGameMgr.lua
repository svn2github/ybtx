gas_require "fb_game/match_game/MatchGameMgrInc"
gac_gas_require "framework/common/CAlarmClock"
gas_require "fb_game/match_game/MatchGame"
gas_require "fb_game/match_game/MatchGameRoom"
gac_gas_require "activity/fb/MercenaryEducateActCfgCheck"
cfg_load "fb_game/MatchGame_Common"
cfg_load "fb_game/MatchGameCount_Server"
cfg_load "fb_game/JoinCountLimit_Server"
cfg_load "fb_game/MatchGameCombinedCount"

g_MatchGameMgr = CMatchGameMgr:new()
g_MatchGameMgr.m_CfgData = {}  --存放转换完的配置表(对应较复杂的列, 简单的列内容仍去 MatchGame_Common 中访问),
g_MatchGameMgr.m_GameTbl = {} -- 所有游戏表

g_MatchGameMgr.m_WaitPlayerTime = 20 --等待玩家继续报名时间
g_MatchGameMgr.m_WaitGameBeginTime = 60 --停止玩家报名 到 游戏倒计时开始 的时间
g_MatchGameMgr.m_CountDownTime = 10 --游戏倒计时 时间
g_MatchGameMgr.m_WaitEnterSceneTime = g_MatchGameMgr.m_WaitPlayerTime + g_MatchGameMgr.m_WaitGameBeginTime + g_MatchGameMgr.m_CountDownTime
g_MatchGameMgr.m_CloseRoomTime = 30 --游戏结束后, 等待关闭房间的时间

g_MatchGameCountCfg = {} --存放 MatchGameCount_Server换后的表
g_MatchGameCombinedCount = {}
g_MatchGameSpecialCount = {}
AddCheckLeakFilterObj(g_MatchGameCountCfg)
AddCheckLeakFilterObj(g_MatchGameMgr.m_CfgData)


--要在场景身上挂 <副本游戏> 相关数据全部通过这个函数设置,方便管理,查看
function SetSceneMatchGameData(scene, name, data )
	if scene.m_MatchGameData == nil then
		scene.m_MatchGameData = {}
	end
	scene.m_MatchGameData[name] = data
end


--实际就是把 由  MatchGame_Common 中的复杂字符串列做个转换 
function CMatchGameMgr:LoadCfg()

	for _, gameName in pairs(MatchGame_Common:GetKeys()) do
		local tbl = MatchGame_Common(gameName)
		self.m_CfgData[gameName] = {}
		local game = self.m_CfgData[gameName]
		game.IsTeamMode = tbl("SingleOrTeam") == 2
		game.TempBag = tbl("TempBag")
		game.EquipDura = tbl("EquipDura")
		game.IsAutoTeam = tbl("AutoTeamPlayerNumber") ~= ""
		game.IsTeamShowName = tbl("TeamShowName") ~= ""
		if tbl("OpenTime") ~= "" then
			game.OpenTime = loadstring("return {" .. tbl("OpenTime") .. "}")()
		end
		local joinNpcName = loadstring("return {" .. tbl("JoinNpcName") .. "}")()
		if tbl("MatchGameCarom") ~= "" then
			game.MatchGameCarom = loadstring("return " .. tbl("MatchGameCarom"))()
		end
		if tbl("CountDownTrigger") ~= "" then
			game.CountDownTriggerTbl = loadstring("return {" .. tbl("CountDownTrigger") .. "}")()
		end
		if tbl("GameBeginTrigger") ~= "" then
			game.BeginTriggerTbl = loadstring("return {" .. tbl("GameBeginTrigger") .. "}")()
		end
		
		local FightStatisticTbl = {
		["伤害比例"] = true,
		["总伤害"] = true,
		["治疗比例"] = true,
		["总治疗"] = true,
		["伤害积分"] = true,
		}
		local StatisticsTbl = GetCfgTransformValue(true, "MatchGame_Common", gameName, "Statistics")
		for i,v in pairs(StatisticsTbl) do
			if FightStatisticTbl[v] then
				game.IsOpenFightStatistic = true
			end
			if v == "伤害积分" then
				game.IsAddFightScore = true
				break 
			end
		end
		
		game.JoinNpcTbl = {}
		for _, npcName in pairs(joinNpcName) do
			game.JoinNpcTbl[npcName] = true
		end
		
		local ScoreBoardTbl = GetCfgTransformValue(true,"MatchGame_Common", gameName,"ScoreBoard")
		local SynScoreTbl = {}
		for i,name in ipairs(ScoreBoardTbl) do
			if name ~= "排名" and name ~= "玩家名称" then
				SynScoreTbl[name] = i
			end
			if not game.IsSynStatistic and FightStatisticTbl[name] then
				--(name == "伤害比例" or name == "治疗比例" or name == "总伤害" or name == "总治疗") then
				game.IsOpenFightStatistic = true
				game.IsSynStatistic = true
			end
		end
		game.SynScoreTbl = SynScoreTbl
		
		
		local ValueFuncTbl = {
		["定时"] = "room:GetValue_Time()",
		["剩余队伍数"] = "room:Value_ExistTeamNum()",
		["队伍剩余人数"] = "room:Value_TeamExistPlayerNum(team)",
		["剩余队伍分数最高"] = "team == room:Value_ExistTeamMaxScore()",
		["所有队伍分数最高"] = "team == room:Value_AllTeamMaxScore()",
		["总分"] = "room:GetValue_AllScore(team)"
		}
		
		--结束条件处理
		local overStr = tbl("OverType")
		local overTypeTbl = {}
		local nStart
		local nEnd = 0
		local strNumber
		local name
		while true do
			nStart, nEnd = string.find(overStr,"\".-\"",nEnd + 1)
			if nStart then
				name = string.sub(overStr, nStart+1, nEnd-1)
				if name == "定时" then
					nStart, nEnd = string.find(overStr,"%d+",nEnd + 1)
					assert(nStart)
					strNumber = string.sub(overStr, nStart, nEnd)
					overTypeTbl[name] = tonumber(strNumber)
					game.GameTime = overTypeTbl[name]
				else
					overTypeTbl[name] = true
				end
			else
				break
			end
		end
		local overFunStr = overStr
		for name in pairs(overTypeTbl) do
			local fieldStr = "\"" .. name .. "\""
			if ValueFuncTbl[name] then
				overFunStr = string.gsub(overFunStr, fieldStr , ValueFuncTbl[name])
			else
				overFunStr = string.gsub(overFunStr, fieldStr , "arg:GetCount(" .. fieldStr .. ")")
			end
		end
		overFunStr = "local function IsOver(arg, team, room)  return (" .. overFunStr .. ") end return IsOver"
		game.IsOverFun = loadstring(overFunStr)()
		
		--积分处理
		local countNameStr = MatchGameCombinedCount(tbl("CountName"), "Content")
		local scoreTbl = {}
		nEnd = 0
		nStart = 0
		while true do
			nStart, nEnd = string.find(countNameStr,"\".-\"",nEnd + 1)
			if nStart then
				name = string.sub(countNameStr, nStart+1, nEnd-1)
				scoreTbl[name] = true
			else
				break
			end
		end
		local countNameFunStr = countNameStr
		
		for name in pairs(scoreTbl) do
			local fieldStr = "\"" .. name .. "\""
			countNameFunStr = string.gsub(countNameFunStr, fieldStr , "arg:GetScore(" .. fieldStr .. ")")
		end
		--print(countNameFunStr)
		countNameFunStr = "local function ScoreFun(arg)  return (" .. countNameFunStr .. ") end return ScoreFun"
		--print(countNameFunStr)
		--game.ScoreTbl = scoreTbl
		game.ScoreFun = loadstring(countNameFunStr)()
		
		if tbl("WinCondition") ~= "" then
			local WinConditionTbl = {}
			nEnd = 0
			nStart = 0
			while true do
				nStart, nEnd = string.find(tbl("WinCondition"),"\".-\"",nEnd + 1)
				if nStart then
					name = string.sub(tbl("WinCondition"), nStart+1, nEnd-1)
					WinConditionTbl[name] = true
				else
					break
				end
			end
			local WinConditionFunStr = tbl("WinCondition")
			
			for name in pairs(WinConditionTbl) do
				local fieldStr = "\"" .. name .. "\""
				if ValueFuncTbl[name] then
					WinConditionFunStr = string.gsub(WinConditionFunStr, fieldStr , ValueFuncTbl[name])
				else
					WinConditionFunStr = string.gsub(WinConditionFunStr, fieldStr , "arg:GetCount(" .. fieldStr .. ")")
				end
			end
			WinConditionFunStr = "local function ScoreFun(arg, team, room)  return (" .. WinConditionFunStr .. ") end return ScoreFun"
			game.WinFun = loadstring(WinConditionFunStr)()
		end
		
	
		if tbl("EnterItem") ~= "" then
			game.EnterItem = loadstring("return {" .. tbl("EnterItem") .. "}")()
		end
		if tbl("DelItem") ~= "" then
			game.DelItem = loadstring("return {" .. tbl("DelItem") .. "}")()
		end
		if tbl("EnterSceneDoSkill") ~= "" then
			local skillTbl = loadstring("return {" .. tbl("EnterSceneDoSkill") .. "}")()
			game.EnterSceneDoSkill = skillTbl.AllDo
			game.EnterSceneTeamDoSkill = skillTbl.TeamDo
		end
		if tbl("LeaveSceneDoSkill") ~= "" then
			game.LeaveSceneDoSkill = loadstring("return {" .. tbl("LeaveSceneDoSkill") .. "}")()
		end
		if tbl("EnterSceneAddTempSkill") ~= "" then
			game.EnterSceneAddTempSkill = loadstring("return {" .. tbl("EnterSceneAddTempSkill") .. "}")()
		end
		if tbl("LeaveSceneDeleteTempSkill") ~= "" then
			game.LeaveSceneDeleteTempSkill = loadstring("return {" .. tbl("LeaveSceneDeleteTempSkill") .. "}")()
		end
		

		game.AwardItem = loadstring("return {" .. tbl("AwardItem") .. "}")()
		
		local enterPoint = loadstring("return {" .. tbl("EnterPoint") .. "}")()
		game.EnterType = enterPoint[1]
		if game.EnterType == 1 then -- 方形区域随机
--			game.EnterCenter = enterPoint[2]
			local pos = GetScenePositionTbl(enterPoint[2])
			game.EnterCenter = pos
			game.EnterRadius = enterPoint[3]
		elseif game.EnterType == 2 then -- 固定点随机
			game.EnterPointTbl = {}
			for i = 1, #enterPoint[2] do
				local pos = GetScenePositionTbl(enterPoint[2][i])
				table.insert(game.EnterPointTbl, pos)
			end
			game.EnterPointCount = #(game.EnterPointTbl)
		end
		
		if tbl("AwardExp") ~= "" then
			local funStr = "local function GetExp(I, L, J, T)  return (" .. tbl("AwardExp") .. ") end return GetExp"
			game.ExpFun = loadstring(funStr)()
		end
		if tbl("AwardMoney") ~= "" then
			local funStr = "local function GetMoney(I, L, J, T)  return (" .. tbl("AwardMoney") .. ") end return GetMoney"
			game.MoneyFun = loadstring(funStr)()
		end
		if tbl("AwardSoul") ~= "" then
			local funStr = "local function GetSoul(I, L, J, T)  return (" .. tbl("AwardSoul") .. ") end return GetSoul"
			game.SoulFun = loadstring(funStr)()
		end
	end
	
	for _, countName in pairs(MatchGameCount_Server:GetKeys()) do	
		local tbl = MatchGameCount_Server(countName)
		if g_MatchGameCountCfg[tbl("TriggerType")] == nil then
			g_MatchGameCountCfg[tbl("TriggerType")] = {}
		end
		g_MatchGameCountCfg[tbl("TriggerType")][tbl("Arg")] = tbl
	end
	
	for _, CombinedName in pairs(MatchGameCombinedCount:GetKeys())do
		local content =  MatchGameCombinedCount(CombinedName, "Content")
		local tbl = {}
		local nEnd = 0
		local nStart = 0
		while true do
			nStart, nEnd = string.find(content,"\".-\"",nEnd + 1)
			if nStart then
				local name = string.sub(content, nStart+1, nEnd-1)
				tbl[name] = true
			else
				break
			end
		end
		
		local funStr = content
		for name in pairs(tbl) do
			local fieldStr = "\"" .. name .. "\""
			funStr = string.gsub(funStr, fieldStr , "arg:GetCount(" .. fieldStr .. ")")
		end
		funStr = "local function StatisticsFun(arg)  return (" .. funStr .. ") end return StatisticsFun"
		
		g_MatchGameCombinedCount[CombinedName] = loadstring(funStr)()
	end
	
	
	local SpecialFunDescribe = {
		["伤害比例"] = "room:GetHurtRate(teamInfo, charInfo, type)",
		["总伤害"] = "room:GetHurt(teamInfo, charInfo, type)",
		["治疗比例"] = "room:GetHealRate(teamInfo, charInfo, type)",
		["总治疗"] = "room:GetHeal(teamInfo, charInfo, type)",
		["伤害积分"] = "room:GetFightScore(teamInfo, charInfo, type)",
	}
	
	for name, str in pairs(SpecialFunDescribe) do
		local funStr = "return function(room, teamInfo, charInfo, type)  return (" .. str .. " ) end"
		g_MatchGameSpecialCount[name] = loadstring(funStr)()
	end
end

--服务器启动调用
function CMatchGameMgr:Init()
	self:LoadCfg()
	
	--初始游戏
	for gameName, tbl in pairs(self.m_CfgData ) do
		self.m_GameTbl[gameName] = CMatchGame:new()
		self.m_GameTbl[gameName]:Init(gameName, tbl)
	end
end
g_MatchGameMgr:Init()

--用于服务器关闭
function CMatchGameMgr:ClearTick()
	--print("副本游戏测试输出:::::::::::::::::::::清理 CMatchGameMgr tick")
	for _, game in pairs(self.m_GameTbl) do
		game:ClearTick()
	end
end

--获取当前开放的游戏
function CMatchGameMgr:GetCurOpenGames(npcName)
	local strNames = ""
	local Keys = MatchGame_Common:GetKeys()
	for i=1, #Keys do
		local gameName = Keys[i]
		if CheckActionIsBeginFunc(gameName) and self.m_GameTbl[gameName]:IsJoinNpc(npcName) then
			strNames = strNames .. "\"" .. gameName .. "\","
		end
	end
	return strNames
end

--count 参数当 type 为 4 有效

function CMatchGameMgr:GetRoomByScene(scene)
	if scene.m_MatchGameData == nil then
		return
	end
	local gameName = scene.m_MatchGameData.GameName
	local roomId = scene.m_MatchGameData.RoomId
	assert(gameName and roomId, "副本游戏 的场景 属性没设置全!")
	local game = self.m_GameTbl[gameName]
	local room = game:GetRoom(roomId)
	if game and room then
		return game, room
	end
end

function CMatchGameMgr:GetCountInfo(type, arg)
	return g_MatchGameCountCfg[type] and g_MatchGameCountCfg[type][arg]
end

function CMatchGameMgr:GetCountInfoByType(type)
	return g_MatchGameCountCfg[type] or {}
end

function CMatchGameMgr:AddMatchGameCount(player, type, arg, count)
	if not IsCppBound(player) then
		return
	end
	local game, room = self:GetRoomByScene(player.m_Scene)
	if room then

		local countInfo = self:GetCountInfo(type, arg)
		if countInfo then
			if arg == "杀人得分" then
				MsgToConn(player.m_Conn, 310001)
			end
			room:AddPlayerCount(player.m_uID, countInfo, count)
		end

	end
end


function CMatchGameMgr:AllTeamAddCount(scene, type, arg, count)
	local game, room = self:GetRoomByScene(scene)
	if room then
		local countInfo = self:GetCountInfo(type, arg)
		if countInfo then
			room:AllTeamAddCount(countInfo, count)
		end
	end
end

function CMatchGameMgr:GetMatchTeanIndex(player)
	local game, room = self:GetRoomByScene(player.m_Scene)
	if room then
		return room:GetPlayerTeamIndex(player)
	end
end

function PlayerOffLineMatchGameFb(player)
	if player.m_TempBag then
		player.m_TempBag:ClearTempBagAllObj(player, true)
	end
	local room = player.m_Scene:GetGameRoom()
	if room then
		room:OnPlayerOffLine(player)
	end
end

function PlayerLoginMatchGame(player)
	local game, room = g_MatchGameMgr:GetRoomByScene(player.m_Scene)
	if game then --玩家副本活动上线,恢复状态
		game:PlayerLogin(player, room)
	end
end

function LeaveMatchGameScene(player, oldSceneId)
	local oldGame, room = g_MatchGameMgr:GetRoomByScene(g_SceneMgr:GetScene(oldSceneId))
	if oldGame then
		oldGame:OnPlayerLeaveScene(player, room)
	end
end

function IntoMatchGameScene(player)
	local newGame, room = g_MatchGameMgr:GetRoomByScene(player.m_Scene)
	if newGame then
		newGame:OnPlayerIntoScene(player, room)
	end
end

function InMatchGameSceneReborn(player)
	local newGame, room = g_MatchGameMgr:GetRoomByScene(player.m_Scene)
	if newGame then
		newGame:OnPlayerAtSceneReborn(player)
	end
end

function OnMatchGameCreateRoom(scene, roomId, gameName, roomMembers, captainsTbl, teamIndexTbl)
	local game = g_MatchGameMgr.m_GameTbl[gameName]
	game:CreateNewRoom(scene, roomId, roomMembers, captainsTbl, teamIndexTbl)
end

function OnMatchGameAddTeam(gameName, roomId, actionTeamId, teamIndex, teamMember, captainName)
	local game = g_MatchGameMgr.m_GameTbl[gameName]
	game:AddTeam(roomId, actionTeamId, teamIndex, teamMember, captainName)
end


-----------------------Rpc -----------------
function Gac2Gas:ShowMatchGameSelWnd(Conn, NpcID)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	if CheckAllNpcFunc(Conn.m_Player, NpcID, "日常活动报名处") then
		local npc = CCharacterDictator_GetCharacterByID(NpcID)
		local npcName = npc.m_Properties:GetCharName()	
		Gas2Gac:RetShowMatchGameSelWnd(Conn, g_MatchGameMgr:GetCurOpenGames(npcName), npcName)
	end
end

function Gac2Gas:JoinMatchGame(Conn, gameName, isAutoTeam)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	--g_MatchGameMgr:JoinMatchGame(Conn, gameName)
	local actionInfo = ActionBasic:GetActionInfo(gameName)
	if isAutoTeam then
		if not actionInfo["IsCanAutoTeam"] then
			return
		end
	else
		if not actionInfo["IsCanNoAutoTeam"] then
			return
		end
	end
	MultiServerJoinAction(Conn, gameName, isAutoTeam)
end

gas_require "fb_game/match_game/MatchGameRoomInc"
gas_require "fb_game/match_game/CountRecord"
--gas_require "fb_game/DrinkShootMgr"
gac_gas_require "framework/common/CMoney"
cfg_load "fb_game/FbAfflatus_Common"
cfg_load "item/MatchGameItem_Common"
gas_require "log_mgr/CLogMgr"

local RoomState = {}
RoomState.eClose = 0
RoomState.eReady = 1  -- 玩家可以继续参加,但不能进场景,(此时场景还没创建)
RoomState.eWaitingPlayerJoin = 2 -- 玩家可以继续参加,也可进场景
RoomState.eWaitingCountDown = 3 -- 等待倒计时
RoomState.eCountDowning = 4 --倒计时中
RoomState.ePlaying = 5 --比赛中
RoomState.ePlayEnd = 6 --比赛结束,房间还没关闭
g_ComboTime = 10000000

function CMatchGameRoom:Init(gameName, roomId)
	self.m_RoomId = roomId
	self.m_GameName = gameName
	self.m_State = RoomState.eClose
	self.m_MatchTeamTbl = {}
	self.m_MatchTeamCount = 0
	self.m_CfgData = g_MatchGameMgr.m_CfgData[gameName]
	self.m_PlayerIndex = {}
	self.m_AllArg = self.CreateCountRecordTbl()
	self.m_ScoreSequenceId = 0
	
	self.m_ProcessTime = GetProcessTime()
	self.m_LogTbl = {"CreateRoom, gameName:"..gameName..", roomId:"..roomId..", serverId:"..g_CurServerId..", now:"..os.date("%c")..", time:"..self.m_ProcessTime}
end

function CMatchGameRoom:ClearTick()
	if self.m_WaitEndTick then
		UnRegisterTick(self.m_WaitEndTick)
		self.m_WaitEndTick = nil
	end
	
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	
	if self.m_BeginCountDownTick then
		UnRegisterTick(self.m_BeginCountDownTick)
		self.m_BeginCountDownTick = nil
	end
	
	if self.m_TimeOverTick then
		UnRegisterTick(self.m_TimeOverTick)
		self.m_TimeOverTick = nil
	end
	
	if self.m_CloseRoomTick then
		UnRegisterTick(self.m_CloseRoomTick)
		self.m_CloseRoomTick = nil
	end
	if self.m_SynStatisticTick then
		UnRegisterTick(self.m_SynStatisticTick)
		self.m_SynStatisticTick = nil
	end
end


function CMatchGameRoom:Open(scene)
	self.m_SceneId = scene.m_SceneId
	self.m_CreateTime = os.time()
	self.m_State = RoomState.eWaitingPlayerJoin
	
	table.insert(self.m_LogTbl, "Open, sceneId:"..self.m_SceneId..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	local NpcLevel = scene.m_SceneBaseLv or 1
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId in pairs(matchTeam.m_PlayerTbl) do
			Gas2GacById:RetIsJoinFbActionAffirm(playerId, self.m_GameName, g_MatchGameMgr.m_WaitPlayerTime, NpcLevel+10)
		end
	end
	
	--注册等待玩家进入结束tick
	local waitTime = g_MatchGameMgr.m_WaitPlayerTime * 1000
	self.m_WaitEndTick = RegClassTick("WaitEndTick", self.WaitPlayerEnd, waitTime, self)
	--print("副本游戏测试输出:::::::::::::::::::::CMatchGameRoom:Open")
end

function CMatchGameRoom:Close()
	if self:IsClose() then
		return
	end
	
	table.insert(self.m_LogTbl, "Close, time:"..GetProcessTime()-self.m_ProcessTime)
	
	--print("关闭房间")
	self:ClearTick()
	self.m_State = RoomState.eClose
	
	g_MatchGameMgr.m_GameTbl[self.m_GameName].m_RoomTbl[self.m_RoomId] = nil
	
	for teamId, matchTeam in pairs(self.m_MatchTeamTbl) do
		table.insert(self.m_LogTbl, "teamId:"..teamId..", index:"..matchTeam.m_Index)
		
		if matchTeam.m_TempTeamId then
			g_TemporaryTeamMgr:BreakTeam(matchTeam.m_TempTeamId)
			matchTeam.m_TempTeamId = nil
		end
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			table.insert(self.m_LogTbl, "playerId:"..playerId)
			
			if self.m_CfgData.DelItem and not playerInfo.m_IsDelItem then
				self:DleGameItem(playerId, self.m_CfgData.DelItem)
				playerInfo.m_IsDelItem = true
			end
			
			self.m_PlayerIndex[playerId] = nil
		end
	end
	self.m_MatchTeamTbl = {}
	
	--通知在副本内的玩家退出副本
	local scene = g_SceneMgr:GetScene(self.m_SceneId)
	if scene then
		local isHavePlayer = false
		for playerId, player in pairs(scene.m_tbl_Player) do
			NotifyPlayerLeaveFbScene(playerId)
			local player = g_GetPlayerInfo(playerId)
			Gas2Gac:NotifySandGlassClose(player.m_Conn)
			--Gas2Gac:CloseTempBag(player.m_Conn)
			if player.m_TempBag then
				player.m_TempBag:ClearTempBagAllObj(player, false)
				player.m_TempBag = nil
			end
			if self.m_CfgData.EquipDura == 2 or self.m_CfgData.EquipDura == 3 then
				player:SetCloseDurabilityChange(false)
			end
			isHavePlayer = true
		end
		
		if not isHavePlayer then
			scene:Destroy(self.m_GameName)
		end
	end
	
	--self:OnLogErr("内网测试，非bug", "")
	self:SaveLog()
	--print("副本游戏测试输出:::::::::::::::::::::关闭 " .. self.m_GameName ..  " 房间 " .. self.m_RoomId)
end

function CMatchGameRoom:IsCanResumeGame(charId)
	if self.m_State == RoomState.ePlayEnd or self.m_State == RoomState.eClose then
		return false
	end
	local team = self:FindPlayer(charId)
	if not team then
		return false
	end
	local playerInfo = team.m_PlayerTbl[charId]
	return playerInfo.m_IsIntoScene
end

--判断是否能关闭房间,所有人都是主动离开房间的话就可以直接关闭
function CMatchGameRoom:IsCanClose()
	if self:IsCanAddPlayer() then
		return false
	end
	
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId in pairs(matchTeam.m_PlayerTbl) do
			if not self.m_PlayerIndex[playerId].m_IsExitGame then
				return false
			end
		end
	end
	return true
end


function CMatchGameRoom:IsGameEnd()
	return self.m_State == RoomState.ePlayEnd or self.m_State == RoomState.eClose 
end

function CMatchGameRoom:IsClose()
	return self.m_State == RoomState.eClose
end

function CMatchGameRoom:SetReady()
	self.m_BuildTime = os.time()
	self.m_State = RoomState.eReady
end

function CMatchGameRoom:IsReady()
	return self.m_State == RoomState.eReady
end

function CMatchGameRoom:IsWaitingPlayerJoin()
	return self.m_State == RoomState.eWaitingPlayerJoin
end

function CMatchGameRoom:IsWaitingCountDown()
	return self.m_State == RoomState.eWaitingCountDown
end

function CMatchGameRoom:IsCountDowning()
	return self.m_State == RoomState.eCountDowning
end

function CMatchGameRoom:IsPlaying()
	return self.m_State == RoomState.ePlaying
end

--判断玩家是否能加入该房间
function CMatchGameRoom:IsCanAddPlayer()
	if self:IsReady() or self:IsWaitingPlayerJoin() then
		return true
	end
	return false
end

function CMatchGameRoom:IsCanEnterScene(charId)
	if charId and not self:FindPlayer(charId) then
		return
	end
	if (self:IsWaitingPlayerJoin() or self:IsWaitingCountDown() or self:IsCountDowning())
		and g_SceneMgr:GetScene(self.m_SceneId) then
		return true
	end
end

function CMatchGameRoom:IsAllPlayerInScene()
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, info in pairs(matchTeam.m_PlayerTbl) do
			if not info.m_IsIntoScene then
				return
			end
		end
	end
	return true
end

function CMatchGameRoom:GetValue_Time()
	return self.m_AllArg:GetCount("定时")
end

function CMatchGameRoom:Value_ExistTeamNum()
	return self.m_AllArg:GetCount("剩余队伍数")
end

function CMatchGameRoom:Value_TeamExistPlayerNum(matchTeam)
	local num = 0

	for charId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
		if self:IsPlayerInScene(playerInfo) then
			num = num + 1
		end
	end

	return num
end

function CMatchGameRoom:Value_ExistTeamMaxScore()
	local team = nil
	local isEqual = false
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for _, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			if self:IsPlayerInScene(playerInfo) then
				if team == nil or team.m_Score < matchTeam.m_Score then
					team = matchTeam
					isEqual = false
				elseif team.m_Score == matchTeam.m_Score then
					if team.m_LastScoreId < matchTeam.m_LastScoreId then
						team = matchTeam
						isEqual = false
					elseif team.m_LastScoreId == matchTeam.m_LastScoreId then
						isEqual = true
					end
				end
				break
			end
		end
	end
	if not isEqual then
		return team
	end
end

function CMatchGameRoom:Value_AllTeamMaxScore()
	local team = nil
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		if team == nil 
			or team.m_Score < matchTeam.m_Score 
			or (team.m_Score == matchTeam.m_Score and team.m_LastScoreId < matchTeam.m_LastScoreId) then
			team = matchTeam
		end
	end
	return team
end

function CMatchGameRoom:GetValue_AllScore(matchTeam)
	return matchTeam.m_Score
end

function CMatchGameRoom:GetHurtRate(teamInfo, charInfo, type)
	if type == "team" then
		return 100
	else
		if not teamInfo.m_AllFightHurt or teamInfo.m_AllFightHurt == 0 then
			return 0
		else
			return math.floor(charInfo.m_AllFightHurt * 100 / teamInfo.m_AllFightHurt)
		end
	end
end

function CMatchGameRoom:GetHurt(teamInfo, charInfo, type)
	if type == "team" then
		return teamInfo.m_AllFightHurt or 0
	else
		return charInfo.m_AllFightHurt or 0
	end
end

function CMatchGameRoom:GetHealRate(teamInfo, charInfo, type)
	if type == "team" then
		return 100
	else
		if not teamInfo.m_AllFightHeal or  teamInfo.m_AllFightHeal == 0 then
			return 0
		else
			return math.floor(charInfo.m_AllFightHeal * 100 / teamInfo.m_AllFightHeal)
		end
	end
end

function CMatchGameRoom:GetHeal(teamInfo, charInfo, type)
	if type == "team" then
		return teamInfo.m_AllFightHeal or 0
	else
		return charInfo.m_AllFightHeal or 0
	end
end

function CMatchGameRoom:GetFightScore(teamInfo, charInfo, type)
	if type == "team" then
		return teamInfo.m_AllFightScore or  0
	else
		if not teamInfo.m_AllFightScore then
			return 0
		end
		local rate = 0
		if teamInfo.m_AllFightHurt + teamInfo.m_AllFightHeal ~= 0 then
			rate = (charInfo.m_AllFightHurt + charInfo.m_AllFightHeal)/(teamInfo.m_AllFightHurt + teamInfo.m_AllFightHeal)
		end
		return math.ceil( rate * teamInfo.m_AllFightScore)
	end
end


function CMatchGameRoom:GetShowCount(name, teamInfo, charInfo, type)
	local DataTbl = (type == "team" and teamInfo) or  charInfo
	if name == "总分" then
		return DataTbl.m_Score
	elseif g_MatchGameCombinedCount[name] then
		return g_MatchGameCombinedCount[name](DataTbl.m_CountRecord)
	else
		assert(g_MatchGameSpecialCount[name], "错误的统计项" .. name)
		return g_MatchGameSpecialCount[name](self, teamInfo, charInfo, type)
	end
end

--目前之用在了最初的界面初始化 
function CMatchGameRoom:GetScoreBoardStr(teamInfo, charInfo, type, showName) --(DataTbl, score, showName)
	local str = ""
	local data = {
	["玩家名称"] = showName,
	["排名"] = "",
	}
	local ScoreBoardTbl = GetCfgTransformValue(true,"MatchGame_Common", self.m_GameName,"ScoreBoard")
	
	for _, itemName in ipairs(ScoreBoardTbl) do
		local value = data[itemName]
		if value then
			if IsNumber(value) or itemName == "玩家名称" then
				str = str .. value .. ","
			else 
				str = str .. "\"" .. value .. "\","
			end
		else
			local res = self:GetShowCount(itemName, teamInfo, charInfo, type)
			str = str .. res .. ","
		end
		
	end
	return  str
end

function CMatchGameRoom:GetStatisticsStr(matchTeam, playerInfo, orderId, gameTime, exp, money, isWin, soul)
	local str = ""
	local data = {
	["名字"] = " "..playerInfo.m_Name.." ",
	["排名"] = orderId,
	["时间"] = gameTime,
	["经验"] = exp,
	["金钱"] = money,
	["等级"] = playerInfo.m_Level,
	["魂值"] = soul,
	}
	local playerId = playerInfo.m_PlayerId
	local player = g_GetPlayerInfo(playerId)
	local StatisticsTbl = GetCfgTransformValue(true, "MatchGame_Common", self.m_GameName, "Statistics")
	
	for _, itemName in pairs(StatisticsTbl) do
		local value = data[itemName]
		if value then
			if itemName == "金钱" then
				local cMoney = CMoney:new()
				local nGold,nArgent,nCopper = cMoney:ChangeMoneyToGoldAndArgent(value)
				str = str .. "\""
				if nGold > 0 then
					str = str .. nGold .. "金"
				end
				if nArgent > 0 then
					str = str .. nArgent .. "银"
				end
				if nCopper > 0 then
					str = str .. nCopper .. "铜"
				end
					str = str .. "\","
			elseif itemName == "排名" then
				if isWin == true then
					value = "_GSTC(9445)"
				elseif isWin == false then
					value = "_GSTC(9446)"
				end
				str = str .. value .. ","
			else
				if IsNumber(value) then
					str = str .. value .. ","
				else
					str = str .. "\"" .. value .. "\","
				end
			end
		else
			data[itemName] = self:GetShowCount(itemName, matchTeam, playerInfo, "char")
			if string.find(itemName,"积分") then
				local isFlag = 1
				if IsCppBound(player) and player:ExistState("双倍积分符buff") then
					isFlag = isFlag + 1
				end
				if g_PlayerBuffTbl and g_PlayerBuffTbl[playerId] then
					for _, addBuffName in pairs(g_PlayerBuffTbl[playerId] ) do
						if addBuffName == "安装三合一豪华版状态" then
							isFlag = isFlag + 2
						end
						if addBuffName == "安装五合一卡状态" then
							isFlag = isFlag + 4
						end
					end
				end
				local itemdata = isFlag * data[itemName]
				if isFlag > 1 then
					str = str .."\"".. itemdata .."\(".."+"..(isFlag - 1)*data[itemName].."\)".."\""..","
				else
					str = str .. data[itemName] ..","
				end
			elseif string.find(itemName,"经验") then 
				local expFlag = 1
				if g_PlayerBuffTbl and g_PlayerBuffTbl[playerId] then
					for _, addBuffName in pairs(g_PlayerBuffTbl[playerId] ) do
						if addBuffName == "三倍经验的buff" then
							expFlag = expFlag + 2
							break
						elseif addBuffName == "安装三合一豪华版状态" then
							expFlag = expFlag + 2
							break
						elseif addBuffName == "安装五合一卡状态" then
							expFlag = expFlag + 4
							break
						end
					end
				end
				local itemdata = expFlag * data[itemName]
				if expFlag > 1 then
					str = str .. itemdata .."\(".."+"..(expFlag - 1)*data[itemName].."\)"..","
				else
					str = str .. data[itemName] ..","
				end
			else
				str = str .. data[itemName] ..","
			end
		end
	end
	
	return  str, data
end

local function GetLevelItem(allItemTbl, level)
	for i = 1, #(allItemTbl) do
		local ItemMgr = allItemTbl[i]
		local LevelTbl = ItemMgr[1]
		local ItemTbl = ItemMgr[2]
		if LevelTbl[1] and LevelTbl[2] then
			if level >= LevelTbl[1] and level <= LevelTbl[2] then
				return ItemTbl
			end
		elseif LevelTbl[1] and level == LevelTbl[1] then
			return ItemTbl
		end
	end
end

function CMatchGameRoom:GetEndGiveItem(PlayerLevel, orderId, isWin, score)
	local allAwardItemTbl = {}
	
	if self.m_CfgData.AwardItem["noScore"] and score == 0 then  --0分奖励
		local noScoreItemTbl = g_MercenaryAwardItemMgr[self.m_CfgData.AwardItem["noScore"]]
		local noScoreItem = noScoreItemTbl and GetLevelItem(noScoreItemTbl,PlayerLevel)
		if noScoreItem and noScoreItem[orderId] then
			table.insert(allAwardItemTbl, noScoreItem[orderId])
		end
		return allAwardItemTbl
	end
	
	local orderItemTbl = g_MercenaryAwardItemMgr[self.m_CfgData.AwardItem["order"]]
	local orderItem = orderItemTbl and GetLevelItem(orderItemTbl,PlayerLevel)
	if orderItem and orderItem[orderId] then
		table.insert(allAwardItemTbl, orderItem[orderId])
	end
	
	if isWin == true then
		local winItemTbl = g_MercenaryAwardItemMgr[self.m_CfgData.AwardItem["win"]]
		local winItem = winItemTbl and GetLevelItem(winItemTbl,PlayerLevel)
		if winItem and next(winItem) then
			table.insert(allAwardItemTbl, winItem[1])
		end
	end
	
	if isWin == false then
		local lostItemTbl = g_MercenaryAwardItemMgr[self.m_CfgData.AwardItem["lost"]]
		local lostItem = lostItemTbl and GetLevelItem(lostItemTbl,PlayerLevel)
		if lostItem and next(lostItem) then
			table.insert(allAwardItemTbl, lostItem[1])
		end
	end
	
	return allAwardItemTbl
end

function CMatchGameRoom:StatisticFightInfo(isEndStatistic)
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			local player = g_GetPlayerInfo(playerId)
			if IsCppBound(player) then
				local addHurt, addHeal = player:GetStatisticFightHurt(""), player:GetStatisticFightHeal("")*2
				playerInfo.m_AllFightHurt = playerInfo.m_AllFightHurt + addHurt
				playerInfo.m_AllFightHeal = playerInfo.m_AllFightHeal + addHeal
				matchTeam.m_AllFightHurt = matchTeam.m_AllFightHurt + addHurt
				matchTeam.m_AllFightHeal = matchTeam.m_AllFightHeal + addHeal
				player:EndStatistic("")
				if not isEndStatistic then
					player:BeginStatistic("")
				end
			end
		end
	end
end

--这个函数当为 "定时" 游戏时候 被tick 调用
function CMatchGameRoom:GameEnd()
	--print("副本游戏测试输出:::::::::::::::::::::比赛结束!!!!!")
	self:ClearTick()
	if self:IsGameEnd() then
		return
	end
	
	table.insert(self.m_LogTbl, "GameEnd, time:"..GetProcessTime()-self.m_ProcessTime)
	
	local gameTime = (self.m_BeginTime and os.time() - self.m_BeginTime) or 0
	local statisticsTbl = {}
	local dbData = {}
	local ChannelIDList = {}
	local Member = {}
	local winPlayerName = nil
	local actionInfo = ActionBasic:GetActionInfo(self.m_GameName)
	local xianxieRes = {}
	
	if actionInfo and actionInfo["MinTeams"] and actionInfo["MaxTeams"]
		and (actionInfo["MinTeams"] > 1 or actionInfo["MaxTeams"] > 1) then
			winPlayerName = ""
	end
	if self.m_CfgData.IsOpenFightStatistic then
		self:StatisticFightInfo(true)
		self:SynFightInfo()
	end
	
	for teamId, matchTeam in pairs(self.m_MatchTeamTbl) do
		table.insert(self.m_LogTbl, "teamId:"..teamId..", index:"..matchTeam.m_Index)
		
		local orderId, isWin = self:GetMatchTeamOrder(matchTeam)
		if statisticsTbl[orderId] == nil then
			statisticsTbl[orderId] = {}
		end
		
		--local award = self.m_CfgData.AwardItem and self.m_CfgData.AwardItem[orderId]
		local score = matchTeam.m_Score
		
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			table.insert(self.m_LogTbl, "playerId:"..playerId)
			
			if winPlayerName and isWin then
				winPlayerName = winPlayerName .. playerInfo.m_Name..","
			end
			local player = g_GetPlayerInfo(playerId)
			
			local exp = 0
			local money = 0
			local soul = 0
			local lv = playerInfo.m_Level
			local str, resultTbl
			if self.m_CfgData.ExpFun then
				exp = playerInfo.m_ExpModulus * self.m_CfgData.ExpFun(orderId, lv, score, gameTime)
				exp = math.floor(exp)
			end
			if self.m_CfgData.MoneyFun then
				money = playerInfo.m_MoneyModulus * self.m_CfgData.MoneyFun(orderId, lv, score, gameTime)
				money = math.floor(money)
			end
			
			if self.m_CfgData.SoulFun then
				soul = self.m_CfgData.SoulFun(orderId, lv, score, gameTime)
				soul = math.floor(soul)
			end
			
			local addBuffName = MatchGame_Common(self.m_GameName, "AddBuffName")
			if IsCppBound(player) and player:ExistState("三倍经验的buff") and string.find(addBuffName, "三倍经验") then
				str, resultTbl = self:GetStatisticsStr(matchTeam, playerInfo, orderId, gameTime, 3* exp, money, isWin, soul)
			else
				str, resultTbl = self:GetStatisticsStr(matchTeam, playerInfo, orderId, gameTime, exp, money, isWin, soul)
			end
			
			table.insert(statisticsTbl[orderId], {player, str})
			
			dbData[playerId] = {}
			if player and IsCppBound(player) then
				table.insert(ChannelIDList, player.m_AccountID)
				table.insert(Member, player)
				if player.m_Scene.m_SceneId == self.m_SceneId then
					dbData[playerId].isInGame = true
					if not player:CppIsLive() then
						--由于玩家死亡和复活时须走c++并设置SizeType，这里直接调用Reborn会导致SizeType因时序问题出错，所以改为1毫秒tick调用
						RegisterOnceTick(player, "GameRebornTick", player.Reborn, 1, player, 1)
					end
					player:PlayerDoFightSkillWithoutLevel("玩法过度无敌")
				end
			end
			
			dbData[playerId].itemTbl = self:GetEndGiveItem(lv, orderId, isWin, score)
			if IsCppBound(player) and player:ExistState("三倍经验的buff") and string.find(addBuffName, "三倍经验") then
				dbData[playerId].exp = 3 * exp
			else
				dbData[playerId].exp = exp
			end
			dbData[playerId].money = money
			dbData[playerId].soul = soul
			
			if IsCppBound(player) and player:ExistState("双倍道具符buff") and string.find(addBuffName, "双倍道具符") then
				dbData[playerId].isFlag = 2
				player:PlayerDoItemSkill("撤销双倍道具符buff", 1)
			else 
				dbData[playerId].isFlag = 1
			end
			
			if self.m_CfgData.IsAddFightScore then 
				local isFlag = 1
				if IsCppBound(player) and player:ExistState("双倍积分符buff") then
					isFlag = isFlag + 1
				end
				if IsCppBound(player) and player:ExistState("安装三合一豪华版状态") then
					isFlag = isFlag + 2
				end
				if IsCppBound(player) and player:ExistState("安装五合一卡状态") then
					isFlag = isFlag + 4
				end
				dbData[playerId].fightScore = isFlag * resultTbl["伤害积分"]
			end
			
			
			if IsCppBound(player) and player:ExistState("安装三合一豪华版状态") then
				dbData[playerId].isFlag = 3
				dbData[playerId].exp = 3 * exp
			end
			if IsCppBound(player) and player:ExistState("安装五合一卡状态") then
				dbData[playerId].isFlag = 5
				dbData[playerId].exp = 5 * exp
			end
			if MatchGame_Common(self.m_GameName, "GameType") == 1 and not isWin then
				dbData[playerId].fightScore = 0
			end
			
			if IsCppBound(player) then
				local uAfflatusValue = FbAfflatus_Common(self.m_GameName, "AfflatusValue")
				if uAfflatusValue then
					player:AddPlayerAfflatusValue(uAfflatusValue)
				end
				if self.m_CfgData.LeaveSceneDoSkill then
					for _, skillName in ipairs(self.m_CfgData.LeaveSceneDoSkill) do
						player:PlayerDoFightSkillWithoutLevel(skillName)
						for server_id in pairs(g_ServerList) do
							Gas2GasAutoCall:ClearPlayerBuffTbl(GetServerAutoConn(server_id), player.m_uID)
						end
					end
					player:PlayerDoFightSkillWithoutLevel("撤销玩法过度无敌")
				end
				if self.m_CfgData.LeaveSceneDeleteTempSkill then
					for _, tempSkillName in pairs(self.m_CfgData.LeaveSceneDeleteTempSkill) do
						player:RemoveTempSkill(tempSkillName, 1)
					end
				end
				player:RemoveAllTempSkill()
			end
			
			if self.m_GameName == "鲜血试炼场" or self.m_GameName == "战魂鲜血试炼场" then
				local n = score - 25
				if n < 0 then
					n = 0
				end
				xianxieRes[playerId] = {isWin, n, resultTbl["鲜血杀人得分"], resultTbl["鲜血死亡得分"]}
			end
		end
	end
	
	local scene = g_SceneMgr:GetScene(self.m_SceneId)
	dbData["sceneName"] = scene.m_SceneName
	dbData["MailTitle"] = 1022
	dbData["MailContent"] = 1026
	
	OnSavePlayerExpFunc(Member)
	SavePlayerWarnValue(self.m_SceneId)
	self:GameEndCallDbTrans(dbData, ChannelIDList)
	
	if self.m_GameName == "鲜血试炼场" then
		CallDbTrans("LogMgrDB", "SaveXianxieLog", nil, xianxieRes, "XianxieLog")
	elseif self.m_GameName == "战魂鲜血试炼场" then
		CallDbTrans("JoinActionDB", "SavePVPXianxieRes", nil, xianxieRes, "PVPXianxieRes")
	end
	
	
	--发统计数据
	local dataList = ""
	for order, tbl in pairs(statisticsTbl) do
		for i, v in pairs(tbl) do
			dataList = dataList .. v[2]
		end
	end
	
	local index = 1
	for order, tbl in pairs(statisticsTbl) do
		for i, v in pairs(tbl) do
			if v[1] and dbData[v[1].m_uID].isInGame then
				Gas2Gac:ShowMatchGameStatisticsWnd(v[1].m_Conn, self.m_GameName, dataList, g_MatchGameMgr.m_CloseRoomTime, index )
				NotifyPlayerLeaveFbScene(v[1].m_uID, g_MatchGameMgr.m_CloseRoomTime)
				Gas2Gac:NotifySandGlassClose(v[1].m_Conn)
				Gas2GacById:RetDelQueueFbAction(v[1].m_uID, self.m_GameName, true)
				--Gas2Gac:CloseTempBag(v[1].m_Conn)
				if v[1].m_TempBag then
					v[1].m_TempBag:ClearTempBagAllObj(v[1], false)
					v[1].m_TempBag = nil
				end
				if self.m_CfgData.EquipDura == 2 or self.m_CfgData.EquipDura == 3 then
					v[1]:SetCloseDurabilityChange(false)
				end
			end
			index = index + 1
		end
	end
	
	--发送滚动消息,说XXX在比赛中得了第一
	if winPlayerName and winPlayerName ~= "" then
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:SendActionRollMsg(GetServerAutoConn(server_id), winPlayerName, self.m_GameName)
		end
	end
	
	self.m_State = RoomState.ePlayEnd
	if scene:IsHavePlayer() then
		local waitTime = (g_MatchGameMgr.m_CloseRoomTime + 5) * 1000
		self.m_CloseRoomTick = RegClassTick("MatchGameCloseRoomTick", self.Close, waitTime, self)
	else
		self:Close()
	end
end

function CMatchGameRoom:TimeOver()
	table.insert(self.m_LogTbl, "TimeOver, time:"..GetProcessTime()-self.m_ProcessTime)
	
	if self.m_TimeOverTick then
		UnRegisterTick(self.m_TimeOverTick)
		self.m_TimeOverTick = nil
	end
	self.m_IsTimeOver = true
	self.m_AllArg:SetCount("定时", self.m_CfgData.GameTime)
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		if self:CheckOver(matchTeam.m_CountRecord) then
			break
		end
	end
end

function CMatchGameRoom:BeginGame()
	--print("副本游戏测试输出:::::::::::::::::::::开始比赛!!!!!")
	table.insert(self.m_LogTbl, "BeginGame, time:"..GetProcessTime()-self.m_ProcessTime)
	
	if self.m_CountDownTick  then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	
	for teamId, matchTeam in pairs(self.m_MatchTeamTbl) do
		table.insert(self.m_LogTbl, "teamId:"..teamId..", index:"..matchTeam.m_Index)
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			if not playerInfo.m_IsIntoScene then
				self:RemovePlayer(matchTeam, playerId)
				Gas2GacById:RetDelQueueFbAction(playerId, self.m_GameName, true)
				MsgToConnById(playerId, 191057, self.m_GameName)
				local data = {}
				data["CharId"] = playerId
				data["RoomId"] = self.m_RoomId
				data["ActionName"] = self.m_GameName
				data["AutoTeam"] = self.m_HasAutoTeam
				CallDbTrans("JoinActionDB", "CancelActionByRoom", nil, data, playerId, self.m_GameName)
			else
				table.insert(self.m_LogTbl, "playerId:"..playerId)
			end
		end
	end
	
	--print("副本游戏测试输出:::::::::::::::::::::房间内有 ".. self.m_MatchTeamCount .. " 支比赛对象")
	if self.m_MatchTeamCount <= 0 then
		if not self:IsClose()  then --无人进入直接关闭房间 判断IsClose是因为可能在上面的self:RemovePlayer(playerId) 时候没人了就关闭
			self:Close()
		end
		return
	end
	
	self.m_BeginTime = os.time()
	self.m_State = RoomState.ePlaying
	
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			local player = g_GetPlayerInfo(playerId)
			if player then
				player.m_CollObjCombo = 0											-- 完美采集
				player.m_PerfectCombo = 0											-- 非常完美采集
				player.m_PrePlusPoint = 0											-- 前一次额外加分
				player.m_MatchGameCaromCount = 0							-- 连击次数
				if self.m_CfgData.GameTime then
					Gas2Gac:BeginGame(player.m_Conn, self.m_CfgData.GameTime)
				else
					Gas2Gac:BeginGame(player.m_Conn, 0)
				end
			end

		end
	end
	
	
	if self.m_CfgData.IsOpenFightStatistic then
		for _, matchTeam in pairs(self.m_MatchTeamTbl) do
			matchTeam.m_AllFightHurt = 0
			matchTeam.m_AllFightHeal = 0
			matchTeam.m_AllFightScore = 0
			for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
				playerInfo.m_AllFightHurt = 0
				playerInfo.m_AllFightHeal = 0
				local player = g_GetPlayerInfo(playerId)
				if IsCppBound(player) then
					player:BeginStatistic("")
				end
				matchTeam.m_AllFightScore = matchTeam.m_AllFightScore + (1500-(150-playerInfo.m_Level)*9.675)/1500*100
			end
		end
	end
	
	if self.m_CfgData.BeginTriggerTbl then
		local scene = g_SceneMgr:GetScene(self.m_SceneId)
		if scene then
			for _, index in pairs(self.m_CfgData.BeginTriggerTbl) do
				g_TriggerScript:RunScriptByIndex(index, nil, nil, {["Scene"] = scene})
			end
		end
	end
	
	if self.m_CfgData.GameTime then
		local waitTime = self.m_CfgData.GameTime * 1000
		self.m_TimeOverTick = RegClassTick("MatchTimeOverTick", self.TimeOver, waitTime, self)
	end
	
	if self.m_CfgData.IsSynStatistic then
		self.m_SynStatisticTick = RegClassTick("SynStatisticTick", self.OnSynStatisticTick, 2000, self)
	end
	
	local WarnInfo = GetCfgTransformValue(true, "FbActionDirect_Common", self.m_GameName, "WarnValue")
	--print(self.m_GameName, self.m_HasAutoTeam)
	if CWarner.GMAllOpen ~= 0 or (WarnInfo and WarnInfo[1]) then
		local WarnTeamTbl = {}
		for teamId, matchTeam in pairs(self.m_MatchTeamTbl) do
			if matchTeam.m_IsAutoTeam then
				WarnTeamTbl[teamId] = matchTeam.m_PlayerTbl
			end
		end
		if next(WarnTeamTbl) then
			CreateWarner(self.m_SceneId, WarnInfo, self.m_CfgData.GameTime or 0, WarnTeamTbl)
		end
	end
	
	--if self.m_GameName == "喝酒射箭" then
	--	self.m_DrinkShootMgr = CDrinkShootMgr:new()
	--	self.m_DrinkShootMgr:BeginGame(self.m_MatchTeamTbl)
	--end
	
	self:UpdateAllArg("剩余队伍数")
end

function CMatchGameRoom:BeginCountDown()
	if self.m_BeginCountDownTick  then
		UnRegisterTick(self.m_BeginCountDownTick)
		self.m_BeginCountDownTick = nil
	end
	
	table.insert(self.m_LogTbl, "BeginCountDown, time:"..GetProcessTime()-self.m_ProcessTime)
	
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			local player = g_GetPlayerInfo(playerId)
			if IsCppBound(player) and playerInfo.m_IsIntoScene then
				Gas2Gac:MatchGameSetWaitTime(player.m_Conn, g_MatchGameMgr.m_CountDownTime)
			end
		end
	end
	
	if self.m_CfgData.CountDownTriggerTbl then
		local scene = g_SceneMgr:GetScene(self.m_SceneId)
		if scene then
			for _, index in pairs(self.m_CfgData.CountDownTriggerTbl) do
				g_TriggerScript:RunScriptByIndex(index, nil, nil, {["Scene"] = scene})
			end
		end
	end
	
	self.m_CountDownTick = RegClassTick("MatchGameBeginTick", self.BeginGame, g_MatchGameMgr.m_CountDownTime * 1000, self)
	self.m_State = RoomState.eCountDowning
	--print("副本游戏测试输出:::::::::::::::::::::开始倒计时, 当前状态玩家停止进入房间  ")
end

function CMatchGameRoom:WaitPlayerEnd()
	--print("副本游戏测试输出:::::::::::::::::::::停止玩家报名")
	if self.m_WaitEndTick then
		UnRegisterTick(self.m_WaitEndTick)
		self.m_WaitEndTick = nil
	end
	
	table.insert(self.m_LogTbl, "WaitPlayerEnd, time:"..GetProcessTime()-self.m_ProcessTime)
	
	self.m_State = RoomState.eWaitingCountDown
	if self:IsAllPlayerInScene() then
		self:BeginCountDown()
	else
		local waitTime = g_MatchGameMgr.m_WaitGameBeginTime * 1000
		self.m_BeginCountDownTick = RegClassTick("MatchGameBeginCountDownTick", self.BeginCountDown, waitTime, self)
	end
	
	CallDbTrans("JoinActionDB","WaitPlayerEnd", nil, {["RoomId"] = self.m_RoomId}, self.m_GameName)
end

function CMatchGameRoom:SynFightInfo()
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			self:UpdateClientScore(matchTeam, playerInfo)
		end
	end
end

function CMatchGameRoom:OnSynStatisticTick()
	self:StatisticFightInfo(false)
	self:SynFightInfo()
end

function CMatchGameRoom:CaromCountScore(Player) --连击次数得
	if not Player.m_MatchGameCaromCount then
		Player.m_MatchGameCaromCount = 0
	end

	local PlayerScore = 0 
	if Player.m_Time then
		if GetHDProcessTime() - Player.m_Time <= g_ComboTime then
			Player.m_MatchGameCaromCount = Player.m_MatchGameCaromCount + 1
			if Player.m_MatchGameCaromCount >= 2 and Player.m_MatchGameCaromCount <= self.m_CfgData.MatchGameCarom[1] then
				if Player.m_MatchGameCaromCount % self.m_CfgData.MatchGameCarom[2] == 0 then
					local BaseParam = Player.m_MatchGameCaromCount / self.m_CfgData.MatchGameCarom[2]
					PlayerScore = self.m_CfgData.MatchGameCarom[3]  + (BaseParam - 1)* (self.m_CfgData.MatchGameCarom[4])
					if Player.m_MatchGameCaromCount ~= self.m_CfgData.MatchGameCarom[1] then
						Gas2Gac:PlayHeadEffect(Player.m_Conn, "yellow", GetStaticTextServer(170001,Player.m_MatchGameCaromCount, PlayerScore), Player:GetEntityID())
					end
				end
				if Player.m_MatchGameCaromCount == self.m_CfgData.MatchGameCarom[1] then
					PlayerScore = PlayerScore  + self.m_CfgData.MatchGameCarom[5]  --其中100为额外奖励
					--Gas2Gac:PlayerPlusPointEffect(Player.m_Conn, "yellow", GetStaticTextServer(170001)..Player.m_MatchGameCaromCount.."+"..GetStaticTextServer(170002)..PlayerScore)
					Gas2Gac:PlayHeadEffect(Player.m_Conn, "yellow", GetStaticTextServer(170002, self.m_CfgData.MatchGameCarom[1], PlayerScore), Player:GetEntityID())
					Player.m_MatchGameCaromCount = 0
				end
			end
		else
			Player.m_MatchGameCaromCount = 0
		end
	end
	Player.m_Time = GetHDProcessTime()
	Gas2Gac:NotifySandGlassLoad(Player.m_Conn, g_ComboTime/1000)
	return PlayerScore
end

function CMatchGameRoom:FindPlayer(playerId)
	local playerInfo = self.m_PlayerIndex[playerId]
	if playerInfo then
		return playerInfo.team
	end
end

function CMatchGameRoom:GetGameCamp(playerId)
	local team = self:FindPlayer(playerId)
	if team then
		if team.m_GameCamp then
			return team.m_GameCamp
		else
			if not self.m_GameCampNum then
				self.m_GameCampNum = 1
			end
			team.m_GameCamp = self.m_GameCampNum
			self.m_GameCampNum = self.m_GameCampNum + 1
			return team.m_GameCamp
		end
	end
end

--从 MatchGame 的报名列表传过来的 玩家Tbl 每个玩家的值是个 true; 
--在MatchGameRoom里要为玩家记录很多信息.通过这个函数来设置
function CMatchGameRoom:InitPlayerTbl(playerTbl)
	for playerId in pairs(playerTbl) do
		playerTbl[playerId] = {}
		local playerInfo = playerTbl[playerId]
		playerInfo.m_IsIntoScene = false   --是否已经进场.
		playerInfo.m_ExpModulus = 1  --保留玩家当次活动的经验和金钱系数,玩家掉线后重上线直接读这里的值
		playerInfo.m_MoneyModulus = 1
		playerInfo.m_CountRecord = self.CreateCountRecordTbl()
		playerInfo.m_PlayerId = playerId
		playerInfo.m_Score = self.m_CfgData.ScoreFun(playerInfo.m_CountRecord)
		playerInfo.m_SynScoreTbl = self:CreateSynScoreTbl(playerInfo) -- 必须写在 m_CountRecord, m_Score 赋值之后
	end
end

function CMatchGameRoom:AddMatchTeam(teamId, teamIndex, playerTbl, count, captain)
	table.insert(self.m_LogTbl, "AddMatchTeam, teamId:"..teamId..", index:"..teamIndex..", teamCount:"..(self.m_MatchTeamCount+1)..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	if not self:IsCanEnterScene() then --服务器间延迟太厉害才会出现这样情况, 为不影响逻辑,主动取消玩家的报名
		--CallDbTrans("JoinActionDB","DeleteActionTeam", nil, {["ActionTeamId"] = teamId}, self.m_GameName)
		local sceneExist = false
		if g_SceneMgr:GetScene(self.m_SceneId) then
			sceneExist = true
		end
		table.insert(self.m_LogTbl, "return because IsCanEnterScene:false, state:"..self.m_State..", sceneExist:"..tostring(sceneExist))
		return
	end
	if self.m_MatchTeamTbl[teamId] then
		table.insert(self.m_LogTbl, "return because m_MatchTeamTbl[teamId]:true")
		return
	end
	
	self.m_MatchTeamTbl[teamId] = {}
	local matchTeam = self.m_MatchTeamTbl[teamId]
	matchTeam.m_TeamId = teamId
	matchTeam.m_Index = teamIndex
	matchTeam.m_LastScoreId = 0
	matchTeam.m_PlayerTbl = playerTbl
	matchTeam.m_PlayerCount = count
	matchTeam.m_CountRecord = self.CreateCountRecordTbl()
	matchTeam.m_Score = self.m_CfgData.ScoreFun(matchTeam.m_CountRecord)
	matchTeam.m_SynScoreTbl = self:CreateSynScoreTbl(matchTeam) -- 必须写在 m_CountRecord, m_Score 赋值之后
	
	if string.sub(captain, 1, 6) == "{Auto}" then
		matchTeam.m_TempTeamId = g_TemporaryTeamMgr:CreateTeam()
		captain = string.sub(captain, 7, -1)
		matchTeam.m_CaptainName = captain
		matchTeam.m_IsAutoTeam = true
	end
	
	if not self.m_CfgData.IsTeamShowName then
		if self.m_CfgData.IsTeamMode then
			matchTeam.m_ShowName = "_GSTC(9404,\" " .. captain.." \")"
		else
			matchTeam.m_ShowName = "\" " .. captain .." \""
		end
	else
		matchTeam.m_ShowName = "_GTN(" .. matchTeam.m_Index .. ")"
		matchTeam.m_EnterPos = self.m_CfgData.EnterPointTbl[matchTeam.m_Index]
	end
	self:InitPlayerTbl(playerTbl)
	self.m_MatchTeamCount = self.m_MatchTeamCount + 1
	
	table.insert(self.m_LogTbl, "teamId:"..teamId..", index:"..matchTeam.m_Index..", auto:"..tostring(matchTeam.m_IsAutoTeam))
	
	local waitTime = g_MatchGameMgr.m_WaitPlayerTime - (os.time()-self.m_CreateTime)
	if waitTime < 1 then
		waitTime = 1
	end
	for playerId, playerInfo in pairs(playerTbl) do
		table.insert(self.m_LogTbl, "playerId:"..playerId)
		if self.m_PlayerIndex[playerId] ~= nil then
			self:OnLogErr("游戏内有此玩家", "playerId:"..playerId)
		end
		assert(self.m_PlayerIndex[playerId] == nil, "游戏内有此玩家")
		self.m_PlayerIndex[playerId] = playerInfo
		playerInfo.team = matchTeam
		Gas2GacById:RetIsJoinFbActionAffirm(playerId, self.m_GameName, waitTime, 0)
	end
	
	self:NotifyAllClientAddTeam(matchTeam)
	--print("副本游戏测试输出:::::::::::::::::::::房间内增加一个比赛队伍,总数: " .. self.m_MatchTeamCount)
end

function CMatchGameRoom:RemoveMatchTeam(teamId)
	local team = self.m_MatchTeamTbl[teamId]
	self.m_MatchTeamTbl[teamId] = nil
	self.m_MatchTeamCount = self.m_MatchTeamCount - 1
	if team.m_TempTeamId then
		g_TemporaryTeamMgr:BreakTeam(team.m_TempTeamId)
		team.m_TempTeamId = nil
	end
	
	table.insert(self.m_LogTbl, "RemoveMatchTeam, teamId:"..teamId..", teamCount:"..self.m_MatchTeamCount..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	self:NotifyAllClientRemoveTeam(teamId)
	--print("副本游戏测试输出:::::::::::::::::::::房间内删除队伍 " .. teamId .. "剩余队伍数: " .. self.m_MatchTeamCount)
	if not self:IsCanEnterScene() and self.m_MatchTeamCount == 0 then
		--print("副本游戏测试输出:::::::::::::::::::::: 房间人数为0 关闭房间 ")
		self:GameEnd()
		return
	end
end

function CMatchGameRoom:RemovePlayer(team, playerId)
	table.insert(self.m_LogTbl, "RemovePlayer, playerId:"..playerId..", teamId:"..team.m_TeamId..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	local playerInfo = team.m_PlayerTbl[playerId]
	team.m_PlayerTbl[playerId] = nil
	team.m_PlayerCount = team.m_PlayerCount - 1
	self.m_PlayerIndex[playerId] = nil
	if team.m_TempTeamId and playerInfo.m_IsIntoScene then
		g_TemporaryTeamMgr:DelMember(playerId, team.m_TempTeamId)
		SavePlayerWarnValue(self.m_SceneId, playerId)
	end
	--print("副本游戏测试输出:::::::::::::::::::::房间内删除玩家" .. playerId .. ",他的队伍还剩人数 : " .. team.m_PlayerCount)
	if team.m_PlayerCount == 0 then
		self:RemoveMatchTeam(team.m_TeamId)
	end
end

--centerPos 格式{x,y}
local function GetRandomPos(centerPos, radius)
	local length = radius * 2 + 1
	local gridCount = length * length
	local offset = math.random(0, gridCount -1)
	local x = centerPos[1] - radius  + math.mod(offset, length)
	local y = centerPos[2] - radius + math.floor(offset / length)
	return {x, y}
end

--随机打乱参数tbl内元素的顺序
--最多打乱次数count
local function ShuffleTbl(tbl, count)
	local n = #tbl
	if n == 0 then
		return
	end
	if count > n then
		count = n
	end
	local temp
	local index
	for i = 1, count do
		index = math.random(n)
		temp = tbl[index]
		tbl[index] = tbl[i]
		tbl[i] = temp
	end
end

function CMatchGameRoom:OnPlayerOffLine(player)
	table.insert(self.m_LogTbl, "OnPlayerOffLine, playerId:"..player.m_uID..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	self:UpdateAllArg("剩余队伍数")
	local playerInfo = self.m_PlayerIndex[player.m_uID]
	if not playerInfo then
		return
	end
	playerInfo.m_IsOffLine = true
	if playerInfo.m_AllFightHurt then
		playerInfo.m_AllFightHurt = playerInfo.m_AllFightHurt + player:GetStatisticFightHurt("") 
		playerInfo.m_AllFightHeal = playerInfo.m_AllFightHeal + player:GetStatisticFightHeal("")*2
		player:EndStatistic("")
	end
end

function CMatchGameRoom:OnPlayerLeaveScene(player)
	if not self:IsGameEnd() then
		if self.m_CfgData.LeaveSceneDoSkill then
			for _, skillName in ipairs(self.m_CfgData.LeaveSceneDoSkill) do
				player:PlayerDoFightSkillWithoutLevel(skillName)
				for server_id in pairs(g_ServerList) do
					Gas2GasAutoCall:ClearPlayerBuffTbl(GetServerAutoConn(server_id), player.m_uID)
				end
			end
			player:PlayerDoFightSkillWithoutLevel("撤销玩法过度无敌")
		end
		if self.m_CfgData.LeaveSceneDeleteTempSkill then
			for _, tempSkillName in pairs(self.m_CfgData.LeaveSceneDeleteTempSkill) do
				player:RemoveTempSkill(tempSkillName, 1)
			end
		end
		player:RemoveAllTempSkill()
	end
	
	local playerInfo = self.m_PlayerIndex[player.m_uID]
	if not playerInfo then
		return
	end
	if playerInfo.m_AllFightHurt then
		playerInfo.m_AllFightHurt = playerInfo.m_AllFightHurt + player:GetStatisticFightHurt("")
		playerInfo.m_AllFightHeal = playerInfo.m_AllFightHeal + player:GetStatisticFightHeal("")*2
		player:EndStatistic("")
	end
	
	self:ExitGame(player.m_uID, true)
end

--初始化客户端积分窗口
function CMatchGameRoom:InitClientCountWnd(player)
	local timeType = 1
	if self.m_CfgData.GameTime then
		timeType = 2
	end
	local nowTime = os.time()
	local time
	local gameState 
	if self:IsPlaying() then
		gameState = 1
		if timeType == 2 then
			if not self.m_IsTimeOver then
				time = self.m_CfgData.GameTime - (nowTime - self.m_BeginTime)
			else-- 时间已经到
				time = 0
			end
		else
			time = nowTime - self.m_BeginTime
		end
	else
		gameState = 0 -- 准备开始
		time =  g_MatchGameMgr.m_WaitEnterSceneTime - (nowTime - self.m_CreateTime)
	end
	Gas2Gac:InitMatchGameCountWnd(player.m_Conn, self.m_GameName, timeType, time, gameState)
	
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		self:NotifyClientAddTeam(matchTeam, player)
	end
	
end

function CMatchGameRoom:NotifyAllClientShowPlayer(team, player, name)
		--通知在场的角色显示
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			if self:IsPlayerInScene(playerInfo) then
				
				local playerTemp = g_GetPlayerInfo(playerId)
				if IsCppBound(playerTemp) then
					if self.m_CfgData.IsTeamMode then
						Gas2Gac:MatchGameShowTeamMember(playerTemp.m_Conn, team.m_TeamId, player.m_uID, name)
					else
						Gas2Gac:MatchGameShowTeam(playerTemp.m_Conn, team.m_TeamId)
					end
				end
				
			end
		end
	end
	
end

function CMatchGameRoom:NotifyClientAddTeam(matchTeam, player)
	local dataListStr = self:GetScoreBoardStr(matchTeam, nil, "team", matchTeam.m_ShowName)
	Gas2Gac:MatchGameAddTeam(player.m_Conn, matchTeam.m_IsIntoScene ,matchTeam.m_TeamId, dataListStr)
	if self.m_CfgData.IsTeamMode then
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			local showName = "\"\""
			if playerInfo.m_Name  then
				showName = "\" " .. playerInfo.m_Name .. " \""
			end
			dataListStr = self:GetScoreBoardStr(matchTeam, playerInfo, "char", showName)
			Gas2Gac:MatchGameAddTeamMember(player.m_Conn, playerInfo.m_IsIntoScene ,matchTeam.m_TeamId, playerId, dataListStr)
		end
	end
end

function CMatchGameRoom:NotifyAllClientAddTeam(team)
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			if self:IsPlayerInScene(playerInfo) then
				local playerTemp = g_GetPlayerInfo(playerId)
				if IsCppBound(playerTemp) then
					self:NotifyClientAddTeam(team, playerTemp)
				end
			end
		end
	end
end

function CMatchGameRoom:NotifyAllClientRemoveTeam(teamId)
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			if type(playerInfo) == "boolean" then
				self:OnLogErr("matchgame playerInfo异常", self.m_GameName.." playerId:"..playerId)
			end
			if self:IsPlayerInScene(playerInfo) then
				
				local playerTemp = g_GetPlayerInfo(playerId)
				if IsCppBound(playerTemp) then
					Gas2Gac:MatchGameRemoveTeam(playerTemp.m_Conn, teamId)
				end
				
			end
		end
	end
end

local function AdaptInfo(result, gameName, player, activityName)
	Gas2Gac:CreateTbl(player.m_Conn)
	for _, info in pairs(result) do
		local itemName = info[1]
		local num = info[2]    
		if IsCppBound(player) and IsCppBound(player.m_Conn) then
--			local addBuffName = MatchGame_Common(gameName, "AddBuffName")
			local buffName = MatchGameItem_Common(itemName, "BuffName")
			if not player:ExistState(buffName) then
				Gas2Gac:InsertTbl(player.m_Conn,MatchGame_Common(gameName, "JoinLimitType"), gameName, itemName,num)
			end
		end
	end
	Gas2Gac:OpenConfirmWnd(player.m_Conn, gameName, activityName)
end

function CMatchGameRoom:OnPlayerIntoScene(team, player)
	if not team or not (team.m_PlayerTbl and team.m_PlayerTbl[player.m_uID]) then
		local time = 0
		local nowTime = os.time()
		if self:IsPlaying() then
			if self.m_CfgData.GameTime then
				if not self.m_IsTimeOver then
					time = self.m_CfgData.GameTime - (nowTime - self.m_BeginTime)
				else-- 时间已经到
					time = 0
				end
			else
				time = nowTime - self.m_BeginTime
			end
		else
			time =  g_MatchGameMgr.m_WaitEnterSceneTime - (nowTime - self.m_CreateTime)
		end
		--print(time)
		if not team then
			--LogErr("MatchGame OnPlayerIntoScene team为nil", player.m_uID.." "..self.m_GameName.." "..self.m_State.." "..time)
			table.insert(self.m_LogTbl, "OnPlayerIntoScene, playerId:"..player.m_uID..", state:"..self.m_State..", time:"..GetProcessTime()-self.m_ProcessTime)
			self:OnLogErr("MatchGame OnPlayerIntoScene team为nil", "playerId:"..player.m_uID)
		else
			--LogErr("MatchGame OnPlayerIntoScene playerInfo为nil", player.m_uID.." "..self.m_GameName.." "..self.m_State.." "..time)
			table.insert(self.m_LogTbl, "OnPlayerIntoScene, playerId:"..player.m_uID..", state:"..self.m_State..", time:"..GetProcessTime()-self.m_ProcessTime)
			self:OnLogErr("MatchGame OnPlayerIntoScene playerInfo为nil", "playerId:"..player.m_uID)
		end
	end
	if IsCppBound(player.m_Conn) then
		Gas2Gac:DelNpcFunTbl(player.m_Conn)
	end
	
	local playerInfo = team.m_PlayerTbl[player.m_uID]
	team.m_IsIntoScene = true
	playerInfo.m_IsIntoScene = true
	playerInfo.m_Level = player:CppGetLevel()
	playerInfo.m_Name = player.m_Properties:GetCharName()
	
	table.insert(self.m_LogTbl, "OnPlayerIntoScene, playerId:"..player.m_uID..", teamId:"..team.m_TeamId..", state:"..self.m_State..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	if self.m_GameName == "古战场" then
		Gas2Gac:ChangeActionWarModel(player.m_Conn)
	end
	self:InitClientCountWnd(player)
	self:NotifyAllClientShowPlayer(team, player, playerInfo.m_Name)
	MultiServerCancelAllAction(player.m_uID)
	
	if team.m_TempTeamId then
		g_TemporaryTeamMgr:AddMember(player.m_uID,team.m_TempTeamId)
		if playerInfo.m_Name == team.m_CaptainName then
			g_TemporaryTeamMgr:SetCaptain(player.m_uID,team.m_TempTeamId)
		end
	end
	
	if self.m_State == RoomState.eWaitingCountDown and self:IsAllPlayerInScene() then
		self:BeginCountDown()
	end
	
	player.m_FbActionExpModulus = 1
	player.m_FbActionMoneyModulus = 1
	if self.m_CfgData.TempBag == 1 then
		player.m_TempBag = nil
		player.m_TempBag = CTempBag:new()
		Gas2Gac:OpenTempBag(player.m_Conn)
	end
	if self.m_CfgData.EquipDura == 2 or self.m_CfgData.EquipDura == 3 then
		player:SetCloseDurabilityChange(true)
	end
	if self.m_GameName == "异界守护塔队伍" then
		AddVarNumForTeamQuest(player, "完成守塔", 1)
	elseif self.m_GameName == "抱小猪选拔赛单人版" then
		AddVarNumForTeamQuest(player, "完成抱小猪选拔赛单人版", 1)
	end
	
	local nowTime = os.time()
	local index = GetExtraRewardActionIndex(self.m_GameName, nowTime)
	--print("当前活动index " .. (index or "空"))
	if index then
		local extraRewardData, criticalTime = GetTodayExtraRewardInfo(nowTime)
		local data = {}
		data["PlayerId"] = player.m_uID
		data["ActionName"] = self.m_GameName
		data["CriticalTime"] = criticalTime
		data["MaxCount"] = extraRewardData.ActionCountTbl[index]
		data["ActionTbl"] = extraRewardData.ActionTbl
		local function CallBack(curCount, AllCount)
			--print("活动 : " .. self.m_GameName .. " 享受额外奖励次数: " .. curCount .. " 总奖励次数: " .. AllCount)
			if curCount < extraRewardData.ActionCountTbl[index]  
				and AllCount < extraRewardData.AllRewardCount  then
				player.m_FbActionExpModulus = extraRewardData.ExpModulusTbl[index]
				player.m_FbActionMoneyModulus = extraRewardData.MoneyModulusTbl[index]
				playerInfo.m_ExpModulus = player.m_FbActionExpModulus  
				playerInfo.m_MoneyModulus = player.m_FbActionMoneyModulus
				--print("本次活动你可以享受额外奖励  ", player.m_FbActionExpModulus, player.m_FbActionMoneyModulus)
			end
		end
		CallAccountAutoTrans(player.m_Conn.m_Account, "ExtraRewardDB", "AddExtraRewardCount", CallBack, data)
	end
	
	local ItemName = MatchGame_Common(self.m_GameName, "CanUseItem")
	local function callBack(result)   --return {count, num}
		if not result then
			return 
		end
		if table.getn(result) == 0 then
			return
		end
		AdaptInfo(result, self.m_GameName, player, MatchGame_Common(self.m_GameName, "JoinLimitType"))
	end
	
	local parameters = {}
	parameters["PlayerId"] = player.m_uID
	parameters["ActivityName"] = MatchGame_Common(self.m_GameName, "JoinLimitType")
	parameters["ItemName"] = ItemName
	parameters["ActionName"] = self.m_GameName
	CallAccountAutoTrans(player.m_Conn.m_Account, "JoinActionDB", "OnEnterRoom", callBack, parameters)
end


function CMatchGameRoom:ResumeGame(team, player)
	local playerInfo = team.m_PlayerTbl[player.m_uID]
	playerInfo.m_IsOffLine = nil
	player.m_FbActionExpModulus = playerInfo.m_ExpModulus
	player.m_FbActionMoneyModulus = playerInfo.m_MoneyModulus
	if playerInfo.m_AllFightHurt then
		player:BeginStatistic("")
	end
	self:InitClientCountWnd(player)
	
	if self.m_CfgData.TempBag == 1 then
		player.m_TempBag = nil
		player.m_TempBag = CTempBag:new()
		Gas2Gac:OpenTempBag(player.m_Conn)
	end
	if self.m_CfgData.EquipDura == 2 or self.m_CfgData.EquipDura == 3 then
		player:SetCloseDurabilityChange(true)
	end
	
	Gas2Gac:RetInitFbActionQueueWnd(player.m_Conn, self.m_GameName)
	self:UpdateAllArg("剩余队伍数")
	
	table.insert(self.m_LogTbl, "ResumeGame, playerId:"..player.m_uID..", teamId:"..team.m_TeamId..", time:"..GetProcessTime()-self.m_ProcessTime)
end

function CMatchGameRoom:GetPlayerTeamIndex(player)
	local team = self:FindPlayer(player.m_uID)
	return team and team.m_Index
end

function CMatchGameRoom:GetTeamEnterPos(team)
	local pos = team.m_EnterPos
	if pos == nil then --新进的队伍
		if self.m_EnterTeamCount == nil then
			self.m_EnterTeamCount = 1
		else
			self.m_EnterTeamCount = self.m_EnterTeamCount + 1
		end
		local gameCfg = self.m_CfgData
		if gameCfg.EnterType == 1 then
			pos = GetRandomPos(gameCfg.EnterCenter, gameCfg.EnterRadius)
		elseif gameCfg.EnterType == 2 then
			local index = (self.m_EnterTeamCount % gameCfg.EnterPointCount) + 1
			pos = gameCfg.EnterPointTbl[index]
		end
		team.m_EnterPos = pos
	end	
	assert(pos ~= nil, "无法得到正确的传入坐标,请检测配置表.")
	return pos
end


function CMatchGameRoom:CreateAwardItem(Player, uBigID, sItemName , nCount)

	if not(IsCppBound(Player) and IsCppBound(Player.m_Conn) ) then
		return
	end
	local function callback(result)
		if IsCppBound(Player) then
			if IsNumber(result) then
				if result == 3 then
					--,'包裹空间不足'
					MsgToConn(Player.m_Conn,191008)
				elseif result == 0 then
					MsgToConn(Player.m_Conn,8003,'CreateFightItem error')
				end
				return
			end
			CRoleQuest.add_item_DB_Ret(Player,uBigID,sItemName,nCount,result)
		end
	end
	
	local data = {}
	data["char_id"]		= Player.m_uID
	data["nType"]		= uBigID
	data["sName"] 		= sItemName
	data["nCount"]		= nCount
	local Conn = Player.m_Conn
	CallAccountAutoTrans(Conn.m_Account, "CharacterMediatorDB", "AddItem", callback, data)
end

function CMatchGameRoom:ExitGame(playerId, mustDelDb)
	local function ExitGame_CallBack()
		StressRpcToClient(Gas2GacById,"ExitMatchGameSuccMsg", playerId)
	end
	
	local team = self:FindPlayer(playerId)
	if not team then
		ExitGame_CallBack()
		return
	end
	if self.m_PlayerIndex[playerId].m_IsExitGame then
		ExitGame_CallBack()
		return
	end
	
	table.insert(self.m_LogTbl, "ExitGame, playerId:"..playerId..", teamId:"..team.m_TeamId..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	self.m_PlayerIndex[playerId].m_IsExitGame = true --主动退出活动的标志
	
	local playerInfo = team.m_PlayerTbl[playerId]
	if self.m_CfgData.DelItem and not playerInfo.m_IsDelItem then
		self:DleGameItem(playerId, self.m_CfgData.DelItem)
		playerInfo.m_IsDelItem = true
	end
	
	self:RemovePlayer(team, playerId)
	
	local player = g_GetPlayerInfo(playerId)
	if IsCppBound(player) and player.m_Scene.m_SceneId == self.m_SceneId then
		NotifyPlayerLeaveFbScene(playerId)
		Gas2Gac:NotifySandGlassClose(player.m_Conn)
		if self.m_GameName == "古战场" then
			Gas2Gac:ChangeActionWarModel(player.m_Conn)
		end
		--Gas2Gac:CloseTempBag(player.m_Conn)
		if player.m_TempBag then
			player.m_TempBag:ClearTempBagAllObj(player, false)
			player.m_TempBag = nil
		end
		if self.m_CfgData.EquipDura == 2 or self.m_CfgData.EquipDura == 3 then
			player:SetCloseDurabilityChange(false)
		end
	end
	
	if mustDelDb then
		Gas2GacById:RetDelQueueFbAction(playerId, self.m_GameName, true)
		local data = {}
		data["CharId"] = playerId
		data["RoomId"] = self.m_RoomId
		CallDbTrans("JoinActionDB", "CancelActionByRoom", ExitGame_CallBack, data, playerId, self.m_GameName)
	else
		ExitGame_CallBack()
	end
	self:UpdateAllArg("剩余队伍数")
end

--获取玩家排名
function CMatchGameRoom:GetMatchTeamOrder(matchTeam)
	local score = matchTeam.m_Score
	local isWin = nil
	if self.m_CfgData.WinFun then
		if self.m_CfgData.WinFun(matchTeam.m_CountRecord, matchTeam, self) then
			isWin = true --成功
		else
			isWin = false -- 失败
		end
	end
	
	local orderId = 1
	if score == 0 then
		orderId = self.m_MatchTeamCount
	else
		for _, team in pairs(self.m_MatchTeamTbl) do
			if team ~= matchTeam then
				if team.m_Score > score then
					orderId = orderId + 1
				elseif team.m_Score == score then
					if team.m_LastScoreId < matchTeam.m_LastScoreId then --更先得到此分
						orderId = orderId + 1
					end
				end
			end
		end
	end
	return orderId, isWin
end

function CMatchGameRoom:IsPlayerInScene(playerInfo)
	return playerInfo.m_IsIntoScene and not playerInfo.m_IsExitGame  and not playerInfo.m_IsOffLine 
end

function CMatchGameRoom:UpdateAllArg(name)
	if not self:IsPlaying() then
		return
	end
	local num = 0
	if name == "剩余队伍数" then
		for _, matchTeam in pairs(self.m_MatchTeamTbl) do
			for _, playerInfo in pairs(matchTeam.m_PlayerTbl) do
				if self:IsPlayerInScene(playerInfo) then
					num = num + 1
					break
				end
			end
		end
		self.m_AllArg:SetCount("剩余队伍数", num)
	end
	
	for _, matchTeam in pairs(self.m_MatchTeamTbl) do
		if self:CheckOver(matchTeam.m_CountRecord, matchTeam) then
			return
		end
	end
end

function CMatchGameRoom:CheckOver(countTbl, matchTeam)
	if self.m_CfgData.IsOverFun(countTbl, matchTeam, self) then
		self:GameEnd()
		return true
	end
end

function CMatchGameRoom:CreateSynScoreTbl(ownerTbl)
	local scoreTbl = {}
	for name, index in pairs(self.m_CfgData.SynScoreTbl) do
		if name == "总分" then
			scoreTbl[index] = ownerTbl.m_Score
		elseif g_MatchGameCombinedCount[name] then
			scoreTbl[index] = g_MatchGameCombinedCount[name](ownerTbl.m_CountRecord)
		else
			scoreTbl[index] = 0
		end
	end
	return scoreTbl
end

function CMatchGameRoom:GetUpdateScoreTbl(oldScoreTbl, teamInfo, charInfo, type)
	local scoreTbl = {}
	for name, index in pairs(self.m_CfgData.SynScoreTbl) do

		local num = self:GetShowCount(name, teamInfo, charInfo, type)
		if oldScoreTbl[index] ~= num then
			scoreTbl[index] = num
			oldScoreTbl[index] = num
		end

	end
	return scoreTbl
end

function CMatchGameRoom:UpdateClientScore(matchTeam, playerInfo)
	local teamScoreTbl = self:GetUpdateScoreTbl(matchTeam.m_SynScoreTbl,matchTeam, playerInfo, "team")
	local scoreIndex = self.m_CfgData.SynScoreTbl["总分"]
	if teamScoreTbl[scoreIndex] then
		matchTeam.m_LastScoreId = self.m_ScoreSequenceId
		self.m_ScoreSequenceId = self.m_ScoreSequenceId + 1
	end
	
	local playerScoreTbl = {}
	local isUpdate = true
	if playerInfo and self.m_CfgData.IsTeamMode then
		playerScoreTbl = self:GetUpdateScoreTbl(playerInfo.m_SynScoreTbl,matchTeam, playerInfo, "char")
	end
	if next (playerScoreTbl) then
		isUpdate = false
	end
	
	for _, team in pairs(self.m_MatchTeamTbl) do
		for playerId, member in pairs(team.m_PlayerTbl) do
			if self:IsPlayerInScene(member) then
				
				local playerTemp = g_GetPlayerInfo(playerId)
				if IsCppBound(playerTemp) then
		
					for index, value in pairs(teamScoreTbl) do
						Gas2Gac:MatchGameUpdateTeamScore(playerTemp.m_Conn, matchTeam.m_TeamId, index, value, isUpdate)
					end
					for index, value in pairs(playerScoreTbl) do
						Gas2Gac:MatchGameUpdatePlayerScore(playerTemp.m_Conn, matchTeam.m_TeamId, playerInfo.m_PlayerId,index, value, true)
					end
						
				end
				
			end
		end
	end

end

function CMatchGameRoom:AddTeamCount(matchTeam, countInfo, count)
	local countName = countInfo("CountName")
	matchTeam.m_CountRecord:AddCount(countName, count)
	matchTeam.m_Score = self.m_CfgData.ScoreFun(matchTeam.m_CountRecord)
	if matchTeam.m_Score < 0 then  --有积分小于0的情况 还原累加的积分
		matchTeam.m_CountRecord:RevertScore(countName, count)
		matchTeam.m_Score = self.m_CfgData.ScoreFun(matchTeam.m_CountRecord)
	end
	self:UpdateClientScore(matchTeam, nil)
	self:CheckOver(matchTeam.m_CountRecord, matchTeam)
end

function CMatchGameRoom:AddPlayerCount(playerId, countInfo, count)
	if not self:IsPlaying() then
		return
	end
	local matchTeam = self:FindPlayer(playerId)
	if not matchTeam then
		return
	end
	local playerInfo = matchTeam.m_PlayerTbl[playerId]
	local countName = countInfo("CountName")
	playerInfo.m_CountRecord:AddCount(countName, count)
	matchTeam.m_CountRecord:AddCount(countName, count)
	
	local GetScore = 0
  if self.m_CfgData.MatchGameCarom then
		local playerTemp = g_GetPlayerInfo(playerId)
		if playerTemp and playerTemp.m_Scene.m_SceneId == self.m_SceneId then
			GetScore = self:CaromCountScore(playerTemp)
			if GetScore ~= 0 then
				matchTeam.m_CountRecord:AddCount("连击积分", GetScore)
				playerInfo.m_CountRecord:AddCount("连击积分", GetScore)
			end
		end
  end
	
	playerInfo.m_Score =  self.m_CfgData.ScoreFun(playerInfo.m_CountRecord)
	matchTeam.m_Score = self.m_CfgData.ScoreFun(matchTeam.m_CountRecord)
	if playerInfo.m_Score < 0 or matchTeam.m_Score < 0 then  --有积分小于0的情况 还原累加的积分
		playerInfo.m_CountRecord:RevertScore(countName, count)
		matchTeam.m_CountRecord:RevertScore(countName, count)
		playerInfo.m_CountRecord:RevertScore("连击积分", GetScore)
		playerInfo.m_Score =  self.m_CfgData.ScoreFun(playerInfo.m_CountRecord)
		matchTeam.m_Score = self.m_CfgData.ScoreFun(matchTeam.m_CountRecord)
	end

	self:UpdateClientScore(matchTeam, playerInfo)
	
	self:CheckOver(matchTeam.m_CountRecord, matchTeam)
end


function CMatchGameRoom:AllTeamAddCount(countInfo, count)
	if countInfo then
		for _, team in pairs(self.m_MatchTeamTbl) do
			self:AddTeamCount(team, countInfo, count)
		end
	end
end


function CMatchGameRoom.CreateCountRecordTbl()
	return CCountRecord:new()
end

--只用在了 把 game 报名列表 拷贝到房间列表中. 后人请勿再使用
function CMatchGameRoom:SetPlayers(matchTeamTbl, teamCount, captainsTbl, teamIndexTbl)
	table.insert(self.m_LogTbl, "SetPlayers, teamCount:"..teamCount..", time:"..GetProcessTime()-self.m_ProcessTime)
	
	self.m_MatchTeamTbl = matchTeamTbl
	for teamId, matchTeam in pairs(self.m_MatchTeamTbl) do
		matchTeam.m_LastScoreId = 0
		matchTeam.m_CountRecord = self.CreateCountRecordTbl()
		matchTeam.m_Index = teamIndexTbl[teamId]
		
		local captain = captainsTbl[teamId]
		if captain == nil then
			self:OnLogErr("matchgame captain异常", self.m_GameName.." teamId:"..teamId)
		end
		if string.sub(captain, 1, 6) == "{Auto}" then
			matchTeam.m_TempTeamId = g_TemporaryTeamMgr:CreateTeam()
			captain = string.sub(captain, 7, -1)
			matchTeam.m_CaptainName = captain
			matchTeam.m_IsAutoTeam = true
		end
		
		if not self.m_CfgData.IsTeamShowName then
			if self.m_CfgData.IsTeamMode then
				matchTeam.m_ShowName = "_GSTC(9404,\" " .. captain.." \")"
			else
				matchTeam.m_ShowName = "\" " .. captain .." \""
			end
		else
			matchTeam.m_ShowName = "_GTN(" .. matchTeam.m_Index .. ")"
			matchTeam.m_EnterPos = self.m_CfgData.EnterPointTbl[matchTeam.m_Index]
		end
		matchTeam.m_Score = self.m_CfgData.ScoreFun(matchTeam.m_CountRecord)
		matchTeam.m_SynScoreTbl = self:CreateSynScoreTbl(matchTeam) -- 必须写在 m_CountRecord, m_Score 赋值之后
		
		table.insert(self.m_LogTbl, "teamId:"..teamId..", index:"..matchTeam.m_Index..", auto:"..tostring(matchTeam.m_IsAutoTeam))
		
		self:InitPlayerTbl(matchTeam.m_PlayerTbl)
		
		for playerId, playerInfo in pairs(matchTeam.m_PlayerTbl) do
			table.insert(self.m_LogTbl, "playerId:"..playerId)
			assert(self.m_PlayerIndex[playerId] == nil, "报名列表内无此玩家")
			self.m_PlayerIndex[playerId] = playerInfo
			playerInfo.team = matchTeam
		end
	end
	self.m_MatchTeamCount = teamCount
end

--玩家活动结束数据库操作
function CMatchGameRoom:GameEndCallDbTrans(data, ChannelIDList)
	local function callback(IsSucc, result)

		if IsSucc then
			for playerId, v in pairs(data) do
				if IsNumber(playerId)then
					if v.fightScore then
						Gas2GacById:UpdatePlayerPoint(playerId, 8, v.fightScore)
					end
					local player = g_GetPlayerInfo(playerId)
					if player and IsCppBound(player) then
						local resultTbl = result[playerId]
						if v.isInGame then
							local expResult = resultTbl[1]
							if expResult and expResult[1] then
								local AddExpTbl = {}
								AddExpTbl["Level"] = expResult[1]
								AddExpTbl["Exp"] = expResult[2]
								AddExpTbl["AddExp"] = v.exp
								AddExpTbl["uInspirationExp"] = 0
								CRoleQuest.AddPlayerExp_DB_Ret(player, AddExpTbl)
							end
							local soulValue = resultTbl[7]
							if soulValue then
								Gas2Gac:ReturnModifyPlayerSoulNum(player.m_Conn, soulValue)
								Gas2Gac:ReturnCharAddSoulMsg(player.m_Conn,v.soul)
							end
							if resultTbl[2][1] then
							else
								if IsNumber(resultTbl[2][2]) then
									MsgToConn(player.m_Conn,resultTbl[2][2])
								end
							end
							
							if resultTbl[3] and next(resultTbl[3]) then
								local itemResult = resultTbl[3][1]
								if IsNumber(itemResult[4]) then
									if itemResult[4] == 3 then
										--,'包裹空间不足'
										MsgToConn(player.m_Conn,191007)
									elseif itemResult[4] == 0 then
										MsgToConn(player.m_Conn,8003,'CreateFightItem error')
									end
								else
									CRoleQuest.add_item_DB_Ret(player, itemResult[1], itemResult[2], itemResult[3],itemResult[4])
								end
							end
							
							if resultTbl[5] then
								MsgToConn(player.m_Conn, 4004)
							end
						else--if v.isInGame then
							if resultTbl then
								MsgToConn(player.m_Conn, 4004)
							end
						end
					
					end--if player and IsCppBound(player) then
				end--if IsNumber(playerId)then
			end--for playerId, v in pairs(data) do
			
		end
		
	end
	
	if not next(ChannelIDList) then
		ChannelIDList[1] = 1
	end
	CallDbTrans("RoleQuestDB", "FbActionAward", callback, data, unpack(ChannelIDList))
end

--删除活动相关的物品
function CMatchGameRoom:DleGameItem(playerId, itemTbl)
	local player = g_GetPlayerInfo(playerId)
	local function CallBack(allRes)
		if IsCppBound(player) then
			local isDelItem = false
			for _, itemRes in ipairs(allRes) do
				if IsTable(itemRes) then
					itemRes = true
				end
			end
		end
	end
	
	local parameter = {}
	parameter["nCharID"] =  playerId
	parameter["itemTbl"] = itemTbl
	local scene = g_SceneMgr:GetScene(self.m_SceneId)
	parameter["sceneName"] = scene.m_SceneName
	local ChannelID = playerId
	if IsCppBound(player) then
		ChannelID = player.m_AccountID
	end
	CallDbTrans("CharacterMediatorDB", "DelActionItem", CallBack, parameter, ChannelID)
end

function AddPlayerExp(Player, addExp)
	if not(Player and IsCppBound(Player) and IsCppBound(Player.m_Conn) ) then
		return
	end
	
	local function callback(CurLevel,LevelExp)
		if IsCppBound(Player) and CurLevel then
			if CurLevel then
				local AddExpTbl = {}
				AddExpTbl["Level"] = CurLevel
				AddExpTbl["Exp"] = LevelExp
				AddExpTbl["AddExp"] = addExp
				AddExpTbl["uInspirationExp"] = 0
				CRoleQuest.AddPlayerExp_DB_Ret(Player, AddExpTbl)
			end
		end
	end
	
	local data = {}
	data["char_id"] 		= Player.m_uID
	data["addExp"] = addExp
	data["addExpType"] = event_type_tbl["杀人获得经验"]
	
	OnSavePlayerExpFunc({Player})
	
	local Conn = Player.m_Conn
	CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB", "AddExp", callback, data)
end

function CMatchGameRoom:UseFbItem(Conn,activityName,actionName, itemName)
	if not IsCppBound(Conn) then
		return 
	end
	local player = Conn.m_Player
	if not IsCppBound(player) then
		return 	
	end
	local times = MatchGameItem_Common(itemName, "Times")
	local sceneName = player.m_Scene.m_SceneName
	local data = {}
	data["charId"] = player.m_uID
	data["sceneName"] = sceneName
	data["actionName"] = actionName
	data["activityName"] = activityName
	data["itemName"] = itemName
	--data["count"] = count --参加过得次数
	data["times"] = times
	if not player:CppIsLive() then
		MsgToConn(Conn, 844, itemName)
		return
	end
	if player:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_InDoingSkill) then
		MsgToConn(Conn, 838)
		return
	end

	local function callback(result)
		if not result then
			return 
		end
		local flag = result[1]
		local res = result[2]

		if flag then
			local BuffSkillName = MatchGameItem_Common(itemName, "BuffName")
			player:PlayerDoItemSkill(BuffSkillName, 1)
			if not player:ExistState(BuffSkillName) then
				local isDead = player:CppIsLive()
				if isDead then
					isDead = "true"
				else
					isDead = "false"
				end
				LogErr("玩家使用"..itemName.."加buff失败", "playerId:"..player.m_uID.."活动名:"..activityName.."玩家是否死亡:"..isDead)
			end
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:SysPlayerBuffTbl(GetServerAutoConn(server_id), player.m_uID, BuffSkillName)
			end
		else
			MsgToConn(Conn,res)
		end
	end
	CallAccountAutoTrans(Conn.m_Account, "ActivityCountDB", "UpdateActionInfoAndDel", callback, data)
end

function CMatchGameRoom:OnLogErr(err, msg)
	local str = msg.."#r"..table.concat(self.m_LogTbl, "#r")
	--print(str)
	LogErr(err, str)
end

function CMatchGameRoom:SaveLog()
	local data = {}
	data["RoomId"] = self.m_RoomId
	data["SceneId"] = self.m_SceneId
	data["ActionName"] = self.m_GameName
	data["LogStr"] = table.concat(self.m_LogTbl, "#r")
	CallDbTrans("LogMgrDB", "SaveMatchGameServerLog", nil, data, "MatchGameLog")
end

gas_require "fb_game/match_game/MatchGameInc"
gas_require "fb_game/match_game/MatchGameRoom"

local GameState = {}
GameState.eOpen = 1
GameState.eClose = 2

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

--判断是否在开放时间里
local function IsInOpenTime(data, length)
	local curDate =  os.date("*t")
	local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
	local timeTbl = {}
	for _, timeStr in pairs(data.time) do
		table.insert(timeTbl, GetOffsetSecond(timeStr))
	end
	curDate.wday = ((curDate.wday == 1) and 7 or curDate.wday-1)  --将系统星期换成填表星期
	local wdayTbl = data.wday
	if wdayTbl == nil then
		wdayTbl = {1,2,3,4,5,6,7}
	end
	
	for _, wday in pairs(wdayTbl) do
		
		for _, offset in pairs(timeTbl) do
			local endOffset = offset + length * 60
			local day = (curDate.wday >= wday and curDate.wday - wday) or curDate.wday + 7 - wday --跨越天数
			local curOffsetSecond = todayOffsetSecond + day * 86400
			
			if curOffsetSecond >= offset and curOffsetSecond < endOffset then
					return true, endOffset - curOffsetSecond
			end
		end
		
	end

	return false
end

function CMatchGame:Init(gameName, cfgData)
	self.m_GameName = gameName
	self.m_CfgData = cfgData
	self.m_JoinTeamTbl = {} --报名表
	self.m_JoinTeamCount = 0 --报名玩家数
	self.m_RoomTbl = {} --房间表

	if self.m_CfgData.OpenTime == nil then -- 没有OpenTime 说明为任意时间都开放的副本游戏游戏,
		self.m_State = GameState.eOpen
	else --定时开放
		self.m_State = GameState.eClose
		g_AlarmClock:AddTask("开放 ".. gameName .. " 报名", self.m_CfgData.OpenTime, self.Open, nil, self)
		local isOpenTime, length = IsInOpenTime(self.m_CfgData.OpenTime, self.m_CfgData.OpenTime.length)
		if isOpenTime then
			self:Open(length)
		end
	end
end

--用于服务器关闭
function CMatchGame:ClearTick()
	if self.m_CloseTick then
		UnRegisterTick(self.m_CloseTick)
		self.m_CloseTick = nil
	end
	
	for _, room in pairs(self.m_RoomTbl) do
		room:ClearTick()
	end
end

function CMatchGame:Open(length)
	if length == nil then
		length = self.m_CfgData.OpenTime.length * 60
	end
--	print("开放持续时间", length)
	if self.m_CfgData.OpenTime then --存在定时开放的游戏,注册关闭tick
		if self.m_CloseTick then -- 可能是,配置表填写的开放时间过长,以至于准备第二次开放 上次还没结束
			UnRegisterTick(self.m_CloseTick)
			self.m_CloseTick = nil
		end
		local closeTime = length * 1000
		self.m_CloseTick = RegClassTick("CloseMatchGameTick", self.Close, closeTime, self)
	end
	
	self.m_State = GameState.eOpen
	--print("副本游戏测试输出:::::::::::::::::::::开启游戏 " .. self.m_GameName .. " 报名")
end

function CMatchGame:Close()
	if self.m_CloseTick then
		UnRegisterTick(self.m_CloseTick)
		self.m_CloseTick = nil
	end
	
	self.m_State = GameState.eClose
	--print("副本游戏测试输出:::::::::::::::::::::关闭游戏 " .. self.m_GameName .. " 报名")
end

function CMatchGame:IsOpen()
	return self.m_State == GameState.eOpen
end

function CMatchGame:IsClose()
	return self.m_State == GameState.eClose
end

function CMatchGame:IsJoinNpc(npcName)
	return self.m_CfgData.JoinNpcTbl[npcName]
end


function CMatchGame:GetRoom(roomId)
	return self.m_RoomTbl[roomId]
end

function CMatchGame:GetNewRoom(roomId)
	local newRoom = CMatchGameRoom:new()
	newRoom:Init(self.m_GameName, roomId)
	self.m_RoomTbl[roomId] = newRoom
	newRoom:SetReady()
	--self.m_NextRoomId = self.m_NextRoomId + 1
	--print("副本游戏测试输出:::::::::::::::::::::增加个新房间  " .. newRoom.m_RoomId)
	return newRoom
end

function CMatchGame:IntoSceneScriptCommon(player, room)
	local campType = MatchGame_Common(self.m_GameName, "CampType")
	local team = room:FindPlayer(player.m_uID)

	if campType == 1 then --统一阵营
		player:SetGameCamp(1)
	elseif campType == 2 then--混乱
		local gameCamp = room:GetGameCamp(player.m_uID)
		if gameCamp then
			player:SetGameCamp(gameCamp) 
		end
	end
	if self.m_CfgData.EnterSceneDoSkill then
		for _, sillName in ipairs(self.m_CfgData.EnterSceneDoSkill) do
			player:PlayerDoFightSkillWithoutLevel(sillName)
		end
	end
	if self.m_CfgData.EnterSceneTeamDoSkill and team then
		if self.m_CfgData.EnterSceneTeamDoSkill[team.m_Index] then
			for _, sillName in ipairs(self.m_CfgData.EnterSceneTeamDoSkill[team.m_Index]) do
				player:PlayerDoFightSkillWithoutLevel(sillName)
			end
		end
	end
	if self.m_CfgData.EnterSceneAddTempSkill then
		for _, tempSkillName in ipairs(self.m_CfgData.EnterSceneAddTempSkill) do
			player:AddTempSkill(tempSkillName, 1)
		end
	end
	
	if MatchGame_Common(self.m_GameName, "ForbidSelectPlayer") == 1 then -- 禁止选择玩家
		Gas2Gac:SetCanBeSelectOtherPlayer(player.m_Conn, false)
	end
end

function CMatchGame:OnPlayerIntoScene(player, room)
	--print("副本游戏测试输出:::::::::::::::::::::玩家进入" .. self.m_GameName .. "游戏场景!")
	if IsCppBound(player) then
		Gas2Gac:RetSetJoinBtnEnable(player.m_Conn, self.m_GameName)
		self:IntoSceneScriptCommon(player, room)
		local team = room:FindPlayer(player.m_uID)
		room:OnPlayerIntoScene(team, player)
	end
end

function CMatchGame:OnPlayerLeaveScene(player, room)
	--print("副本游戏测试输出:::::::::::::::::::::玩家离开" .. self.m_GameName .. "游戏场景!")
	if IsCppBound(player) then
		player:SetGameCamp(0)
		if MatchGame_Common(self.m_GameName, "ForbidSelectPlayer") == 1 then --恢复 允许选择玩家
			Gas2Gac:SetCanBeSelectOtherPlayer(player.m_Conn, true)
		end
		player.m_FbActionExpModulus = nil
		player.m_FbActionMoneyModulus = nil
		room:OnPlayerLeaveScene(player)
		Gas2Gac:CloseList(player.m_Conn)
	end
end

function CMatchGame:OnPlayerAtSceneReborn(player)
--print("副本游戏测试输出:::::::::::::::::::::玩家在副本中重生" .. self.m_GameName .. "游戏场景!")
	if IsCppBound(player) then
		local game, room = g_MatchGameMgr:GetRoomByScene(player.m_Scene)
		if game then --玩家副本活动上线,恢复状态
			self:IntoSceneScriptCommon(player, room)
		end
		
--		if self.m_CfgData.EnterSceneDoSkill then
--			for _, sillName in ipairs(self.m_CfgData.EnterSceneDoSkill) do
--				player:PlayerDoFightSkillWithoutLevel(sillName)
--			end
--		end
		
--		if self.m_CfgData.EnterSceneAddTempSkill then
--			for _, tempSkillName in ipairs(self.m_CfgData.EnterSceneAddTempSkill) do
--				player:AddTempSkill(tempSkillName, 1)
--			end
--		end
	end
end

function CMatchGame:PlayerOffLine(player)
	
end

function CMatchGame:PlayerLogin(player, room)
	local  team = room:FindPlayer(player.m_uID)
	self:IntoSceneScriptCommon(player, room)
	room:ResumeGame(team, player)
end

--将数据库的成员table转换成原来结构的table
local function TransformMemberTbl(dbPlayerTbl)
	local playerTbl = {}
	for _, playerId in pairs(dbPlayerTbl) do
		playerTbl[playerId] = true
	end
	return playerTbl
end

function CMatchGame:CreateNewRoom(scene, roomId, roomMembers, captainsTbl, teamIndexTbl)
	local newRoom = self:GetNewRoom(roomId)
	local oldTbl = {}
	local teamCount = 0
	for teamId, v in pairs(roomMembers) do
		local teamTbl = {}
		teamTbl.m_PlayerTbl = TransformMemberTbl(v)
		teamTbl.m_TeamId = teamId
		teamTbl.m_PlayerCount = #v
		
		oldTbl[teamId] = teamTbl
		teamCount = teamCount + 1
	end
	SetSceneMatchGameData(scene, "GameName", newRoom.m_GameName)
	SetSceneMatchGameData(scene, "RoomId", newRoom.m_RoomId)
	newRoom:SetPlayers(oldTbl, teamCount, captainsTbl, teamIndexTbl)
	newRoom:Open(scene)
end

function CMatchGame:AddTeam(roomId, actionTeamId, teamIndex, teamMember, captainName)
	local room = self:GetRoom(roomId)
	if room then
		room:AddMatchTeam(actionTeamId, teamIndex, TransformMemberTbl(teamMember), #teamMember, captainName)
	end
end

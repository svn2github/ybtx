

--特殊技能
function Gac2Gas:FightSpecialSkill(Conn, SkillName)
	if not IsCppBound(Conn.m_Player) then
		return
  end
	if SkillName == "气魂击" then
		Conn.m_Player:PlayerDoPassiveSkill(SkillName, 1)
		Conn.m_Player:IsFirstTimeAndSendMsg("气魂击", 2004, sMsg)
	else
		assert(false ,"Gac2Gas:FightSpecialSkill 非法请求")
	end
end

--计算魂值公式
local function CalculateSingleFetch(MonsterLevel,CharacterLevel)
	local uLevelDiff = MonsterLevel - CharacterLevel;
	local uFetchValue=0

	if uLevelDiff <= -20 then
		return 1
	end

	if uLevelDiff > 5 then
		uLevelDiff = 5
	end

	uFetchValue = ((CharacterLevel*3)+50) * (1 + uLevelDiff*4 / (CharacterLevel+60))
	return uFetchValue
end

--计算魂值公式,玩法剧情本
local function CalculateSingleFetchInMission(MonsterLevel,CharacterLevel)
	--怪物魂值=2*((人物等级*3)+50)*(0.6+0.4*(怪物等级/人物等级)）
	local uLevelDiff = MonsterLevel - CharacterLevel;
	local uFetchValue = ((CharacterLevel*3)+50) * (0.6 + 0.4*(MonsterLevel/CharacterLevel))
	return uFetchValue
end

local function GetFetchValue(self,Monster,uSoulMul)
	local CharacterLevel = self:CppGetLevel()
  local MonsterLevel   = Monster:CppGetLevel()
  local MonsterName = Monster.m_Properties:GetCharName()
  local NpcFetchModulus = 1
  local ExpType = 0
  if Npc_Common(MonsterName) then
   	NpcFetchModulus = Npc_Common(MonsterName,"FetchModulus")
  	ExpType = Npc_Common(MonsterName,"ExpType")
  end
  local uFetchValue = 1
  if ExpType == 0 then
		uFetchValue = CalculateSingleFetch(MonsterLevel,CharacterLevel)*uSoulMul*NpcFetchModulus*2
	else
		uFetchValue = CalculateSingleFetchInMission(MonsterLevel,CharacterLevel)*uSoulMul*NpcFetchModulus*2
	end
  return uFetchValue
end

local function Check2PlayerInSameArea(Player1,Player2)
	if Player1 == nil or Player2 == nil then
		return false
	end
	
	local sceneId1 = Player1.m_Scene.m_SceneId
	local sceneId2 = Player2.m_Scene.m_SceneId
	local AreaName1 = Player1.m_AreaName
	local AreaName2 = Player2.m_AreaName
	return (AreaName1~= "") and (sceneId1 == sceneId2) and (AreaName1 == AreaName2)
end



local function GetFetchModulus(Player)
	local expModulus = 1
	local SceneType = Player.m_Scene.m_SceneAttr.SceneType
	
	if Player.m_FbActionExpModulus then
--		expModulus = Player.m_FbActionExpModulus
--		if IsCppBound(Player) and Player:ExistState("三倍经验的buff") then 
--			expModulus = expModulus * Player.m_ExpModulusByItem 
--		end
		if IsCppBound(Player) and Player:ExistState("安装三合一豪华版状态") then 
			expModulus = Player.m_FetchModulus 
		end
		if IsCppBound(Player) and Player:ExistState("安装五合一卡状态") then 
			expModulus = Player.m_FetchModulus 
		end
--	elseif SceneType == 23 and SceneType == 24 then
--		expModulus = Player.m_FetchModulus
	elseif SceneType ~= 18 and SceneType ~= 19 then
		expModulus = Player.m_FetchModulus
	end
	return expModulus
end

--队伍没满：A获得的魂值 = A单打*(0.7+0.3*(队伍内所有成员的等级和/队伍内成员最高等级))/队伍成员总数
--队伍满：A获得的魂值 = A单打*((0.7+0.3*(队伍内所有成员的等级和/队伍内成员最高等级))/队伍成员总数 + 0.1)
--没组队：A获得的魂值 = A单打*1
local function AbsorbSoul(uSoulMul,Monster,player)
	local uTeamID = player.m_Properties:GetTeamID()
	local uCanShareNum = 1
	local soul_rate = 1
	local uLevelSum = player:CppGetLevel()
	local SceneType = player.m_Scene.m_SceneAttr.SceneType
	local levelTbl = {}
	table.insert(levelTbl,uLevelSum)
  local TeamMembersFetchModulus = {}
  TeamMembersFetchModulus[player.m_uID] = GetFetchModulus(player)
  if uTeamID ~= 0 then
		local tblMembers = g_TeamMgr:GetMembers(uTeamID)
		if #tblMembers > 0 then
			for i = 1, #(tblMembers) do
				local Member = g_GetPlayerInfo(tblMembers[i])
				if Member and IsServerObjValid(Member) then
					if player ~= Member and Check2PlayerInSameArea(player,Member) then
						TeamMembersFetchModulus[tblMembers[i]] = GetFetchModulus(Member)
						uLevelSum = uLevelSum + Member:CppGetLevel()
						table.insert(levelTbl,Member:CppGetLevel())
						uCanShareNum = uCanShareNum + 1
					end
				end
			end
			if uCanShareNum > 1 then
				local high_level = math.max(unpack(levelTbl))
				if uCanShareNum < 5 then
					soul_rate = (0.7+0.3*(uLevelSum/high_level))/uCanShareNum
				else
					soul_rate = (0.7+0.3*(uLevelSum/high_level))/uCanShareNum + 0.1
				end
			end
		end
	else
		uTeamID = nil
	end
	for j,v in pairs(TeamMembersFetchModulus) do
		local tempPlayer = g_GetPlayerInfo(j)
		if tempPlayer then
			local uContinuousBurstTimes = tempPlayer:GetContinuousBurstTimes()
			if uContinuousBurstTimes > 10 then
				uContinuousBurstTimes = 10
			elseif uContinuousBurstTimes < 1 then
				uContinuousBurstTimes = 1
			end
			local uFetchValue = GetFetchValue(tempPlayer,Monster,uSoulMul)
			uFetchValue = uFetchValue*(1+(uContinuousBurstTimes-1)/6)
			uFetchValue = string.format("%.0f", uFetchValue*soul_rate)
			local uAddSoulNum = uFetchValue * v
			if uAddSoulNum < 1 then
  			uAddSoulNum = 1
  		end
			local left_count = CExpOrSoulStorage.StorageSoulFuc(tempPlayer,uAddSoulNum)
			if left_count > 0 then
				tempPlayer.m_uSoulValue = tempPlayer.m_uSoulValue + left_count
				tempPlayer.m_uKillDropSoulCount = tempPlayer.m_uKillDropSoulCount + left_count
				Gas2Gac:ReturnModifyPlayerSoulNum(tempPlayer.m_Conn,tempPlayer.m_uSoulValue)
				Gas2Gac:ReturnCharAddSoulMsg(tempPlayer.m_Conn,left_count)
			end
		end
	end
end

local function AbsorbSoulInMission(uSoulMul,Monster,player)
	local uTeamID = player.m_Properties:GetTeamID()
  local TeamMembersFetchModulus = {}
  TeamMembersFetchModulus[player.m_uID] = GetFetchModulus(player)
  local uTeamMembersCount = 1
  if uTeamID ~= 0 then
		local tblMembers = g_TeamMgr:GetMembers(uTeamID)
		uTeamMembersCount = #tblMembers
		for i = 1, uTeamMembersCount do
			local Member = g_GetPlayerInfo(tblMembers[i])
			if Member and IsServerObjValid(Member) then
				if player ~= Member and Check2PlayerInSameArea(player,Member) then
					TeamMembersFetchModulus[tblMembers[i]] = GetFetchModulus(Member)
				end
			end
		end
	else
		uTeamID = nil
	end
	uTeamMembersCount = uTeamMembersCount > 1 and uTeamMembersCount or 1
	uTeamMembersCount = uTeamMembersCount > 5 and 5 or uTeamMembersCount
	local tbl = {1,0.65,0.53,0.475,0.54}
	local MultiInTeam = tbl[uTeamMembersCount]
	if MultiInTeam == nil then
		MultiInTeam = 1
	end
	for j,v in pairs(TeamMembersFetchModulus) do
		local tempPlayer = g_GetPlayerInfo(j)
		if tempPlayer then
			local uContinuousBurstTimes = tempPlayer:GetContinuousBurstTimes()
			if uContinuousBurstTimes > 10 then
				uContinuousBurstTimes = 10
			elseif uContinuousBurstTimes < 1 then
				uContinuousBurstTimes = 1
			end
			local uFetchValue = GetFetchValue(tempPlayer,Monster,uSoulMul)*MultiInTeam
			uFetchValue = string.format("%.0f", uFetchValue*(1+(uContinuousBurstTimes-1)/6))
			local uAddSoulNum = uFetchValue * v
			if uAddSoulNum < 1 then
  			uAddSoulNum = 1
  		end
  		local left_count = CExpOrSoulStorage.StorageSoulFuc(tempPlayer,uAddSoulNum)
			if left_count > 0 then
				tempPlayer.m_uSoulValue = tempPlayer.m_uSoulValue + left_count
				tempPlayer.m_uKillDropSoulCount = tempPlayer.m_uKillDropSoulCount + left_count
				Gas2Gac:ReturnModifyPlayerSoulNum(tempPlayer.m_Conn,tempPlayer.m_uSoulValue)
				Gas2Gac:ReturnCharAddSoulMsg(tempPlayer.m_Conn,left_count)
			end
		end
	end
end

function CCharacterMediator:OnBurstSoulPrize(Monster,uSoulMul,bExtra)
	local uContinuousBurstTimes = self:GetContinuousBurstTimes()
	if uContinuousBurstTimes >= 1 then
		--两秒内暴魂超过2次,显示界面特效 1魄，2魄，3魄
		Gas2Gac:UpdateComboHitsTimes(self.m_Conn, uContinuousBurstTimes)
	end
	local MonsterName = Monster.m_Properties:GetCharName()
  if Npc_Common(MonsterName) and Npc_Common(MonsterName,"ExpType") == 0 then
  	AbsorbSoul(uSoulMul,Monster,self)
  else
  	AbsorbSoulInMission(uSoulMul,Monster,self)
  end
end

function CCharacterMediator:InitBurstSoulTime(BurstSoulTime)
	self:SetBurstSoulTimes(BurstSoulTime)
end

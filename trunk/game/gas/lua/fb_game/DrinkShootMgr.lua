cfg_load "fb_game/DrinkShoot_Common"

local ActionName = "ºÈ¾ÆÉä¼ý"
--ºÈ¾ÆÉä¼ý»î¶¯±¨Ãû´¦
CDrinkShootMgr = class()

function CDrinkShootMgr:Ctor()
	self.m_MaxRound = 3
	self.m_NowRound = 1
	self.m_NextPlayerNum = 0
	self.m_Players = {}
end

g_DrinkShootMgr = CDrinkShootMgr:new()
function CDrinkShootMgr:BeginGame(Conn)
	local TeamId = Conn.m_Player.m_Properties:GetTeamID()
	if not TeamId or TeamId == 0 then
		return
	end
	
	local SceneName = DrinkShoot_Common(ActionName, "SceneName")
	local MemberTbl = g_TeamMgr:GetMembers(TeamId)
	self.m_Players = {}
	for i=1, #(MemberTbl) do
		local MemberId = MemberTbl[i]
		local Member = g_GetPlayerInfo(MemberId)
		if IsCppBound(Member) and IsCppBound(Member.m_Conn)
			and Member.m_Scene.m_SceneName == SceneName then
				Member:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, true)
				Member:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, true)
				Member:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, true)
				Member:DisableDirectorMoving(true)
				self:ChangeActionBag(Member, "diao")
				table.insert(self.m_Players, {Member, 0})
		end
	end
	
	local n = #(self.m_Players)
	if n < 3 then
		--return
	end
	self.m_NowRound = 1
	self.m_MaxRound = n
	self.m_NextPlayerNum = math.random(1, n)
	
	local function fun()
		self:NextPlayer()
		for i=1, #(self.m_Players) do
			local Player = self.m_Players[i][1]
			self:DoAction(Player.m_Conn, "stand01")
		end
	end
	for i=1, #(self.m_Players) do
		local Player = self.m_Players[i][1]
		self:DoAction(Player.m_Conn, "start01")
	end
	RegisterOnceTick(g_App, "BeginDrinkShootTick", fun, 85*33)
end

function CDrinkShootMgr:NextPlayer(Conn)
	if self.m_NextPlayerNum == 0 then
		return
	end
	
	local tbl = GetCfgTransformValue(true, "DrinkShoot_Common", "ºÈ¾ÆÉä¼ý", "Skills")
	if Conn and IsCppBound(Conn) then
		Conn.m_Player:RemoveTempSkill(tbl[1], 1)
	end
	
	local i = self.m_NextPlayerNum
	local n = #(self.m_Players)
	if i == n then
		self.m_NextPlayerNum = 1
	else
		self.m_NextPlayerNum = i + 1
	end
	if not IsCppBound(self.m_Players[i][1]) then
		table.remove(self.m_Players, i)
		self.NextPlayer()
		return
	end
	
	local pos = CFPos:new()
	self.m_Players[i][1]:AddTempSkill(tbl[1], 1)
	self.m_Players[i][1]:GetPixelPos(pos)
	for j=1, n do
		Gas2Gac:UpdateDrinkShootTurnArrow(self.m_Players[j][1].m_Conn, pos.x, pos.y)
	end
	Gas2Gac:BeginDrinkShoot(self.m_Players[i][1].m_Conn)
end

function CDrinkShootMgr:NextRound(KillerEntityId)
	if self.m_NowRound == self.m_MaxRound then
		self:GameOver()
	else
		local Killer = CEntityServerManager_GetEntityByID(KillerEntityId)
		local n = #(self.m_Players)
		for i=1, n do
			if self.m_Players[i][1].m_uID == Killer.m_uID then
				self.m_Players[i][2] = self.m_Players[i][2] + 1
				local tbl = GetCfgTransformValue(true, "DrinkShoot_Common", "ºÈ¾ÆÉä¼ý", "Skills")
				self.m_Players[i][1]:RemoveTempSkill(tbl[1], 1)
				break
			end
		end
		self.m_NowRound = self.m_NowRound + 1
		self.m_NextPlayerNum = math.random(1, n)
		--self:NextPlayer()
	end
end

function CDrinkShootMgr:GameOver()
	for i=1, #(self.m_Players) do
		local Player = self.m_Players[i][1]
		self:ChangeActionBag(Player, "")
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, false)
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, false)
		Player:DisableDirectorMoving(false)
		local tbl = GetCfgTransformValue(true, "DrinkShoot_Common", "ºÈ¾ÆÉä¼ý", "Skills")
		Player:RemoveTempSkill(tbl[1], 1)
		Gas2Gac:UpdateDrinkShootTurnArrow(Player.m_Conn, 0, 0)
	end
	g_DrinkShootMgr = CDrinkShootMgr:new()
end

local action = {
	["start01"] 		= EActionState.eAS_Throw,
	["stand01"]			= EActionState.eAS_WaitBite,
	["disturbed01"] = EActionState.eAS_Disturbed01,
	["disturbed02"] = EActionState.eAS_Disturbed02,
	["disturbed03"] = EActionState.eAS_Disturbed03,
	["bite01"] 			= EActionState.eAS_Bite,
	["stand02"] 		= EActionState.eAS_WaitPull,
	["pull01"] 			= EActionState.eAS_Pull01,
	["pull02"] 			= EActionState.eAS_Pull02,
	["pull03"] 			= EActionState.eAS_Pull03,
	["succeed01"] 	= EActionState.eAS_Succeed01,
	["succeed02"]		= EActionState.eAS_Succeed02,
	["end01"]				= EActionState.eAS_Idle_BackWpn,
}

--²¥·Å¶¯×÷
function CDrinkShootMgr:DoAction(Conn, ActionName)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	if self.m_NextPlayerNum == 0 then
		return
	end
	Conn.m_Player:SetAndSyncActionState(action[ActionName])
end

--¸ü»»¶¯×÷°ü
function CDrinkShootMgr:ChangeActionBag(Player, AniFileTailName)
	local EntityID = Player:GetEntityID()
	Player.m_Properties:SetBehaviorType(AniFileTailName)
	Gas2Gac:UpdateModel( Player.m_Conn, EntityID)
	Gas2Gac:UpdateModel( Player:GetIS(0), EntityID)
	if AniFileTailName == "" then
		Gas2Gac:UpdateAni( Player.m_Conn, EntityID)
		Gas2Gac:UpdateAni( Player:GetIS(0), EntityID)
		Player:SetAndSyncActionState(action["end01"])
	end
end

function Gac2Gas:DrinkShootDoSkill(Conn, SkillName, PosX, PosY)
	local pos = CFPos:new()
	pos.x, pos.y = PosX, PosY
	Conn.m_Player:PlayerDoPosSkillWithoutLevel(SkillName, pos)
end

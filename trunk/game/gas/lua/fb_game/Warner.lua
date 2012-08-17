
g_WarnerMgr = {}

CWarner = class()

CWarner.OnTickTime = 30000
CWarner.MaxHoldTime = math.ceil(120000 / CWarner.OnTickTime)
CWarner.GMPerValue = 0
CWarner.GMAllOpen = 0

local ClearValueTime = {wday = {1}, time = {"5:00"}}

function Gas2GasDef:GMSetWarner(Conn, GMPerValue, OnTickTime, MaxHoldTime, AllOpen)
	CWarner.GMPerValue = GMPerValue
	CWarner.OnTickTime = (OnTickTime ~= 0) and OnTickTime*1000 or 30000
	MaxHoldTime = (MaxHoldTime ~= 0) and MaxHoldTime*1000 or 120000
	CWarner.MaxHoldTime = math.ceil(MaxHoldTime / CWarner.OnTickTime)
	CWarner.GMAllOpen = AllOpen
end

function CWarner:Ctor(SceneId, PerValue, AddBuff, TotalTime, TeamTbl)
	self.m_WarnTbl = {}
	self.m_TeamTbl = {}
	for TeamId, st in pairs(TeamTbl) do
		self.m_TeamTbl[TeamId] = {}
		for PlayerId, _ in pairs(st) do
			self.m_WarnTbl[PlayerId] = {}
			self.m_WarnTbl[PlayerId].WarnValue = 0
			self.m_WarnTbl[PlayerId].HoldTime = 0
			self.m_WarnTbl[PlayerId].TeamId = TeamId
			self.m_TeamTbl[TeamId][PlayerId] = self.m_WarnTbl[PlayerId]
			local Player = g_GetPlayerInfo(PlayerId)
			if IsCppBound(Player) then
				MsgToConn(Player.m_Conn, 191052)
			end
		end
	end
	
	self.m_SceneId = SceneId
	self.m_PerValue = ((CWarner.GMPerValue ~= 0) and CWarner.GMPerValue) or PerValue
	self.m_AddBuff = AddBuff
	self.m_TotalTime = ((TotalTime == 0) and 0) or math.ceil( TotalTime / (CWarner.OnTickTime/1000) )
	self.m_NowTime = 0
	
	self.m_WarnTick = RegClassTick("WarnTick", self.OnWarnTick, CWarner.OnTickTime, self)
end

function CWarner:Destroy()
	--print("Destroy=====================================")
	
	for PlayerId, _ in pairs(self.m_WarnTbl) do
		local Player = g_GetPlayerInfo(PlayerId)
		if IsCppBound(Player) then
			Player:PlayerDoPassiveSkill("5人用出场撤销", 1)
			Player:ClearActivityValue()
		end
	end
	
	local SceneId = self.m_SceneId
	UnRegisterTick(self.m_WarnTick)
	self.m_WarnTick = nil
	self.m_PerValue = nil
	self.m_AddBuff = nil
	self.m_TotalTime = nil
	self.m_NowTime = nil
	self.m_WarnTbl = nil
	self.m_TeamTbl = nil
	self.m_SceneId = nil
	self = nil
	g_WarnerMgr[SceneId] = nil
end

function CWarner:OnWarnTick()
	for PlayerId, st in pairs(self.m_WarnTbl) do
		local Player = g_GetPlayerInfo(PlayerId)
		if IsCppBound(Player) then
			
			if Player:GetActivityValue() == 0 then	--无伤害
				st.HoldTime = st.HoldTime + 1
				if st.HoldTime >= CWarner.MaxHoldTime then --挂机状态
					if st.WarnValue < 10000 and self.m_PerValue > 0 then
						st.WarnValue = st.WarnValue + self.m_PerValue
						MsgToConn(Player.m_Conn, 191049, self.m_PerValue)
					end
					if self.m_AddBuff == 1 and st.HoldTime % CWarner.MaxHoldTime == 0 then
						self:AddBuff(Player)
					end
				end
			else
				st.HoldTime = 0
			end
			
			Player:ClearActivityValue()
		end
		--print(PlayerId, st.WarnValue, st.HoldTime, CWarner.MaxHoldTime, self.m_NowTime + 1, self.m_TotalTime)
	end
	
	if self.m_TotalTime ~= 0 then
		self.m_NowTime = self.m_NowTime + 1
		if self.m_NowTime >= self.m_TotalTime then
			self:OnSaveWarnValue()
		end
	end
end

function CWarner:AddBuff(Player)
	local TeamId = self.m_WarnTbl[Player.m_uID].TeamId
	local PlayerName = Player.m_Properties:GetCharName()
	for id, st in pairs(self.m_TeamTbl[TeamId]) do
		if id ~= Player.m_uID then
			local Member = g_GetPlayerInfo(id)
			if IsCppBound(Member) and st.HoldTime < CWarner.MaxHoldTime and not Member:ExistState("5人用不给经验状态") then
				Member:PlayerDoPassiveSkill("5人用百分之二十经验增加", 1)
				MsgToConn(Member.m_Conn, 191050, PlayerName)
			end
		end
	end
	Player:PlayerDoPassiveSkill("5人用不给经验", 1)
	MsgToConn(Player.m_Conn, 191051, PlayerName)
end

function CWarner:DeletePlayer(PlayerId)
	if not self.m_WarnTbl[PlayerId] then
		return
	end
	
	if self.m_WarnTbl[PlayerId].WarnValue ~= 0 then
		self:OnSaveWarnValue(PlayerId)
	else
		local Player = g_GetPlayerInfo(PlayerId)
		if IsCppBound(Player) then
			Player:PlayerDoPassiveSkill("5人用出场撤销", 1)
			Player:ClearActivityValue()
		end
		self.m_TeamTbl[ self.m_WarnTbl[PlayerId].TeamId ][PlayerId] = nil
		self.m_WarnTbl[PlayerId] = nil
		if not next(self.m_WarnTbl) then
			self:Destroy()
		end
	end
end

function CWarner:OnSaveWarnValue(PlayerId)
	--print("OnSaveWarnValue")
	local WarnTbl
	if PlayerId then
		WarnTbl = {}
		WarnTbl[PlayerId] = {["WarnValue"] = self.m_WarnTbl[PlayerId].WarnValue}
	else
		WarnTbl = self.m_WarnTbl
	end
	
	CallDbTrans("JoinActionDB", "UpdateWarnValue", nil, WarnTbl, self.m_SceneId)
	
	if PlayerId then
		local Player = g_GetPlayerInfo(PlayerId)
		if IsCppBound(Player) then
			Player:PlayerDoPassiveSkill("5人用出场撤销", 1)
			Player:ClearActivityValue()
		end
		self.m_TeamTbl[ self.m_WarnTbl[PlayerId].TeamId ][PlayerId] = nil
		self.m_WarnTbl[PlayerId] = nil
		if not next(self.m_WarnTbl) then
			self:Destroy()
		end
	else
		self:Destroy()
	end
end

function SavePlayerWarnValue(SceneId, PlayerId)
	if not g_WarnerMgr[SceneId] then
		return
	end
	
	if PlayerId then
		g_WarnerMgr[SceneId]:DeletePlayer(PlayerId)
	else
		g_WarnerMgr[SceneId]:OnSaveWarnValue()
	end
end

function CreateWarner(SceneId, WarnInfo, TotalTime, TeamTbl)
	assert(not g_WarnerMgr[SceneId], "CreateWarner g_WarnerMgr[SceneId] "..SceneId)
	g_WarnerMgr[SceneId] = CWarner:new(SceneId, WarnInfo[1], WarnInfo[2], TotalTime, TeamTbl)
end

function ClearAllWarner()
	for _, warner in pairs(g_WarnerMgr) do
		warner:Destroy()
	end
end

local function ClearAllWarnValue()
	local function CallBack()
		Gas2Gac:SetActionPanelWarnValue(g_AllPlayerSender, 0)
	end
	CallDbTrans("JoinActionDB", "ClearAllWarnValue", CallBack, {}, "ClearAllWarnValue")
end

g_AlarmClock:AddTask("ClearWarnValue", ClearValueTime, ClearAllWarnValue)

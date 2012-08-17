gas_require "world/tong_area/WarZoneSentryInc"

local SentryFile = {}
SentryFile[1] = "艾米哨所"
SentryFile[2] = "神圣哨所"
SentryFile[3] = "修斯哨所"

local SoldierRebornTime = 30 * 1000
local DefenderRebornTime =  30 * 60 * 1000
local HelperBornTime = 30 * 1000

function CWarZoneSentry:Init(coreWarZone, sentryIndex)
	self.m_SentryIndex = sentryIndex
	self.m_CoreWarZone = coreWarZone
	self.m_CenterPos = g_WarZoneMgr.m_SentryCenterPos[sentryIndex]
	self.m_Camp = math.floor(sentryIndex / 10)
	self.m_Number = math.floor(sentryIndex % 10)
	self:Create()
end

function CWarZoneSentry:Create()
	self.m_CurrentState = "BeOccupied"
	self.m_SoldierRebornTickTbl = {}
	self.m_SoldierTbl = {}
	self.m_HelperTbl = {}
	self.m_HelperDeadTbl = {}
	self.m_InBattleNum = 0
	
	local sentryIndex = self.m_SentryIndex
	local warZone = self.m_CoreWarZone
	local number = self.m_Number
	local defaultCamp = self.m_Camp
	
	if number == 1 or number == 2 then
		local stationIndex = tonumber(defaultCamp .. "11")
		local nowCamp = warZone.m_StationOwnerCamp[stationIndex]
		local IsOccupied = (defaultCamp ~= nowCamp)
		self:DoCreate(IsOccupied)
	elseif number == 3 or number == 4 then
		local stationIndex1 = tonumber(defaultCamp .. "21")
		local stationIndex2 = tonumber(defaultCamp .. "22")
		local stationIndex3 = tonumber(defaultCamp .. "23")
		local IsOccupied = true
		if defaultCamp == self.m_CoreWarZone.m_StationOwnerCamp[stationIndex1] or
			 defaultCamp == self.m_CoreWarZone.m_StationOwnerCamp[stationIndex2] or
			 defaultCamp == self.m_CoreWarZone.m_StationOwnerCamp[stationIndex3] then
			IsOccupied = false
		end
		self:DoCreate(IsOccupied)
	end
end

function CWarZoneSentry:DoCreate(IsOccupied)
	if not IsOccupied then
		local sentryIndex = self.m_SentryIndex
		local camp = self.m_Camp
		local centerPos = self.m_CenterPos
		local npcTbl = g_DynamicCreateMgr:Create(self.m_CoreWarZone.m_Scene, SentryFile[camp], centerPos)
		for index, npc in pairs(npcTbl) do
			npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
			npc:CppSetCamp(camp)
			npc.m_SentryIndex = sentryIndex
			npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
			
			if npc.m_Name == g_WarZoneMgr.m_SentrySoldierTbl[camp] then
				npc.m_SoldierIndex = index
				self.m_SoldierTbl[index] = npc
			elseif npc.m_Name == g_WarZoneMgr.m_SentryDefenderTbl[camp] then
				self.m_Defender = npc
			end
		end
		self.m_CurrentState = "Normal"
	end
end

function CWarZoneSentry:Destroy()
	for _,npc in pairs(self.m_SoldierTbl) do
		g_NpcServerMgr:DestroyServerNpcNow(npc, false)
	end
	
	g_NpcServerMgr:DestroyServerNpcNow(self.m_Defender, false)
end

function CWarZoneSentry:Recover()
	self:Destroy()
	self:Create(false)
	self:RecoverPatrol()
end

function CWarZoneSentry:SoldierRebornTick(tick, soldierIndex, bornPos)
	if self.m_SoldierRebornTickTbl[soldierIndex] then
		local camp = self.m_Camp
		local warZone = self.m_CoreWarZone
		local pos = CFPos:new(bornPos[1] * EUnits.eGridSpan, bornPos[2] * EUnits.eGridSpan)
		local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_SentrySoldierTbl[camp], g_WarZoneMgr.m_NpcLevel, warZone.m_Scene, pos)
		if not IsServerObjValid(npc) then
			LogErr("帮会战创建Npc失败", logMsg)
			return
		end
		npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		local fPos = CFPos:new(bornPos[1], bornPos[2])
		npc.m_RebornPos = fPos
		npc.m_SentryIndex = self.m_SentryIndex
		npc.m_SoldierIndex = soldierIndex
		self.m_SoldierTbl[soldierIndex] = npc
		UnRegisterTick(self.m_SoldierRebornTickTbl[soldierIndex])
		self.m_SoldierRebornTickTbl[soldierIndex] = nil	
	end
end

function CWarZoneSentry:SoldierDead(Npc)
	local soldierIndex = Npc.m_SoldierIndex
	local bornPos = {}
	bornPos[1] = Npc.m_RebornPos.x
	bornPos[2] = Npc.m_RebornPos.y
	
	if not IsServerObjValid(self.m_Defender) then
		return
	end
	
	if not self.m_SoldierRebornTickTbl[soldierIndex] then
		self.m_SoldierRebornTickTbl[soldierIndex] = RegClassTick("SentrySoldierRebornTick", self.SoldierRebornTick, SoldierRebornTime, self, soldierIndex, bornPos)
	end
end

function CWarZoneSentry:DefenderRebornTick(tick, bornPos)
	if self.m_DefenderRebornTick then
		if self.m_CurrentState == "Normal" then
			self:Recover()
		end
		UnRegisterTick(self.m_DefenderRebornTick)
		self.m_DefenderRebornTick = nil
	end
end

function CWarZoneSentry:DefenderDead()
	if self.m_CurrentState == "Normal" and not self.m_DefenderRebornTick then
		self.m_DefenderRebornTick = RegClassTick("SentryDefenderRebornTick", self.DefenderRebornTick, DefenderRebornTime, self)
	end
	
	for soldierIndex,soldierRebornTick in pairs(self.m_SoldierRebornTickTbl) do
		if soldierRebornTick then
			UnRegisterTick(soldierRebornTick)
			self.m_SoldierRebornTickTbl[soldierIndex] = nil
		end
	end
	
	if self.m_HelperBornTick then
		UnRegisterTick(self.m_HelperBornTick)
		self.m_HelperBornTick = nil
	end
	self:DestroyPatrol()
end

function CWarZoneSentry:DestroyPatrol()
	local number = self.m_Number
	local camp = self.m_Camp
	local patrolIndex
	local warZone = self.m_CoreWarZone
	
	if number == 1 or number == 2 then
		patrolIndex = camp * 10 + number
		warZone.m_SentryPatrolTbl[patrolIndex]:Destroy()
	elseif number == 3 then
		patrolIndex = camp * 10 + 1
		warZone.m_SentryPatrolTbl[patrolIndex]:Destroy()
		
		patrolIndex = camp * 10 + 3
		warZone.m_SentryPatrolTbl[patrolIndex]:Destroy()
	elseif number == 4 then
		patrolIndex = camp * 10 + 2
		warZone.m_SentryPatrolTbl[patrolIndex]:Destroy()
		
		patrolIndex = camp * 10 + 4
		warZone.m_SentryPatrolTbl[patrolIndex]:Destroy()
	end
end

function CWarZoneSentry:RecoverPatrol()
	local number = self.m_Number
	local camp = self.m_Camp
	local patrolIndex
	local warZone = self.m_CoreWarZone
	
	if number == 1 then
		if warZone.m_SentryTbl[camp * 10 + 3] and IsServerObjValid(warZone.m_SentryTbl[camp * 10 + 3].m_Defender) then
			patrolIndex = camp * 10 + 1
			warZone.m_SentryPatrolTbl[patrolIndex]:Recover()
		end
	elseif number == 2 then
		if warZone.m_SentryTbl[camp * 10 + 4] and IsServerObjValid(warZone.m_SentryTbl[camp * 10 + 4].m_Defender) then
			patrolIndex = camp * 10 + 2
			warZone.m_SentryPatrolTbl[patrolIndex]:Recover()
		end
	elseif number == 3 then
		if warZone.m_SentryTbl[camp * 10 + 1] and IsServerObjValid(warZone.m_SentryTbl[camp * 10 + 1].m_Defender) then
			patrolIndex = camp * 10 + 1
			warZone.m_SentryPatrolTbl[patrolIndex]:Recover()
		end
		
		patrolIndex = camp * 10 + 3
		warZone.m_SentryPatrolTbl[patrolIndex]:Recover()
	elseif number == 4 then
		if warZone.m_SentryTbl[camp * 10 + 2] and IsServerObjValid(warZone.m_SentryTbl[camp * 10 + 2].m_Defender) then
			patrolIndex = camp * 10 + 2
			warZone.m_SentryPatrolTbl[patrolIndex]:Recover()
		end
		
		patrolIndex = camp * 10 + 4
		warZone.m_SentryPatrolTbl[patrolIndex]:Recover()
	end
end

function CWarZoneSentry:HelperDead(helperIndex)
	table.insert(self.m_HelperDeadTbl, helperIndex)
end

function CWarZoneSentry:HelperBornTick()
	if self.m_HelperBornTick then
		local camp = self.m_Camp
		local sentryIndex = self.m_SentryIndex
		local helpNpcNum = 0
		for _, helpNpc in pairs(self.m_HelperTbl) do
			if IsServerObjValid(helpNpc) and helpNpc:CppIsLive() then
				helpNpcNum = helpNpcNum + 1
			end
		end
		
		for i = helpNpcNum, 5 do
			local RandomX = math.random(0, 5)/4
			local RandomY = math.random(0, 5)/4
			local pos = g_WarZoneMgr.m_SentryCenterPos[sentryIndex]
			local fPos = CFPos:new((pos[1] + RandomX) * EUnits.eGridSpan, (pos[2] + RandomY) * EUnits.eGridSpan)
			local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_SentryHelperTbl[camp], g_WarZoneMgr.m_NpcLevel, self.m_CoreWarZone.m_Scene, fPos)
			if not IsServerObjValid(npc) then
				LogErr("帮会战创建Npc失败", logMsg)
				return
			end
			npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
			npc.m_SentryIndex = sentryIndex
			if helpNpcNum == 0 and #self.m_HelperDeadTbl == 0 then
				npc.m_HelperIndex = i + 1
				self.m_HelperTbl[i + 1] = npc
			else
				for k, index in pairs(self.m_HelperDeadTbl) do
					npc.m_HelperIndex = index
					self.m_HelperTbl[index] = npc
					self.m_HelperDeadTbl[k] = nil
					break
				end
			end
		end
	end
end

function CWarZoneSentry:EnterBattle()
	if not IsServerObjValid(self.m_Defender) then
		return
	end
	
	self.m_InBattleNum = self.m_InBattleNum + 1
	
	if not self.m_HelperBornTick then
		self.m_HelperBornTick = RegClassTick("SentryHelperBornTick", self.HelperBornTick, HelperBornTime, self)
	end
end

function CWarZoneSentry:LeaveBattle()
	self.m_InBattleNum = self.m_InBattleNum - 1
	if self.m_InBattleNum < 0 then
		self.m_InBattleNum = 0
	end
	
	if self.m_InBattleNum == 0 and self.m_HelperBornTick then
		for index,npc in pairs(self.m_HelperTbl) do
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
			self.m_HelperTbl[index] = nil
		end
		UnRegisterTick(self.m_HelperBornTick)
		self.m_HelperBornTick = nil
		self.m_HelperDeadTbl = {}
	end
end

function CWarZoneSentry:Occupied()
	self.m_CurrentState = "BeOccupied"
	if self.m_DefenderRebornTick then
		UnRegisterTick(self.m_DefenderRebornTick)
		self.m_DefenderRebornTick = nil
	end
end
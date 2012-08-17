gas_require "world/tong_area/WarZoneCommandInc"

local CommandFile = {}
CommandFile[1] = "艾米司令部"
CommandFile[2] = "神圣司令部"
CommandFile[3] = "修斯司令部"

local SoldierRebornTime = 30 * 1000
local DefenderRebornTime =  30 * 60 * 1000
local HelperBornTime = 15 * 1000

function CWarZoneCommand:Init(coreWarZone, commandIndex)
	self.m_CommandIndex = commandIndex
	self.m_CoreWarZone = coreWarZone
	self.m_CenterPos = g_WarZoneMgr.m_CommandCenterPos[commandIndex]
	
	self.m_SoldierTbl = {}
	self.m_HelperTbl = {}
	self.m_DefenderTbl = {}
	self.m_HelperDeadTbl = {}
	self.m_InBattleNum = 0
	self.m_SoldierRebornTickTbl = {}
	self.m_DefenderRebornTickTbl = {}
	self:Create()
end

function CWarZoneCommand:Create()
	local commandIndex = self.m_CommandIndex
	local npcTbl = g_DynamicCreateMgr:Create(self.m_CoreWarZone.m_Scene, CommandFile[commandIndex], self.m_CenterPos)
	for index, npc in pairs(npcTbl) do
		npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
		npc:CppSetCamp(commandIndex)
		npc.m_CommandIndex = commandIndex
		npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		
		if npc.m_Name == g_WarZoneMgr.m_CommandSoldierTbl[commandIndex] then
			npc.m_SoldierIndex = index
			self.m_SoldierTbl[index] = npc
		elseif npc.m_Name == g_WarZoneMgr.m_CommandDefenderTbl[commandIndex] then
			npc.m_DefenderIndex = index
			self.m_DefenderTbl[index] = npc
		end
	end
end

function CWarZoneCommand:SoldierRebornTick(tick, soldierIndex, bornPos)
	if self.m_SoldierRebornTickTbl[soldierIndex] then
		local warZone = self.m_CoreWarZone
		local pos = CFPos:new(bornPos[1] * EUnits.eGridSpan, bornPos[2] * EUnits.eGridSpan)
		local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_CommandSoldierTbl[self.m_CommandIndex], g_WarZoneMgr.m_NpcLevel, warZone.m_Scene, pos)
		if not IsServerObjValid(npc) then
			LogErr("帮会战创建Npc失败", logMsg)
			return
		end
		npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		local fPos = CFPos:new(bornPos[1], bornPos[2])
		npc.m_RebornPos = fPos
		npc.m_CommandIndex = self.m_CommandIndex
		npc.m_SoldierIndex = soldierIndex
		self.m_SoldierTbl[soldierIndex] = npc
		UnRegisterTick(self.m_SoldierRebornTickTbl[soldierIndex])
		self.m_SoldierRebornTickTbl[soldierIndex] = nil	
	end
end

function CWarZoneCommand:SoldierDead(Npc)
	local soldierIndex = Npc.m_SoldierIndex
	local bornPos = {}
	bornPos[1] = Npc.m_RebornPos.x
	bornPos[2] = Npc.m_RebornPos.y
	
	if not self.m_SoldierRebornTickTbl[soldierIndex] then
		self.m_SoldierRebornTickTbl[soldierIndex] = RegClassTick("CommandSoldierRebornTick", self.SoldierRebornTick, SoldierRebornTime, self, soldierIndex, bornPos)
	end
end

function CWarZoneCommand:DefenderRebornTick(tick, defenderIndex, bornPos)
	if self.m_DefenderRebornTickTbl[defenderIndex] then
		local commandIndex = self.m_CommandIndex
		local pos = CFPos:new(bornPos[1] * EUnits.eGridSpan, bornPos[2] * EUnits.eGridSpan)
		local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_CommandDefenderTbl[commandIndex], g_WarZoneMgr.m_NpcLevel, self.m_CoreWarZone.m_Scene, pos)
		if not IsServerObjValid(npc) then
			LogErr("帮会战创建Npc失败", logMsg)
			return
		end
		npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		local fPos = CFPos:new(bornPos[1], bornPos[2])
		npc.m_RebornPos = fPos
		npc.m_CommandIndex = commandIndex
		npc.m_DefenderIndex = defenderIndex
		self.m_DefenderTbl[defenderIndex] = npc
		UnRegisterTick(self.m_DefenderRebornTickTbl[defenderIndex])
		self.m_DefenderRebornTickTbl[defenderIndex] = nil
	end
end

function CWarZoneCommand:DefenderDead(Npc)
	local defenderIndex = Npc.m_DefenderIndex
	local bornPos = {}
	bornPos[1] = Npc.m_RebornPos.x
	bornPos[2] = Npc.m_RebornPos.y
	
	if not self.m_DefenderRebornTickTbl[defenderIndex] then
		self.m_DefenderRebornTickTbl[defenderIndex] = RegClassTick("CommandDefenderRebornTick", self.DefenderRebornTick, DefenderRebornTime, self, defenderIndex, bornPos)
	end
end

function CWarZoneCommand:HelperDead(helperIndex)
	table.insert(self.m_HelperDeadTbl, helperIndex)
end

function CWarZoneCommand:HelperBornTick(tick)
	if self.m_HelperBornTick then
		local commandIndex = self.m_CommandIndex
		local helpNpcNum = 0
		for _, helpNpc in pairs(self.m_HelperTbl) do
			if IsServerObjValid(helpNpc) and helpNpc:CppIsLive() then
				helpNpcNum = helpNpcNum + 1
			end
		end
		
		for i = helpNpcNum, 39 do
			local RandomX = math.random(0, 39)/8
			local RandomY = math.random(0, 39)/8
			local pos = g_WarZoneMgr.m_CommandCenterPos[commandIndex]
			local fPos = CFPos:new((pos[1] + RandomX) * EUnits.eGridSpan, (pos[2] + RandomY) * EUnits.eGridSpan)
			if not g_WarZoneMgr.m_CommandHelperTbl[commandIndex] then
				LogErr("战场战区助战Npc不存在，CommandIndex为", commandIndex)
			end
			local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_CommandHelperTbl[commandIndex], g_WarZoneMgr.m_NpcLevel, self.m_CoreWarZone.m_Scene, fPos)
			if not IsServerObjValid(npc) then
				LogErr("帮会战创建Npc失败", logMsg)
				return
			end
			npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
			
			npc.m_CommandIndex = commandIndex
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

function CWarZoneCommand:EnterBattle()
	self.m_InBattleNum = self.m_InBattleNum + 1
	local commandIndex = self.m_CommandIndex
	local pos = g_WarZoneMgr.m_CommandCenterPos[commandIndex]
	local fPos = CFPos:new()
	fPos.x = pos[1] * EUnits.eGridSpan
	fPos.y = pos[2] * EUnits.eGridSpan
	if not self.m_HelpBoss then
		local Boss, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_CommandBossTbl[commandIndex], g_WarZoneMgr.m_NpcLevel, self.m_CoreWarZone.m_Scene, fPos)
		if not IsServerObjValid(Boss) then
			LogErr("帮会战创建Npc失败", logMsg)
			return
		end
		Boss.m_CommandIndex = commandIndex
		Boss.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		self.m_HelpBoss = Boss
	end
	
	if not self.m_HelperBornTick then
		self.m_HelperBornTick = RegClassTick("CommandHelperBornTick", self.HelperBornTick, HelperBornTime, self)
	end
end

function CWarZoneCommand:LeaveBattle()
	self.m_InBattleNum = self.m_InBattleNum - 1
	if self.m_InBattleNum < 0 then
		self.m_InBattleNum = 0
	end
	
	if self.m_InBattleNum == 0 and self.m_HelperBornTick then
		if self.m_HelpBoss then
			g_NpcServerMgr:DestroyServerNpcNow(self.m_HelpBoss, false)
		end
		
		for index,npc in pairs(self.m_HelperTbl) do
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
			self.m_HelperTbl[index] = nil
		end
		
		UnRegisterTick(self.m_HelperBornTick)
		self.m_HelperBornTick = nil
		self.m_HelperDeadTbl = {}
	end
end
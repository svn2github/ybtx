gas_require "world/tong_area/WarZoneNpcStationInc"

local StationFile = {}
StationFile[1] = "艾米NPC驻地"
StationFile[2] = "神圣NPC驻地"
StationFile[3] = "修斯NPC驻地"

local SoldierRebornTime = 60 * 1000
local StationManagerOverTime = 60 * 60 * 1000

function CWarZoneNpcStation:Init(coreWarZone, stationIndex, ownerTong)
	self.m_StationIndex = stationIndex
	self.m_CoreWarZone = coreWarZone
	self.m_CenterPos = g_WarZoneMgr.m_StationPos[stationIndex]
	
	self.m_DefenderTbl = {}
	self.m_SoldierTbl = {}
	self.m_SoldierRebornTickTbl = {}
	self.m_InBattleNum = 0
	if ownerTong == 0 then
		self:Create()
	end
end

function CWarZoneNpcStation:Create()
	local stationIndex = self.m_StationIndex
	local camp = WarZoneBasic:GetStationDefaultCamp(stationIndex)
	
	self.m_DefenderTbl = {}
	self.m_SoldierTbl = {}
	self.m_SoldierRebornTickTbl = {}
	self.m_InBattleNum = 0
	
	local npcTbl = g_DynamicCreateMgr:Create(self.m_CoreWarZone.m_Scene, StationFile[camp], self.m_CenterPos)
	for index, npc in pairs(npcTbl) do
		npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
		npc:CppSetCamp(camp)
		npc.m_StationIndex = stationIndex
		npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		if npc.m_Name == g_WarZoneMgr.m_StationDefenderTbl[camp] then
			if not g_WarZoneMgr.m_Warring then
				npc:ServerDoNoRuleSkill("国战全免疫", npc)
			end
			npc.m_DefenderIndex = index
			self.m_DefenderTbl[index] = npc
		elseif npc.m_Name == g_WarZoneMgr.m_StationSoldierTbl[camp] then
			npc.m_SoldierIndex = index
			self.m_SoldierTbl[index] = npc
		elseif npc.m_Name == g_WarZoneMgr.m_StationBaseCampTbl[camp] then
			if not g_WarZoneMgr.m_Warring then
				npc:ServerDoNoRuleSkill("国战全免疫", npc)
			end
			self.m_BaseCamp = npc
		end
	end
end

function CWarZoneNpcStation:SoldierRebornTick(tick, Npc)
	local npcName, warZoneId, stationIndex, soldierIndex = Npc.m_Name, Npc.m_WarZoneId, Npc.m_StationIndex, Npc.m_SoldierIndex
	local bornPos = {}
	bornPos[1] = Npc.m_RebornPos.x
	bornPos[2] = Npc.m_RebornPos.y
	local Scene = self.m_CoreWarZone.m_Scene
	if self.m_SoldierRebornTickTbl[soldierIndex] then
		local baseCamp = self.m_BaseCamp
		if not baseCamp or not IsServerObjValid(baseCamp) then
			return
		end
		local FileName  = Npc.m_SettingFileName
		local Index = Npc.m_SettingFileIndex
		local centerPosX = 0
		local centerPosY = 0
		if Scene.m_DynamicCenterPos then
			centerPosX = Scene.m_DynamicCenterPos[1]
			centerPosY = Scene.m_DynamicCenterPos[2]
		end
		local Setting = g_NpcBornMgr._m_SinglePointBornTbl[FileName][Index]
		local camp = self.m_CoreWarZone.m_StationOwnerCamp[stationIndex]
		local pos = CFPos:new(bornPos[1] * EUnits.eGridSpan, bornPos[2] * EUnits.eGridSpan)
		local npc, logMsg = g_NpcServerMgr:CreateServerNpc(npcName, g_WarZoneMgr.m_NpcLevel, self.m_CoreWarZone.m_Scene, pos)
		if not IsServerObjValid(npc) then
			LogErr("帮会战创建Npc失败", logMsg)
			return
		end
		npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
		npc.m_StationIndex = stationIndex
		
		local centerPosTbl = {centerPosX, centerPosY}
		g_NpcBornMgr:SetNpcMovePath(npc, Setting, centerPosTbl)
		
		local fPos = CFPos:new(bornPos[1], bornPos[2])
		npc.m_RebornPos = fPos
		npc.m_SettingFileName = FileName
		npc.m_SettingFileIndex = Index
		npc.m_SoldierIndex = soldierIndex
		
		self.m_SoldierTbl[soldierIndex] = npc
		UnRegisterTick(self.m_SoldierRebornTickTbl[soldierIndex])
		self.m_SoldierRebornTickTbl[soldierIndex] = nil
	end
end

function CWarZoneNpcStation:SoldierDead(Npc)
	local soldierIndex = Npc.m_SoldierIndex
	local baseCamp = self.m_BaseCamp
	if not baseCamp and not IsServerObjValid(baseCamp) then
		return
	end
	
	if not self.m_SoldierRebornTickTbl[soldierIndex] then
		self.m_SoldierRebornTickTbl[soldierIndex] = RegClassTick("NpcStationSoldierRebornTick", self.SoldierRebornTick, SoldierRebornTime, self, Npc)
	end
end

function CWarZoneNpcStation:StationManagerOverTick(tick)
	if self.m_StationManagerOverTick then
		g_NpcServerMgr:DestroyServerNpcNow(self.m_StationManager, false)
		self:Create()
		UnRegisterTick(self.m_StationManagerOverTick)
		self.m_StationManagerOverTick = nil
		self.m_CoreWarZone.m_StationOwnerCamp[self.m_StationIndex] = WarZoneBasic:GetStationDefaultCamp(self.m_StationIndex)
		self:DoRecoverSentry(self.m_StationIndex)
	end
end

function CWarZoneNpcStation:CreateStationManager()
	local pos = self.m_CenterPos
	local fPos = CFPos:new()
	fPos.x = pos[1] * EUnits.eGridSpan
	fPos.y = pos[2] * EUnits.eGridSpan
	local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_StationManager, g_WarZoneMgr.m_NpcLevel, self.m_CoreWarZone.m_Scene, fPos)
	if not IsServerObjValid(npc) then
		LogErr("帮会战创建Npc失败", logMsg)
		return
	end
	npc.m_StationIndex = self.m_StationIndex
	npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
	self.m_StationManager = npc
	
	if not self.m_StationManagerOverTick then
		self.m_StationManagerOverTick = RegClassTick("StationManagerOverTick", self.StationManagerOverTick, StationManagerOverTime, self)
	end
end

function CWarZoneNpcStation:BaseCampDead()
	for _,npc in pairs(self.m_DefenderTbl) do
		g_NpcServerMgr:DestroyServerNpcNow(npc, false)
	end
	
	for soldierIndex in pairs(self.m_SoldierRebornTickTbl) do
		if self.m_SoldierRebornTickTbl[soldierIndex] then
			UnRegisterTick(self.m_SoldierRebornTickTbl[soldierIndex])
			self.m_SoldierRebornTickTbl[soldierIndex] = nil
		end
	end
	--self.m_CoreWarZone.m_LifeTreesTbl[self.m_StationIndex]:Destroy()
	self:CreateStationManager()
end

function CWarZoneNpcStation:DoRecoverSentry()
	local stationIndex = self.m_StationIndex
	local Line = WarZoneBasic:GetStationWarLine(stationIndex)
	local warZoneSentry, sentryIndex
	local defaultCamp = WarZoneBasic:GetStationDefaultCamp(stationIndex)
	if Line == 1 then
		sentryIndex = defaultCamp * 10 + 1
		warZoneSentry = self.m_CoreWarZone.m_SentryTbl[sentryIndex]
		if warZoneSentry.m_CurrentState == "BeOccupied" then
			warZoneSentry:Recover()
		end
		
		sentryIndex = defaultCamp * 10 + 2
		warZoneSentry = self.m_CoreWarZone.m_SentryTbl[sentryIndex]
		if warZoneSentry.m_CurrentState == "BeOccupied" then
			warZoneSentry:Recover()
		end
	elseif Line == 2 then
		sentryIndex = defaultCamp * 10 + 3
		warZoneSentry = self.m_CoreWarZone.m_SentryTbl[sentryIndex]
		if warZoneSentry.m_CurrentState == "BeOccupied" then
			warZoneSentry:Recover()
		end
		
		sentryIndex = defaultCamp * 10 + 4
		warZoneSentry = self.m_CoreWarZone.m_SentryTbl[sentryIndex]
		if warZoneSentry.m_CurrentState == "BeOccupied" then
			warZoneSentry:Recover()
		end
	end
end

function CWarZoneNpcStation:Destroy()
	if self.m_BaseCamp and IsServerObjValid(self.m_BaseCamp) then
		g_NpcServerMgr:DestroyServerNpcNow(self.m_BaseCamp, false)
	end
	
	for _,npc in pairs(self.m_SoldierTbl) do
		if IsServerObjValid(npc) then
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
		end
	end
	
	for _,npc in pairs(self.m_DefenderTbl) do
		if IsServerObjValid(npc) then
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
		end
	end
	
	for soldierIndex,SoldierRebornTick in pairs(self.m_SoldierRebornTickTbl) do
		UnRegisterTick(SoldierRebornTick)
		self.m_SoldierRebornTickTbl[soldierIndex] = nil
	end
end
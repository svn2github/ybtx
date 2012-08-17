gas_require "world/tong_area/WarZoneSentryPatrolInc"

local PatrolPathNameTbl = {}
PatrolPathNameTbl[11] = "艾米B到A01"
PatrolPathNameTbl[12] = "艾米B到A02"
PatrolPathNameTbl[13] = "艾米D到B01"
PatrolPathNameTbl[14] = "艾米D到B02"
PatrolPathNameTbl[21] = "神圣B到A01"
PatrolPathNameTbl[22] = "神圣B到A02"
PatrolPathNameTbl[23] = "神圣D到B01"
PatrolPathNameTbl[24] = "神圣D到B02"
PatrolPathNameTbl[31] = "修斯B到A01"
PatrolPathNameTbl[32] = "修斯B到A02"
PatrolPathNameTbl[33] = "修斯D到B01"
PatrolPathNameTbl[34] = "修斯D到B02"

local PatrolGroupNpcDynamicFile = {}
PatrolGroupNpcDynamicFile[11] = "艾米B到A01"
PatrolGroupNpcDynamicFile[12] = "艾米B到A02"
PatrolGroupNpcDynamicFile[13] = "艾米D到B01"
PatrolGroupNpcDynamicFile[14] = "艾米D到B02"
PatrolGroupNpcDynamicFile[21] = "神圣B到A01"
PatrolGroupNpcDynamicFile[22] = "神圣B到A02"
PatrolGroupNpcDynamicFile[23] = "神圣D到B01"
PatrolGroupNpcDynamicFile[24] = "神圣D到B02"
PatrolGroupNpcDynamicFile[31] = "修斯B到A01"
PatrolGroupNpcDynamicFile[32] = "修斯B到A02"
PatrolGroupNpcDynamicFile[33] = "修斯D到B01"
PatrolGroupNpcDynamicFile[34] = "修斯D到B02"

local direction = {{0, 1}, {-1, -1}, {1, -1}}

function CWarZoneSentryPatrol:Init(coreWarZone, patrolIndex)
	self.m_PatrolIndex = patrolIndex
	self.m_CoreWarZone = coreWarZone
	
	self:Create()
end

function CWarZoneSentryPatrol:Create()
	local patrolIndex = self.m_PatrolIndex
	local filename = PatrolGroupNpcDynamicFile[patrolIndex]
	local patrolCamp = math.floor(patrolIndex / 10)
	local patrolNumber = math.floor(patrolIndex % 10)
	local warZone = self.m_CoreWarZone
	self.m_NpcTbl = {}
	self.m_NpcDeadTbl = {}
	if patrolNumber == 1 then
		if warZone.m_SentryTbl[patrolCamp * 10 + 1] and
			IsServerObjValid(warZone.m_SentryTbl[patrolCamp * 10 + 1].m_Defender) and
			warZone.m_SentryTbl[patrolCamp * 10 + 3] and
			IsServerObjValid(warZone.m_SentryTbl[patrolCamp * 10 + 3].m_Defender) then
			
			self.m_NpcTbl = g_DynamicCreateMgr:Create(warZone.m_Scene, filename)
			for index,npc in pairs(self.m_NpcTbl) do
				npc.m_WarZoneId = warZone.m_WarZoneId
				npc.m_PatrolIndex = patrolIndex
				npc.m_Index = index
				npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
				warZone.m_ColonyIDToSentryIndexTbl[npc.m_ColonyID] = patrolIndex
			end
		end
	elseif patrolNumber == 2 then
		if warZone.m_SentryTbl[patrolCamp * 10 + 2] and
			IsServerObjValid(warZone.m_SentryTbl[patrolCamp * 10 + 2].m_Defender) and
			warZone.m_SentryTbl[patrolCamp * 10 + 4] and
			IsServerObjValid(warZone.m_SentryTbl[patrolCamp * 10 + 4].m_Defender) then
			
			self.m_NpcTbl = g_DynamicCreateMgr:Create(warZone.m_Scene, filename)
			for index,npc in pairs(self.m_NpcTbl) do
				npc.m_WarZoneId = warZone.m_WarZoneId
				npc.m_PatrolIndex = patrolIndex
				npc.m_Index = index
				npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
				warZone.m_ColonyIDToSentryIndexTbl[npc.m_ColonyID] = patrolIndex
			end
		end
	elseif patrolNumber == 3 then
		if warZone.m_SentryTbl[patrolCamp * 10 + 3] and
			IsServerObjValid(warZone.m_SentryTbl[patrolCamp * 10 + 3].m_Defender) then
			
			self.m_NpcTbl = g_DynamicCreateMgr:Create(warZone.m_Scene, filename)
			for index,npc in pairs(self.m_NpcTbl) do
				npc.m_WarZoneId = warZone.m_WarZoneId
				npc.m_PatrolIndex = patrolIndex
				npc.m_Index = index
				npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
				warZone.m_ColonyIDToSentryIndexTbl[npc.m_ColonyID] = patrolIndex
			end
		end
	elseif patrolNumber == 4 then
		if warZone.m_SentryTbl[patrolCamp * 10 + 4] and
			IsServerObjValid(warZone.m_SentryTbl[patrolCamp * 10 + 4].m_Defender) then
			
			self.m_NpcTbl = g_DynamicCreateMgr:Create(warZone.m_Scene, filename)
			for index,npc in pairs(self.m_NpcTbl) do
				npc.m_WarZoneId = warZone.m_WarZoneId
				npc.m_PatrolIndex = patrolIndex
				npc.m_Index = index
				npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
				warZone.m_ColonyIDToSentryIndexTbl[npc.m_ColonyID] = patrolIndex
			end
		end
	end
end

function CWarZoneSentryPatrol:PatrolerDead(index)
	table.insert(self.m_NpcDeadTbl, index)
end

function CWarZoneSentryPatrol:Recover()
	self:Destroy()
	self:Create()
end

function CWarZoneSentryPatrol:Destroy()
	for _,npc in pairs(self.m_NpcTbl) do
		g_NpcServerMgr:DestroyServerNpcNow(npc, false)
	end
end
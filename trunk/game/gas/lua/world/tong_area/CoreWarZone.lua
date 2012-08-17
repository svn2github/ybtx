gas_require "world/tong_area/CoreWarZoneInc"
gas_require "world/tong_area/WarZoneSentry"
gas_require "world/tong_area/WarZoneCommand"
gas_require "world/tong_area/WarZoneNpcStation"
gas_require "world/tong_area/WarZoneSentryPatrol"
--gas_require "world/tong_area/WarZoneLifeTrees"

local PatrolGroupTbl = {}
PatrolGroupTbl[11] = 0
PatrolGroupTbl[12] = 0
PatrolGroupTbl[13] = 0
PatrolGroupTbl[14] = 0
PatrolGroupTbl[21] = 0
PatrolGroupTbl[22] = 0
PatrolGroupTbl[23] = 0
PatrolGroupTbl[24] = 0
PatrolGroupTbl[31] = 0
PatrolGroupTbl[32] = 0
PatrolGroupTbl[33] = 0
PatrolGroupTbl[34] = 0

function CCoreWarZone:Init(warZoneId, data, scene)
	self.m_WarZoneId = warZoneId
	self.m_Scene = scene
	scene.m_WarZone = self
	
	self.m_StationOwnerTong = data["stationTongInfo"]
	self.m_StationOwnerCamp = data["stationCampInfo"]
	self.m_StationOwnerTongName = data["stationTongNameInfo"]
	local stationBuilding = data["stationBuilding"]
	
	for stationId in pairs(g_WarZoneMgr.m_StationPos) do 
		if self.m_StationOwnerTong[stationId] then
			g_WarZoneMgr:AddTongIdIndex(self.m_StationOwnerTong[stationId], warZoneId, stationId)
		else
			self.m_StationOwnerTong[stationId] = 0
			self.m_StationOwnerTongName[stationId] = "Npc驻地"
		end
		if not self.m_StationOwnerCamp[stationId] then
			self.m_StationOwnerCamp[stationId] = WarZoneBasic:GetStationDefaultCamp(stationId)
		end
	end
	
	self:CreateAllBuilding(stationBuilding)
	
	self.m_SentryTbl = {}
	self.m_CommandTbl = {}
	self.m_NpcStationTbl = {}
	self.m_SentryPatrolTbl = {}
	self.m_ColonyIDToSentryIndexTbl = {}
	self.m_LifeTreesTbl = {}
	
	self:CreateAllSentry()
	self:CreateAllCommand()
	self:CreateAllNpcStation()
	self:CreateAllSentryPatrol()
	--self:CreateAllLifeTree()
end

function CCoreWarZone:CreateAllSentry()
	for sentryIndex in pairs(g_WarZoneMgr.m_SentryCenterPos) do
		local warZoneSentry = CWarZoneSentry:new()
		warZoneSentry:Init(self, sentryIndex)
		self.m_SentryTbl[sentryIndex] = warZoneSentry
	end
end

function CCoreWarZone:CreateAllCommand()
	for commandIndex in pairs(g_WarZoneMgr.m_CommandCenterPos) do
		local warZoneCommand = CWarZoneCommand:new()
		warZoneCommand:Init(self, commandIndex)
		self.m_CommandTbl[commandIndex] = warZoneCommand
	end
end

function CCoreWarZone:CreateAllNpcStation()
	for stationIndex in pairs(g_WarZoneMgr.m_StationPos) do
		local warZoneNpcStation = CWarZoneNpcStation:new()
		local ownerTong = self.m_StationOwnerTong[stationIndex]
		warZoneNpcStation:Init(self, stationIndex, ownerTong)
		self.m_NpcStationTbl[stationIndex] = warZoneNpcStation
	end
end

--function CCoreWarZone:CreateAllLifeTree()
--	for stationIndex in pairs(g_WarZoneMgr.m_StationPos) do
--		local warZoneLifeTrees = CWarZoneLifeTrees:new()
--		warZoneLifeTrees:Init(self, stationIndex)
--		self.m_LifeTreesTbl[stationIndex] = warZoneLifeTrees
--	end
--end

function CCoreWarZone:CreateAllSentryPatrol()
	for patrolIndex in pairs(PatrolGroupTbl) do
		local warZoneSentryPatrol = CWarZoneSentryPatrol:new()
		warZoneSentryPatrol:Init(self, patrolIndex)
		self.m_SentryPatrolTbl[patrolIndex] = warZoneSentryPatrol
	end
end

function CCoreWarZone:SentrySoldierDead(Npc)
	local sentryIndex = Npc.m_SentryIndex
	local warZoneSentry = self.m_SentryTbl[sentryIndex]
	warZoneSentry:SoldierDead(Npc)
end

function CCoreWarZone:SentryDefenderDead(sentryIndex)
	local warZoneSentry = self.m_SentryTbl[sentryIndex]
	warZoneSentry:DefenderDead()
end

function CCoreWarZone:SentryHelperDead(sentryIndex, helperIndex)
	local warZoneSentry = self.m_SentryTbl[sentryIndex]
	warZoneSentry:HelperDead(helperIndex)
end

function CCoreWarZone:SentryPatrolerDead(patrolIndex, index)
	local warZoneSentryPatrol = self.m_SentryPatrolTbl[patrolIndex]
	warZoneSentryPatrol:PatrolerDead(index)
end

function CCoreWarZone:SentryEnterBattle(sentryIndex)
	local warZoneSentry = self.m_SentryTbl[sentryIndex]
	warZoneSentry:EnterBattle()
end

function CCoreWarZone:SentryLeaveBattle(sentryIndex)
	local warZoneSentry = self.m_SentryTbl[sentryIndex]
	warZoneSentry:LeaveBattle()
end

function CCoreWarZone:CommandSoldierDead(Npc)
	local commandIndex = Npc.m_CommandIndex
	local warZoneCommand = self.m_CommandTbl[commandIndex]
	warZoneCommand:SoldierDead(Npc)
end

function CCoreWarZone:CommandDefenderDead(Npc)
	local commandIndex = Npc.m_CommandIndex
	local warZoneCommand = self.m_CommandTbl[commandIndex]
	warZoneCommand:DefenderDead(Npc)
end

function CCoreWarZone:CommandHelperDead(commandIndex, helperIndex)
	local warZoneCommand = self.m_CommandTbl[commandIndex]
	warZoneCommand:HelperDead(helperIndex)
end

function CCoreWarZone:CommandEnterBattle(commandIndex)
	local warZoneCommand = self.m_CommandTbl[commandIndex]
	warZoneCommand:EnterBattle()
end

function CCoreWarZone:CommandLeaveBattle(commandIndex)
	local warZoneCommand = self.m_CommandTbl[commandIndex]
	warZoneCommand:LeaveBattle()
end

function CCoreWarZone:NpcStationSoldierDead(Npc)
	local stationIndex = Npc.m_StationIndex
	local warZoneNpcStation = self.m_NpcStationTbl[stationIndex]
	warZoneNpcStation:SoldierDead(Npc)
end

function CCoreWarZone:NpcStationBaseCampDead(stationIndex)
	local warZoneNpcStation = self.m_NpcStationTbl[stationIndex]
	warZoneNpcStation:BaseCampDead()
end

--清理驻地所属(无数据库操作)
function CCoreWarZone:ClearStationOwner(stationId)
		local tongId = self.m_StationOwnerTong[stationId]
		if tongId ~= 0 then
			self.m_StationOwnerTong[stationId] = 0
			self.m_StationOwnerTongName[stationId] = "Npc驻地"
			self.m_StationOwnerCamp[stationId] = 0	-- 0表示没有阵营占领
			g_WarZoneMgr:RemoveTongIdIndex(tongId)
		end
end

function CCoreWarZone:SetStationOwner(stationId, tongId, camp, tongName)
	assert(self.m_StationOwnerTong[stationId] == 0)
	assert(tongId ~= 0)
	self.m_StationOwnerTong[stationId] = tongId
	self.m_StationOwnerCamp[stationId] = camp
	self.m_StationOwnerTongName[stationId] = tongName
	g_WarZoneMgr:AddTongIdIndex(tongId, self.m_WarZoneId, stationId)
end

function CCoreWarZone:CreateStationBuilding(stationId, camp, buildingTbl)
	local centerPos = g_WarZoneMgr:GetStationPos(stationId)
	for _, buildingInfo in pairs(buildingTbl) do
		local npc = g_BuildingMgr:CreateBuildingNpc(self.m_Scene, buildingInfo, camp, centerPos)
		if npc and not g_WarZoneMgr.m_Warring then
			npc:ServerDoNoRuleSkill("国战全免疫", npc)
		end
	end
end

function CCoreWarZone:CreateAllBuilding(stationBuilding)
	for stationId, buildingTbl in pairs(stationBuilding) do
		self:CreateStationBuilding(stationId, self.m_StationOwnerCamp[stationId], buildingTbl)
	end
end

function CCoreWarZone:SetBuildingInvincible()
	for stationId, tongId in pairs(self.m_StationOwnerTong) do
		if tongId ~= 0 then
			if (g_BuildingMgr.m_AllBuildingNpc[tongId]) then
				for _, npc in pairs(g_BuildingMgr.m_AllBuildingNpc[tongId]) do
					npc:ServerDoNoRuleSkill("国战全免疫",npc)
				end
			end
		else
			local npcStation = self.m_NpcStationTbl[stationId]
			local baseCamp = npcStation.m_BaseCamp
			if baseCamp and IsServerObjValid(baseCamp) then
				baseCamp:ServerDoNoRuleSkill("国战全免疫",baseCamp)
			end
			
			if npcStation.m_DefenderTbl then
				for _,defender in pairs(npcStation.m_DefenderTbl) do
					if IsServerObjValid(defender) then
						defender:ServerDoNoRuleSkill("国战全免疫", defender)
					end
				end
			end
		end
		
--		local warZoneLifeTrees = self.m_LifeTreesTbl[stationId]
--		for _,npc in pairs(warZoneLifeTrees.m_LifeTreeTbl) do
--			if IsServerObjValid(npc) then
--				npc:ServerDoNoRuleSkill("国战全免疫" ,npc)
--			end
--		end
	end
end

function CCoreWarZone:UnsetBuildingInvincible()
	for stationId,tongId in pairs(self.m_StationOwnerTong) do
		if tongId ~= 0 then
			if (g_BuildingMgr.m_AllBuildingNpc[tongId]) then
				for _, npc in pairs(g_BuildingMgr.m_AllBuildingNpc[tongId])do 
					npc:ServerDoNoRuleSkill("国战全免疫撤销", npc)
				end
			end
		else
			local npcStation = self.m_NpcStationTbl[stationId]
			local baseCamp = npcStation.m_BaseCamp
			if baseCamp and IsServerObjValid(baseCamp) then
				baseCamp:ServerDoNoRuleSkill("国战全免疫撤销", baseCamp)
			end
			
			if npcStation.m_DefenderTbl then
				for _,defender in pairs(npcStation.m_DefenderTbl) do
					if IsServerObjValid(defender) then
						defender:ServerDoNoRuleSkill("国战全免疫撤销", defender)
					end
				end
			end
		end
		
--		local warZoneLifeTrees = self.m_LifeTreesTbl[stationId]
--		for _,npc in pairs(warZoneLifeTrees.m_LifeTreeTbl) do
--			if IsServerObjValid(npc) then
--				npc:ServerDoNoRuleSkill("国战全免疫撤销", npc)
--			end
--		end
	end
end

function CCoreWarZone:WarStart()
	--解除建筑无敌
	self:UnsetBuildingInvincible()
end

function CCoreWarZone:WarEnd()
	--恢复建筑无敌,以及未被任何帮会占领的驻地 进行NPC托管
	self:SetBuildingInvincible()
end
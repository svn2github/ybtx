gas_require "world/tong_area/WarZoneLifeTreesInc"

function CWarZoneLifeTrees:Init(coreWarZone, stationIndex)
	self.m_StationIndex = stationIndex
	self.m_CoreWarZone = coreWarZone
	self.m_CenterPos = g_WarZoneMgr.m_StationPos[stationIndex]
	self.m_LifeTreesFile = {}
	self.m_LifeTreesFile[1] = "aimistone"
	self.m_LifeTreesFile[2] = "shenshengstone"
	self.m_LifeTreesFile[3] = "xiusistone"
	self.m_LifeTreeTbl = {}
	self:Create()
end

function CWarZoneLifeTrees:Create()
	local nCamp = self.m_CoreWarZone.m_StationOwnerCamp[self.m_StationIndex]
	self.m_OwnerTongID = self.m_CoreWarZone.m_StationOwnerTong[self.m_StationIndex]
	local function CallBack(nLevel)
		local npcTbl = g_DynamicCreateMgr:Create(self.m_CoreWarZone.m_Scene, self.m_LifeTreesFile[nCamp], self.m_CenterPos)
		for nIndex, npc in pairs(npcTbl) do
			npc.m_WarZoneId = self.m_CoreWarZone.m_WarZoneId
			npc.m_OwnedStationIndex = self.m_StationIndex
			npc.m_Index = nIndex
			if not g_WarZoneMgr.m_Warring then
				npc:ServerDoNoRuleSkill("¹úÕ½È«ÃâÒß", npc)
			end
			if IsServerObjValid(npc) then
				npc:CppSetCamp(nCamp)
				npc:CppSetLevel(nLevel)
			end
			self.m_LifeTreeTbl[nIndex] = npc
		end
	end
	if self.m_OwnerTongID ~= 0 then
		CallDbTrans("GasTongBasicDB", "GetTongMaxLevelRpc", CallBack,{["nTongID"] = self.m_OwnerTongID}, self.m_OwnerTongID)
	else
		CallBack(g_WarZoneMgr.m_NpcLevel)
	end
end

function CWarZoneLifeTrees:Destroy()
	for nIndex, npc in pairs(self.m_LifeTreeTbl) do
		if IsServerObjValid(npc) then
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
		end
	end
end

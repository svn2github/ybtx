gas_require "world/tong_area/TongMonsAttackStationMgrInc"
cfg_load "tong/TongMonsAttackStartTime_Server"
cfg_load "tong/TongStartTime_Common"
cfg_load "tong/TongMonsAttackNpcFile_Server"

g_TongMonsAttackStationMgr = CTongMonsAttackStationMgr:new()

function CTongMonsAttackStationMgr:GameStart(tick, nWarZoneId, nStationIndex)
	if self.m_GameReadyTickTbl[nWarZoneId] and self.m_GameReadyTickTbl[nWarZoneId][nStationIndex] then
		UnRegisterTick(self.m_GameReadyTickTbl[nWarZoneId][nStationIndex])
		self.m_GameReadyTickTbl[nWarZoneId][nStationIndex] = nil
		if not next(self.m_GameReadyTickTbl[nWarZoneId]) then
			self.m_GameReadyTickTbl[nWarZoneId] = nil
		end
	end
	
	local warZone = g_WarZoneMgr:GetCoreWarZone(nWarZoneId)
	local nOwnerTongID = warZone.m_StationOwnerTong[nStationIndex]
	if nOwnerTongID ~= 0 then
		local tblMembers = g_GasTongMgr:GetMembers(nOwnerTongID)
		for i = 1, #(tblMembers) do
			local MemberId = tblMembers[i]
			local member = g_GetPlayerInfo(MemberId)
			if member and member.m_Scene
				 and member.m_Scene.m_WarZone
				 and member.m_Scene.m_WarZone.m_WarZoneId == nWarZoneId then
				Gas2Gac:TongMonsAttackBegin(member.m_Conn, self.m_GameLength)
				Gas2Gac:OpenTongMonsAttackExitBtnWnd(member.m_Conn)
			end
		end
		if not self.m_GameOverTickTbl[nWarZoneId] then
			self.m_GameOverTickTbl[nWarZoneId] = {}
		end
		
		if not self.m_CurrentState[nWarZoneId] then
			self.m_CurrentState[nWarZoneId] = {}
		end
		self.m_CurrentState[nWarZoneId][nStationIndex] = 3
		
		self.m_GameOverTickTbl[nWarZoneId][nStationIndex] = RegClassTick("TongMonsAttackGameOverTick", self.GameOver, self.m_GameLength * 1000, self, nWarZoneId, nStationIndex)
		local centerPos = g_WarZoneMgr:GetStationPos(nStationIndex)
		self:MonsCreateTick(nil, 1, nWarZoneId, nStationIndex)
	end
end

function CTongMonsAttackStationMgr:GameReady()
	for nWarZoneId in pairs(g_WarZoneMgr.m_WarZoneTbl) do
		if not self.m_CurrentState[nWarZoneId] then
			self.m_CurrentState[nWarZoneId] = {}
		end
		local warZone = g_WarZoneMgr:GetCoreWarZone(nWarZoneId)
		for nStationIndex, nOwnerTongID in pairs(warZone.m_StationOwnerTong) do
			if nOwnerTongID ~= 0 then
				self.m_CurrentState[nWarZoneId][nStationIndex] = 2
				if not self.m_GameReadyTickTbl[nWarZoneId] then
					self.m_GameReadyTickTbl[nWarZoneId] = {}
				end
				local nDelaySecond = TongMonsAttackStartTime_Server(nStationIndex, "DelaySecond")
				self.m_GameReadyTickTbl[nWarZoneId][nStationIndex] = RegClassTick("TongMonsAttackGameStartTick", self.GameStart, (self.m_GameReadyLength + nDelaySecond) * 1000, self, nWarZoneId, nStationIndex)
				local function CallBack(bFlag, tblMember)
					if bFlag then
						for nIndex = 1, #tblMember do
							local Player = g_GetPlayerInfo(tblMember[nIndex])
							if Player and IsServerObjValid(Player) and Player.m_Conn and IsCppBound(Player.m_Conn) then
--								Gas2Gac:SysMovementNotifyToClient(Player.m_Conn, GetStaticTextServer(9120))
								if Player.m_Scene and Player.m_Scene.m_WarZone
									 and Player.m_Scene.m_WarZone.m_WarZoneId == nWarZoneId then
									Gas2Gac:InitTongDownTimeWnd(Player.m_Conn, self.m_GameReadyLength + nDelaySecond, 2)
								end
							end
						end
					end
				end
				local parameter = {}
				parameter["uTongID"] = nOwnerTongID
				CallDbTrans("ChannelDB", "GetPartyOnlinePlayer", CallBack, parameter, nOwnerTongID)
			end
		end
	end
end

function CTongMonsAttackStationMgr:Init()
	self.m_MonsAttackFileTbl = {}
	local tblIndex = TongMonsAttackNpcFile_Server:GetKeys()
	for _,index in pairs(tblIndex) do
		self.m_MonsAttackFileTbl[index] = TongMonsAttackNpcFile_Server(index, "DynamicFileName")
	end
	
	self.m_AdvanceStoneTbl = {}
	self.m_AdvanceStoneTbl[1] = "粗糙"
	self.m_AdvanceStoneTbl[2] = "原质"
	self.m_AdvanceStoneTbl[3] = "淬炼"
	self.m_AdvanceStoneTbl[4] = "纯净"
	self.m_AdvanceStoneTbl[5] = "卓越"
	self.m_AdvanceStoneTbl[6] = "完美"
	
	self.m_KillMonsCountTbl = {}
--	self.m_MonsDeathCountTbl = {}
	self.m_MonsLevelTbl = {}
	
	self.m_GameReadyTickTbl = {}
	self.m_MonsCreateTickTbl = {}
	self.m_GameOverTickTbl = {}
	
	self.m_GameStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团怪物攻城开始时间", "OpenTime")
	self.m_GameReadyLength = TongStartTime_Common("佣兵团怪物攻城进场时长", "OpenTime") * 60
	self.m_GameLength = TongStartTime_Common("佣兵团怪物攻城持续时长", "OpenTime") * 60
	self.m_NpcCreateInterval = TongStartTime_Common("佣兵团怪物攻城刷怪间隔", "OpenTime") * 60
	self.m_MonsCreateTimes = math.floor(self.m_GameLength / self.m_NpcCreateInterval)
	self.m_AllMonsTbl = {}
	--m_CurrentState：nil 关闭状态、2 进场、3 比赛中
	self.m_CurrentState = {}
	
	g_AlarmClock:AddTask("MonsterAttackTongStart", self.m_GameStartTime, self.GameReady, nil, self)
end

g_TongMonsAttackStationMgr:Init()

function CTongMonsAttackStationMgr:MonsCreateTick(tick, nNo, nWarZoneId, nStationIndex)
	if self.m_MonsCreateTickTbl[nNo]
		 and self.m_MonsCreateTickTbl[nNo][nWarZoneId]
		 and self.m_MonsCreateTickTbl[nNo][nWarZoneId][nStationIndex] then
		UnRegisterTick(self.m_MonsCreateTickTbl[nNo][nWarZoneId][nStationIndex])
		self.m_MonsCreateTickTbl[nNo][nWarZoneId][nStationIndex] = nil
		if not next(self.m_MonsCreateTickTbl[nNo][nWarZoneId]) then
			self.m_MonsCreateTickTbl[nNo][nWarZoneId] = nil
		end
		if not next(self.m_MonsCreateTickTbl[nNo]) then
			self.m_MonsCreateTickTbl[nNo] = nil
		end
	end
	
	local warZone = g_WarZoneMgr:GetCoreWarZone(nWarZoneId)
	local nOwnerTongID = warZone.m_StationOwnerTong[nStationIndex]
	if nOwnerTongID ~= 0 then
		if nNo < self.m_MonsCreateTimes then
			if not self.m_MonsCreateTickTbl[nNo + 1] then
				self.m_MonsCreateTickTbl[nNo + 1] = {}
			end
			if not self.m_MonsCreateTickTbl[nNo + 1][nWarZoneId] then
				self.m_MonsCreateTickTbl[nNo + 1][nWarZoneId] = {}
			end
			if not self.m_MonsCreateTickTbl[nNo + 1][nWarZoneId][nStationIndex] then
				self.m_MonsCreateTickTbl[nNo + 1][nWarZoneId][nStationIndex] = RegClassTick("TongMonsCreateTick", self.MonsCreateTick, self.m_NpcCreateInterval * 1000, self, nNo + 1, nWarZoneId, nStationIndex)
			end
		end
		
		local centerPos = g_WarZoneMgr:GetStationPos(nStationIndex)
		local npcTbl = g_DynamicCreateMgr:Create(warZone.m_Scene, self.m_MonsAttackFileTbl[nNo], centerPos)
		local function CallBack(bFlag, nLevel)
			if not bFlag then
				return
			end
			if nNo == 1 then
				if not self.m_MonsLevelTbl[nWarZoneId] then
					self.m_MonsLevelTbl[nWarZoneId] = {}
				end
				self.m_MonsLevelTbl[nWarZoneId][nStationIndex] = nLevel
			end
			for nIndex, npc in pairs(npcTbl) do
				npc.m_AttackWarZoneId = nWarZoneId
				npc.m_WhichNo = nNo
				npc.m_MonsStationIndex = nStationIndex
				npc.m_Index = nIndex
				
				if bFlag then
					npc:CppSetLevel(nLevel)
				end
				if not self.m_AllMonsTbl[nWarZoneId] then
					self.m_AllMonsTbl[nWarZoneId] = {}
				end
				
				if not self.m_AllMonsTbl[nWarZoneId][nStationIndex] then
					self.m_AllMonsTbl[nWarZoneId][nStationIndex] = {}
				end
				
				if not self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo] then
					self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo] = {}
				end
				
				self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo][nIndex] = npc
			end
		end
		
		local parameter = {}
		parameter["nTongID"] = nOwnerTongID
		CallDbTrans("GasTongBasicDB", "GetTongMemberLevelInfo", CallBack, parameter, nOwnerTongID)
	end
end

function CTongMonsAttackStationMgr:GameOver(tick, nWarZoneId, nStationIndex, bSuccessful)
	if self.m_CurrentState[nWarZoneId] and self.m_CurrentState[nWarZoneId][nStationIndex] then
		self.m_CurrentState[nWarZoneId][nStationIndex] = nil
		if not next(self.m_CurrentState[nWarZoneId]) then
			self.m_CurrentState[nWarZoneId] = nil
		end
	end
	for nWhichNo in pairs(self.m_MonsCreateTickTbl) do
		for nWarZoneId in pairs(self.m_MonsCreateTickTbl[nWhichNo]) do
			for nStationIndex in pairs(self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId]) do
				UnRegisterTick(self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId][nStationIndex])
				self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId][nStationIndex] = nil
			end
			self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId] = nil
		end
		self.m_MonsCreateTickTbl[nWhichNo] = nil
	end

	if self.m_GameOverTickTbl[nWarZoneId] and self.m_GameOverTickTbl[nWarZoneId][nStationIndex] then
		UnRegisterTick(self.m_GameOverTickTbl[nWarZoneId][nStationIndex])
		self.m_GameOverTickTbl[nWarZoneId][nStationIndex] = nil
		if not next(self.m_GameOverTickTbl[nWarZoneId]) then
			self.m_GameOverTickTbl[nWarZoneId] = nil
		end
	end
	
--	self:DestroyAttackNpc(nWarZoneId, nStationIndex)
	
	local warZone = g_WarZoneMgr:GetCoreWarZone(nWarZoneId)
	local nOwnerTongID = warZone.m_StationOwnerTong[nStationIndex]
	if nOwnerTongID ~= 0 then
		local function CallBack(bFlag, tblMember)
			if bFlag then
				for nIndex = 1, #tblMember do
					local Player = g_GetPlayerInfo(tblMember[nIndex])
					if Player and IsServerObjValid(Player) and Player.m_Conn and IsCppBound(Player.m_Conn) then
						if Player.m_Scene.m_SceneId == warZone.m_Scene.m_SceneId then
							Gas2Gac:CloseTongWarPanel(Player.m_Conn)
							Gas2Gac:OpenTongMonsAttackCountWnd(Player.m_Conn)
						end
					end
				end
			end
		end
		local parameter = {}
		parameter["uTongID"] = nOwnerTongID
		CallDbTrans("ChannelDB", "GetPartyOnlinePlayer", CallBack, parameter, nOwnerTongID)
		
		if self.m_MonsLevelTbl[nWarZoneId] == nil then
			LogErr("帮会战怪物攻城奖励错误", "WarZoneId:" .. nWarZoneId)
		elseif self.m_MonsLevelTbl[nWarZoneId][nStationIndex] == nil then
			LogErr("帮会战怪物攻城奖励错误", "WarZoneId:" .. nWarZoneId .. ", StationIndex:" .. nStationIndex)
		end
		
		local experience = (50 + self.m_MonsLevelTbl[nWarZoneId][nStationIndex] * 3) / 10 * 60 * 20 * 1.5
		local data = {}
		data["nTongID"] = nOwnerTongID
		data["nItemBigID"] = g_ItemInfoMgr:GetSoulPearlBigID()
		data["sItemName"] = "经验球:"
		data["nExperience"] = experience
		data["nDepotIndex"] = 5
		data["sSceneName"] = warZone.m_Scene.m_SceneName
		
--		local nStoneInterZone = self:GetStoneInterZone(self.m_MonsLevelTbl[nWarZoneId][nStationIndex])
--		local sStoneType = self.m_AdvanceStoneTbl[nStoneInterZone]
--		data["sStoneName1"] = sStoneType .. "的火灵石"
--		data["sStoneName2"] = sStoneType .. "的风歌石"
--		data["sStoneName3"] = sStoneType .. "的雷鸣石"
--		data["sStoneName4"] = sStoneType .. "的黑魔石"
--		data["nStoneBigID"] = g_ItemInfoMgr:GetAdvanceStoneBigID()
		data["sItem1Name"] = "花语荣誉奖章:10000"
		data["nItem1BigID"] = g_ItemInfoMgr:GetSoulPearlBigID()
		CallDbTrans("RoleQuestDB", "MonsAttackTongAward", callback, data, nOwnerTongID)
	end
end

function CTongMonsAttackStationMgr:GetStoneInterZone(nLevel)
	if nLevel >= 76 and nLevel <= 80 then
		return 6
	elseif nLevel >=70 and nLevel <= 75 then
		return 5
	elseif nLevel >=60 and nLevel <= 69 then
		return 4
	elseif nLevel >=50 and nLevel <= 59 then
		return 3
	elseif nLevel >=40 and nLevel <= 49 then
		return 2
	elseif nLevel <= 40 then
		return 1
	end
end

function CTongMonsAttackStationMgr:AddKillMonsCount(nOwnerId, nWarZoneId, nStationIndex)
	local Entity = CEntityServerManager_GetEntityByID(nOwnerId)
	if Entity and IsServerObjValid(Entity) then
		local nPlayerID = Entity.m_uID
		local Scene = Entity.m_Scene
		if Scene.m_SceneAttr.SceneType ~= 7 then
			return
		end
		
		local uTongID
		if Entity.m_Properties:GetType() == ECharacterType.Player then
			uTongID = Entity.m_Properties:GetTongID()
		elseif Entity.m_Properties:GetType() == ECharacterType.Npc then
			return
		end
		
		local warZone = Scene.m_WarZone
		local nOwnerTongID = warZone.m_StationOwnerTong[nStationIndex]
		if nOwnerTongID ~= uTongID then
			return
		end
		
		if not self.m_KillMonsCountTbl[nWarZoneId] then
			self.m_KillMonsCountTbl[nWarZoneId] = {}
		end
		if not self.m_KillMonsCountTbl[nWarZoneId][nOwnerTongID] then
			self.m_KillMonsCountTbl[nWarZoneId][nOwnerTongID] = {}
		end
		if not self.m_KillMonsCountTbl[nWarZoneId][nOwnerTongID][nPlayerID] then
			self.m_KillMonsCountTbl[nWarZoneId][nOwnerTongID][nPlayerID] = 0
		end
		self.m_KillMonsCountTbl[nWarZoneId][nOwnerTongID][nPlayerID] = self.m_KillMonsCountTbl[nWarZoneId][nOwnerTongID][nPlayerID] + 1
	end
end

function CTongMonsAttackStationMgr:DestroyAttackNpc(nWarZoneId, nStationIndex)
	local allMonsNpcTbl = self.m_AllMonsTbl[nWarZoneId] and self.m_AllMonsTbl[nWarZoneId][nStationIndex]
	if allMonsNpcTbl then
		for nWhichNo, npcTbl in pairs(allMonsNpcTbl) do
			for _, npc in pairs(npcTbl) do
				if IsServerObjValid(npc) then
					g_NpcServerMgr:DestroyServerNpcNow(npc, false)
				end
			end
		end
	end
end

function CTongMonsAttackStationMgr:RemoveAttackNpc(Npc)
	if Npc.m_AttackWarZoneId then
		if Npc.m_MonsStationIndex then
			local nWarZoneId = Npc.m_AttackWarZoneId
			local nStationIndex = Npc.m_MonsStationIndex
			local nWhichNo = Npc.m_WhichNo
			local nIndex = Npc.m_Index
			
			if self.m_AllMonsTbl[nWarZoneId]
				 and self.m_AllMonsTbl[nWarZoneId][nStationIndex]
				 and self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo]
				 and self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo][nIndex] then
				if IsServerObjValid(self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo][nIndex]) then
					self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo][nIndex] = nil
					if not next(self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo]) then
						self.m_AllMonsTbl[nWarZoneId][nStationIndex][nNo] = nil
					end
					if not next(self.m_AllMonsTbl[nWarZoneId][nStationIndex]) then
						self.m_AllMonsTbl[nWarZoneId][nStationIndex] = nil
					end
					if not next(self.m_AllMonsTbl[nWarZoneId]) then
						self.m_AllMonsTbl[nWarZoneId] = nil
					end
				end
			end
		end
	end
end

function CTongMonsAttackStationMgr:ClearTick()
	for nWarZoneId in pairs(self.m_GameReadyTickTbl) do
		for nStationIndex in pairs(self.m_GameReadyTickTbl[nWarZoneId]) do
			UnRegisterTick(self.m_GameReadyTickTbl[nWarZoneId][nStationIndex])
			self.m_GameReadyTickTbl[nWarZoneId][nStationIndex] = nil
		end
		self.m_GameReadyTickTbl[nWarZoneId] = nil
	end
	
	for nWhichNo in pairs(self.m_MonsCreateTickTbl) do
		for nWarZoneId in pairs(self.m_MonsCreateTickTbl[nWhichNo]) do
			for nStationIndex in pairs(self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId]) do
				UnRegisterTick(self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId][nStationIndex])
				self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId][nStationIndex] = nil
			end
			self.m_MonsCreateTickTbl[nWhichNo][nWarZoneId] = nil
		end
		self.m_MonsCreateTickTbl[nWhichNo] = nil
	end
	
	for nWarZoneId in pairs(self.m_GameOverTickTbl) do
		for nStationIndex in pairs(self.m_GameOverTickTbl[nWarZoneId]) do
			UnRegisterTick(self.m_GameOverTickTbl[nWarZoneId][nStationIndex])
			self.m_GameOverTickTbl[nWarZoneId][nStationIndex] = nil
		end
		self.m_GameOverTickTbl[nWarZoneId] = nil
	end
end

function Gac2Gas:GetTongMonsAttackListInfo(Conn)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local nPlayerID = Player.m_uID
	if Player and IsServerObjValid(Player) then
		local nTongID = Player.m_Properties:GetTongID()
		if nTongID == 0 then
			return
		end
		local Scene = Player.m_Scene
		local warZone = Scene.m_WarZone
		if not warZone then
			return
		end
		local nWarZoneId = warZone.m_WarZoneId
		
		local function CallBack(tblRes)
			Gas2Gac:ReturnTongMonsAttackListInfoBegin(Conn)
			for nPlayerID in pairs(tblRes) do
				local sMemberName = tblRes[nPlayerID].sName
				local nLevel = tblRes[nPlayerID].nLevel
				local nCount = 0
				if g_TongMonsAttackStationMgr.m_KillMonsCountTbl[nWarZoneId]
					 and g_TongMonsAttackStationMgr.m_KillMonsCountTbl[nWarZoneId][nTongID]
					 and g_TongMonsAttackStationMgr.m_KillMonsCountTbl[nWarZoneId][nTongID][nPlayerID] then
					nCount = g_TongMonsAttackStationMgr.m_KillMonsCountTbl[nWarZoneId][nTongID][nPlayerID]
				end
				Gas2Gac:ReturnTongMonsAttackListInfo(Conn, sMemberName, nLevel, nCount)
			end
			Gas2Gac:ReturnTongMonsAttackListInfoEnd(Conn)
		end
		
		local parameter = {}
		parameter["uTongID"] = nTongID
		CallAccountAutoTrans(Conn.m_Account, "GasTongBasicDB", "GetTongKillMonsCountInfo", CallBack, parameter, nTongID)
	end
end

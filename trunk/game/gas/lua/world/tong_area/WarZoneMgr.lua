gas_require "world/tong_area/DynamicCreateMgr"
gas_require "world/tong_area/WarZoneMgrInc"
gas_require "world/tong_area/CoreWarZone"
gas_require "world/tong_area/TongBattleCountMgr"
gac_gas_require "framework/common/CAlarmClock"
gac_gas_require "war/WarZoneBasic"
gas_require "world/trigger_action/obj/CollectRes"
cfg_load "tong/TongStartTime_Common"
cfg_load "tong/TongMonsAttackStartTime_Server"

--CWarZoneMgr主要管理些所有 花语平原战区共有的属性和功能函数(如:各种坐标信息)一个服务器只有一个实例 g_WarZoneMgr
--对于单独的一个花语平原战区相关功能, 在 CCoreWarZone 中实现, 一个Scene上会挂一个CoreWarZone, 也会保存在g_WarZoneMgr 里
WarZoneBasic = CWarZoneBasic:new()
g_WarZoneMgr = CWarZoneMgr:new()
g_WarZoneMgr.m_StationManager = "驻地占领光圈"

--哨所的Npc
g_WarZoneMgr.m_SentrySoldierTbl = {}
g_WarZoneMgr.m_SentrySoldierTbl[1] = "艾米哨所卫兵"
g_WarZoneMgr.m_SentrySoldierTbl[2] = "神圣哨所卫兵"
g_WarZoneMgr.m_SentrySoldierTbl[3] = "修斯哨所卫兵"

g_WarZoneMgr.m_SentryHelperTbl = {}
g_WarZoneMgr.m_SentryHelperTbl[1] = "艾米哨所助战"
g_WarZoneMgr.m_SentryHelperTbl[2] = "神圣哨所助战"
g_WarZoneMgr.m_SentryHelperTbl[3] = "修斯哨所助战"

g_WarZoneMgr.m_SentryDefenderTbl = {}
g_WarZoneMgr.m_SentryDefenderTbl[1] = "艾米哨所防御塔"
g_WarZoneMgr.m_SentryDefenderTbl[2] = "神圣哨所防御塔"
g_WarZoneMgr.m_SentryDefenderTbl[3] = "修斯哨所防御塔"


--战区司令部的Npc
g_WarZoneMgr.m_CommandSoldierTbl = {}
g_WarZoneMgr.m_CommandSoldierTbl[1] = "艾米司令部卫兵"
g_WarZoneMgr.m_CommandSoldierTbl[2] = "神圣司令部卫兵"
g_WarZoneMgr.m_CommandSoldierTbl[3] = "修斯司令部卫兵"

g_WarZoneMgr.m_CommandHelperTbl = {}
g_WarZoneMgr.m_CommandHelperTbl[1] = "艾米司令部助战"
g_WarZoneMgr.m_CommandHelperTbl[2] = "神圣司令部助战"
g_WarZoneMgr.m_CommandHelperTbl[3] = "修斯司令部助战"

g_WarZoneMgr.m_CommandDefenderTbl = {}
g_WarZoneMgr.m_CommandDefenderTbl[1] = "艾米司令部防御塔"
g_WarZoneMgr.m_CommandDefenderTbl[2] = "神圣司令部防御塔"
g_WarZoneMgr.m_CommandDefenderTbl[3] = "修斯司令部防御塔"

g_WarZoneMgr.m_CommandBossTbl = {}
g_WarZoneMgr.m_CommandBossTbl[1] = "艾米司令部BOSS"
g_WarZoneMgr.m_CommandBossTbl[2] = "神圣司令部BOSS"
g_WarZoneMgr.m_CommandBossTbl[3] = "修斯司令部BOSS"


--Npc驻地的Npc
g_WarZoneMgr.m_StationDefenderTbl = {}
g_WarZoneMgr.m_StationDefenderTbl[1] = "艾米NPC驻地防御塔"
g_WarZoneMgr.m_StationDefenderTbl[2] = "神圣NPC驻地防御塔"
g_WarZoneMgr.m_StationDefenderTbl[3] = "修斯NPC驻地防御塔"

g_WarZoneMgr.m_StationSoldierTbl = {}
g_WarZoneMgr.m_StationSoldierTbl[1] = "艾米NPC驻地守卫"
g_WarZoneMgr.m_StationSoldierTbl[2] = "神圣NPC驻地守卫"
g_WarZoneMgr.m_StationSoldierTbl[3] = "修斯NPC驻地守卫"

g_WarZoneMgr.m_StationBaseCampTbl = {}
g_WarZoneMgr.m_StationBaseCampTbl[1] = "艾米大本营"
g_WarZoneMgr.m_StationBaseCampTbl[2] = "神圣大本营"
g_WarZoneMgr.m_StationBaseCampTbl[3] = "修斯大本营"

g_WarZoneMgr.m_WarZoneDefenderTbl = {}
g_WarZoneMgr.m_WarZoneDefenderTbl[1] = "艾米小型防御设施"
g_WarZoneMgr.m_WarZoneDefenderTbl[2] = "神圣小型防御设施"
g_WarZoneMgr.m_WarZoneDefenderTbl[3] = "修斯小型防御设施"

--哨所和司令部Trap
g_WarZoneMgr.m_SentryTrapTbl = {}
g_WarZoneMgr.m_SentryTrapTbl[1] = "艾米战区哨所"
g_WarZoneMgr.m_SentryTrapTbl[2] = "神圣战区哨所"
g_WarZoneMgr.m_SentryTrapTbl[3] = "修斯战区哨所"

g_WarZoneMgr.m_CommandTrapTbl = {}
g_WarZoneMgr.m_CommandTrapTbl[1] = "艾米战区司令部"
g_WarZoneMgr.m_CommandTrapTbl[2] = "神圣战区司令部"
g_WarZoneMgr.m_CommandTrapTbl[3] = "修斯战区司令部"

g_WarZoneMgr.m_ChallengeBaseCamp = {}
g_WarZoneMgr.m_ChallengeBaseCamp[1] = "艾米挑战副本大本营"
g_WarZoneMgr.m_ChallengeBaseCamp[2] = "神圣挑战副本大本营"
g_WarZoneMgr.m_ChallengeBaseCamp[3] = "修斯挑战副本大本营"

g_WarZoneMgr.m_ChallengeDefender = {}
g_WarZoneMgr.m_ChallengeDefender[1] = "艾米挑战副本防御设施"
g_WarZoneMgr.m_ChallengeDefender[2] = "神圣挑战副本防御设施"
g_WarZoneMgr.m_ChallengeDefender[3] = "修斯挑战副本防御设施"

g_WarZoneMgr.m_NpcLevel = 0
g_WarZoneMgr.m_WarStartTime 	= GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团国战开始时间", "OpenTime") --国战开始时间
g_WarZoneMgr.m_WarTimeLength 	= TongStartTime_Common("国战时长", "OpenTime") * 60 * 1000 --国战时长
g_WarZoneMgr.m_Warring 			= false		-- 国战是否进行中

g_WarZoneMgr.m_StationDefenderRebornTime = GetCfgTransformValue(false, "TongStartTime_Common", "战场防御塔重生时间", "OpenTime") --Npc驻地防御设施重生时间
g_WarZoneMgr.m_TransportStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团资源运输开始时间", "OpenTime") --资源运输开始时间
g_WarZoneMgr.m_ChallengeCompeteStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团挑战竞标开始时间", "OpenTime")
g_WarZoneMgr.m_ResourceOccupyStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "军资占领开始时间", "OpenTime")

g_WarZoneMgr.m_TransportTimeLength = TongStartTime_Common("资源运输时长", "OpenTime") * 60 * 1000 --资源运输时长
g_WarZoneMgr.m_ChallengeCompeteTimeLength = TongStartTime_Common("挑战竞标时长", "OpenTime") * 60 * 1000 --挑战竞标时长
g_WarZoneMgr.m_ResourceOccupyTimeLength = TongStartTime_Common("军资占领时长", "OpenTime") * 60 * 1000 --军资石抢占活动时长

g_WarZoneMgr.m_OccupyTime = 5 * 1000
g_WarZoneMgr.m_WarZoneTbl = {}
g_WarZoneMgr.m_WarZoneDbData = {}--数据库存在的战场编号表
g_WarZoneMgr.m_WarZoneStationDbData = {}

g_WarZoneMgr.m_SentryCenterPos = {} --哨所坐标
g_WarZoneMgr.m_CommandCenterPos = {}
g_WarZoneMgr.m_StationRebornPos = {}
g_WarZoneMgr.m_StationPos = {} --驻地坐标 = m_StationPos[驻地索引]  驻地索引规律,百位为原始阵营,十位为战线,个位为编号

g_WarZoneMgr.m_WzsDefenderDeadPos = {} --记录npc驻地防御塔死亡坐标

g_WarZoneMgr.m_TongIndexTbl = {} --帮会在战场位置的Id索引, 方便快速找到帮会位置

g_WarZoneMgr.m_CurrentTransportState = "TransportEnd"
g_WarZoneMgr.m_CurrentChallengeCompeteState = "Close"

g_WarZoneMgr.m_StationMoveType = 
{
	["Occupy"] = 1,
	["Exchange"] = 2,
	["ActiveRetreat"] = 3,
	["PassiveRetreat"] = 4,
}

function CWarZoneMgr:AddWarZoneScene(warZoneId, scene)
	assert(self.m_WarZoneTbl[warZoneId] == nil, "重复添加同一战区的花语平原场景")
	assert(self.m_WarZoneDbData[warZoneId] and self.m_WarZoneDbData[warZoneId] == scene.m_SceneId)
	assert(self.m_WarZoneStationDbData[warZoneId])
	scene.m_CoreWarZone = CCoreWarZone:new()
	
	self.m_WzsDefenderDeadPos[warZoneId] = {}
	self.m_WarZoneTbl[warZoneId] = scene.m_CoreWarZone
	scene.m_CoreWarZone:Init(warZoneId, g_WarZoneMgr.m_WarZoneStationDbData[warZoneId], scene)
end

function CWarZoneMgr:SentrySoldierDead(Npc)
	local warZoneId = Npc.m_WarZoneId
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:SentrySoldierDead(Npc)
end

function CWarZoneMgr:SentryDefenderDead(Npc)
	local warZoneId, sentryIndex = Npc.m_WarZoneId, Npc.m_SentryIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:SentryDefenderDead(sentryIndex)
end

function CWarZoneMgr:SentryHelperDead(Npc)
	Npc.m_bDead = true
	local warZoneId, sentryIndex, helperIndex = Npc.m_WarZoneId, Npc.m_SentryIndex, Npc.m_HelperIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:SentryHelperDead(sentryIndex, helperIndex)
end

function CWarZoneMgr:SentryPatrolerDead(Npc)
	local warZoneId, patrolIndex, index = Npc.m_WarZoneId, Npc.m_PatrolIndex, Npc.m_Index
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:SentryPatrolerDead(patrolIndex)
end

function CWarZoneMgr:SentryEnterBattle(Npc)
	local warZoneId, sentryIndex = Npc.m_WarZoneId, Npc.m_SentryIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:SentryEnterBattle(sentryIndex)
end

function CWarZoneMgr:SentryLeaveBattle(Npc)
	local warZoneId, sentryIndex = Npc.m_WarZoneId, Npc.m_SentryIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:SentryLeaveBattle(sentryIndex)
end

function CWarZoneMgr:CommandSoldierDead(Npc)
	local warZoneId = Npc.m_WarZoneId
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:CommandSoldierDead(Npc)
end

function CWarZoneMgr:CommandDefenderDead(Npc)
	local warZoneId = Npc.m_WarZoneId
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:CommandDefenderDead(Npc)
end

function CWarZoneMgr:CommandHelperDead(Npc)
	Npc.m_bDead = true
	local warZoneId, commandIndex, helperIndex = Npc.m_WarZoneId, Npc.m_CommandIndex, Npc.m_HelperIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:CommandHelperDead(commandIndex, helperIndex)
end

function CWarZoneMgr:CommandEnterBattle(Npc)
	local warZoneId, commandIndex = Npc.m_WarZoneId, Npc.m_CommandIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:CommandEnterBattle(commandIndex)
end

function CWarZoneMgr:CommandLeaveBattle(Npc)
	local warZoneId, commandIndex = Npc.m_WarZoneId, Npc.m_CommandIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:CommandLeaveBattle(commandIndex)
end

function CWarZoneMgr:NpcStationSoldierDead(Npc)
	local warZoneId = Npc.m_WarZoneId
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:NpcStationSoldierDead(Npc)
end

function CWarZoneMgr:NpcStationDefenderDead(Npc)
	local warZoneId, stationIndex, bornPos, defenderIndex = Npc.m_WarZoneId, Npc.m_StationIndex, Npc.m_RebornPos, Npc.m_DefenderIndex
	if not self.m_WzsDefenderDeadPos[warZoneId][stationIndex] then
		self.m_WzsDefenderDeadPos[warZoneId][stationIndex] = {}
	end
	self.m_WzsDefenderDeadPos[warZoneId][stationIndex]["Pos"] = bornPos
	self.m_WzsDefenderDeadPos[warZoneId][stationIndex]["DefendIndex"] = defenderIndex
end

function CWarZoneMgr:NpcStationBaseCampDead(Npc)
	local warZoneId, stationIndex = Npc.m_WarZoneId, Npc.m_StationIndex
	local warZone = self:GetCoreWarZone(warZoneId)
	warZone:NpcStationBaseCampDead(stationIndex)
end
--添加以帮会id为索引的帮会位置信息
function CWarZoneMgr:AddTongIdIndex(tongId, warZoneId, stationId)
	assert(self.m_TongIndexTbl[tongId] == nil)
	self.m_TongIndexTbl[tongId] = {}
	self.m_TongIndexTbl[tongId].m_WarZoneId = warZoneId
	self.m_TongIndexTbl[tongId].m_StationId = stationId
end

function CWarZoneMgr:RemoveTongIdIndex(tongId)
	self.m_TongIndexTbl[tongId] = nil
end

function CWarZoneMgr:FindTong(tongId)
	return self.m_TongIndexTbl[tongId]
end

function CWarZoneMgr:GetWarZoneScene(warZoneId)
	local warZone = self.m_WarZoneTbl[warZoneId]
	if warZone then
		return warZone.m_Scene
	elseif self.m_WarZoneDbData[warZoneId] then
		return g_SceneMgr:GetScene(self.m_WarZoneDbData[warZoneId])
	end
end

function CWarZoneMgr:GetCoreWarZone(warZoneId)
	if not self.m_WarZoneTbl[warZoneId] then
		g_SceneMgr:GetScene(self.m_WarZoneDbData[warZoneId])
	end
	return self.m_WarZoneTbl[warZoneId]
end

--获取进入坐标
function CWarZoneMgr:GetEnterPos(camp)
	return self.m_CommandCenterPos[camp]
end

function CWarZoneMgr:GetStationPos(stationId)
	return self.m_StationPos[stationId]
end

function CWarZoneMgr:GetStationEnterPos(stationId, camp)
	assert(self.m_StationRebornPos[stationId] and self.m_StationRebornPos[stationId][camp], "TongStationRebornPos_Common没有StationIndex为" .. stationId .. "Camp为" .. camp .. "的坐标")
	return self.m_StationRebornPos[stationId][camp]
end

--用来断言 驻地迁移的数据库操作前后, 内存驻地位置没有变化
--稳定后可以屏蔽对这个函数的调用
local function assertPosition(oldPosition, curPosition)
	if oldPosition and  oldPosition.m_WarZoneId and oldPosition.m_StationId then
		if curPosition == nil 
			or curPosition.m_WarZoneId ~= oldPosition.m_WarZoneId 
			or curPosition.m_StationId ~= oldPosition.m_StationId then
			assert(false, "占领的操作中有其他的搬迁操作!!!")
		end
	else
		assert(curPosition == nil, "占领的操作中有其他的搬迁操作!!!")
	end
end

function CWarZoneMgr:ExchangeStation(playerId, tongId, warZoneId, stationId)
	local warZone = self:GetCoreWarZone(warZoneId)
	local targetTongId = warZone.m_StationOwnerTong[stationId]
	
	local parameters = {}
	parameters["PlayerId"] = playerId
	parameters["WarZoneId"] = warZoneId
	parameters["StationId"] = stationId
	parameters["TongId"] = tongId
	parameters["TargetTongId"] = targetTongId
	parameters["StationMoveType"] = self.m_StationMoveType.Exchange
	parameters["ActiveOrPassive"] = "Active"
	CallDbTrans("WarZoneDB", "StationMoveGetTongStationInfo", nil, parameters, tongId, targetTongId, stationId)
end

--第一战线的驻地全部被敌国占领时，哨所A变为不可用
--第二战线的驻地全部被敌国占领时，哨所B变为不可用
function CWarZoneMgr:SentryDeal(warZoneId, stationId)
	local defaultCamp = WarZoneBasic:GetStationDefaultCamp(stationId)
	local Line = WarZoneBasic:GetStationWarLine(stationId)
	local warZoneSentry, sentryIndex, nowCamp
	local warZone = self:GetCoreWarZone(warZoneId)
	if Line == 1 then
		nowCamp = warZone.m_StationOwnerCamp[stationId]
		if defaultCamp ~= nowCamp then
			sentryIndex = defaultCamp * 10 + 1
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "Normal" then
				warZoneSentry:Occupied()
			end
			
			sentryIndex = defaultCamp * 10 + 2
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "Normal" then
				warZoneSentry:Occupied()
			end
		else
			sentryIndex = defaultCamp * 10 + 1
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "BeOccupied" then
				warZoneSentry:Recover()
			end
			
			sentryIndex = defaultCamp * 10 + 2
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "BeOccupied" then
				warZoneSentry:Recover()
			end
		end
	elseif Line == 2 then
		local wzs1 = tonumber(defaultCamp .. "21")
		local wzs2 = tonumber(defaultCamp .. "22")
		local wzs3 = tonumber(defaultCamp .. "23")
		
		local sign = true
		if defaultCamp == warZone.m_StationOwnerCamp[wzs1] or
			 defaultCamp == warZone.m_StationOwnerCamp[wzs2] or
			 defaultCamp == warZone.m_StationOwnerCamp[wzs3] then
			sign = false
		end
		
		if sign then
			sentryIndex = defaultCamp * 10 + 3
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "Normal" then
				warZoneSentry:Occupied()
			end
			
			sentryIndex = defaultCamp * 10 + 4
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "Normal" then
				warZoneSentry:Occupied()
			end
		else
			sentryIndex = defaultCamp * 10 + 3
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "BeOccupied" then
				warZoneSentry:Recover()
			end
			
			sentryIndex = defaultCamp * 10 + 3
			warZoneSentry = warZone.m_SentryTbl[sentryIndex]
			if warZoneSentry.m_CurrentState == "BeOccupied" then
				warZoneSentry:Recover()
			end
		end
	end
end

function CWarZoneMgr:OccupyStation(Conn, npc)
	local warZoneId = npc.m_WarZoneId
	local warZone = self:GetCoreWarZone(warZoneId)
	local stationId = npc.m_StationIndex
	local player = Conn.m_Player
	if not (IsCppBound(Conn) and IsServerObjValid(player))then
		return
	end
	local tongId = player.m_Properties:GetTongID()
	if tongId == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	local parameters = {}
	parameters["PlayerId"] = player.m_uID
	parameters["WarZoneId"] = warZoneId
	parameters["StationId"] = stationId
	parameters["TongId"] = tongId
	parameters["StationMoveType"] = self.m_StationMoveType.Occupy
	
	local function ProgressSucceed()
		if not (IsServerObjValid(player) and IsCppBound(Conn)) then
			return
		end
		
		if not IsServerObjValid(npc) then
			MsgToConn(Conn, 9409)
			return 
		end
		
		CallAccountManualTrans(Conn.m_Account, "WarZoneDB", "StationMoveGetTongStationInfo", nil, parameters, tongId)	
	end
	
	local function ProgressFail()
	end
	
	local function CheckCallBack(isCan , msgID, tblRet)
		if not (IsServerObjValid(player) and IsCppBound(Conn)) then
			return
		end
		if not isCan then
			if IsNumber(msgID) then
				if tblRet then
					MsgToConn(Conn, msgID, tblRet)
				else
					MsgToConn(Conn, msgID)
				end
			end
			return
		end
		
		if not IsServerObjValid(npc) then
			 MsgToConn(Conn, 9409) return 
		end 
		TongLoadProgress(player, g_WarZoneMgr.m_OccupyTime, ProgressSucceed, ProgressFail,{}) 
	end
	
	CallAccountManualTrans(Conn.m_Account, "WarZoneDB", "IsCanOccupyStation",CheckCallBack, parameters, tongId)
end

--function CWarZoneMgr:UpDateTongStation(oldData, tongId, warZoneId, stationId, buildingTbl, camp)
--	local warZone = self:GetCoreWarZone(warZoneId)
--	assert(warZone.m_StationOwnerTong[stationId] == 0)
--	if oldData.SceneType == 7 then
--		local tongPosition = self:FindTong(tongId)
--		assertPosition(oldData, tongPosition)
--		warZone:ClearStationOwner(oldData.m_StationId)
--		
--		local npcTbl = g_BuildingMgr.m_AllBuildingNpc[tongId]
--		if npcTbl then
--			for _, buildingInfo in pairs(buildingTbl) do
--				local npc = npcTbl[buildingInfo[1]]
--				if IsServerObjValid(npc) then
--					local curHp = npc:CppGetHP()
--					if curHp > 1 then
--						buildingInfo[5] = curHp / npc:CppGetMaxHP()
--					else
--						buildingInfo[5] = 1 / npc:CppGetMaxHP()
--					end
--				end
--			end
--		end
--		g_BuildingMgr:DestroyAllTongBuildingNpc(tongId)
--
--		local npcStation = warZone.m_NpcStationTbl[oldData.m_StationId]
--		npcStation:CreateStationManager()
--	else--if oldData.SceneType == 6 then
--		if oldData.ServerId ~= 0 then
--			Gas2GasAutoCall:CloseTongScene(GetServerAutoConn(oldData.ServerId), tongId, oldData.SceneId)
--		end
--	end
--	warZone:SetStationOwner(stationId, tongId, camp)
--	warZone:CreateStationBuilding(stationId, camp, buildingTbl)
--end

function CWarZoneMgr:RetreatStation(playerId, tongId)
	local tongPosition = self:FindTong(tongId)
	if not tongPosition then
		return
	end
	local data = {}
	data["PlayerId"] = playerId
	data["TongId"] = tongId
	data["CurPosition"] = tongPosition
	data["StationMoveType"] = self.m_StationMoveType.PassiveRetreat
	CallDbTrans("WarZoneDB", "StationMoveGetTongStationInfo", nil, data, tongId)
end

----------------------------- 国战相关 -----------------------------
-- 国战开启
function TongWarStart(length)
	if GasConfig.GasType == 1 then
		return
	end
	if g_WarZoneMgr.m_WarEndTick then
		return false
	end
	if not length then
		length = g_WarZoneMgr.m_WarTimeLength
	end
	for warZoneId, warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
		local scene = warZone.m_Scene
		for _, player in pairs(scene.m_tbl_Player) do
			Gas2Gac:TongWarBattleBegin(player.m_Conn, length / 1000)
			if IsServerObjValid(player) then
				Gas2Gac:OpenTongWarExitBtnWnd(player.m_Conn)
			end
		end
	end
--	Gas2Gac:SysMovementNotifyToClient(g_AllPlayerSender, GetStaticTextServer(9109))
	for _, warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
		warZone:WarStart()
	end
	g_WarZoneMgr.m_Warring = true
	g_WarZoneMgr.m_WarEndTick = RegisterTick("TongWarEndTick", TongWarEnd, length)
	g_TongBattleCountMgr.m_AmGetExploitCountNum = 0
	g_TongBattleCountMgr.m_SsGetExploitCountNum = 0
	g_TongBattleCountMgr.m_XsGetExploitCountNum = 0
	if g_CurServerId == 1 then
		local function DealCampExploit()
			local sceneId = 0
			for _,warZone in pairs (g_WarZoneMgr.m_WarZoneTbl) do
				local SceneId = warZone.m_Scene.m_SceneId
				sceneId = SceneId
			end
			if sceneId ~= 0 then
				CWarZoneMgr.DealCampExploit(sceneId)
			end
		end
		DealCampExploitTick = RegisterTick("DealCampExploitTick",DealCampExploit,5*1000)
	end
	-- 给在线玩家弹个窗口选择传送到国战场景
	local function CallBack(result)
		if result then
			local num = result:GetRowNum()
			for i = 0, num - 1  do
				Gas2GacById:CreateWarTransprot(result:GetData(i,0))
			end
		end
	end
	CallDbTrans("MultiServerDB", "GetOnlinePlayerInfo", CallBack, {})
	
	return true
end

-- 国战结束
function TongWarEnd(tick)
	if g_WarZoneMgr.m_WarEndTick then
--		Gas2Gac:SysMovementNotifyToClient(g_AllPlayerSender, GetStaticTextServer(9110))
		for _, warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
			warZone:WarEnd()
		end
		UnRegisterTick(g_WarZoneMgr.m_WarEndTick)
		g_WarZoneMgr.m_WarEndTick = nil
		g_WarZoneMgr.m_Warring = false
		if DealCampExploitTick then
			UnRegisterTick(DealCampExploitTick)
			DealCampExploitTick = nil
		end
		g_TongBattleCountMgr:ClearCountAndEffect()
		g_TongBattleCountMgr.m_AmGetExploitCountNum = nil
		g_TongBattleCountMgr.m_SsGetExploitCountNum = nil
		g_TongBattleCountMgr.m_XsGetExploitCountNum = nil
		for warZoneId in pairs(g_WarZoneMgr.m_WarZoneTbl) do
			local warZone = g_WarZoneMgr:GetCoreWarZone(warZoneId)
			local scene = warZone.m_Scene
			for _,player in pairs(scene.m_tbl_Player) do
				if player and player.m_Conn then
					Gas2Gac:CloseTongWarPanel(player.m_Conn)
				end
			end
		end
		local function CallBack(GetExploitTbl, GetAwardTbl)
			if GetExploitTbl == nil or GetAwardTbl == nil then
				for warZoneId in pairs(g_WarZoneMgr.m_WarZoneTbl) do		--国战没有任何人获得声望和奖励	
					local warZone = g_WarZoneMgr:GetCoreWarZone(warZoneId)
					local scene = warZone.m_Scene
					for _,player in pairs(scene.m_tbl_Player) do
						Gas2Gac:ShowNoPlayerGetAwardWnd(player.m_Conn)
					end
				end
				return false
			end
			for warZoneId in pairs(g_WarZoneMgr.m_WarZoneTbl) do					
				local warZone = g_WarZoneMgr:GetCoreWarZone(warZoneId)
				local scene = warZone.m_Scene
				for _,player in pairs(scene.m_tbl_Player) do
					if player and player.m_Conn then
						local nTongID = player.m_Properties:GetTongID()
						local nCharID = player.m_uID
						if nTongID == 0 and GetExploitTbl[nCharID] == nil then									--给国战中没有获得声望的人弹面板
							Gas2Gac:ShowNoTongNoneAwardWnd(player.m_Conn)
						elseif nTongID ~= 0 and GetExploitTbl[nCharID] == nil then
							local tong_members = {}
							local count = 0
							for playerId ,v in pairs(GetExploitTbl) do
								if nTongID == v[4] then
									count = count + 1
									table.insert(tong_members,playerId)
								end
							end
							if count == 0 then
								Gas2Gac:ShowNoneAwardWnd(player.m_Conn)
							else  --对国战结束时在战场的没有获得声望但队友有获得声望的玩家弹面板
								Gas2Gac:ReturnTongWarAwardListInfoBegin(player.m_Conn)								
								for i=1,#tong_members do
									local memberId = tong_members[i]
									if GetExploitTbl[memberId] ~= nil and memberId ~= nCharID then
										local sPlayerName = GetExploitTbl[memberId][1]
										local nLevel = GetExploitTbl[memberId][2]
										local nAddProffer = GetAwardTbl[memberId][1]
										local nCampAward = GetAwardTbl[memberId][2]
										Gas2Gac:ReturnTongWarAwardListInfo(player.m_Conn, sPlayerName, nLevel, GetExploitTbl[memberId][3], nAddProffer, nCampAward)								
									end
								end
								Gas2Gac:ReturnTongWarAwardListInfoEnd(player.m_Conn)
							end	
						end
					end
				end
			end
			
			if g_CurServerId == 1 then 
				for playerId, nGetExploit in pairs(GetExploitTbl) do  --对在国战中获得了声望的在线玩家弹面板，不论他在国战结束时在不在战场中
					Gas2GacById:ReturnTongWarAwardListInfoBegin(playerId)				
					local tongId = GetExploitTbl[playerId][4]
					local tong_members = {}
					for charId,v in pairs(GetExploitTbl) do
						if tongId == v[4] and charId ~= playerId then
							table.insert(tong_members,charId)
						end
					end
					local sPlayerName = GetExploitTbl[playerId][1]
					local nLevel = GetExploitTbl[playerId][2]
					local nAddProffer = GetAwardTbl[playerId][1]
					local nCampAward = GetAwardTbl[playerId][2]
					Gas2GacById:ReturnTongWarAwardListInfo(playerId, sPlayerName, nLevel, GetExploitTbl[playerId][3], nAddProffer, nCampAward)
					if tongId ~= 0 then
						for i=1,#tong_members do
							local memberId = tong_members[i]
							if GetExploitTbl[memberId] ~= nil and memberId ~= playerId then
								local sMemPlayerName = GetExploitTbl[memberId][1]
								local nMemLevel = GetExploitTbl[memberId][2]
								local nMemAddProffer = GetAwardTbl[memberId][1]
								local nMemCampAward = GetAwardTbl[memberId][2]
								Gas2GacById:ReturnTongWarAwardListInfo(playerId, sMemPlayerName, nMemLevel, GetExploitTbl[memberId][3], nMemAddProffer, nMemCampAward)								
							end
						end
					end
					Gas2GacById:ReturnTongWarAwardListInfoEnd(playerId)
				end
				CWarZoneMgr.SendAward(GetExploitTbl,GetAwardTbl)
				local function ClearData()
					CWarZoneMgr.ClearDataInDataBase()
				end
				DealDataTick = RegisterTick("DealAwardAndData",ClearData,60*60*1000)
			end
		end
		CallDbTrans("GasTongBasicDB", "GetAllMemAndCeByTongIDRpc",CallBack,{},678)
		return true
	else
		return false
	end
end

function CWarZoneMgr.SendAward(GetExploitTbl,GetAwardTbl)
	local data = {}
	data.ExploitTbl = GetExploitTbl
	data.AwardTbl = GetAwardTbl
	CallDbTrans("GasTongBasicDB","SendWarAwardDB",nil,data)
end

function CWarZoneMgr.ClearDataInDataBase(GetExploitTbl,GetAwardTbl)
	if DealDataTick then
		UnRegisterTick(DealDataTick)
		DealDataTick = nil
	end
	CallDbTrans("GasTongBasicDB","ClearDataInDataBaseDB",nil,{})
end

local function ChallengeCompeteEndTick(length)
	if g_WarZoneMgr.m_ChallengeCompeteEndTick then
		g_WarZoneMgr.m_CurrentChallengeCompeteState = "Close"
		UnRegisterTick(g_WarZoneMgr.m_ChallengeCompeteEndTick)
		g_WarZoneMgr.m_ChallengeCompeteEndTick = nil
	end
end

local function ResourceOccupyEndTick(length)
	if g_WarZoneMgr.m_ResourceOccupyEndTick then
		g_WarZoneMgr.m_ResourceOccupyState = "Close"
		UnRegisterTick(g_WarZoneMgr.m_ResourceOccupyEndTick)
		g_WarZoneMgr.m_ResourceOccupyEndTick = nil
	end
end

function ChallengeCompeteStart(length)
	if g_WarZoneMgr.m_ChallengeCompeteEndTick then
		return
	end
	
	if not length then
		length = g_WarZoneMgr.m_ChallengeCompeteTimeLength
	end
	
	g_WarZoneMgr.m_CurrentChallengeCompeteState = "Open"
	g_WarZoneMgr.m_ChallengeCompeteEndTick = RegisterTick("ChallengeCompeteEndTick", ChallengeCompeteEndTick, length)
end

function ResourceTransportStart(length)
	if g_WarZoneMgr.m_TransportEndTick then
		return
	end
--	Gas2Gac:SysMovementNotifyToClient(g_AllPlayerSender, GetStaticTextServer(9111))
	if not length then
		length = g_WarZoneMgr.m_TransportTimeLength
	end
	g_WarZoneMgr.m_CurrentTransportState = "Transporting"
	g_WarZoneMgr.m_TransportEndTick = RegisterTick("ResourceTransportEndTick", ResourceTransportEnd, length)
end

function ResourceOccupyStart(length)
	if g_WarZoneMgr.m_ResourceOccupyEndTick then
		return
	end
	
	if not length then
		length = g_WarZoneMgr.m_ResourceOccupyTimeLength
	end
	
	g_WarZoneMgr.m_ResourceOccupyState = "Open"
	g_WarZoneMgr.m_ResourceOccupyEndTick = RegisterTick("ResourceOccupyEndTick", ResourceOccupyEndTick, length)
end

function ResourceTransportEnd()
	if g_WarZoneMgr.m_TransportEndTick then
--		Gas2Gac:SysMovementNotifyToClient(g_AllPlayerSender, GetStaticTextServer(9112))
		UnRegisterTick(g_WarZoneMgr.m_TransportEndTick)
		g_WarZoneMgr.m_TransportEndTick = nil
		g_WarZoneMgr.m_CurrentTransportState = "TransportEnd"
	end
end

--服务器关闭时候调用, 清理些因服务器在国战期间关闭而遗留的一些tick
function CWarZoneMgr:ClearTick()
	if g_WarZoneMgr.m_WarEndTick then
		UnRegisterTick(g_WarZoneMgr.m_WarEndTick)
		g_WarZoneMgr.m_WarEndTick = nil
	end
	
	if g_WarZoneMgr.m_TransportEndTick then
		UnRegisterTick(g_WarZoneMgr.m_TransportEndTick)
		g_WarZoneMgr.m_TransportEndTick = nil
	end
	
	if g_WarZoneMgr.m_ChallengeCompeteEndTick then
		UnRegisterTick(g_WarZoneMgr.m_ChallengeCompeteEndTick)
		g_WarZoneMgr.m_ChallengeCompeteEndTick = nil
	end
	
	if g_WarZoneMgr.m_ResourceOccupyEndTick then
		UnRegisterTick(g_WarZoneMgr.m_ResourceOccupyEndTick)
		g_WarZoneMgr.m_ResourceOccupyEndTick = nil
	end
	
	for _,warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
		for _,warZoneSentry in pairs(warZone.m_SentryTbl) do
			UnRegisterTick(warZoneSentry.m_HelperBornTick)
			warZoneSentry.m_HelperBornTick = nil
			
			UnRegisterTick(warZoneSentry.m_DefenderRebornTick)
			warZoneSentry.m_DefenderRebornTick = nil
			
			for soldierIndex in pairs(warZoneSentry.m_SoldierRebornTickTbl) do
				UnRegisterTick(warZoneSentry.m_SoldierRebornTickTbl[soldierIndex])
				warZoneSentry.m_SoldierRebornTickTbl[soldierIndex] = nil
			end
		end
		
		for _,warZoneCommand in pairs(warZone.m_CommandTbl) do
			UnRegisterTick(warZoneCommand.m_HelperBornTick)
			warZoneCommand.m_HelperBornTick = nil
			
			for defenderIndex in pairs(warZoneCommand.m_DefenderRebornTickTbl) do
				UnRegisterTick(warZoneCommand.m_DefenderRebornTickTbl[defenderIndex])
				warZoneCommand.m_DefenderRebornTickTbl[defenderIndex] = nil
			end
			
			for soldierIndex in pairs(warZoneCommand.m_SoldierRebornTickTbl) do
				UnRegisterTick(warZoneCommand.m_SoldierRebornTickTbl[soldierIndex])
				warZoneCommand.m_SoldierRebornTickTbl[soldierIndex] = nil
			end
		end
		
		for _,npcStation in pairs(warZone.m_NpcStationTbl) do
			UnRegisterTick(npcStation.m_StationManagerOverTick)
			npcStation.m_StationManagerOverTick = nil
		
			for soldierIndex in pairs(npcStation.m_SoldierRebornTickTbl) do
				UnRegisterTick(npcStation.m_SoldierRebornTickTbl[soldierIndex])
				npcStation.m_SoldierRebornTickTbl[soldierIndex] = nil
			end
		end
	end
end

function CWarZoneMgr:RemoveWarZoneHelper()
	for warZoneId,warZone in pairs(self.m_WarZoneTbl) do
		for sentryIndex,warZoneSentry in pairs(warZone.m_SentryTbl) do
			for _,index in pairs(warZoneSentry.m_HelperDeadTbl) do
				warZoneSentry.m_HelperTbl[index] = nil
			end
		end
		
		for commandIndex,warZoneCommand in pairs(warZone.m_CommandTbl) do
			for _,index in pairs(warZoneCommand.m_HelperDeadTbl) do
				warZoneCommand.m_HelperTbl[index] = nil
			end
		end
	end
end

function CWarZoneMgr:RemoveWarZoneNpc(npc)
	if npc.m_WarZoneId then
		local warZone = self:GetCoreWarZone(npc.m_WarZoneId)
		if npc.m_SentryIndex then
			local warZoneSentry = warZone.m_SentryTbl[npc.m_SentryIndex]
			if npc.m_SoldierIndex then
				warZoneSentry.m_SoldierTbl[npc.m_SoldierIndex] = nil
			elseif npc.m_HelperIndex then
				if not npc.m_bDead then
					if IsServerObjValid(warZoneSentry.m_HelperTbl[npc.m_HelperIndex]) then
						warZoneSentry.m_HelperTbl[npc.m_HelperIndex] = nil
					end
				end
			else
				warZoneSentry.m_Defender = nil
			end
		elseif npc.m_CommandIndex then
			local warZoneCommand = warZone.m_CommandTbl[npc.m_CommandIndex]
			if npc.m_SoldierIndex then
				warZoneCommand.m_SoldierTbl[npc.m_SoldierIndex] = nil
			elseif npc.m_DefenderIndex then
				warZoneCommand.m_DefenderTbl[npc.m_DefenderIndex] = nil
			elseif npc.m_HelperIndex then
				if not npc.m_bDead then
					if IsServerObjValid(warZoneCommand.m_HelperTbl[npc.m_HelperIndex]) then
						warZoneCommand.m_HelperTbl[npc.m_HelperIndex] = nil
					end
				end
			else
				warZoneCommand.m_HelpBoss = nil
			end
		elseif npc.m_StationIndex then
			local npcStation = warZone.m_NpcStationTbl[npc.m_StationIndex]
			if npc.m_SoldierIndex then
				npcStation.m_SoldierTbl[npc.m_SoldierIndex] = nil
			elseif npc.m_DefenderIndex then
				npcStation.m_DefenderTbl[npc.m_DefenderIndex] = nil
			else
				local npcName = npc.m_Properties:GetCharName()
				if npcName == g_WarZoneMgr.m_StationBaseCampTbl[1] or npcName == g_WarZoneMgr.m_StationBaseCampTbl[2] or npcName == g_WarZoneMgr.m_StationBaseCampTbl[3] then
					npcStation.m_BaseCamp = nil
				elseif npcName == g_WarZoneMgr.m_StationManager then
					npcStation.m_StationManager = nil
				end
			end
		elseif npc.m_PatrolIndex then
			local warZoneSentryPatrol = warZone.m_SentryPatrolTbl[npc.m_PatrolIndex]
			if npc.m_Index then
				if IsServerObjValid(warZoneSentryPatrol.m_NpcTbl[npc.m_Index]) then
					warZoneSentryPatrol.m_NpcTbl[npc.m_Index] = nil
				end
			end
--		elseif npc.m_OwnedStationIndex then
--			local warZoneLifeTrees = warZone.m_LifeTreesTbl[npc.m_OwnedStationIndex]
--			if npc.m_Index then
--				if IsServerObjValid(warZoneLifeTrees.m_LifeTreeTbl[npc.m_Index]) then
--					warZoneLifeTrees.m_LifeTreeTbl[npc.m_Index] = nil
--				end
--			end
		end
	end
end

function StationDefenderReborn()
	local function CallBack(level)
		g_WarZoneMgr.m_NpcLevel = level
		for warZoneId,warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
			for stationIndex, npcStation in pairs(warZone.m_NpcStationTbl) do
				if npcStation.m_BaseCamp and IsServerObjValid(npcStation.m_BaseCamp) then
					local livedDefenderNum = 0
					for _,defender in pairs(npcStation.m_DefenderTbl) do
						if IsServerObjValid(defender) then
							livedDefenderNum = livedDefenderNum + 1
						end
					end
					
					if livedDefenderNum < 6 then
						local bornPos = g_WarZoneMgr.m_WzsDefenderDeadPos[warZoneId][stationIndex]["Pos"]
						local defenderIndex = g_WarZoneMgr.m_WzsDefenderDeadPos[warZoneId][stationIndex]["DefendIndex"]
						local pos = {bornPos.x, bornPos.y}
						local fPos = CFPos:new()
						fPos.x = pos[1] * EUnits.eGridSpan
						fPos.y = pos[2] * EUnits.eGridSpan
						
						local camp = warZone.m_StationOwnerCamp[stationIndex]
						
						local npc, logMsg = g_NpcServerMgr:CreateServerNpc(g_WarZoneMgr.m_StationDefenderTbl[camp], g_WarZoneMgr.m_NpcLevel, warZone.m_Scene, fPos)
						if not IsServerObjValid(npc) then
							LogErr("帮会战创建Npc失败", logMsg)
							return
						end
						npc.m_WarZoneId = warZoneId
						npc.m_StationIndex = stationIndex
						npc:CppSetCamp(camp)
						npc.m_RebornPos = bornPos
						npc.m_DefenderIndex = defenderIndex
						
						if not g_WarZoneMgr.m_Warring then
							npc:ServerDoNoRuleSkill("国战全免疫", npc)
						end
						npcStation.m_DefenderTbl[defenderIndex] = npc
					end
				end
			end
		end
	end
	
	CallDbTrans("WarZoneDB", "GetPlayerLevel", CallBack, {})
end

local function GetOffsetSecond(strTime)
	local index1 = string.find(strTime, ":")
	local index2 = string.find(strTime, ":", index1 + 1) or 0
	local hour = tonumber(string.sub(strTime, 1, index1 - 1))
	local min = tonumber(string.sub(strTime, index1 + 1, index2 -1))
	local sec = 0
	if index2 ~= 0 then
		sec = tonumber(string.sub(strTime, index2 + 1, -1))
	end
	assert(hour and min and sec, "时间格式错误")
	return (hour * 60 + min) * 60  + sec
end

local function IsInOpenTime(data, length)
	local curDate =  os.date("*t")
	local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
	local offset = GetOffsetSecond(data.time[1])
	curDate.wday = ((curDate.wday == 1) and 7 or curDate.wday-1)  --将系统星期换成填表星期
	local wdayTbl = data.wday
	for _, wday in pairs(wdayTbl) do
		local endOffset = offset + length / 1000
		local day = (curDate.wday >= wday and curDate.wday - wday) or curDate.wday + 7 - wday --跨越天数
		local curOffsetSecond = todayOffsetSecond + day * 86400
		if curOffsetSecond >= offset and curOffsetSecond < endOffset then
				return true, endOffset - curOffsetSecond
		end
	end
	return false
end

cfg_load "tong/TongCommandPos_Common"
cfg_load "tong/TongSentryPos_Common"
cfg_load "tong/TongStationPos_Common"
cfg_load "tong/TongStationRebornPos_Common"

--服务器启动时候调用
function CWarZoneMgr:Init(result)
	self.m_WarZoneDbData = result.WarZoneTbl
	self.m_WarZoneStationDbData = result.StationTongAndBuildInfoTbl
	self.m_NpcLevel = result.WorldLevel
	
	for _, sentryIndex in pairs(TongSentryPos_Common:GetKeys()) do
		local tbl = TongSentryPos_Common(sentryIndex)
		local posTbl = loadstring(" return " .. tbl("Pos"))()
		self.m_SentryCenterPos[sentryIndex] = posTbl
	end
	
	for _, commandIndex in pairs(TongCommandPos_Common:GetKeys()) do
		local tbl = TongCommandPos_Common(commandIndex)
		local posTbl = loadstring(" return " .. tbl("Pos"))()
		self.m_CommandCenterPos[commandIndex] = posTbl
	end
	
	for _, i in pairs(TongStationRebornPos_Common:GetKeys()) do
		local tbl = TongStationRebornPos_Common(i)
		local stationIndex = tbl("StationIndex")
		local posTbl = loadstring(" return " .. tbl("Pos"))()
		local camp = tbl("Camp")
		if not self.m_StationRebornPos[stationIndex] then
			self.m_StationRebornPos[stationIndex] = {}
		end
		self.m_StationRebornPos[stationIndex][camp] = posTbl
	end
	
	for _, stationIndex in pairs(TongStationPos_Common:GetKeys()) do
		local tbl = TongStationPos_Common(stationIndex)
		local posTbl = loadstring(" return " .. tbl("Pos"))()
		self.m_StationPos[stationIndex] = posTbl
	end
	
	local isOpenTime, length = IsInOpenTime(g_WarZoneMgr.m_WarStartTime, g_WarZoneMgr.m_WarTimeLength)
	if isOpenTime then
		TongWarStart(length * 1000)
	end
	
	isOpenTime, length = IsInOpenTime(g_WarZoneMgr.m_TransportStartTime, g_WarZoneMgr.m_TransportTimeLength)
	if isOpenTime then
		ResourceTransportStart(length * 1000)
	end
	
	isOpenTime, length = IsInOpenTime(g_WarZoneMgr.m_ChallengeCompeteStartTime, g_WarZoneMgr.m_ChallengeCompeteTimeLength)
	if isOpenTime then
		ChallengeCompeteStart(length * 1000)
	end
	
	isOpenTime, length = IsInOpenTime(g_WarZoneMgr.m_ResourceOccupyStartTime, g_WarZoneMgr.m_ResourceOccupyTimeLength)
	if isOpenTime then
		ResourceOccupyStart(length * 1000)
	end
	
	--添加国战开始 定时任务
	self.m_WarTaskIndex = g_AlarmClock:AddTask("WarStart", g_WarZoneMgr.m_WarStartTime, TongWarStart, nil)
	g_AlarmClock:AddTask("StationDefenderReborn", g_WarZoneMgr.m_StationDefenderRebornTime, StationDefenderReborn)
	g_AlarmClock:AddTask("TransportStart", g_WarZoneMgr.m_TransportStartTime, ResourceTransportStart, nil)
	g_AlarmClock:AddTask("TongChallengeCompete", g_WarZoneMgr.m_ChallengeCompeteStartTime, ChallengeCompeteStart, nil)
	g_AlarmClock:AddTask("ResourceOccupyStart", g_WarZoneMgr.m_ResourceOccupyStartTime, ResourceOccupyStart)
end

function CWarZoneMgr:OnNpcDead(Npc)
	local NpcName = Npc.m_Properties:GetCharName()
	if NpcName == g_WarZoneMgr.m_SentrySoldierTbl[1] or	--哨所卫兵死亡
		 NpcName == g_WarZoneMgr.m_SentrySoldierTbl[2] or
		 NpcName == g_WarZoneMgr.m_SentrySoldierTbl[3] then
		g_WarZoneMgr:SentrySoldierDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_SentryDefenderTbl[1] or	--哨所防御设施死亡
		 NpcName == g_WarZoneMgr.m_SentryDefenderTbl[2] or
		 NpcName == g_WarZoneMgr.m_SentryDefenderTbl[3] then
		g_WarZoneMgr:SentryDefenderDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_SentryHelperTbl[1] or	--哨所助战死亡
		 NpcName == g_WarZoneMgr.m_SentryHelperTbl[2] or
		 NpcName == g_WarZoneMgr.m_SentryHelperTbl[3] then
		g_WarZoneMgr:SentryHelperDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_CommandSoldierTbl[1] or	--司令部卫兵死亡
		 NpcName == g_WarZoneMgr.m_CommandSoldierTbl[2] or
		 NpcName == g_WarZoneMgr.m_CommandSoldierTbl[3] then
		g_WarZoneMgr:CommandSoldierDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_CommandDefenderTbl[1] or --司令部防御设施死亡
		 NpcName == g_WarZoneMgr.m_CommandDefenderTbl[2] or
		 NpcName == g_WarZoneMgr.m_CommandDefenderTbl[3] then
		g_WarZoneMgr:CommandDefenderDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_CommandHelperTbl[1] or	--司令部助战死亡
		 NpcName == g_WarZoneMgr.m_CommandHelperTbl[2] or
		 NpcName == g_WarZoneMgr.m_CommandHelperTbl[3] then
		g_WarZoneMgr:CommandHelperDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_StationSoldierTbl[1] or	--Npc驻地守卫死亡
		 NpcName == g_WarZoneMgr.m_StationSoldierTbl[2] or
		 NpcName == g_WarZoneMgr.m_StationSoldierTbl[3] then
		if Npc.m_Scene.m_SceneName == "帮会挑战本" then
			return
		end
		g_WarZoneMgr:NpcStationSoldierDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_StationDefenderTbl[1] or	--Npc驻地的防御设施死亡
		 NpcName == g_WarZoneMgr.m_StationDefenderTbl[2] or
		 NpcName == g_WarZoneMgr.m_StationDefenderTbl[3] then
		g_WarZoneMgr:NpcStationDefenderDead(Npc)
	elseif NpcName == g_WarZoneMgr.m_StationBaseCampTbl[1] or --Npc驻地大本营被打爆
		 NpcName == g_WarZoneMgr.m_StationBaseCampTbl[2] or
		 NpcName == g_WarZoneMgr.m_StationBaseCampTbl[3] then
		g_WarZoneMgr:NpcStationBaseCampDead(Npc)
	elseif NpcName ~= g_WarZoneMgr.m_CommandBossTbl[1] and --巡逻npc死亡
		 NpcName ~= g_WarZoneMgr.m_CommandBossTbl[2] and
		 NpcName ~= g_WarZoneMgr.m_CommandBossTbl[3] and
		 Npc.m_OwnedStationIndex == nil then
		g_WarZoneMgr:SentryPatrolerDead(Npc)
	end
end

function CWarZoneMgr:OnEnterBattle(Npc)
	local NpcName = Npc.m_Properties:GetCharName()
	if NpcName == g_WarZoneMgr.m_SentrySoldierTbl[1] or NpcName == g_WarZoneMgr.m_SentrySoldierTbl[2] or NpcName == g_WarZoneMgr.m_SentrySoldierTbl[3] or
		 NpcName == g_WarZoneMgr.m_SentryDefenderTbl[1] or NpcName == g_WarZoneMgr.m_SentryDefenderTbl[2] or NpcName == g_WarZoneMgr.m_SentryDefenderTbl[3] or
		 NpcName == g_WarZoneMgr.m_SentryHelperTbl[1] or NpcName == g_WarZoneMgr.m_SentryHelperTbl[2] or NpcName == g_WarZoneMgr.m_SentryHelperTbl[3] then
		--哨所受袭
		g_WarZoneMgr:SentryEnterBattle(Npc)	
	elseif NpcName == g_WarZoneMgr.m_CommandSoldierTbl[1] or NpcName == g_WarZoneMgr.m_CommandSoldierTbl[2] or NpcName == g_WarZoneMgr.m_CommandSoldierTbl[3] or
		 NpcName == g_WarZoneMgr.m_CommandDefenderTbl[1] or NpcName == g_WarZoneMgr.m_CommandDefenderTbl[2] or NpcName == g_WarZoneMgr.m_CommandDefenderTbl[3] or
		 NpcName == g_WarZoneMgr.m_CommandHelperTbl[1] or NpcName == g_WarZoneMgr.m_CommandHelperTbl[2] or NpcName == g_WarZoneMgr.m_CommandHelperTbl[3] or
		 NpcName == g_WarZoneMgr.m_CommandBossTbl[1] or NpcName == g_WarZoneMgr.m_CommandBossTbl[2] or NpcName == g_WarZoneMgr.m_CommandBossTbl[3] then
		--司令部受袭
		g_WarZoneMgr:CommandEnterBattle(Npc)
	end
end

function CWarZoneMgr:OnLeaveBattle(Npc)
	local NpcName = Npc.m_Properties:GetCharName()
	if NpcName == g_WarZoneMgr.m_SentrySoldierTbl[1] or NpcName == g_WarZoneMgr.m_SentrySoldierTbl[2] or NpcName == g_WarZoneMgr.m_SentrySoldierTbl[3] or
		 NpcName == g_WarZoneMgr.m_SentryDefenderTbl[1] or NpcName == g_WarZoneMgr.m_SentryDefenderTbl[2] or NpcName == g_WarZoneMgr.m_SentryDefenderTbl[3] or
		 NpcName == g_WarZoneMgr.m_SentryHelperTbl[1] or NpcName == g_WarZoneMgr.m_SentryHelperTbl[2] or NpcName == g_WarZoneMgr.m_SentryHelperTbl[3] then
		--哨所的Npc脱离战斗
		g_WarZoneMgr:SentryLeaveBattle(Npc)
	elseif NpcName == g_WarZoneMgr.m_CommandSoldierTbl[1] or NpcName == g_WarZoneMgr.m_CommandSoldierTbl[2] or NpcName == g_WarZoneMgr.m_CommandSoldierTbl[3] or
		 NpcName == g_WarZoneMgr.m_CommandDefenderTbl[1] or NpcName == g_WarZoneMgr.m_CommandDefenderTbl[2] or NpcName == g_WarZoneMgr.m_CommandDefenderTbl[3] or
		 NpcName == g_WarZoneMgr.m_CommandHelperTbl[1] or NpcName == g_WarZoneMgr.m_CommandHelperTbl[2] or NpcName == g_WarZoneMgr.m_CommandHelperTbl[3] or
		 NpcName == g_WarZoneMgr.m_CommandBossTbl[1] or NpcName == g_WarZoneMgr.m_CommandBossTbl[2] or NpcName == g_WarZoneMgr.m_CommandBossTbl[3] then
		--司令部的Npc脱离战斗
		g_WarZoneMgr:CommandLeaveBattle(Npc)
	end
end

function Gac2Gas:RequestRetreatStation(Conn)
	if g_WarZoneMgr.m_CurrentChallengeCompeteState == "Open" or g_ChallengeCurrentState ~= 4 then --g_ChallengeCurrentState为4表示关闭
		MsgToConn(Conn, 9424)
		return
	end
	
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	
	local parameters = {}
	parameters["uPlayerId"] = uPlayerID
	parameters["tongId"] = uTongID
	local function CallBack(Flag, arg)
		if not Flag then
			if IsNumber(arg) then
				MsgToConn(Conn, arg)
			end
			return
		else
			local serverId = arg
			Gas2GasAutoCall:PlayerRetreatStation(GetServerAutoConn(serverId), uTongID, uPlayerID)
		end
	end
	CallAccountManualTrans(Conn.m_Account, "WarZoneDB", "IsCanRetreatStation", CallBack, parameters, uPlayerID, uTongID)
end

function Db2Gas:TongStationMove(data)
	local stationMoveType = data.StationMoveType
	if stationMoveType == g_WarZoneMgr.m_StationMoveType.Occupy then
		local parameters = {}
		parameters["PlayerId"] = data.PlayerId
		parameters["WarZoneId"] = data.WarZoneId
		parameters["StationId"] = data.StationId
		parameters["TongId"] = data.TongId
		parameters["BuildingHpTbl"] = g_BuildingMgr:GetTongBuildingHpTbl(data.TongId)
		CallDbTrans("WarZoneDB", "OccupyStationRpc", nil, parameters, data.TongId, data.StationId)
	elseif stationMoveType == g_WarZoneMgr.m_StationMoveType.Exchange then
		if data.ActiveOrPassive == "Active" then
			if data.TargetTongId ~= 0 then
				local parameters = {}
				parameters["PlayerId"] = data.PlayerId
				parameters["WarZoneId"] = data.WarZoneId
				parameters["StationId"] = data.StationId
				parameters["TongId"] = data.TongId
				parameters["TargetTongId"] = data.TargetTongId
				parameters["StationMoveType"] = g_WarZoneMgr.m_StationMoveType.Exchange
				parameters["ActiveOrPassive"] = "Passive"
				parameters["ActiveBuildingHpTbl"] = g_BuildingMgr:GetTongBuildingHpTbl(data.TongId)
				CallDbTrans("WarZoneDB", "StationMoveGetTongStationInfo", nil, parameters, data.TongId, data.TargetTongId, data.StationId)
			else
				local parameters = {}
				parameters["PlayerId"] = data.PlayerId
				parameters["WarZoneId"] = data.WarZoneId
				parameters["StationId"] = data.StationId
				parameters["TongId"] = data.TongId
				parameters["TargetTongId"] = data.TargetTongId
				parameters["ActiveBuildingHpTbl"] = g_BuildingMgr:GetTongBuildingHpTbl(data.TongId)
				parameters["PassiveBuildingHpTbl"] = {}
				CallDbTrans("WarZoneDB", "ExchangeStation", nil, parameters, data.TongId, data.StationId, data.TargetTongId)
			end
		elseif data.ActiveOrPassive == "Passive" then
			local parameters = {}
			parameters["PlayerId"] = data.PlayerId
			parameters["WarZoneId"] = data.WarZoneId
			parameters["StationId"] = data.StationId
			parameters["TongId"] = data.TongId
			parameters["TargetTongId"] = data.TargetTongId
			parameters["ActiveBuildingHpTbl"] = data.ActiveBuildingHpTbl
			parameters["PassiveBuildingHpTbl"] = g_BuildingMgr:GetTongBuildingHpTbl(data.TongId)
			CallDbTrans("WarZoneDB", "ExchangeStation", nil, parameters, data.TongId, data.StationId, data.TargetTongId)
		end
	elseif stationMoveType == g_WarZoneMgr.m_StationMoveType.PassiveRetreat then
		local parameters = {}
		parameters["PlayerId"] = data.PlayerId
		parameters["TongId"] = data.TongId
		parameters["CurPosition"] = data.CurPosition
		parameters["BuildingHpTbl"] = g_BuildingMgr:GetTongBuildingHpTbl(data.TongId)
		CallDbTrans("WarZoneDB", "RetreatStationRpc", CallBack, parameters, data.TongId)
	elseif stationMoveType == g_WarZoneMgr.m_StationMoveType.ActiveRetreat then
		
		local function CallBack(isSucceed, playerId, msgId)
			if isSucceed then
				MsgToConnById(playerId, 9415)
			elseif IsNumber(msgId) then
				MsgToConnById(playerId, msgId)
			end
		end
		local parameters = {}
		parameters["PlayerId"] = data.PlayerId
		parameters["TongId"] = data.TongId
		parameters["BuildingHpTbl"] = g_BuildingMgr:GetTongBuildingHpTbl(data.TongId)
		
		CallDbTrans("WarZoneDB", "PlayerRetreatStation", CallBack, parameters, data.TongId)
	end
end

function Db2Gas:RetreatStation(data)
	local tongId = data.TongId
	local warZoneId = data.WarZoneId
	local stationId = data.StationId
	if tongId ~= 0 then
		local dropResourceTbl = data.DropResourceTbl
		local memberTbl = data.MemberTbl
		local warZone = g_WarZoneMgr:GetCoreWarZone(warZoneId)
		assert(warZone.m_StationOwnerTong[stationId] == tongId)
		
		warZone:ClearStationOwner(stationId)
		local npcStation = warZone.m_NpcStationTbl[stationId]
		npcStation:CreateStationManager()
		local scene = warZone.m_Scene
		local centerPos = g_WarZoneMgr:GetStationPos(stationId)
		for buildingId, resInfo in pairs(dropResourceTbl) do
			DropResource(scene, (centerPos[1] + resInfo.x) * EUnits.eGridSpan, (centerPos[2] + resInfo.y) * EUnits.eGridSpan, resInfo.resNum)
		end
		
		g_BuildingMgr:DestroyAllTongBuildingNpc(tongId)
--		warZone.m_LifeTreesTbl[stationId]:Destroy()
		
		if memberTbl then
			for i = 1, #memberTbl do
				local nPlayerId = memberTbl[i][1]
				local nServerId = memberTbl[i][2]
				if nServerId == g_CurServerId then
					local Player = g_GetPlayerInfo(nPlayerId)
					if Player and Player.m_Conn then
						SysMovementNotifyToClient(Player.m_Conn, 1008)
						MsgToConn(Player.m_Conn, 9434)
					end
				else
					Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1008)
				end
			end
		end
		
		-- 改变原来驻地的图标
		Gas2Gac:UpdateWarZoneState(scene.m_CoreScene, stationId, 0, "")
		warZone.m_StationOwnerCamp[stationId] = 0

	else
		local warZone = g_WarZoneMgr:GetCoreWarZone(warZoneId)
		local npcStation = warZone.m_NpcStationTbl[stationId]
		npcStation:Destroy()
--		warZone.m_LifeTreesTbl[stationId]:Destroy()
	end
end

		
function Db2Gas:DeleteOldStation(data)
	local tongId = data.TongId
	if data.SceneType == 6 then
		g_TongFbMgr:CloseTongScene(tongId, data.SceneId)
	else
		local warZone = g_WarZoneMgr:GetCoreWarZone(data.WarZoneId)
		assert(warZone.m_StationOwnerTong[data.StationId] == tongId)
		warZone:ClearStationOwner(data.StationId)
		g_BuildingMgr:DestroyAllTongBuildingNpc(tongId)
		local npcStation = warZone.m_NpcStationTbl[data.StationId]
		npcStation:CreateStationManager()
		
		-- 改变原来驻地的图标
		local Scene = g_SceneMgr:GetScene(data.SceneId)
		Gas2Gac:UpdateWarZoneState(Scene.m_CoreScene, data.StationId, 0, "")
		Scene.m_WarZone.m_StationOwnerCamp[data.StationId] = 0
--		warZone.m_LifeTreesTbl[data.StationId]:Destroy()
	end
end

function Db2Gas:CreateNewStation(data)
	local warZoneId = data.WarZoneId
	local stationId = data.StationId
	local tongId = data.TongId
	local camp = data.Camp
	local tongName = data.TongName
	local memberTbl = data.MemberTbl
	local warZone = g_WarZoneMgr:GetCoreWarZone(warZoneId)
	assert(warZone.m_StationOwnerTong[stationId] == 0)
	warZone:SetStationOwner(stationId, tongId, camp, tongName)
	local buildinfo = data.BuildingTbl
	warZone:CreateStationBuilding(stationId, camp, buildinfo)
	local npcStation = warZone.m_NpcStationTbl[stationId]
	if npcStation and npcStation.m_StationManagerOverTick then
		UnRegisterTick(npcStation.m_StationManagerOverTick)
		npcStation.m_StationManagerOverTick = nil
	end
	if IsServerObjValid(npcStation.m_StationManager) then
		g_NpcServerMgr:DestroyServerNpcNow(npcStation.m_StationManager, false)
	end
	g_WarZoneMgr:SentryDeal(warZoneId, stationId)
--	warZone.m_LifeTreesTbl[stationId]:Create()
	if memberTbl then
		for i = 1, #memberTbl do
			local nPlayerId = memberTbl[i][1]
			local nServerId = memberTbl[i][2]
			if nServerId == g_CurServerId then
				local Player = g_GetPlayerInfo(nPlayerId)
				if Player and Player.m_Conn then
					SysMovementNotifyToClient(Player.m_Conn, 1009)
					MsgToConn(Player.m_Conn, 9435)
				end
			else
				Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1009)
			end
		end
	end
	
	-- 改变占领的新的驻地的图标
	local Scene = g_SceneMgr:GetScene(data.SceneId)
	Gas2Gac:UpdateWarZoneState(Scene.m_CoreScene, data.StationId, data.Camp, data.TongName)
end


---------------------------------服务器间消息-------------------------------
function Gas2GasDef:PlayerRetreatStation(Conn, tongId, charId)
--	local function CallBack(isSucceed, msgId)
--		if isSucceed then
--			MsgToConnById(charId, 9415)
--		elseif IsNumber(msgId) then
--			MsgToConnById(charId, msgId)
--		end
--	end
	local data = {}
	data["PlayerId"] = charId
	data["TongId"] = tongId
	data["StationMoveType"] = g_WarZoneMgr.m_StationMoveType.ActiveRetreat
	CallDbTrans("WarZoneDB", "StationMoveGetTongStationInfo", nil, data, tongId)
end

function Gas2GasDef:CloseTongScene(Conn, tongId, sceneId)
	g_TongFbMgr:CloseTongScene(tongId, sceneId)
end

function Gas2GasDef:ExchangeStation(Conn, playerId, challengerTongId, targetWarZoneId, targetStationId)
	g_WarZoneMgr:ExchangeStation(playerId, challengerTongId, targetWarZoneId, targetStationId)
end

function Gac2Gas:RequestShowTongWarBattleCountWnd(Conn)
	if not g_WarZoneMgr.m_Warring then
		return
	end
	local player = Conn.m_Player
	if not player then
		return
	end
	local nSceneID = player.m_Scene.m_SceneId
	if not IsCppBound(Conn) or not IsServerObjValid(player) then
		return
	end
--	local nTongID = player.m_Properties:GetTongID()
--	if nTongID == 0 then
--		return
--	end
	Gas2Gac:OpenTongWarBattleCountWnd(Conn)
end

function Gac2Gas:GetTongWarMemberListInfo(Conn)
	if not g_WarZoneMgr.m_Warring then
		return
	end
	local player = Conn.m_Player
	if not player then
		return
	end
	local nSceneID = player.m_Scene.m_SceneId
	if not IsCppBound(Conn) or not IsServerObjValid(player) then
		return
	end
	local nTongID = player.m_Properties:GetTongID()
	local nCharID = player.m_uID
	
	local function CallBack(memberTbl,GetExploitTbl)
		Gas2Gac:ReturnTongWarMemberListInfoBegin(Conn)
		local nTongGetExploitSum = 0
		if memberTbl:GetRowNum() == 0 then
			if player and IsServerObjValid(player) then
				for warZoneId, warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
					local scene = warZone.m_Scene
					local sceneId = player.m_Scene.m_SceneId
					if scene.m_SceneId == sceneId then					
						local nLevel = player:CppGetLevel()
						local nKillNum = 0
						if g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nCharID] then
							nKillNum = g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nCharID]
						end
						local nMaxContiKillNum = 0
						if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nCharID] then
							nMaxContiKillNum = g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nCharID]
						end
						if IsCppBound(player.m_Conn) and IsServerObjValid(player) then
							local sPlayerName = player.m_Properties:GetCharName()
							local nDeathCount = 0
							if g_TongBattleCountMgr.m_DeathCountTbl[nSceneID] and g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][nCharID] then
								nDeathCount = g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][nCharID]
							end
							Gas2Gac:ReturnTongWarMemberListInfo(Conn, sPlayerName, nLevel, nKillNum, nMaxContiKillNum, nDeathCount, GetExploitTbl[nCharID])
							CWarZoneMgr.GetCampExploitSum(nCharID)
						end
					end
				end
			end
		else
			for index = 1, memberTbl:GetRowNum() do
				local playerId = memberTbl(index, 1)
				nTongGetExploitSum = nTongGetExploitSum + GetExploitTbl[playerId]
				local player = g_GetPlayerInfo(playerId)
				if player and IsServerObjValid(player) then
					for warZoneId, warZone in pairs(g_WarZoneMgr.m_WarZoneTbl) do
						local scene = warZone.m_Scene
						local sceneId = player.m_Scene.m_SceneId
						if scene.m_SceneId == sceneId then					
							local nLevel = player:CppGetLevel()
							local nKillNum = 0
							if g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][playerId] then
								nKillNum = g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][playerId]
							end
							local nMaxContiKillNum = 0
							if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][playerId] then
								nMaxContiKillNum = g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][playerId]
							end
							if IsCppBound(player.m_Conn) and IsServerObjValid(player) then
								local sPlayerName = player.m_Properties:GetCharName()
								local nDeathCount = 0
								if g_TongBattleCountMgr.m_DeathCountTbl[nSceneID] and g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][playerId] then
									nDeathCount = g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][playerId]
								end
								Gas2Gac:ReturnTongWarMemberListInfo(Conn, sPlayerName, nLevel, nKillNum, nMaxContiKillNum, nDeathCount, GetExploitTbl[playerId])
								CWarZoneMgr.GetCampExploitSum(nCharID)
							end
						end
					end
				end
			end
		end
		Gas2Gac:ReturnTongGetExploitSum(Conn,nTongGetExploitSum)
		Gas2Gac:ReturnTongWarMemberListInfoEnd(Conn)
	end
	local parameter = {}
	parameter["uTongID"] = nTongID
	parameter["sceneId"] = nSceneID
	parameter["uCharID"] = nCharID
	CallAccountAutoTrans(Conn.m_Account, "GasTongBasicDB", "GetAllMemAndGeByTongIDRpc", CallBack, parameter, nTongID)
end

function CWarZoneMgr.DealCampExploit(nSceneID)
	local function CallBack(serverIdTbl)
		for _, server_id in pairs (serverIdTbl) do
			Gas2GasAutoCall:UpdateCampExploit(GetServerAutoConn(server_id))
		end
	end
	local data = {}
	data["sceneId"] = nSceneID
	CallDbTrans("SceneMgrDB","GetSceneServerId",CallBack,data)
end

function CWarZoneMgr.GetCampExploitSum(playerId)
	local function CallBack(nAmGetExploitSum, nSsGetExploitSum, nXsGetExploitSum)
		Gas2GacById:ReturnCampGetExploitSum(playerId, nAmGetExploitSum, nSsGetExploitSum, nXsGetExploitSum)
	end
	CallDbTrans("GasTongBasicDB","GetCampExploitSumDB",CallBack,{})
end

function Gac2Gas:JudgeWarTransport(Conn, sceneName) 
	if not IsCppBound(Conn) then
		return 
	end
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local function CallBack(Flag, MsgId)
		if Flag then
			if IsCppBound(Player) then
				local pos = g_WarZoneMgr:GetEnterPos(Player:CppGetCamp())
				if sceneName == Player.m_Scene.m_SceneName then
					local temppos = CPos:new()
					temppos.x = pos[1]
					temppos.y = pos[2]
					Player:SetGridPosByTransport(temppos)
				else
					ChangeSceneByName(Conn, sceneName, pos[1],pos[2])
				end
			end
		else
			MsgToConn(Conn, MsgId)
			return
		end
	end
	
	local data = {}
	data["uPlayerID"] = Player.m_uID
	CallDbTrans("GasTongBasicDB", "CanEnterWarZone", CallBack, data, Player.m_uID)
end

function InitNewTongMemberPanel(uCharID)
	local Player = g_GetPlayerInfo(uCharID)
	if not Player or not IsCppBound(Player) then
		return
	end
	local Scene = Player.m_Scene
	local nSceneType = Scene.m_SceneAttr.SceneType
	local nSceneID = Scene.m_SceneId
	local nTongID = Player.m_Properties:GetTongID()
--	if nSceneType == 7 and g_WarZoneMgr.m_Warring  then
--		Gas2Gac:OpenTongWarExitBtnWnd(Player.m_Conn)
--	end
	
	if nSceneType == 8 and nTongID ~= 0 then
		local function CallBack(bFlag, nBattleState)
			if bFlag and nBattleState ~= 4 then
				Gas2Gac:OpenTongChallengeExitBtnWnd(Player.m_Conn)
			end
		end
		local parameter = {}
		parameter["nTongID"] = nTongID
		CallAccountAutoTrans(Player.m_Conn.m_Account, "GasTongBasicDB", "GetTongChallengeBattleState", CallBack, parameter, nTongID)
	end
end

function IntoWarZoneScene(Player)
	local nTongID = Player.m_Properties:GetTongID()
	if nTongID ~= 0 then
		local function CallBack(nSceneType, nSceneId, nServerId, nWarZoneId, nStationIndex)
			if nSceneType == 7 and nWarZoneId == Player.m_Scene.m_WarZone.m_WarZoneId then
				local nState
				if g_TongMonsAttackStationMgr.m_CurrentState[nWarZoneId] then
					nState = g_TongMonsAttackStationMgr.m_CurrentState[nWarZoneId][nStationIndex]
				end
				
				if nState then
					local curDate =  os.date("*t")
					local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
					local offset = GetOffsetSecond(g_TongMonsAttackStationMgr.m_GameStartTime.time[1])
					local leftTime = 0
					if nState == 2 then
						leftTime = offset + g_TongMonsAttackStationMgr.m_GameReadyLength + TongMonsAttackStartTime_Server(nStationIndex, "DelaySecond") - todayOffsetSecond
					elseif nState == 3 then
						leftTime = offset + g_TongMonsAttackStationMgr.m_GameReadyLength + g_TongMonsAttackStationMgr.m_GameLength + TongMonsAttackStartTime_Server(nStationIndex, "DelaySecond") - todayOffsetSecond
					end
					
					Gas2Gac:InitTongDownTimeWnd(Player.m_Conn, leftTime, nState)
					if nState == 3 then
						Gas2Gac:OpenTongMonsAttackExitBtnWnd(Player.m_Conn)
					end
				end
			end
		end
		CallDbTrans("WarZoneDB", "GetTongStationInfo", CallBack, {["tongId"] = nTongID}, nTongID)
	end
	if not g_WarZoneMgr.m_Warring then
		return
	end
	g_TongBattleCountMgr:ResumeEffect(Player)
	local isOpenTime, length = IsInOpenTime(g_WarZoneMgr.m_WarStartTime, g_WarZoneMgr.m_WarTimeLength)
	if isOpenTime then
		Gas2Gac:OpenTongWarExitBtnWnd(Player.m_Conn)
		local nKillNum = 0
		local nSceneID = Player.m_Scene.m_SceneId
		local nPlayerID = Player.m_uID
		if g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nPlayerID] then
			nKillNum = g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nPlayerID]
		end
		local nContiKillNum = 0
		if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] then
			nContiKillNum = g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID]
		end
		local nDeathCount = 0
		if g_TongBattleCountMgr.m_DeathCountTbl[nSceneID] and g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][nPlayerID] then
			nDeathCount = g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][nPlayerID]
		end
		Gas2Gac:InitTongBattlePersonalCountWnd(Player.m_Conn, length, 3, nKillNum, nContiKillNum, nDeathCount)
	end
end

function LeaveWarZoneScene(Player, nSceneID)
	local Conn = Player.m_Conn
	local nPlayerID = Player.m_uID
	Gas2Gac:CloseTongWarPanel(Conn)
	g_TongBattleCountMgr:ResetContiKillNum(Player)
	
	--清除国战杀人总数和连续杀人加的状态
	if IsServerObjValid(Player) then
		if Player:CppIsLive() then
			Player:PlayerDoPassiveSkill("国战结束清除", 1)
		else
			Player:PlayerDoPassiveSkill("国战结束尸体清除", 1)
		end
	end
end

function PlayerLoginWarZone(Player)
	local nTongID = Player.m_Properties:GetTongID()
	if nTongID ~= 0 then
		local function CallBack(nSceneType, nSceneId, nServerId, nWarZoneId, nStationIndex)
			if nSceneType == 7 and nWarZoneId == Player.m_Scene.m_WarZone.m_WarZoneId then
				local nState
				if g_TongMonsAttackStationMgr.m_CurrentState[nWarZoneId] then
					nState = g_TongMonsAttackStationMgr.m_CurrentState[nWarZoneId][nStationIndex]
				end
				
				if nState then
					local curDate =  os.date("*t")
					local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
					local offset = GetOffsetSecond(g_TongMonsAttackStationMgr.m_GameStartTime.time[1])
					local leftTime = 0
					if nState == 2 then
						leftTime = offset + g_TongMonsAttackStationMgr.m_GameReadyLength + TongMonsAttackStartTime_Server(nStationIndex, "DelaySecond") - todayOffsetSecond
					elseif nState == 3 then
						leftTime = offset + g_TongMonsAttackStationMgr.m_GameReadyLength + g_TongMonsAttackStationMgr.m_GameLength + TongMonsAttackStartTime_Server(nStationIndex, "DelaySecond") - todayOffsetSecond
					end
					
					Gas2Gac:InitTongDownTimeWnd(Player.m_Conn, leftTime, nState)
					
					if nState == 3 then
						Gas2Gac:OpenTongMonsAttackExitBtnWnd(Player.m_Conn)
					end
				end
			end
		end
		CallDbTrans("WarZoneDB", "GetTongStationInfo", CallBack, {["tongId"] = nTongID}, nTongID)
	end
	
	if not g_WarZoneMgr.m_Warring then
		return
	end
	
	g_TongBattleCountMgr:ResumeEffect(Player)
	local isOpenTime, length = IsInOpenTime(g_WarZoneMgr.m_WarStartTime, g_WarZoneMgr.m_WarTimeLength)
	if isOpenTime then
		Gas2Gac:OpenTongWarExitBtnWnd(Player.m_Conn)
		local nKillNum = 0
		local nSceneID = Player.m_Scene.m_SceneId
		local nPlayerID = Player.m_uID
		if g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nPlayerID] then
			nKillNum = g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nPlayerID]
		end
		local nContiKillNum = 0
		if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] then
			nContiKillNum = g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID]
		end
		local nDeathCount = 0
		if g_TongBattleCountMgr.m_DeathCountTbl[nSceneID] and g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][nPlayerID] then
			nDeathCount = g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][nPlayerID]
		end
		Gas2Gac:InitTongBattlePersonalCountWnd(Player.m_Conn, length, 3, nKillNum, nContiKillNum, nDeathCount)
	end
end

function PlayerOffLineWarZone(Player)
	local Conn = Player.m_Conn
	local nSceneID = Player.m_Scene.m_SceneId
	local nPlayerID = Player.m_uID
	Gas2Gac:CloseTongWarPanel(Conn)
	if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] then
		g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] = nil
		if not next(g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID]) then
			g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] = nil
		end
	end
	
	--清除国战杀人总数和连续杀人加的状态
	if IsServerObjValid(Player) then
		if Player:CppIsLive() then
			Player:PlayerDoPassiveSkill("国战结束清除", 1)
		else
			Player:PlayerDoPassiveSkill("国战结束尸体清除", 1)
		end
	end
end

function Gas2GasDef:SendTongMessage(Conn, nPlayerID, nMsgID)
	local Player = g_GetPlayerInfo(nPlayerID)
	if Player and Player.m_Conn then
		SysMovementNotifyToClient(Player.m_Conn, nMsgID)
	end
end

------------------------------- 国战控制 ------------------------------- by beingway
-- 重新设置国战时间，放弃静态表配置
function CWarZoneMgr:SetAllServerWarTime(time, wday, length)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:SetAllServerWarTime(GetServerAutoConn(server_id), time, wday, length)
	end
end

function CWarZoneMgr:SetWarTime(time, wday, length)
	g_AlarmClock:RemoveTask(self.m_WarTaskIndex)
	self.m_WarStartTime = {}
	self.m_WarStartTime["time"] = {time}
	self.m_WarStartTime["wday"] = {wday}
	self.m_WarTaskIndex = g_AlarmClock:AddTask("WarStart", g_WarZoneMgr.m_WarStartTime, TongWarStart, nil)
	if length then
		self.m_WarTimeLength = length
	end
	return "SetWarTime Done"
end

-- 开启国战
function CWarZoneMgr:StartAllServerWar(length)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:StartAllServerWar(GetServerAutoConn(server_id), length)
	end
end

function CWarZoneMgr:StartWar(length)
	if not self.m_WarEndTick then
		self.m_WarStartTime = {}
		local curDate = os.date("*t")
		local time = curDate.hour..":"..curDate.min
		local wday = ((curDate.wday == 1) and 7 or curDate.wday - 1)
		self.m_WarStartTime["time"] = {time}
		self.m_WarStartTime["wday"] = {wday}
		self.m_WarTimeLength = length * 60 * 1000
		TongWarStart(self.m_WarTimeLength)
		return "StartWar Done"
	else
		return "Is Warring! Don't repeat operation!"
	end
end

-- 停止国战
function CWarZoneMgr:StopAllServerWar()
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:StopAllServerWar(GetServerAutoConn(server_id))
	end
end

function CWarZoneMgr:StopWar()
	if TongWarEnd() then
		return "StopWar Done"
	else
		return "Is Not Warring! Don't repeat operation!"
	end
end

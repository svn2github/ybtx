cfg_load "tong/TongStartTime_Common"
gas_require "world/tong_area/TongBattleCountMgr"
gas_require "log_mgr/CLogMgr"

local ChallengeStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团挑战开始时间", "OpenTime") --loadstring("return " .. TongStartTime_Common["佣兵团挑战开始时间"].OpenTime)()
local ChallengeCompeteStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团挑战竞标开始时间", "OpenTime") --loadstring("return " .. TongStartTime_Common["佣兵团挑战竞标开始时间"].OpenTime)()
local MakeListLength = TongStartTime_Common("挑战确定对战表时长", "OpenTime") * 60
local ReadyLength = TongStartTime_Common("挑战进场时长", "OpenTime") * 60
local ChallengeLength = TongStartTime_Common("挑战战斗时长", "OpenTime") * 60

local ChallengeSceneName = "帮会挑战本"
local BaseCampTbl = {}
BaseCampTbl[1] = "艾米挑战副本大本营"
BaseCampTbl[2] = "神圣挑战副本大本营"
BaseCampTbl[3] = "修斯挑战副本大本营"

local BuildItem = {}
BuildItem[1] = {{39,"艾米挑战副本防御设施",1}}
BuildItem[2] = {{39,"神圣挑战副本防御设施",1}}
BuildItem[3] = {{39,"修斯挑战副本防御设施",1}}

local GameState = {
	["eChlgList"] = 1,
	["eReady"] = 2,
	["ePlay"] = 3,
	["eClose"] =4,
}

g_TongChlgScTbl = {}
g_ChallengeCurrentState = GameState["eClose"]

local WzsIsChlged = {}
local TongIsChlg = {}

local TongChallengePlayTick
local TongChallengeEndTick
local TongChallengeStartTick

local delItemTbl = {}
delItemTbl[1] = {{39,"艾米挑战副本防御设施"}}
delItemTbl[2] = {{39,"神圣挑战副本防御设施"}}
delItemTbl[3] = {{39,"修斯挑战副本防御设施"}}

local DynamicFileTbl = {}
DynamicFileTbl[1] = "艾米挑战NPC"
DynamicFileTbl[2] = "神圣挑战NPC"
DynamicFileTbl[3] = "修斯挑战NPC"

local DynamicStoneFileTbl = {}
DynamicStoneFileTbl[1] = "aimitiaozhanshi"
DynamicStoneFileTbl[2] = "shenshengtiaozhanshi"
DynamicStoneFileTbl[3] = "xiusitiaozhanshi"

local BarrierFile = "tiaozhanzhangai"

local CHALLENGE_STATE =
{
	["Challenging"] = 1,
	["ChallengeSucc"] = 2,
}
local HonorChlgerAdd, HonorTgtAdd = 3, 9

-- BEGIN 解决主干挑战结束后还能继续打的BUG，分支已整理阵营更新代码无BUG，融合时应删掉 BEGIN -> END
-- 帮会挑战玩法阵营
GAME_CAMP_PEACE 	= 50
GAME_CAMP_CHLGER 	= 51
GAME_CAMP_DFNDER 	= 52

function UpdateAllGameCamp(scene)

	if scene.m_BaseCampOfChlger then
		scene.m_BaseCampOfChlger:CppSetGameCamp(scene.m_ChlgerGameCamp)
	end
	if scene.m_BaseCampOfDfnder then
		scene.m_BaseCampOfDfnder:CppSetGameCamp(scene.m_DfnderGameCamp)
	end
			
	for _, player in pairs(scene.m_tbl_Player) do
		local tongID = player.m_Properties:GetTongID()
		if scene.m_ChlgerTongId == tongID then
			player:CppSetGameCamp(scene.m_ChlgerGameCamp)
		elseif scene.m_TgtTongId == tongID then
			player:CppSetGameCamp(scene.m_DfnderGameCamp)
		end
	end
	
	local tblChlgerStoneNpc = scene.m_TongStoneNpcTbl[scene.m_ChlgerTongId]
	if tblChlgerStoneNpc then
		for _, npc in pairs(tblChlgerStoneNpc) do
			if IsServerObjValid(npc) then
				npc:CppSetGameCamp(scene.m_ChlgerGameCamp)
			end
		end
	end
	
	local tblDfnderStoneNpc = scene.m_TongStoneNpcTbl[scene.m_TgtTongId]
	if tblDfnderStoneNpc then
		for _, npc in pairs(tblDfnderStoneNpc) do
			if IsServerObjValid(npc) then
				npc:CppSetGameCamp(scene.m_DfnderGameCamp)
			end
		end
	end
		
	local tblChlgerTongBuilding = scene.m_TongBuildingTbl[scene.m_ChlgerTongId]
	if tblChlgerTongBuilding then
		for _, buildingNpc in pairs(tblChlgerTongBuilding) do
			if IsServerObjValid(buildingNpc) then
				buildingNpc:CppSetGameCamp(scene.m_ChlgerGameCamp)
			end
		end
	end
	
	local tblDfnderTongBuilding = scene.m_TongBuildingTbl[scene.m_TgtTongId]
	if tblDfnderTongBuilding then
		for _, buildingNpc in pairs(tblDfnderTongBuilding) do
			if IsServerObjValid(buildingNpc) then
				buildingNpc:CppSetGameCamp(scene.m_DfnderGameCamp)
			end
		end
	end
	
	if scene.NpcTbl then
		for _, npc in pairs(scene.NpcTbl) do
			if IsServerObjValid(npc) then
				npc:CppSetGameCamp(scene.m_DfnderGameCamp)
			end
		end
	end
end
-- END

local function ChallengeEnd()
	
	local HaveNotify = {}
	for tongId, scene in pairs(g_TongChlgScTbl) do
		if not scene.m_ChlgIsOver then
			scene.m_NotCanEnter = true

			scene.m_ChlgerGameCamp = GAME_CAMP_PEACE
			scene.m_DfnderGameCamp = GAME_CAMP_PEACE
			UpdateAllGameCamp(scene)
			
			for playerId,player in pairs(scene.m_tbl_Player) do
				if not HaveNotify[playerId] then
					HaveNotify[playerId] = 0
					local uTongID = player.m_Properties:GetTongID()
					if uTongID ~= 0 then
						local ChlgKillBuildNum = g_TongChlgScTbl[uTongID].m_ChlgKillBuildNum
						local TgtKillBuildNum = g_TongChlgScTbl[uTongID].m_TgtKillBuildNum
						local ChlgKillPersonNum = g_TongChlgScTbl[uTongID].m_ChlgKillPersonNum
						local TgtKillPersonNum = g_TongChlgScTbl[uTongID].m_TgtKillPersonNum
						
						local function CallBack(Flag, ChlgTongName, TgtTongName)
							if Flag then
								Gas2Gac:ShowTongChallengeStatisticsWnd(player.m_Conn, TgtTongName, TgtKillBuildNum, TgtKillPersonNum, HonorTgtAdd, ChlgTongName, ChlgKillBuildNum, ChlgKillPersonNum, HonorChlgerAdd)
							end
						end
						
						local parameter = {}
						parameter["ChlgerTongId"] = scene.m_ChlgerTongId
						parameter["TgtTongId"] = scene.m_TgtTongId
						CallAccountAutoTrans(player.m_Conn.m_Account, "GasTongBasicDB", "GetChlgerNameAndTgtName", CallBack, parameter, scene.m_ChlgerTongId, scene.m_TgtTongId)
					end
				end
			end
			
			local parameters = {}
			parameters["uChlgTongId"] = scene.m_ChlgerTongId
			parameters["uTgtTongId"] = scene.m_TgtTongId
			parameters["uChlgerHonor"] = HonorChlgerAdd
			parameters["uTgtHonor"] = HonorTgtAdd
			CallDbTrans("GasTongWarDB", "ChallengeEndUpdateHonor", nil, parameters, scene.m_ChlgerTongId, scene.m_TgtTongId)

			-- 写LOG
			TongChallengeEndLog(scene.m_ChlgerTongId,scene.m_TgtTongId,scene.m_TgtWzId,scene.m_TgtWzsId,0)
		end
	end
	
	local function CallBack(nFlag)
		if not nFlag then
			LogErr("帮会挑战出错", "挑战结束清除对战表出错,服务器Id：" .. g_CurServerId)
		end
	end
	CallDbTrans("GasTongWarDB", "ClearSelfServerChallengeBattleInfo", nil, {["uServerId"] = g_CurServerId}, g_CurServerId)
end

local function ChallengeEndTick()
	if TongChallengeEndTick then
		g_ChallengeCurrentState = GameState["eClose"]
		UnRegisterTick(TongChallengeEndTick)
		TongChallengeEndTick = nil
		ChallengeEnd()
	end
end

local function DestroyChallengeBarrier(uSceneID)
	if g_AllIntObj[uSceneID] then
		for _, IntObj in pairs(g_AllIntObj[uSceneID]) do
			if IsServerObjValid(IntObj) then
				if IntObj.m_Properties:GetCharName() ~= "中立复活旗" then
					g_IntObjServerMgr:Destroy(IntObj, false)
				end
			end
		end
	end
end

local function ChallengePlayTick()
	if TongChallengePlayTick then
		g_ChallengeCurrentState = GameState["ePlay"]
		
		UnRegisterTick(TongChallengePlayTick)
		TongChallengePlayTick = nil
		TongChallengeEndTick = RegisterTick("TongChallengeEndTick", ChallengeEndTick, ChallengeLength * 1000)
		
		for _, scene in pairs(g_TongChlgScTbl) do
			DestroyChallengeBarrier(scene.m_SceneId)
			
			scene.m_ChlgerGameCamp = GAME_CAMP_CHLGER
			scene.m_DfnderGameCamp = GAME_CAMP_DFNDER
			UpdateAllGameCamp(scene)
			
			for _, player in pairs(scene.m_tbl_Player) do
				Gas2Gac:TongChallengeBattleBegin(player.m_Conn, ChallengeLength)
			end
		end
		
		local function PlayCallBack(resTbl)
			for _,memberSet in pairs(resTbl) do
				local memberSetTbl = memberSet:GetTableSet()
				for i = 1, memberSet:GetRowNum() do
					local nPlayerId = memberSetTbl(i, 1)
					local nServerId = memberSetTbl(i, 2)
					if nServerId == g_CurServerId then
						local Player = g_GetPlayerInfo(nPlayerId)
						if Player and Player.m_Conn then
							SysMovementNotifyToClient(Player.m_Conn, 1004)
						end
					else
						Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1004)
					end
				end
			end
		end
		local parameter = {}
		parameter["uState"] = GameState["ePlay"]
		parameter["uServerId"] = g_CurServerId
		CallDbTrans("GasTongWarDB", "UpdateSelfServerBattleStateAndSendMsg", PlayCallBack, parameter)
	end
end

local function ChallengeStartTick()
	if TongChallengeStartTick then
		g_ChallengeCurrentState = GameState["eReady"]
		
		UnRegisterTick(TongChallengeStartTick)
		TongChallengeStartTick = nil
		TongChallengePlayTick = RegisterTick("TongChallengePlayTick", ChallengePlayTick, ReadyLength * 1000)
		
		local function CallBack(Flag, resTbl)
			if Flag then
				for _, tbl in pairs(resTbl) do
					local nPlayerId = tbl[1]
					local nServerId = tbl[2]
					if nServerId == g_CurServerId then
						local Player = g_GetPlayerInfo(nPlayerId)
						if Player and Player.m_Conn then
							SysMovementNotifyToClient(Player.m_Conn, 1005)
						end
					else
						Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1005)
					end
				end
			end
		end
		
		local chanelId = 100
		CallDbTrans("GasTongWarDB", "ReturnMoneyAndClearChallengeInfo", CallBack, {}, chanelId)
		
		local function EnterCallBack(resTbl)
			for _,memberSet in pairs(resTbl) do
				local memberSetTbl = memberSet:GetTableSet()
				for i = 1, memberSet:GetRowNum() do
					local nPlayerId = memberSetTbl(i, 1)
					local nServerId = memberSetTbl(i, 2)
					if nServerId == g_CurServerId then
						local Player = g_GetPlayerInfo(nPlayerId)
						if Player and Player.m_Conn then
							SysMovementNotifyToClient(Player.m_Conn, 1006)
						end
					else
						Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1006)
					end
				end
			end
		end
		local parameter = {}
		parameter["uState"] = GameState["eReady"]
		parameter["uServerId"] = g_CurServerId
		
		CallDbTrans("GasTongWarDB", "UpdateSelfServerBattleStateAndSendMsg", EnterCallBack, parameter)
	end
end

--如果targetTongId 为0 则挑战目标为npc
function CreateChallengeScene(scene, challengerTongId, targetWarZoneId, targetStationId, targetTongId, Camp)
	
	scene.m_ChlgerTongId = challengerTongId
	scene.m_TgtWzId = targetWarZoneId
	scene.m_TgtWzsId = targetStationId
	scene.m_TgtTongId = targetTongId
	scene.m_Camp = Camp
	scene.m_ChallengePlayerTbl = {} --记录哪些领取过建筑,出场景时删除建筑
	scene.m_IntObjTbl = {}
	
	scene.m_ChlgKillPersonNum = 0
	scene.m_TgtKillPersonNum = 0
	scene.m_ChlgKillBuildNum = 0
	scene.m_TgtKillBuildNum = 0
	
	scene.m_TongBuildingTbl = {} --挑战交战双方建筑的表
	
	scene.m_TongStoneNpcTbl = {} --挑战双方守护石Npc Tbl
	if g_ChallengeCurrentState == GameState["ePlay"] then
		scene.m_ChlgerGameCamp = GAME_CAMP_CHLGER
		scene.m_DfnderGameCamp = GAME_CAMP_DFNDER
	else
		g_DynamicCreateMgr:Create(scene, BarrierFile)
		scene.m_ChlgerGameCamp = GAME_CAMP_PEACE
		scene.m_DfnderGameCamp = GAME_CAMP_PEACE
	end

	--创建挑战方大本营
	local posChlger = g_TongChallengeFbMgr:GetChallengerCenterPos()
	local ChlgerPos = CFPos:new(posChlger[1] * EUnits.eGridSpan, posChlger[2] * EUnits.eGridSpan)
	local ChlgerBaseCamp, logMsg = g_NpcServerMgr:CreateServerNpc(BaseCampTbl[Camp], g_WarZoneMgr.m_NpcLevel, scene, ChlgerPos)
	if not IsServerObjValid(ChlgerBaseCamp) then
		LogErr("帮会战创建Npc失败", logMsg)
		return
	end
	ChlgerBaseCamp.m_TongID = challengerTongId
	ChlgerBaseCamp:CppSetGameCamp(scene.m_ChlgerGameCamp)
	scene.m_BaseCampOfChlger = ChlgerBaseCamp

	
	--创建被挑战方大本营
	local posTgt = g_TongChallengeFbMgr:GetTargetCenterPos()
	local TgtPos = CFPos:new(posTgt[1] * EUnits.eGridSpan, posTgt[2] * EUnits.eGridSpan)
	local TgtBaseCamp, logMsg = g_NpcServerMgr:CreateServerNpc(BaseCampTbl[Camp], g_WarZoneMgr.m_NpcLevel, scene, TgtPos)
	if not IsServerObjValid(TgtBaseCamp) then
		LogErr("帮会战创建Npc失败", logMsg)
		return
	end
	TgtBaseCamp.m_TongID = targetTongId
	TgtBaseCamp:CppSetGameCamp(scene.m_DfnderGameCamp)
	scene.m_BaseCampOfDfnder = TgtBaseCamp

	
	if targetTongId == 0 then
		local npcTbl = g_DynamicCreateMgr:Create(scene, DynamicFileTbl[Camp], posTgt)
		scene.NpcTbl = npcTbl
		for _, npc in pairs(scene.NpcTbl) do
			npc:CppSetLevel(g_WarZoneMgr.m_NpcLevel)
			npc.m_TongID = targetTongId
		end
	end
	
	local Pos = CFPos:new(110, 157)
	local IntObj = CreateIntObj(scene, Pos, "中立复活旗", nil, nil)
	IntObj.m_ObjIndex = 1
	scene.m_IntObjTbl[1] = IntObj
	
	Pos = CFPos:new(223, 175)
	IntObj = CreateIntObj(scene, Pos, "中立复活旗", nil, nil)
	IntObj.m_ObjIndex = 2
	scene.m_IntObjTbl[2] = IntObj
	
	scene.m_TongStoneNpcTbl[challengerTongId] = {}
	scene.m_TongStoneNpcTbl[targetTongId] = {}
	
	local tblChlgerStoneNpc = g_DynamicCreateMgr:Create(scene, DynamicStoneFileTbl[Camp], g_TongChallengeFbMgr:GetChallengerCenterPos())
	scene.m_TongStoneNpcTbl[challengerTongId] = tblChlgerStoneNpc
	
	local function ActiveCallBack(nLevel)
		for _, npc in pairs(tblChlgerStoneNpc) do
			if IsServerObjValid(npc) then
				npc:CppSetLevel(nLevel)
			end
		end
	end
	CallDbTrans("GasTongBasicDB", "GetTongMaxLevelRpc", ActiveCallBack, {["nTongID"] = challengerTongId}, challengerTongId)
	
	local tblTgtStoneNpc = g_DynamicCreateMgr:Create(scene, DynamicStoneFileTbl[Camp], g_TongChallengeFbMgr:GetTargetCenterPos())
	scene.m_TongStoneNpcTbl[targetTongId] = tblTgtStoneNpc
	
	local function PassiveCallBack(nLevel)
		for _, npc in pairs(tblTgtStoneNpc) do
			if IsServerObjValid(npc) then
				npc:CppSetLevel(nLevel)
			end
		end
	end
	
	if targetTongId ~= 0 then
		CallDbTrans("GasTongBasicDB", "GetTongMaxLevelRpc", PassiveCallBack, {["nTongID"] = targetTongId}, targetTongId)
	else
		PassiveCallBack(g_WarZoneMgr.m_NpcLevel)
	end
	
	if not g_TongChlgScTbl[challengerTongId] then
		g_TongChlgScTbl[challengerTongId] = scene
	end
	if not g_TongChlgScTbl[targetTongId] and targetTongId ~= 0 then
		g_TongChlgScTbl[targetTongId] = scene
	end
	
	UpdateAllGameCamp(scene)
end

local function DeleteGameItem(playerId)
	local player = g_GetPlayerInfo(playerId)
	if not player or not IsServerObjValid(player) then
		return
	end
	local camp = player:CppGetCamp()
	local itemTbl = delItemTbl[camp]
	local function CallBack(allRes)
	end
	
	local parameter = {}
	parameter["nCharID"] =  playerId
	parameter["itemTbl"] = itemTbl
	parameter["sceneName"] = ChallengeSceneName
	
	local ChannelID = playerId
	if IsServerObjValid(player) then
		ChannelID = player.m_AccountID
	end
	CallDbTrans("CharacterMediatorDB", "DelActionItem", CallBack, parameter, ChannelID)
end

function IsCanReSumeTongChallenge(scene)
	if not scene or scene.m_NotCanEnter then
		return false
	end
	
	if g_ChallengeCurrentState == GameState.eClose then
		return false
	end
	
	return true
end

function PlayerLoginTongChallenge(player)
	local Scene = player.m_Scene
	local tongId = player.m_Properties:GetTongID()
	
	local function CallBack(Flag, BattleState)
		if Flag and player then
			local curDate =  os.date("*t")
			local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
			local offset = GetOffsetSecond(ChallengeStartTime.time[1])
			local leftTime = 0
			if BattleState == 2 then
				leftTime = offset + MakeListLength + ReadyLength - todayOffsetSecond
			else
				leftTime = offset + MakeListLength + ReadyLength + ChallengeLength - todayOffsetSecond
			end
			local nKillNum = 0
			if g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][playerId] then
				nKillNum = g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][playerId]
			end
			local nContiKillNum = 0
			if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][playerId] then
				nContiKillNum = g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][playerId]
			end
			local nDeathCount = 0
			if g_TongBattleCountMgr.m_DeathCountTbl[nSceneID] and g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][playerId] then
				nDeathCount = g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][playerId]
			end
			Gas2Gac:InitTongBattlePersonalCountWnd(player.m_Conn, leftTime, BattleState, nKillNum, nContiKillNum, nDeathCount)
			Gas2Gac:RetSetSceneStateForClient(player.m_Conn, 2) --佣兵团挑战头顶文字的特殊显示
			
			if BattleState == GameState.ePlay then
				if tongId == Scene.m_TgtTongId then
					player:CppSetGameCamp(GAME_CAMP_DFNDER)
				elseif tongId == Scene.m_ChlgerTongId then
					player:CppSetGameCamp(GAME_CAMP_CHLGER)
				else
					player:CppSetGameCamp(GAME_CAMP_PEACE)
				end
			else
				player:CppSetGameCamp(GAME_CAMP_PEACE)
			end
		end
	end
	
	local parameter = {}
	parameter["ChlgerTongId"] = Scene.m_ChlgerTongId
	parameter["TgtTongId"] = Scene.m_TgtTongId
	CallAccountAutoTrans(player.m_Conn.m_Account, "GasTongBasicDB", "GetChallengeBattleState", CallBack, parameter, Scene.m_ChlgerTongId, Scene.m_TgtTongId)
	
	if tongId ~= 0 then
		Gas2Gac:OpenTongChallengeExitBtnWnd(player.m_Conn)
	end
	g_TongBattleCountMgr:ResumeEffect(player)
end

function PlayerOffLineTongChallengeScene(Player)
	local Conn = Player.m_Conn
	local nSceneID = Player.m_Scene.m_SceneId
	local nPlayerID = Player.m_uID
	Gas2Gac:RetSetSceneStateForClient(Player.m_Conn, 0) --清除佣兵团挑战头顶文字的特殊显示
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
	DeleteGameItem(nPlayerID)
end

function IntoTongChallengeScene(Player)
	local Scene = Player.m_Scene
	local Conn = Player.m_Conn
	local tongId = Player.m_Properties:GetTongID()
	local function CallBack(Flag, BattleState)
		if Flag then
			local curDate =  os.date("*t")
			local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
			local offset = GetOffsetSecond(ChallengeStartTime.time[1])
			local leftTime = 0
			if BattleState == 2 then
				leftTime = offset + MakeListLength + ReadyLength - todayOffsetSecond
			else
				leftTime = offset + MakeListLength + ReadyLength + ChallengeLength - todayOffsetSecond
			end
			local nKillNum = 0
			if g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][playerId] then
				nKillNum = g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][playerId]
			end
			local nContiKillNum = 0
			if g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][playerId] then
				nContiKillNum = g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][playerId]
			end
			local nDeathCount = 0
			if g_TongBattleCountMgr.m_DeathCountTbl[nSceneID] and g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][playerId] then
				nDeathCount = g_TongBattleCountMgr.m_DeathCountTbl[nSceneID][playerId]
			end
			Gas2Gac:InitTongBattlePersonalCountWnd(Player.m_Conn, leftTime, BattleState, nKillNum, nContiKillNum, nDeathCount)
			Gas2Gac:RetSetSceneStateForClient(Player.m_Conn, 2) --佣兵团挑战头顶文字的特殊显示
			
			if BattleState == GameState.ePlay then
				if tongId == Scene.m_TgtTongId then
					Player:CppSetGameCamp(GAME_CAMP_DFNDER)
				elseif tongId == Scene.m_ChlgerTongId then
					Player:CppSetGameCamp(GAME_CAMP_CHLGER)
				else
					Player:CppSetGameCamp(GAME_CAMP_PEACE)
				end
			else
				Player:CppSetGameCamp(GAME_CAMP_PEACE)
			end
		end
	end
	
	local parameter = {}
	parameter["ChlgerTongId"] = Scene.m_ChlgerTongId
	parameter["TgtTongId"] = Scene.m_TgtTongId
	CallAccountAutoTrans(Conn.m_Account, "GasTongBasicDB", "GetChallengeBattleState", CallBack, parameter, Scene.m_ChlgerTongId, Scene.m_TgtTongId)
	
	if tongId ~= 0 then
		Gas2Gac:OpenTongChallengeExitBtnWnd(Conn)
	end
	g_TongBattleCountMgr:ResumeEffect(Player)
end

function LeaveTongChallengeScene(Player, nSceneID)
	local Conn = Player.m_Conn
	local nPlayerID = Player.m_uID
	Gas2Gac:CloseTongChallengePanel(Conn)
	Gas2Gac:RetSetSceneStateForClient(Player.m_Conn, 0) --清除佣兵团挑战头顶文字的特殊显示
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
	DeleteGameItem(nPlayerID)
end

function MakeChlgList()
	g_ChallengeCurrentState = GameState["eChlgList"]
	TongChallengeStartTick = RegisterTick("TongChallengeStartTick", ChallengeStartTick, MakeListLength * 1000)
	
	local function BattleCountCallBack(Flag, resTbl)
		if Flag then
			for _, tbl in pairs(resTbl) do
				local nPlayerId = tbl[1]
				local nServerId = tbl[2]
				if nServerId == g_CurServerId then
					local Player = g_GetPlayerInfo(nPlayerId)
					if Player and Player.m_Conn then
						SysMovementNotifyToClient(Player.m_Conn, 1007)
					end
				else
					Gas2GasAutoCall:SendTongMessage(GetServerAutoConn(nServerId), nPlayerId, 1007)
				end
			end
		end
	end
	
	local parameter = {}
	parameter["uServerId"] = g_CurServerId
	
	local chanelId = 100
	CallDbTrans("GasTongWarDB", "GetTongChallengeBattleCount", BattleCountCallBack, parameter, chanelId)
end

function GetOffsetSecond(strTime)
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

function InitTongChallenge()
	CallDbTrans("GasTongWarDB", "ClearSelfServerChallengeBattleInfo", nil, {["uServerId"] = g_CurServerId}, g_CurServerId)
end

g_AlarmClock:AddTask("确定帮会对战表", ChallengeStartTime, MakeChlgList, nil, nil)

function Gac2Gas:TongFetchBuilding(Conn)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	local Scene = Player.m_Scene
	local SceneId = Scene.m_SceneId
	
	if uTongID == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	
	if uTongID ~= Scene.m_ChlgerTongId and uTongID ~= Scene.m_TgtTongId then
		MsgToConn(Conn, 9413)
		return
	end
	
	local Camp = Player:CppGetCamp()
	
	local GetItemTbl = BuildItem[Camp]
	
	local function CallBack(result1,result2)
		if IsCppBound(Conn) and IsServerObjValid(Conn.m_Player) then
			local Player = Conn.m_Player
			if not result1 then
				if result2 == 3 then
					MsgToConn(Conn, 8003, '包裹空间不足')
					return
				end
			end
			
			if result2 then
				for i,itemtbl in pairs(result2) do
					CRoleQuest.add_item_DB_Ret(Player, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
				end
			end
			
			if not Scene.m_ChallengePlayerTbl[uTongID] then
				Scene.m_ChallengePlayerTbl[uTongID] = {}
			end
			Scene.m_ChallengePlayerTbl[uTongID][uPlayerID] = true
		end
	end
	local data = {}
	data["PlayerId"] = Conn.m_Player.m_uID
	data["AddItemsTbl"] = GetItemTbl
	data["sceneName"]	= Scene.m_SceneName
	data["createType"]	= event_type_tbl["帮会挑战领取挑战建筑"]
	CallAccountManualTrans(Conn.m_Account, "CharacterMediatorDB", "GetItems", CallBack, data)
end

function Gac2Gas:ExitTongChallenge(Conn)
	local player = Conn.m_Player
	if not player then
		return
	end
	player:SetGameCamp(0)
	local uPlayerID = player.m_uID
	local TongID = Conn.m_Player.m_Properties:GetTongID()
	local Scene = g_TongChlgScTbl[TongID]
	local sceneName = player.m_MasterSceneName
	local pos = player.m_MasterScenePos
	if Scene.m_ChallengePlayerTbl[TongID] and Scene.m_ChallengePlayerTbl[TongID][uPlayerID] then
		Scene.m_ChallengePlayerTbl[TongID][uPlayerID] = nil
		if not next(Scene.m_ChallengePlayerTbl[TongID]) then
			Scene.m_ChallengePlayerTbl[TongID] = nil
		end
	end
	DeleteGameItem(uPlayerID)
	ChangeSceneByName(player.m_Conn, sceneName, pos.x, pos.y)
	Gas2Gac:CloseTongChallengePanel(Conn)
end

function AddKillChallengeBuildNum(KillerID)
	local Entity = CEntityServerManager_GetEntityByID(KillerID)
	if Entity and IsServerObjValid(Entity) then
		if Entity.m_Scene.m_SceneName == ChallengeSceneName then
			local uTongID
			if Entity.m_Properties:GetType() == ECharacterType.Player then
				uTongID = Entity.m_Properties:GetTongID()
			elseif Entity.m_Properties:GetType() == ECharacterType.Npc then
				uTongID = Entity.m_TongID
			end
			
			local scene = g_TongChlgScTbl[uTongID]
			if scene then
				if uTongID == scene.m_ChlgerTongId then
					scene.m_ChlgKillBuildNum = scene.m_ChlgKillBuildNum + 1
				elseif uTongID == scene.m_TgtTongId then
					scene.m_TgtKillBuildNum = scene.m_TgtKillBuildNum + 1
				end
			end
		end
	end
end

function AddKillChallengePersonNum(KillerId)
	local Entity = CEntityServerManager_GetEntityByID(KillerId)
	if Entity and IsServerObjValid(Entity) then
		if Entity.m_Scene.m_SceneName == ChallengeSceneName then
			local uTongID
			if Entity.m_Properties:GetType() == ECharacterType.Player then
				uTongID = Entity.m_Properties:GetTongID()
			elseif Entity.m_Properties:GetType() == ECharacterType.Npc then
				uTongID = Entity.m_TongID
			end
			
			local scene = g_TongChlgScTbl[uTongID]
			if scene then
				if uTongID == scene.m_ChlgerTongId then
					scene.m_ChlgKillPersonNum = scene.m_ChlgKillPersonNum + 1
				elseif uTongID == scene.m_TgtTongId then
					scene.m_TgtKillPersonNum = scene.m_TgtKillPersonNum + 1
				end
			end
		end
	end
end

function ReMoveChallengeObj(IntObj)
	local scene = IntObj.m_Scene
	local objIndex = IntObj.m_ObjIndex
	scene.m_IntObjTbl[objIndex] = nil
end

function RemoveChallengeScene(Scene)
	local ChlgerTongId = Scene.m_ChlgerTongId
	local TgtTongId = Scene.m_TgtTongId
	g_TongChlgScTbl[ChlgerTongId] = nil
	g_TongChlgScTbl[TgtTongId] = nil
end

function ClearTongChallengeTick()
	if TongChallengeStartTick then
		UnRegisterTick(TongChallengeStartTick)
		TongChallengeStartTick = nil
	end
	
	if TongChallengePlayTick then
		UnRegisterTick(TongChallengePlayTick)
		TongChallengePlayTick = nil
	end
	
	if TongChallengeEndTick then
		UnRegisterTick(TongChallengeEndTick)
		TongChallengeEndTick = nil
	end
end

function Gac2Gas:RequestShowTongChallengeBattleCountWnd(Conn)
	local player = Conn.m_Player
	if not player then
		return
	end
	local nSceneID = player.m_Scene.m_SceneId
	if not IsCppBound(Conn) or not IsServerObjValid(player) then
		return
	end
	local nTongID = player.m_Properties:GetTongID()
	if nTongID == 0 then
		return
	end
	Gas2Gac:OpenTongChallengeBattleCountWnd(Conn)
end

function Gac2Gas:GetTongChallengeMemberListInfo(Conn)
	local player = Conn.m_Player
	if not player then
		return
	end
	local nSceneID = player.m_Scene.m_SceneId
	if not IsCppBound(Conn) or not IsServerObjValid(player) then
		return
	end
	local nTongID = player.m_Properties:GetTongID()
	if nTongID == 0 then
		return
	end
	
	local function CallBack(memberTbl)
		Gas2Gac:ReturnTongChallengeMemberListInfoBegin(Conn)
		local row = memberTbl:GetRowNum()
		for index = 1,row  do
			local playerId = memberTbl(index,1)
			local player = g_GetPlayerInfo(playerId)
			if player and IsServerObjValid(player) then
				local sceneId = player.m_Scene.m_SceneId
				if nSceneID == sceneId then
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
						Gas2Gac:ReturnTongChallengeMemberListInfo(Conn, sPlayerName, nLevel, nKillNum, nMaxContiKillNum, nDeathCount)
					end
				end
			end
		end
		Gas2Gac:ReturnTongChallengeMemberListInfoEnd(Conn)
	end
	local parameter = {}
	parameter["uTongID"] = nTongID
	CallAccountAutoTrans(Conn.m_Account, "GasTongBasicDB", "GetAllMemByTongIDRpc", CallBack, parameter, nTongID)
end

local BID_LEVEL_SELF_LINE_2 = 5

function Gac2Gas:JudgeOpenChallenge(Conn)
	local Flag = nil 
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	local tongId = Player.m_Properties:GetTongID()
	local data = {}
	data["tongId"] = tongId
	local function CallBack(result1, result2)
		if not result1 or not result2 then
			return
		end
		local tongLevel = result1
		if tongLevel >= BID_LEVEL_SELF_LINE_2 then
			Flag = true
		else
			Flag = false
		end
		Gas2Gac:ReturnTongLevelInfo(Conn, Flag)
	end
	CallAccountAutoTrans(Conn.m_Account, "GasTongBasicDB", "GetTongLevelAndHonorById", CallBack, data, PlayerId)
end

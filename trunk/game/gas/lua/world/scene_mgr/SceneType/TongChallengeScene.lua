gas_require "world/scene_mgr/SceneType/TongChallengeSceneInc"
cfg_load "tong/TongStartTime_Common"
gac_gas_require "activity/quest/QuestMgr"

local GameState = 
{
	["eChlgList"] = 1,
	["eReady"] = 2,
	["ePlay"] = 3,
	["eClose"] = 4,
}

local ChallengeStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团挑战开始时间", "OpenTime") --loadstring("return " .. TongStartTime_Common("佣兵团挑战开始时间", "OpenTime"))()
local MakeListLength = TongStartTime_Common("挑战确定对战表时长", "OpenTime") * 60
local ReadyLength = TongStartTime_Common("挑战进场时长", "OpenTime") * 60
local ChallengeLength = TongStartTime_Common("挑战战斗时长", "OpenTime") * 60

local BuildItem = {}
BuildItem[1] = {{39,"艾米挑战副本防御设施"}}
BuildItem[2] = {{39,"神圣挑战副本防御设施"}}
BuildItem[3] = {{39,"修斯挑战副本防御设施"}}

local function DeleteGameItem(playerId, Scene)
	local player = g_GetPlayerInfo(playerId)
	local Camp = Scene.m_Camp
	local itemTbl = BuildItem[Camp]
	local function CallBack(allRes)
	end
	
	local parameter = {}
	parameter["nCharID"] =  playerId
	parameter["itemTbl"] = itemTbl
	parameter["sceneName"] = Scene.m_SceneName
	
	local ChannelID = playerId
	if player and IsCppBound(player) then
		ChannelID = player.m_AccountID
	end
	CallDbTrans("CharacterMediatorDB", "DelActionItem", CallBack, parameter, ChannelID)
end

--初始化帮会挑战副本
function CTongChallengeScene:Init(uSceneId, sSceneName, CoreScene)
	CSceneBase.Init(self, uSceneId, sSceneName, CoreScene)
end


function CTongChallengeScene:JoinScene( Player, sSceneName)
	return sSceneName 
end

function CTongChallengeScene:OnCreate()
	local chlgerTongId = self.uChlgerTongId
	local tgtWarZoneId = self.uTgtWarZoneId
	local tgtStationId = self.uTgtStationId
	local tgtTongId = self.uTgtTongId
	local camp = self.uCamp
	CreateChallengeScene(self, chlgerTongId, tgtWarZoneId, tgtStationId, tgtTongId, camp)
end

function CTongChallengeScene:IsCanDestroy()
	if not self.m_NotCanEnter then
		return false
	end
	return true
end

function CTongChallengeScene:OnDestroy()
	local ChlgerTongID = self.m_ChlgerTongId
	local TgtTongID = self.m_TgtTongId
	local SceneId = self.m_SceneId
	
	--删除临时建筑物品
	if self.m_ChallengePlayerTbl[ChlgerTongID] then
		for playerId in pairs(self.m_ChallengePlayerTbl[ChlgerTongID]) do
			DeleteGameItem(playerId, self)
			self.m_ChallengePlayerTbl[ChlgerTongID][playerId] = nil
		end
		self.m_ChallengePlayerTbl[ChlgerTongID] = nil
	end
	if self.m_ChallengePlayerTbl[TgtTongID] then
		for playerId in pairs(self.m_ChallengePlayerTbl[TgtTongID]) do
			DeleteGameItem(playerId, self)
			self.m_ChallengePlayerTbl[TgtTongID][playerId] = nil
		end
		self.m_ChallengePlayerTbl[TgtTongID] = nil
	end
	self.m_ChallengePlayerTbl = nil
	
	--清理建筑Npc的引用
	if self.m_TongBuildingTbl[ChlgerTongID] then
		for index in pairs(self.m_TongBuildingTbl[ChlgerTongID]) do
			self.m_TongBuildingTbl[ChlgerTongID][index] = nil
		end
		self.m_TongBuildingTbl[ChlgerTongID] = nil
	end
	if self.m_TongBuildingTbl[TgtTongID] then
		for index in pairs(self.m_TongBuildingTbl[TgtTongID]) do
			self.m_TongBuildingTbl[TgtTongID][index] = nil
		end
		self.m_TongBuildingTbl[TgtTongID] = nil
	end
	self.m_TongBuildingTbl = nil
	
	--清理生命石的引用
	if self.m_TongStoneNpcTbl[ChlgerTongID] then
		for index in pairs(self.m_TongStoneNpcTbl[ChlgerTongID]) do
			self.m_TongStoneNpcTbl[ChlgerTongID][index] = nil
		end
		self.m_TongStoneNpcTbl[ChlgerTongID] = nil
	end
	if self.m_TongStoneNpcTbl[TgtTongID] then
		for index in pairs(self.m_TongStoneNpcTbl[TgtTongID]) do
			self.m_TongStoneNpcTbl[TgtTongID][index] = nil
		end
		self.m_TongStoneNpcTbl[TgtTongID] = nil
	end
	self.m_TongStoneNpcTbl = nil
	
--	for index in pairs(self.m_IntObjTbl) do
--		self.m_IntObjTbl[index] = nil
--	end
--	self.m_IntObjTbl = nil
end

function CTongChallengeScene:OnPlayerChangeOut(Player)
	LeaveTongChallengeScene(Player, self.m_SceneId)
end


function CTongChallengeScene:OnPlayerChangeIn(Player)
	IntoTongChallengeScene(Player)
	CScenePkMgr.ChangePkState(Player)
end

function CTongChallengeScene:OnPlayerLogIn(Player)
	PlayerLoginTongChallenge(Player)
	CScenePkMgr.ChangePkState(Player)
end

function CTongChallengeScene:OnPlayerLogOut(Player)
	PlayerOffLineTongChallengeScene(Player)
end


function ChangeTongChallengeScene(Conn, ChlgerTongId, tgtTongId, tgtWarZoneId, tgtStationId, SceneId, ServerId, nState, nCamp)
	local tongId = Conn.m_Player.m_Properties:GetTongID()
	if tongId == 0 then
		return
	end
	
	if SceneId == 0 then
		return
	end
	
	local pos
	if tongId == ChlgerTongId then
		pos = g_TongChallengeFbMgr:GetChallengerEnterPos()
	elseif tongId == tgtTongId then
		pos = g_TongChallengeFbMgr:GetTargetEnterPos()
	end
	local charId = Conn.m_Player.m_uID
	MultiServerChangeScene(Conn, SceneId, ServerId, pos[1], pos[2])
end

--进入帮会挑战副本
function Gac2Gas:EnterTongChallengeArea(Conn, npcId)
	if not CheckAllNpcFunc(Conn.m_Player, npcId, "佣兵团挑战进场处") then
		return
	end
	
	local TongID = Conn.m_Player.m_Properties:GetTongID()
	
	if TongID == 0 then
		MsgToConn(Conn, 9401)
		return
	end

	local function CallBack(Flag, MsgID, ChlgerTongId, tgtTongId, tgtWarZoneId, tgtStationId, SceneId, ServerId, nState, nCamp)
		if Flag then
			if nState == GameState["eChlgList"] then
				MsgToConn(Conn, 9414)
				return
			elseif nState == GameState["eClose"] then
				if g_ChallengeCurrentState == GameState["eClose"] then
					MsgToConn(Conn, 9414)
					return
				else
					MsgToConn(Conn, 9422)
					return
				end
			end
			ChangeTongChallengeScene(Conn, ChlgerTongId, tgtTongId, tgtWarZoneId, tgtStationId, SceneId, ServerId, nState, nCamp)
		else
			if IsNumber(MsgID) then
				MsgToConn(Conn, MsgID)
				return
			end
		end
	end
	
	if g_ChallengeCurrentState == GameState["eClose"] then
		MsgToConn(Conn, 9414)
		return
	else
		local parameter = {}
		parameter["uPlayerID"] = Conn.m_Player.m_uID
		parameter["uTongID"] = TongID
		CallDbTrans("GasTongWarDB", "GetTongChallengeBattleInfo", CallBack, parameter, TongID)
	end
end

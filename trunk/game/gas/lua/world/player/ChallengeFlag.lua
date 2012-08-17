cfg_load "player/ChallengeLimitScene_Server"
lan_load "appellation/Lan_AppellationText_Common"

CChanllengeFlagMgr = class()

local challengeCostTime = {}

local function GetArrowHeadFromPK(Player,Obj)
	local ObjPos = CFPos:new()
	Obj:GetPixelPos(ObjPos)
	Gas2Gac:SetCompassHeadDir(Player.m_Conn,1,ObjPos.x,ObjPos.y)
end

local function CheckSceneLimit(Player)
	local Scene = Player.m_Scene
	local SceneName = Scene.m_SceneName
	local Camp = Scene.m_SceneAttr.Camp
	if Player:CppGetCamp() ~= Camp then
		return 1
	end
	local LimitInfo = ChallengeLimitScene_Server(SceneName)
	if LimitInfo then
		if LimitInfo("AreaName") ~= "" then
			local AreaNameTbl = GetCfgTransformValue(false, "ChallengeLimitScene_Server", SceneName, "AreaName")
			local pos = CPos:new()
			Player:GetGridPos(pos)
			local AreaName = g_AreaMgr:GetSceneAreaName(SceneName, pos)
			for i, Area in pairs(AreaNameTbl) do
				if Area == AreaName then
					return 2
				end
			end
		else
			return 2
		end
	end
end

local function SetDuelWinCount(Obj, Content, Count)
	if Content == nil then
		Obj.m_Properties:SetDuelWinCount(" ")
	else
		Content = Lan_AppellationText_Common(MemH64(Content),"Content") 
		Obj.m_Properties:SetDuelWinCount(Content.."("..Count..")")
	end
end
----------------------------------------------------------------------------
--@brief 创建挑战旗
function CChanllengeFlagMgr.CreateChallengeFlag(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	if Player.m_ChallengeFlag then
		return
	end
	
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) then
		MsgToConn(Conn, 193502)
		return
	end
	
	if Player.m_ChallengeObj then
		MsgToConn(Conn, 193502)
		return
	end
	
	if Player.m_ChallengeTick then
		MsgToConn(Conn, 193502)
		return
	end
	
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	if IsServerObjValid(Truck) then
		MsgToConn(Conn, 193524)
		return
	end
	
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		MsgToConn(Conn, 193501)
		return
	end
	
	local limit = CheckSceneLimit(Player)
	if IsNumber(limit) then
		if limit == 1 then
			MsgToConn(Conn, 193518)
		else
			MsgToConn(Conn, 193515)
		end
		return
	end
	
	local function Callback(DuelWinCount,Content)
		if not IsCppBound(Player) then
			return
		end
		if not IsNumber(DuelWinCount) then
			return
		end

		local pos = GetCreatePos(Player)
		pos = RandomCreatPos(pos,1)
		local Obj = CreateIntObj(Player.m_Scene,pos,"挑战旗")
		if Obj == nil then
			return
		end
		Player.m_ChallengeFlag = Obj
		Obj.m_Creator = Player
		SetDuelWinCount(Obj, Content, DuelWinCount)
		Obj.m_Properties:SetCreatorName(Player.m_Properties:GetCharName())
		
		GetArrowHeadFromPK(Player,Obj)
		Gas2Gac:SetChallengeFlag(Player.m_Conn, true)
	end
	
	local data = {}
	data["char_id"] = Player.m_uID
	data["char_level"] = Player:CppGetLevel()
	CallAccountManualTrans(Player.m_Conn.m_Account, "ChallengeFlagDB", "QueryDuelWinCount", Callback, data)
end
----------------------------------------------------------------
--销毁Obj及小箭头特效
local function DestroyChallengeFlag(Player)
	if IsCppBound(Player) and IsServerObjValid(Player.m_ChallengeFlag) then
		local Obj = Player.m_ChallengeFlag
		if Obj.m_Challenger then
			Obj.m_Challenger.m_ChallengeObj = nil
			Gas2Gac:CancelChallengeWnd(Obj.m_Challenger.m_Conn, false)
			MsgToConn(Obj.m_Challenger.m_Conn, 193503, Player.m_Properties:GetCharName())
		end
		g_IntObjServerMgr:Destroy(Obj,false)
		Player.m_ChallengeFlag = nil
		Gas2Gac:SetChallengeFlag(Player.m_Conn, false)
	end
end

--@brief 销毁Obj及小箭头特效
function CChanllengeFlagMgr.DestroyChallengeFlag(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) then
		MsgToConn(Conn, 193502)
		return
	end
	
	if Player.m_ChallengeObj then
		MsgToConn(Conn, 193502)
		return
	end
	
	if Player.m_ChallengeTick then
		MsgToConn(Conn, 193502)
		return
	end
	
	if not Player.m_ChallengeFlag then
		return 
	end
	
	DestroyChallengeFlag(Player)
end

local function CancelChallengeRequest(Player)
	local Obj = Player.m_ChallengeObj
	local Creator = Obj.m_Creator
	Player.m_ChallengeObj = nil
	Gas2Gac:CancelChallengeWnd(Player.m_Conn, false)
	Gas2Gac:CancelChallengeWnd(Creator.m_Conn, true)
	if Creator.m_ChallengeTick then
		UnRegisterTick(Creator.m_ChallengeTick)
		Creator.m_ChallengeTick = nil
	end
	MsgToConn(Creator.m_Conn, 193503, Player.m_Properties:GetCharName())
	Obj.m_Challenger = nil
end
	
function CChanllengeFlagMgr.JudgeDistance(Player)
	if not IsCppBound(Player) then 
		return
	end

	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) then
		local GameCamp = Player:CppGetGameCamp()
		local Scene = Player.m_Scene
		if GameCamp < 0 then
			GameCamp = -GameCamp
		end
		local tbl = Scene.m_GameCampTbl[GameCamp]
		local Defender, Challenger = tbl[1], tbl[2]
		if Defender == Player then
			if IsCppBound(Challenger) then
				Challenger:OnFinishDuel(true)
			end
		else
			if IsCppBound(Defender) then
				Defender:OnFinishDuel(true)
			end
		end
		if Defender.m_ChallengeTick then
			UnRegisterTick(Defender.m_ChallengeTick)
			Defender.m_ChallengeTick = nil
			Defender.m_ChallengeFlag.m_Challenger = nil
			Challenger.m_ChallengeObj = nil
		end
		Player:OnFinishDuel(false)
	elseif Player.m_ChallengeFlag then
		if Player.m_ChallengeTick then
			UnRegisterTick(Player.m_ChallengeTick)
			Player.m_ChallengeTick = nil
		end
		DestroyChallengeFlag(Player)
	elseif Player.m_ChallengeObj then
		CancelChallengeRequest(Player)
	end
end

function CChanllengeFlagMgr.SendChallengeMessage(Conn, EntityID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	if IsServerObjValid(Truck) then
		Gas2Gac:CancelChallengeWnd(Conn, false)
		MsgToConn(Conn, 193525)
		return
	end
	
	local Obj = CIntObjServer_GetIntObjServerByID(EntityID)
	if not IsCppBound(Obj) then
		Gas2Gac:CancelChallengeWnd(Conn, false)
		return
	end
	if IsCppBound(Obj.m_Challenger) then
		Gas2Gac:CancelChallengeWnd(Conn, false)
		MsgToConn(Conn, 193508, Obj.m_Creator.m_Properties:GetCharName(), Obj.m_Challenger.m_Properties:GetCharName())
		return
	end
	if Player.m_ChallengeFlag then
		DestroyChallengeFlag(Player)
	end
	Obj.m_Challenger = Player
	Player.m_ChallengeObj = Obj
	GetArrowHeadFromPK(Player,Obj)
	local PlayerLevel = Player:CppGetLevel()
	local minLevelKeys = DuelContWinAppellation_Common:GetKeys()
	for _, minLevel in pairs(minLevelKeys) do
		if PlayerLevel >= tonumber(minLevel) then
			local maxLevelKeys = DuelContWinAppellation_Common(minLevel):GetKeys()
			for _, maxLevel in pairs(maxLevelKeys) do
				if PlayerLevel <= tonumber(maxLevel) then
					Gas2Gac:PropChallengeAdvertMessage(Obj.m_Creator.m_Conn, Player:GetEntityID(),Player.m_Properties:GetCharName(),PlayerLevel,tonumber(minLevel),tonumber(maxLevel))
					return
				end
			end
		end
	end
	Gas2Gac:PropChallengeMessage(Obj.m_Creator.m_Conn, Player:GetEntityID(),Player.m_Properties:GetCharName())
end

--挑战开始
local function StartBattle(Player, Challenger)
	Player:OnStartDuel(Challenger)
	Challenger:OnStartDuel(Player)
	Player:ChangeChallengeState(true)
	Challenger:ChangeChallengeState(true)
	Gas2Gac:UpdateHeadInfoByEntityID(Player.m_Conn, Player:GetEntityID())
	Gas2Gac:UpdateHeadInfoByEntityID(Challenger.m_Conn, Challenger:GetEntityID())
	Gas2Gac:ExposePlayer(Player.m_Conn, Challenger:GetEntityID())
	Gas2Gac:ExposePlayer(Challenger.m_Conn, Player:GetEntityID())
end

function CChanllengeFlagMgr.StartChallenge(Conn, isAccept)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local Obj = Player.m_ChallengeFlag
	if not Obj then
		return
	end
	
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	if IsServerObjValid(Truck) then
		MsgToConn(Conn, 193525)
		return
	end
	
	local Challenger = Obj.m_Challenger
	
	if not IsCppBound(Challenger) then
		MsgToConn(Player.m_Conn, 193506)
		Obj.m_Challenger = nil
		return
	end
	Gas2Gac:CancelChallengeWnd(Challenger.m_Conn, isAccept)
	if not isAccept then
		MsgToConn(Challenger.m_Conn, 193504, Player.m_Properties:GetCharName())
		Challenger.m_ChallengeObj = nil
		Obj.m_Challenger = nil
		return
	end
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		Challenger.m_ChallengeObj = nil
		Obj.m_Challenger = nil
		MsgToConn(Conn, 193501)
		MsgToConn(Challenger.m_Conn, 193519, Player.m_Properties:GetCharName())
		Gas2Gac:CancelChallengeWnd(Challenger.m_Conn, false)
		return
	elseif Challenger:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		Challenger.m_ChallengeObj = nil
		Obj.m_Challenger = nil
		MsgToConn(Challenger.m_Conn, 193501)
		MsgToConn(Conn, 193519, Challenger.m_Properties:GetCharName())
		Gas2Gac:CancelChallengeWnd(Challenger.m_Conn, false)
		return
	end 
	
	local GameCamp = g_GameCampMgr:AssignCamp(Player.m_Scene, {Player,Challenger})
	if not GameCamp then
		Gas2Gac:CancelChallengeWnd(Challenger.m_Conn, false)
		Challenger.m_ChallengeObj = nil
		return
	end

	Player:SetGameCamp(GameCamp)
	Challenger:SetGameCamp(-GameCamp)
	StartBattle(Player, Challenger)
	
	local CountDown = 5
	local function tickfun(Tick, Player, Challenger)
		CountDown = CountDown - 1
		if CountDown <= 0 then
			UnRegisterTick(Player.m_ChallengeTick)
			Player.m_ChallengeTick = nil
			Obj.m_Challenger.m_ChallengeObj = nil
			Obj.m_Challenger = nil
			if IsCppBound(Challenger) then
				Challenger:SetGameCamp(GameCamp)
			end
			challengeCostTime[GameCamp] = os.time()
			MsgToConn(Player.m_Conn, 193522)
			MsgToConn(Challenger.m_Conn, 193522)
			return
		end
		MsgToConn(Player.m_Conn, 193505, CountDown)
		MsgToConn(Challenger.m_Conn, 193505, CountDown)
	end
	MsgToConn(Player.m_Conn, 193505, CountDown)
	MsgToConn(Challenger.m_Conn, 193505, CountDown)
	Player.m_ChallengeTick = RegisterTick("ChallengeTick", tickfun, 1000, Player, Challenger)
end

local function SaveDuelWinCountToDB(Player, Challenger, DuelWinCount)
	if not IsCppBound(Player) or not IsCppBound(Player.m_Conn) then
		return
	end 
	local camp = Player:CppGetGameCamp()
	local playerInfo = GetPropertyValue(Player)
	local challengerInfo = GetPropertyValue(Challenger)
	local function callback(DuelCount,Content,succ,appellationId)
		challengeCostTime[camp] = nil
		if IsCppBound(Player) then
			if IsServerObjValid(Player.m_ChallengeFlag) then
				if Content then
					local ChallengeFlag = Player.m_ChallengeFlag
					SetDuelWinCount(ChallengeFlag, Content, DuelCount)
					Gas2Gac:UpdateHeadInfoByEntityID(ChallengeFlag:GetIS(0), ChallengeFlag:GetEntityID())
				end
			end
			if succ then
				Gas2Gac:ReturnAddNewAppellation(Player.m_Conn,appellationId)
				Content = Lan_AppellationText_Common(MemH64(Content),"Content")
				MsgToConn(Player.m_Conn,37,Content )
			end
		end
	end	
	local data = {}
	data["DuelWinCount"]	= DuelWinCount
	data["char_id"]				= Player.m_uID
	data["challengerId"]	= Challenger.m_uID
	data["char_level"]		= Player:CppGetLevel()
	data["uFlagId"] = Player.m_ChallengeFlag:GetEntityID()
	data["playerFightingTotalPoint"] = Player.m_uFightingTotalPoint
	data["challengerFightingTotalPoint"] = Challenger.m_uFightingTotalPoint
	local time = challengeCostTime[camp]
	if time then
		data["time"] = os.time() - time
	else
		data["time"] = 0
	end
	data["playerInfo"] = playerInfo
	data["challengerInfo"] = challengerInfo
	CallAccountManualTrans(Player.m_Conn.m_Account, "ChallengeFlagDB", "UpdateDuelWinCount", callback, data)
end

local function PlayerShowEffect(Player)
	Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create")
	Gas2Gac:UseItemTargetEffect(Player:GetIS(0),"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create", Player:GetEntityID())
end

--回收玩法阵营
local function ResumeCamp(Scene, camp)
	if Scene.m_GameCampTbl and Scene.m_GameCampTbl[camp] then
		Scene.m_GameCampTbl[camp] = nil
	end
end

--挑战结束 判定
function CChanllengeFlagMgr.ChallengeLoserSendInform(Player)
	local GameCamp = Player:CppGetGameCamp()
	local Scene = Player.m_Scene
	if GameCamp < 0 then
		GameCamp = -GameCamp
	end
	if Scene.m_GameCampTbl == nil or Scene.m_GameCampTbl[GameCamp] == nil then
		local CoreScene = Scene.m_CoreScene
		local IsDestory = CoreScene:Destroying()
		if IsDestory then
			LogErr("挑战结束判定出现异常","场景"..Scene.m_SceneName.."正在析构")
		else
			LogErr("挑战结束判定出现异常","场景名:"..Scene.m_SceneName.." 场景ID:"..Scene.m_SceneId.." 玩法阵营:"..GameCamp.." 玩家ID:"..Player.m_uID)
		end
		return
	end
	local tbl = Scene.m_GameCampTbl[GameCamp]
	local Defender, Challenger = tbl[1], tbl[2]
	local ChallengeFlag = Defender.m_ChallengeFlag
	if Player == Defender then
		SaveDuelWinCountToDB(Defender, Challenger, 0)
		DestroyChallengeFlag(Player)
		Challenger:ChangeChallengeState(false)
		PlayerShowEffect(Challenger)
		MsgToConn(Challenger.m_Conn, 193512)
	else
		if Defender.m_ChallengeFlag and Defender.m_ChallengeFlag.m_AddCount then
			SaveDuelWinCountToDB(Defender, Challenger, 1)
		else
			MsgToConn(Defender.m_Conn, 193523)
		end
		Defender:ChangeChallengeState(false)
		PlayerShowEffect(Defender)
		MsgToConn(Defender.m_Conn, 193512)
	end
	MsgToConn(Player.m_Conn, 193513)
	Gas2Gac:ClearCompassHeadDir(Challenger.m_Conn)
	Player:ChangeChallengeState(false)
	Gas2Gac:UpdateHeadInfoByEntityID(Defender.m_Conn, Defender:GetEntityID())
	Gas2Gac:UpdateHeadInfoByEntityID(Challenger.m_Conn, Challenger:GetEntityID())
	
	ResumeCamp(Scene, GameCamp)
end

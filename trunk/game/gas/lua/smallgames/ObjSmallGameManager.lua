gas_require "framework/main_frame/SandBoxDef"

local CheckMgrTickTbl = {}
local g_GetDistance = g_GetDistance
local g_QuestNeedMgr = g_QuestNeedMgr
local g_ObjActionArgTbl = g_ObjActionArgTbl
local MsgToConn = MsgToConn
local CallAccountAutoTrans = CallAccountAutoTrans
local SmallGame_Common = SmallGame_Common
local CEntityServerManager_GetEntityByID = CEntityServerManager_GetEntityByID
local EFighterCtrlState = EFighterCtrlState
local IsCppBound = IsCppBound
local IsServerObjValid = IsServerObjValid
--local loadstring = loadstring
local os = os
local g_GetPlayerInfo = g_GetPlayerInfo
local g_SmallGameStrTbl = g_SmallGameStrTbl
local Entry  = CreateSandBox(...)


local function CheckUserOnline1(Tick, data)
	local PlayerId, GlobalID = data[1],data[2]
	local Obj = CEntityServerManager_GetEntityByID(GlobalID)
	if Obj == nil then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end

	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		Obj.m_UserId = nil
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end
	
	if Player.m_IsGameOver then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		Player.m_IsGameOver = nil
		return
	end
	
	if Obj.m_UserId ~= PlayerId then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end
	
	if Player:IsMoving() or not Player:CppIsLive() 
		or Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		if Obj.m_SmallGameTick then
			UnRegisterTick(Obj.m_SmallGameTick)
			Obj.m_SmallGameTick = nil
		end
		Obj.m_UserId = nil
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		Gas2Gac:ExitSmallGame(Player.m_Conn)
		return
	end
end

local function CheckUserOnline2(Tick, data)
	local PlayerId, GlobalID = data[1],data[2]
	local Obj = CEntityServerManager_GetEntityByID(GlobalID)
	if Obj == nil then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end

	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		Obj.m_UserId = nil
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end
	
	if Player.m_IsGameOver then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		Player.m_IsGameOver = nil
		return
	end
	
	if Player:IsMoving() or not Player:CppIsLive() 
		or Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		if Obj.m_SmallGameTick then
			UnRegisterTick(Obj.m_SmallGameTick)
			Obj.m_SmallGameTick = nil
		end
		Obj.m_UserId = nil
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		Gas2Gac:ExitSmallGame(Player.m_Conn)
		return
	end
end

function Entry.Exec(Conn, Obj, ObjName, GlobalID)
	if g_GetDistance(Conn.m_Player,Obj) > 6 then --判断距离
		return
	end

	if g_ObjActionArgTbl[ObjName] then
		local SmallGameName = g_ObjActionArgTbl[ObjName][1]
		if SmallGameName then
			Entry.BeginSmallGame(Conn, Obj,SmallGameName)
		end
	end
end

local function ClearTargetInfo(Tick, data)
	local PlayerId,Obj = data[1],data[2]

	if Obj then
		if Obj.m_SmallGameTick then
			UnRegisterTick(Obj.m_SmallGameTick)
			Obj.m_SmallGameTick = nil
		end
		Obj.m_UserId = nil
	end

	local Player = g_GetPlayerInfo(PlayerId)
	if Player then
		Gas2Gac:CloseSmallGameMsgWnd(Player.m_Conn)
	end
end

local function CheckTargetInfo(Player,Obj,CDTime,IsLock)
	if not IsServerObjValid(Obj) then
		return true
	end
	local ObjId = Obj:GetEntityID()
	
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_OnMission) then
		return true
	end
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		MsgToConn(Player.m_Conn, 3252)
		return true
	else
		if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) or Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse) then--骑乘
			Player:PlayerDoFightSkillWithoutLevel("下马")
		end
	end
	
	--检测目标是不是正在被使用	
	if Obj.m_UserId then
		local Target = g_GetPlayerInfo(Obj.m_UserId)
		if IsCppBound(Target) and Target.m_IsInToTargetID == ObjId then
			if Obj.m_UserId ~= Player.m_uID then
				if g_GetDistance(Target,Obj)<=5 then --判断距离
					MsgToConn(Player.m_Conn,3253)--目标正在被别人使用中..
					return true
				end
			else
				return true
			end
		end
	end
	
	if Obj.m_CoolTime and (os.time()-Obj.m_CoolTime) <= CDTime then
		MsgToConn(Player.m_Conn, 3254)
		return true
	end
	
	if not IsLock or IsLock ~= 0 then
		Obj.m_UserId = Player.m_uID --添加标志
		Player.m_IsInToTargetID = ObjId
		
		if Obj.m_SmallGameTick then
			UnRegisterTick(Obj.m_SmallGameTick)
			Obj.m_SmallGameTick = nil
		end
		local data = {Player.m_uID,Obj}
		Obj.m_SmallGameTick = RegisterTick("SmallGameTick",ClearTargetInfo,30*1000,data)
	end
	
	return false
end

function Entry.BeginSmallGame(Conn, Obj,SmallGameName)
	local Player = Conn.m_Player
		
	local GameCommon = SmallGame_Common(SmallGameName)
	if not GameCommon then
		return
	end
	
	local PlayerId = Player.m_uID
	local GlobalID = Obj:GetEntityID()
	
	local QuestName = GameCommon("QuestName")
	local ResultTab = g_SmallGameStrTbl[SmallGameName].QuestResult --GameCommon.QuestResult
	local QuestType = GameCommon("QuestType")
	
	local QuestVar = nil
	if GameCommon("QuestType") == "剧情需求" then
		QuestVar = ResultTab[1]
	elseif GameCommon("QuestType") == "物品需求" then
		QuestVar = ResultTab[2]
	end
	
	local IsLock = GameCommon("TargetIsLock")
	------------------------------------------------------------------
	local function StartGame(Obj,Player,SmallGameName)
		-------------------
		--判断相关信息
		if CheckTargetInfo(Player,Obj,GameCommon("CoolTime"),IsLock) then
			return
		end
		-------------------
		if CheckMgrTickTbl[GlobalID] then
			UnRegisterTick(CheckMgrTickTbl[GlobalID])
			CheckMgrTickTbl[GlobalID] = nil 
		end
		local data = {PlayerId, GlobalID}
		
		Player.m_IsGameOver = nil
		if not IsLock or IsLock ~= 0 then
			CheckMgrTickTbl[GlobalID] =  RegisterTick("ObjCheckUserOnline1", CheckUserOnline1, 200, data)
		else
			CheckMgrTickTbl[GlobalID] =  RegisterTick("ObjCheckUserOnline2", CheckUserOnline2, 200, data)
		end
		Gas2Gac:RetMsgBeginSmallGame(Player.m_Conn,SmallGameName,GlobalID)
		Player.m_SmallGameName = SmallGameName
	end
	------------------------------------------------------------------
	
	if QuestName and QuestName ~= "" then
		----------------------------------------------
		local function CallBack(NeedNum)
			local Obj = CEntityServerManager_GetEntityByID(GlobalID)
			if not NeedNum or not IsCppBound(Player) then
				return
			end
			
			if Obj then
				StartGame(Obj,Player,SmallGameName)
			end
		end
		----------------------------------------------
		local QuestVar = nil
		if GameCommon("QuestType") == "剧情需求" then
			QuestVar = ResultTab[1]
		elseif GameCommon("QuestType") == "物品需求" then
			QuestVar = ResultTab[2]
		end
		if QuestType == "剧情需求" then
			local QuestVar = ResultTab[1]
			if QuestVar and g_QuestNeedMgr[QuestName] and g_QuestNeedMgr[QuestName][QuestVar] then
				local data =
				{
					["sQuestName"] = QuestName,
					["sVarName"] = QuestVar,
					["uCharId"] = PlayerId
				}
				CallAccountAutoTrans(Conn.m_Account,"RoleQuestDB","CheckQuestVarNeedNum", CallBack, data)
			else
				StartGame(Obj,Player,SmallGameName)
			end
		elseif QuestType == "物品需求" then
			local ItemType = ResultTab[1]
			local NeedItem = ResultTab[2]
			if NeedItem and g_QuestNeedMgr[QuestName] and g_QuestNeedMgr[QuestName][NeedItem] then
				local data =
				{
					["sQuestName"] = QuestName,
					["iItemType"] = ItemType,
					["sItemName"] = NeedItem,
					["uCharId"] = PlayerId
				}
				CallAccountAutoTrans(Conn.m_Account,"RoleQuestDB","CheckQuestItemNeedNum", CallBack, data)
			else
				StartGame(Obj,Player,SmallGameName)
			end
		else
			local data = 
			{
				["sQuestName"] = QuestName,
				["uCharId"] = PlayerId
			}
			CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB","CheckQuestSatisfy", CallBack, data)
		end
	else
		StartGame(Obj,Player,SmallGameName)
	end
end

return Entry
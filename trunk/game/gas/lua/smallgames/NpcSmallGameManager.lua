local CheckMgrTickTbl = {}

local function CheckUserOnline1(Tick, data)
	local PlayerId, GlobalID = data[1],data[2]
	local TiggerCage = CCharacterDictator_GetCharacterByID(GlobalID)
	if TiggerCage == nil then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end

	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		TiggerCage.m_UserId = nil
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
	
	if TiggerCage.m_UserId ~= PlayerId then
		UnRegisterTick(CheckMgrTickTbl[GlobalID])
		CheckMgrTickTbl[GlobalID] = nil 
		return
	end
	
	if Player:IsMoving() or not Player:CppIsLive() 
		or Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		if TiggerCage.m_SmallGameTick then
			UnRegisterTick(TiggerCage.m_SmallGameTick)
			TiggerCage.m_SmallGameTick = nil
		end
		TiggerCage.m_UserId = nil
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

local function ClearTargetInfo(Tick, data)
	local PlayerId,Npc = data[1],data[2]
	if Npc then
		if Npc.m_SmallGameTick then
			UnRegisterTick(Npc.m_SmallGameTick)
			Npc.m_SmallGameTick = nil
		end
		Npc.m_UserId = nil
	end
	local Player = g_GetPlayerInfo(PlayerId)
	if Player then
		Gas2Gac:CloseSmallGameMsgWnd(Player.m_Conn)
	end
end

local function CheckTargetInfo(Player,Npc,CDTime,IsLock)
	if not IsServerObjValid(Npc) then
		return true
	end
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_OnMission) then
		return true
	end
	
	local NpcId = Npc:GetEntityID()
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		MsgToConn(Player.m_Conn, 3252)
		return true
	else
		if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) or Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse) then--骑乘
			Player:PlayerDoFightSkillWithoutLevel("下马")
		end
	end
	
	--检测目标是不是正在被使用	
	if Npc.m_UserId then
		local Target = g_GetPlayerInfo(Npc.m_UserId)
		if IsCppBound(Target) and Target.m_IsInToTargetID == NpcId then
			if Npc.m_UserId ~= Player.m_uID then
				if g_GetDistance(Target,Npc)<=5 then --判断距离
					MsgToConn(Player.m_Conn,3253)--目标正在被别人使用中..
					return true
				end
			else
				return true
			end
		end
	end
	
	if Npc.m_CoolTime and (os.time()-Npc.m_CoolTime) <= CDTime then
		MsgToConn(Player.m_Conn, 3254)
		return true
	end
	
	if not IsLock or IsLock ~= 0 then
		Npc.m_UserId = Player.m_uID --添加标志
		Player.m_IsInToTargetID = NpcId
		
		if Npc.m_SmallGameTick then
			UnRegisterTick(Npc.m_SmallGameTick)
			Npc.m_SmallGameTick = nil
		end
		local data = {Player.m_uID,Npc}
		Npc.m_SmallGameTick = RegisterTick("SmallGameTick",ClearTargetInfo,30*1000,data)
	end
	
	return false
end

local function BeginSmallGame(Npc,Conn,SmallGameName)
	local Player = Conn.m_Player
	
	local GameCommon = SmallGame_Common(SmallGameName)
	if not GameCommon then
		return
	end
	
	local PlayerId = Player.m_uID
	local GlobalID = Npc:GetEntityID()
	
	local QuestName = GameCommon("QuestName")
	local ResultTab = g_SmallGameStrTbl[SmallGameName].QuestResult --GameCommon.QuestResult
	
	local IsLock = GameCommon("TargetIsLock")
	---------------------------------------------------------
	local function StartGame(Npc,Player,SmallGameName)
		-------------------
		--判断相关信息
		if CheckTargetInfo(Player,Npc,GameCommon("CoolTime"),IsLock) then
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
			CheckMgrTickTbl[GlobalID] =  RegisterTick("NpcCheckUserOnline1", CheckUserOnline1, 200, data)
		else
			CheckMgrTickTbl[GlobalID] =  RegisterTick("NpcCheckUserOnline2",CheckUserOnline2,200,data)
		end
		Gas2Gac:RetMsgBeginSmallGame(Player.m_Conn,SmallGameName,GlobalID)
		Player.m_SmallGameName = SmallGameName
	end
	----------------------------------------------------------
	
	if QuestName and QuestName ~= "" then
		-------------------------------------------
		local function CallBack(NeedNum)
			local Npc = CCharacterDictator_GetCharacterByID(GlobalID)
			if not NeedNum or not IsCppBound(Player) then
				return
			end
			
			if Npc then
				StartGame(Npc,Player,SmallGameName)
			end
		end
		
--		local QuestVar = nil
--		local NeedItem = nil
--		local ItemType = nil
		local QuestType = GameCommon("QuestType")
--		if GameCommon.QuestType == "剧情需求" then
--			QuestVar = ResultTab[1]
--		elseif GameCommon.QuestType == "物品需求" then
--			ItemType = ResultTab[1]
--			NeedItem = ResultTab[2]
--		end
		
		--------------------------------------------
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
				StartGame(Npc,Player,SmallGameName)
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
				StartGame(Npc,Player,SmallGameName)
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
		StartGame(Npc,Player,SmallGameName)
	end
end

local function Exec(Arg, Npc, Player)
	if not IsCppBound(Player) then
		return
	end
	if g_GetDistance(Player,Npc) > 6 then --判断距离
		return
	end
--	local NpcName = Npc.m_Properties:GetCharName()
	local SmallGameName = Arg[1]

--	if NpcName and NpcTriggerEvent_Common[NpcName] then
--		for i=1,table.getn(NpcTriggerEvent_Common[NpcName]) do
--			for _, v in pairs(NpcTriggerEvent_Common[NpcName][i].ScriptArg ) do 
--				if v[1] == "小游戏" then
--					SmallGameName = v[2]
--					break
--				end
--			end
----			SmallGameName = NpcTriggerEvent_Common[NpcName][i].ScriptArg[i]
--			if SmallGameName then
--				break
--			end
--		end
--	end
	
	if SmallGameName ~= nil and SmallGameName ~= "" then
		BeginSmallGame(Npc,Player.m_Conn,SmallGameName)
	end
end

return Exec

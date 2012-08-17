gas_require "log_mgr/CLogMgr"

local DestroyRecreateTickTbl = {}
RegMemCheckTbl("DestroyRecreateTickTbl", DestroyRecreateTickTbl)

--替换NPC++++++++++++++++++++++++
local function DestroyNpc(Npc,GlobalID)
	g_NpcServerMgr:DestroyServerNpcNow(Npc, false)
	if DestroyRecreateTickTbl[GlobalID] then
		UnRegisterTick(DestroyRecreateTickTbl[GlobalID])
		DestroyRecreateTickTbl[GlobalID] = nil
	end
end

local function MoveNPC(Npc,CurrPos)
	local x = math.random(5,10) * (math.random(0,1)*2 - 1)
	local y = math.random(5,10) * (math.random(0,1)*2 - 1)
	local MoveToPos = CFPos:new()
	MoveToPos.x = CurrPos.x + (x + 0.5) * EUnits.eGridSpanForObj
	MoveToPos.y = CurrPos.y + (y + 0.5) * EUnits.eGridSpanForObj
	local step = math.max( ((math.abs( MoveToPos.x - CurrPos.x )+math.abs( MoveToPos.y - CurrPos.y ))/EUnits.eGridSpanForObj),5 )
	Npc:MoveToInAStarPath( MoveToPos, 3, EFindPathType.eFPT_HypoAStar, EBarrierType.eBT_HighBarrier, 0, step)
end

local function DestroyAndRecreateNpc(LockObjID,DestroyName,CreateName)
	if LockObjID and DestroyName and CreateName then
		local Npc = CEntityServerManager_GetEntityByID(LockObjID)
		if Npc then
			Npc.m_UserId = nil
			local NpcName = Npc.m_Properties:GetCharName()
			if (NpcName ~= DestroyName) or (Npc.m_Properties:GetType() ~= ECharacterType.Npc) then
				return
			end
			
			local Scene = Npc.m_Scene
			local Pos = CFPos:new()
			Npc:GetPixelPos(Pos)
			g_NpcServerMgr:DestroyServerNpcNow(Npc, true)
			
			local CreateNpc = g_NpcServerMgr:CreateServerNpc(CreateName,0,Scene,Pos)
			--CreateNpc:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
			if IsServerObjValid(CreateNpc) then
				local CreateNpcID = CreateNpc:GetEntityID()
				
				local index = 0
				local function MoveAndDelNpc(Tick, data)
					local GlobalID, NpcID, Pos = unpack(data)
					local Npc = CEntityServerManager_GetEntityByID(NpcID)
					index = index + 1
					if Npc then
						if index == 2 then
							MoveNPC(Npc,Pos)
						elseif index == 10 then
							DestroyNpc(Npc,GlobalID)
						end
					else
						if DestroyRecreateTickTbl[GlobalID] then
							UnRegisterTick(DestroyRecreateTickTbl[GlobalID])
							DestroyRecreateTickTbl[GlobalID] = nil
						end
					end
				end
				
				local data = {LockObjID, CreateNpcID, Pos}
				DestroyRecreateTickTbl[LockObjID] = RegisterTick("DestroyRecreateTick", MoveAndDelNpc,1000,data)
			end
		end
	end
end
--++++++++++++++++++++++++++++++++++++

--删除锁定的对象++++++++++++++++++++++++
local function OverDestroyTarget(Type,LockObjID,TargetName)
	if LockObjID and TargetName then
		local Target = CEntityServerManager_GetEntityByID(LockObjID)
		if Target then
			Target.m_UserId = nil
			local NpcName = Target.m_Properties:GetCharName()
			local NpcType = Target.m_Properties:GetType()
			if NpcName ~= TargetName then
				return
			end

			if Type == "NPC" and NpcType == ECharacterType.Npc then
				g_NpcServerMgr:DestroyServerNpcNow(Target, true)
			elseif Type == "OBJ" and NpcType == ECharacterType.IntObj then
				g_IntObjServerMgr:Destroy(Target,true)
			end
		end
	end
end
--+++++++++++++++++++++++++++++++

--创建对象
local function OverCreateTarget(PlayerId, Type, LockObjID, TargetName)
	if TargetName and TargetName ~= "" then
		local Target = CEntityServerManager_GetEntityByID(LockObjID)
		
		local Pos = CPos:new()
		local Scene = nil
		if Target then
			Target.m_CoolTime = os.time()
			Target.m_UserId = nil
			Target:GetGridPos(Pos)
			Scene = Target.m_Scene
		else
			local Player = g_GetPlayerInfo(PlayerId)
			if Player then
				Player:GetGridPos(Pos)
				Scene = Player.m_Scene
			else
				return
			end
		end
		
		local CreateTarget = nil
		if Type == "NPC" and Npc_Common(TargetName) then
			local fPos = CFPos:new( Pos.x * EUnits.eGridSpan, Pos.y * EUnits.eGridSpan )
			CreateTarget = g_NpcServerMgr:CreateServerNpc(TargetName,0,Scene,fPos)
		elseif Type == "OBJ" and IntObj_Common(TargetName) then
			CreateTarget = CreateIntObj(Scene, Pos, TargetName, true, PlayerId)
		end
		
		if IsServerObjValid(CreateTarget) then
			CreateTarget.m_OwnerId = PlayerId
		end
	end
end

local function RunScript(PlayerId, ScriptArg, LockObjID)
	if ScriptArg[1] == "替换NPC" then
		DestroyAndRecreateNpc(LockObjID,ScriptArg[2],ScriptArg[3])
	elseif ScriptArg[1] == "删除NPC" then
		OverDestroyTarget("NPC",LockObjID,ScriptArg[2])
	elseif ScriptArg[1] == "删除OBJ" then
		OverDestroyTarget("OBJ",LockObjID,ScriptArg[2])
	elseif ScriptArg[1] == "刷出NPC" then
		OverCreateTarget(PlayerId,"NPC",LockObjID,ScriptArg[2])
	elseif ScriptArg[1] == "刷出OBJ" then
		OverCreateTarget(PlayerId,"OBJ",LockObjID,ScriptArg[2])
	end
end

function Gac2Gas:SmallGameSucceed(Conn, GameName, LockObjID)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local Npc = CEntityServerManager_GetEntityByID(LockObjID)
	if IsCppBound(Npc) then
		Npc.m_SmallGameTime = os.time() - Npc.m_SmallGameTime
	end
	
	if Player.m_IsGameOver then
		return
	end
	
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
	Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
	Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
	Player.m_IsGameOver = true
	Player.m_SetCtrlState = 21
	local GameCommon = SmallGame_Common(GameName)
	if not GameCommon then
		return 
	end
	local PlayerId = Player.m_uID
	
	local QuestName = GameCommon("QuestName")
	local ResultTab = g_SmallGameStrTbl[GameName].QuestResult --GameCommon.QuestResult
	
	local ItemType = nil
	local QuestVar = nil
	local AddVarNum = nil

	local IsQuestAdd = false
	local IsItemAdd = false
	local IsOtherAdd = false
	if GameCommon("QuestType") == "物品需求" then
		IsItemAdd = true
		ItemType = ResultTab[1]
		QuestVar = ResultTab[2]
		AddVarNum = ResultTab[3]
	elseif GameCommon("QuestType") == "剧情需求" then
		IsQuestAdd = true
		QuestVar = ResultTab[1]
		AddVarNum = ResultTab[2]
	elseif GameCommon("QuestType") == "其它需求" then
		IsOtherAdd = true
		AddVarNum = ResultTab[1]
	end
	-------------------------------------
	--如果需要创建
	local QuestArg = g_SmallGameStrTbl[GameName].SuccArg --GameCommon.SuccArg
	
	if next(QuestArg) then
		RunScript(PlayerId,QuestArg,LockObjID)
	else
		--不用创建的,就注册一个冷却时间
		if LockObjID then
			local Npc = CEntityServerManager_GetEntityByID(LockObjID)
			if Npc then
				Npc.m_CoolTime = os.time()
				Npc.m_UserId = nil
			end
		end
	end
	
	Player.m_CallbackHandler:OnSetLockObj(Player:GetEntityID(),0)
	------------------------------------
	
	Gas2Gac:PlayerSmallGameSuccFx(Conn,GameName,Player:GetEntityID())
	Gas2Gac:PlayerSmallGameSuccFx(Player:GetIS(0),GameName,Player:GetEntityID())
	
	--+++++++++++++++++++++++++++++++++++++++++++
	--加物品
	local function CallBack1(itemTbl)
		if IsCppBound(Player) then
			if IsNumber(itemTbl) then
				if itemTbl == 3 then
					MsgToConn(Conn, 160000)
				elseif itemTbl == 0 then
					MsgToConn(Conn, 8003,'CreateFightItem error')
				elseif itemTbl == 4 then
--					MsgToConn(Conn, 3000, "你没有任务物品"..QuestVar.."的需求")	
				end
			else
				CRoleQuest.add_item_DB_Ret(Player, ItemType, QuestVar, AddVarNum, itemTbl)
			end
		end
	end
	--++++++++++++++++++++++++++++++++++++++
	
	if IsQuestAdd then
		g_AddQuestVarNumForTeam(Player,QuestName,QuestVar,AddVarNum)
	elseif IsItemAdd then
		local parameter = {}
		parameter["char_id"] = Player.m_uID
		parameter["nType"] = ItemType
		parameter["sName"] = QuestVar
		parameter["nCount"] = AddVarNum
		parameter["sceneName"] = Player.m_Scene.m_SceneName
		parameter["createType"] = event_type_tbl["小游戏"]
		
		CallAccountManualTrans(Conn.m_Account, "CharacterMediatorDB","AddItem",CallBack1, parameter)
	elseif IsOtherAdd then
		--经验奖励
		parameter["Exp"] = AddVarNum
		
		--CallDbTrans("CharacterMediatorDB","AddItem", CallBack1, parameter, Conn.m_Account:GetID())
	end
end

function Gac2Gas:SmallGameFaild(Conn, GameName)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	if Conn.m_Player.m_IsGameOver then
		return
	end
	
	Conn.m_Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
	Conn.m_Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
	Conn.m_Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
	Conn.m_Player.m_IsGameOver = true
	Conn.m_Player.m_SetCtrlState = 22
	local GameCommon = SmallGame_Common(GameName)
	if not GameCommon then
		return
	end
	local QuestName = GameCommon("QuestName")
	if QuestName and QuestName ~= "" then
		Gac2Gas:GiveUpQuest(Conn,QuestName)
	end
end

local function ClearSmallGameState(Conn, GameName, LockObjID, IsExit)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local GameCommon = SmallGame_Common(GameName)
	if not GameCommon then
		return 
	end
	
	local TiggerCage = CEntityServerManager_GetEntityByID(LockObjID)
	if TiggerCage and TiggerCage.m_UserId == Conn.m_Player.m_uID then
		TiggerCage.m_UserId = nil
	end
	
	if not Conn.m_Player.m_IsGameOver and Conn.m_Player:CppIsLive() then
		
		Conn.m_Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
		Conn.m_Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
		if not IsExit then
			Conn.m_Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
		end
		Conn.m_Player.m_SetCtrlState = 23
	end
	Conn.m_Player.m_IsGameOver = true
end

function Gac2Gas:SmallGameCancel(Conn, GameName, LockObjID)
	ClearSmallGameState(Conn, GameName, LockObjID, false)
end

function Gac2Gas:ExitSmallGame(Conn, GameName, LockObjID)
	ClearSmallGameState(Conn, GameName, LockObjID, true)
end

function Gac2Gas:AgreeBeginSmallGame(Conn,SmallGameName,GlobalID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local TiggerCage = CEntityServerManager_GetEntityByID(GlobalID)
	if TiggerCage then
		if TiggerCage.m_SmallGameTick then
			UnRegisterTick(TiggerCage.m_SmallGameTick)
			TiggerCage.m_SmallGameTick = nil
		end
		TiggerCage.m_SmallGameTime = os.time()
	end
	
	if Conn.m_Player.m_SmallGameName == SmallGameName then
		Conn.m_Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, true)
		Conn.m_Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, true)
		Conn.m_Player:SetAndSyncActionState(EActionState.eAS_Gather)--加动作播放
		Gas2Gac:BeginSmallGame(Conn,SmallGameName,GlobalID)
		Conn.m_Player.m_IsGameOver = nil
	else
		if TiggerCage and TiggerCage.m_UserId == Conn.m_Player.m_uID then
			TiggerCage.m_UserId = nil
		end
	end
end

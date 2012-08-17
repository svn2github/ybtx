CActiveBehavior = class()

--cfg_load "model_res/NpcRes_Common"

local ReleaseStateTick = {}--时间到,自动释放状态

RegMemCheckTbl("ReleaseStateTick", ReleaseStateTick)

EAction = {}
EAction.WF1_Clasp	= "抱"
EAction.WF1_Dropt	= "放"
EAction.WF2_Lift	= "举"
EAction.WF2_Smash	= "砸"
EAction.WF3_PickUp	= "捡"
EAction.WF3_Throw	= "投"


---------------------------------------------------
--举起后,长时间不放下,就给扔掉
local function DeleteTick(PlayerEntityID)
	if ReleaseStateTick[PlayerEntityID] then
		UnRegisterTick(ReleaseStateTick[PlayerEntityID])
		ReleaseStateTick[PlayerEntityID] = nil
	end
end

local function TimeStart(Tick, data)
	local PlayerEntityID,uEntityID = data[1],data[2]
	DeleteTick(PlayerEntityID)
	local Player = CEntityServerManager_GetEntityByID(PlayerEntityID)
	if Player and Player.m_EmbraceNpc and Player.m_HandObjID == uEntityID then
		Gas2Gac:RetHoldThingTimeOver(Player:GetConnection() ,uEntityID)
		CActiveBehavior.RemovePlayerChildModel(Player)
		Player.m_HandObjID = nil
	end
end
----------------------------------------------------


local function OnObjInHandsChanged(Player, TargetType, uTargetEntityID, TargetName)
	local function Init()
		if not IsCppBound(Player) then
			return
		end
		local uEntityID = Player:GetEntityID()
		-------------------------
		if TargetType ~= 0 then
			if TargetType == ECharacterType.IntObj then		
				if IntObj_Common(TargetName) then
					local TargerRes = IntObj_Common(TargetName, "ModelName")
					Player.m_Properties:SetHandsBehaviorName(TargerRes)
					--为了得到玩家要拿起的东西的名称
					Gas2Gac:RetHoldThingName(Player:GetConnection() ,uEntityID, TargetName)
					--加一个计时的Tick,到时间不扔,就自动给去掉
					if g_ObjActionArgTbl[TargetName] then
						local name = g_ObjActionArgTbl[TargetName][1]--技能的名称
						local time = g_ObjActionArgTbl[TargetName][2]--技能的时间
						Player:AddTempSkill(name,1)
						if not time then
							time = 30
						end
						local data = {uEntityID, uTargetEntityID}
						Player.m_HandObjID = uTargetEntityID
						ReleaseStateTick[uEntityID] = RegisterTick("ReleaseStateTick",TimeStart,time*1000,data)
					end
				end	
				
			elseif TargetType == ECharacterType.Npc then			
				if NpcRes_Common(TargetName) then
					Player.m_Properties:SetHandsBehaviorName(TargetName)
				end			
			end
		else
			Player.m_Properties:SetHandsBehaviorName("")
			DeleteTick(uEntityID)
			Player.m_HandObjID = nil
		end
		
		if not Player:DirectorMovingIsDisabled() then
			Gas2Gac:UpdateModel( Player:GetConnection() ,uEntityID)
		end
		Gas2Gac:UpdateModel( Player:GetIS(0) ,uEntityID)
	end
	
	apcall(Init)
end

local function OnBehaviorChanged(Player, AniFileTailName)	
	local uEntityID = Player:GetEntityID()
	Player.m_Properties:SetBehaviorType(AniFileTailName)
	
	if not Player:DirectorMovingIsDisabled() then
		Gas2Gac:UpdateModel( Player:GetConnection() ,uEntityID)
	end
	Gas2Gac:UpdateModel( Player:GetIS(0) ,uEntityID)
	
	if AniFileTailName ~= "" then
		local szAniType = "wf," .. AniFileTailName
		local szAniFileName = MakeAniFileName(Player:CppGetClass(), Player.m_Properties:GetSex(), szAniType) .. ".fky"
		Player:OnChangeWFAniKeyFrame(szAniFileName)
	else
		Gas2Gac:UpdateAni( Player:GetConnection(), uEntityID)
		Gas2Gac:UpdateAni( Player:GetIS(0), uEntityID)
	end
end

-- 公用接口 by beingway
local function FireKey(Player)
	if IsCppBound(Player) then
		OnObjInHandsChanged(Player, 0, 0, "")
		if Player.m_ActionSkillName then
			Player:PlayerDoPosSkill(Player.m_ActionSkillName, 1, Player.m_ActionSkillPos)
			Player.m_ActionSkillName = nil
			Player.m_ActionSkillPos = nil
		end
		if Player.m_NextAction == EAction.WF1_Dropt then
			CClaspBehavior.DroptBehaviorActualize(Player)
		end
	end	
end

local function FireEnd(Player)
	if IsCppBound(Player) and Player.m_NextAction then
		OnBehaviorChanged(Player, "")
		Player.m_BehaviorFireEnd = nil
		Player.m_NextAction = nil
		if Player:CppIsLive() then
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_OnMission, false)
			Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
			Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
			Player.m_SetCtrlState = 24
		end
	end
end

local function TakeKey(SelfCharacter, TargetEntityID, TargetName, ActionName)
	if IsCppBound(SelfCharacter) then
		
		if ActionName == EAction.WF1_Clasp then
			local IsClaspSucc,TargetType = CClaspBehavior.ClaspBehaviorActualize(SelfCharacter, TargetEntityID)
			if IsClaspSucc then			
				OnObjInHandsChanged(SelfCharacter, TargetType, TargetEntityID, TargetName)
			else
				FireEnd(SelfCharacter)	
			end
		else
			--统一的投、砸接口
			-----------------------------------------------------
			if not TargetEntityID or TargetEntityID == 0 then
				FireEnd(SelfCharacter)
			else
				local Target = CEntityServerManager_GetEntityByID(TargetEntityID)
				if Target then
					SelfCharacter.m_EmbraceNpc = Target.m_Properties:GetCharName()
					local TargetType = Target.m_Properties:GetType()
					g_IntObjServerMgr:Destroy(Target,true)
					
					OnObjInHandsChanged(SelfCharacter, TargetType, TargetEntityID, TargetName)
				else
					FireEnd(SelfCharacter)
				end
			end
			-----------------------------------------------------
		end
		
	end
end

local function BeginBehavior(ActionName, Player, TargetEntityID, TargetName)
	local AniFileTailName = ""
	if ActionName == EAction.WF1_Clasp then
		Player.m_NextAction = EAction.WF1_Dropt
		AniFileTailName = "bao"
	elseif ActionName == EAction.WF2_Lift then
		Player.m_NextAction = EAction.WF2_Smash
		AniFileTailName = "za"
	elseif ActionName == EAction.WF3_PickUp then
		Player.m_NextAction = EAction.WF3_Throw
		AniFileTailName = "tou"
	else
		return
	end

	Player:CppSetCtrlState(EFighterCtrlState.eFCS_OnMission, true)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, true)
	Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, true)
	Player:StopMoving()

	-- 更换主角玩法动作包
	OnBehaviorChanged(Player, AniFileTailName)
	
	Player:SetAndSyncActionState(EActionState.eAS_Take)
	
	-- 添加和同步模型,Tick
	local KeyTime = Player:GetKeyFrameNumOfTakeAni()
	RegisterOnceTick(Player, "TakeKey", TakeKey, KeyTime*33, Player, TargetEntityID, TargetName, ActionName)
end

local function FireBehavior(Player)
	local SelfEntityID = Player:GetEntityID()
	DeleteTick(SelfEntityID)
	
	Player:SetAndSyncActionState(EActionState.eAS_Fire)
	
	Player.m_BehaviorFireEnd = true
	
	local KeyTime = Player:GetKeyFrameNumOfFireAni()
	local EndTime = Player:GetEndFrameNumOfFireAni()
	RegisterOnceTick(Player,"FireKey", FireKey, KeyTime*33, Player)
	RegisterOnceTick(Player,"FireEnd", FireEnd, EndTime*33, Player)
	
	local EmbraceNpc = Player.m_EmbraceNpc
	if g_ObjActionArgTbl[EmbraceNpc] then
		local name = g_ObjActionArgTbl[EmbraceNpc][1]--技能的名称
		Player:RemoveTempSkill(name, 1)
	end
end

local function EndBehavior(Player)
	local NextAction = Player.m_NextAction
	if NextAction then
		
		if NextAction == EAction.WF1_Dropt then--放
			FireBehavior(Player)
		elseif NextAction == EAction.WF2_Smash or NextAction == EAction.WF3_Throw then--砸,投
			if not Player.m_ActionState then
				Player.m_ActionState = true
			else
				Player.m_ActionState = nil
				FireBehavior(Player)
				Player.m_EmbraceNpc = nil
			end
		end
		
	end
end

--释放临时技能
function CActiveBehavior.ActiveBehaviorDoSkill(Player, TargetID ,SkillName,PosX,PosZ)
	CActiveBehavior.TargetDoBehavior(Player, TargetID)
	local PixelPos = CFPos:new()
	PixelPos.x = PosX
	PixelPos.y = PosZ
	Player.m_ActionSkillName = SkillName
	Player.m_ActionSkillPos = PixelPos
end

function CActiveBehavior.TargetDoBehavior(Player, TargetEntityID)
	local EntityID = Player:GetEntityID()
	if not Player or Player.m_BehaviorFireEnd then
		return
	end
	
	local Target = CEntityServerManager_GetEntityByID(TargetEntityID)
	if Target then
		local TargetName = Target.m_Properties:GetCharName()
		--是OBJ还是NPC
		local ActionName = nil
		local ActionType = nil
		if Target.m_Properties:GetType() == ECharacterType.IntObj then
			if IntObj_Common(TargetName) then
				ActionName = IntObj_Common(TargetName, "TriggerAction")
			end
		elseif Target.m_Properties:GetType() == ECharacterType.Npc then
			local ScriptInfo = GetNpcTriggerArg(TargetName, "点击")
			for i, v in pairs(ScriptInfo) do
				ActionName = i
				break
			end
		end
		
		BeginBehavior(ActionName,Player,TargetEntityID,TargetName)
		
	else
		EndBehavior(Player)
	end
end

local function ClearPlayerState(Player, IsDestroy)
	local function Init()
		local uEntityID = Player:GetEntityID()
		if Player.m_EmbraceNpc and not Player.m_BehaviorFireEnd then
			local EmbraceNpc = Player.m_EmbraceNpc
			Player.m_EmbraceNpc = nil
			DeleteTick(uEntityID)
			Player.m_HandObjID = nil
			Player.m_NextAction = nil
			Player.m_ActionState = nil
			
			if not IsDestroy and g_ObjActionArgTbl[EmbraceNpc] then
				local name = g_ObjActionArgTbl[EmbraceNpc][1]--技能的名称
				Player:RemoveTempSkill(name, 1)
			end
			
			if not IsDestroy and Player:CppIsLive() then
				
				Player:CppSetCtrlState(EFighterCtrlState.eFCS_OnMission, false)
				Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
				Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
				Player.m_SetCtrlState = 25
			end
			
			OnObjInHandsChanged(Player, 0, 0, "")
			OnBehaviorChanged(Player, "")
		end
		if Player.m_EmbraceSkillName then
--			print("2",Player.m_EmbraceSkillName)
			Player:PlayerDoItemSkill(Player.m_EmbraceSkillName,1)
			Player.m_EmbraceSkillName = nil
		end
	end
	apcall(Init)
end

function CActiveBehavior.RemovePlayerChildModel(Player)
	ClearPlayerState(Player,false)
end

function CActiveBehavior.ClearBehaviorTick(Player)
	ClearPlayerState(Player,true)
end

function CActiveBehavior.QuitFitoutState(Conn)
	if IsCppBound(Conn.m_Player) and Conn.m_Player.m_EmbraceNpc then
		if Conn.m_Player.m_ActionState then
			Conn.m_Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
			--Conn.m_Player:DelListeningWnd()
			Conn.m_Player.m_ActionState = nil
		else
			ClearPlayerState(Conn.m_Player,false)
		end
	end
end

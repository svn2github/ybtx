gas_require "world/int_obj/IntObjCallbackHandlerServerInc"
cfg_load "scene/Trap_Common"

function CIntObjCallbackHandler:OnDestroyObj(GlobalID)
	local IntObj = CIntObjServer_GetIntObjServerByID(GlobalID)
	if not IsServerObjValid(IntObj) then
		return
	end

	g_IntObjServerMgr:Destroy(IntObj,false)
end

function CIntObjCallbackHandler:OnSetLockObj(GlobalID)
end

local function IntoTrapAoi(ObjEntityID, IntObj, Character)
	if not IsServerObjValid(IntObj) or not IsServerObjValid(Character) then
		return
	end
	local TrapName = IntObj.m_Properties:GetCharName()
	local CharacterType = Character.m_Properties:GetType()
	
	local function fun()
		if IntObj.m_TrapHandler == nil then
			return
		end
		IntObj.m_OnTrapCharIDTbl[Character:GetEntityID()] = Character:GetEntityID()
		
		local isInTrap = true
		local TrapTbl = Character.m_Properties.m_InTrapTblName 
		if TrapTbl then
			if TrapTbl[TrapName] == nil then
				TrapTbl[TrapName] = {}
			end
			if next(TrapTbl[TrapName]) then
				isInTrap = false
			end
			TrapTbl[TrapName][ObjEntityID] = ObjEntityID
		end
			
		if isInTrap then
			IntObj.m_TrapHandler:OnStepInByCharacter(Character)
			g_TriggerMgr:Trigger("进", IntObj, Character)
		end
	end
	
	if CharacterType == ECharacterType.Player then
		local ConditionTbl = GetTriggerCondition("Trap", TrapName, "进", "任务需求")
		local QuestNameTbl = ConditionTbl and ConditionTbl.Arg
		local QstName = QuestNameTbl and QuestNameTbl[1]
		--print(TrapName,ConditionTbl,QstName)
		if QstName == nil then
			fun()
		else
			if CRoleQuest.DoingQuest_Check(Character, QstName) then
				fun()
			end
		end
	else
		fun()
	end
end

local function LeaveTrapAoi(ObjEntityID, IntObj, Character)

	if not IsServerObjValid(IntObj) or not IsServerObjValid(Character) then
		return
	end
	if IntObj.m_TrapHandler == nil then
		return
	end

	local TrapName = IntObj.m_Properties:GetCharName()	
	local TrapCharIDTbl = IntObj.m_OnTrapCharIDTbl
	if TrapCharIDTbl[Character:GetEntityID()] then
		TrapCharIDTbl[Character:GetEntityID()] = nil
	end

	local isLeaveTrap = true
	local TrapTbl = Character.m_Properties.m_InTrapTblName 
	if TrapTbl then
		if TrapTbl[TrapName] and TrapTbl[TrapName][ObjEntityID] then
			TrapTbl[TrapName][ObjEntityID] = nil
			IntObj.m_TrapHandler:ClearTriggertblData()
			if next(TrapTbl[TrapName]) then	
				isLeaveTrap = false
			end	
		else
			isLeaveTrap = false
		end
	end
	
	if isLeaveTrap then
		IntObj.m_TrapHandler:OnStepOutByCharacter(Character)
		g_TriggerMgr:Trigger("出", IntObj, Character)
	end
end

function CIntObjCallbackHandler:OnIntoTrapViewAoi(ObjEntityID, IntObj, Character)
	IntoTrapAoi( ObjEntityID, IntObj, Character)
end

function CIntObjCallbackHandler:OnLeaveTrapViewAoi( ObjEntityID, IntObj, Character)
	LeaveTrapAoi( ObjEntityID, IntObj, Character)
end

function CIntObjCallbackHandler:OnObjAoiMsgFromSelf(ObjEntityID, IntObj)
	if IntObj.m_IsBornAction then
		local function SetIntObjAction()
			IntObj.m_Properties:SetBornAction(1)
		end
		RegisterOnceTick(IntObj, "SetIntObjAction", SetIntObjAction, 1000)
		--Gas2Gac:NotifyIntObjDoAction(IntObj:GetIS(0), ObjEntityID, "birth", 1)
		IntObj.m_Properties:SetActionNum(1)
		IntObj.m_IsBornAction = nil
	end
end

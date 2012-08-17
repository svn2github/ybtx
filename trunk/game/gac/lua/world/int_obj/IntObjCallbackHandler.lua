cfg_load "int_obj/ObjEffect_Common"
cfg_load "scene/Trap_Common"
cfg_load "model_res/IntObjRes_Client"
cfg_load "scene/Transport_Server"
gac_gas_require "activity/Trigger/NpcTrigger"

CIntObjCallbackHandler = class( IIntObjCallbackHandler )

function CIntObjCallbackHandler:OnCreated(IntObj)
	--print("-----------------------OnCreated")
	SetEvent( Event.Test.ObjectCreated , true )
	IntObj.m_Properties = CIntObjProperties:new()
    IntObj.m_Properties:Init(IntObj)

end

function InitTrapEffect(Trap)
	local Name = Trap.m_Properties:GetCharName()
	local EffectName = Trap_Common(Name, "ObjEffectName")
--	local QuestName = Trap_Common(Name, "CorrelationQstName")
	local ConditionTbl = GetTriggerCondition("Trap", Name, "进", "任务需求")
	local QuestNameTbl = ConditionTbl and ConditionTbl.Arg
	local QuestName = QuestNameTbl and QuestNameTbl[1]
	local questneed = nil
	
	if EffectName == "" then
		return
	end
	
	if QuestName and QuestName ~= "" then
		if not g_GameMain.m_QuestRecordWnd then
			return
		end
		questneed = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName]
		if questneed == nil then
			if Trap.m_EffectId then
				Trap:GetRenderObject():DelEffect( Trap.m_EffectId )
				Trap.m_EffectId = nil
			end
			return 
		end	
	end	
	
	
	local TriggerTime = Trap_Common(Name, "TriggerTime")
	local Effect = ObjEffect_Common(EffectName)
	if TriggerTime ~= ""  or Effect == nil then
		return
	end
	local RenderObj = Trap:GetRenderObject()	
	--RenderObj:CreateDefault()
	
	if not Trap.m_EffectId then
		Trap.m_EffectId = RenderObj:AddEffect( Effect("FilePath"), Effect("EffectName"), -1, nil )
	end
end

function CIntObjCallbackHandler:OnIntoSyncAoi(IntObj)
--print("OnIntoSyncAoi(IntObj)")
	IntObj.m_Properties:InitSync()
--	local ModelName = IntObj.m_Properties:GetModelName()	
	local RenderObject = IntObj:GetRenderObject()
	local Type = IntObj.m_Properties:GetType()
	if Type == ECharacterType.IntObj then
		IntObj:SetIntObjRenderType()
		local ObjName = IntObj.m_Properties:GetCharName()
		local ModelName = IntObj_Common(ObjName, "ModelName")
		IntObjLoadModel(RenderObject, ModelName)		
		local ActionNum = IntObj.m_Properties:GetActionNum()
		local IsBornAction = IntObj.m_Properties:GetBornAction()
		if ActionNum ~= 0 and IsBornAction ~= 2 then
			ObjFollowerDoAction(IntObj, ModelName, "stand0"..ActionNum)
		elseif IsBornAction == 2 then
			NotifyIntObjDoAction(IntObj:GetEntityID(), "birth", 1)
		end
		if g_MainPlayer~= nil then
			g_MainPlayer:IntObjIntoVision(IntObj)
		end
		g_GameMain.m_NpcHeadSignMgr:NpcAndObjInView(IntObj)
		g_GameMain.m_CharacterInSyncMgr:CharacterInSync(IntObj)
		g_GameMain.m_HideFollowerMgr:DealHideIntObjIntoSync(IntObj)
	elseif Type == ECharacterType.Trap then
		InitTrapEffect(IntObj)
		--  加载Trap模型
		local trapName = IntObj.m_Properties:GetCharName()
		local ModelName = Trap_Common(trapName, "ModelName")
		TrapLoadModel(RenderObject, ModelName)
		IntObj:SetCanBeSelected(false)
		IntObj:GetRenderObject():SetRenderStyle( 0x009 )
		g_GameMain.m_NpcHeadSignMgr:NpcAndObjInView(IntObj)
		
		local camp = g_MainPlayer:CppGetBirthCamp()
		local sceneName = g_GameMain.m_SceneName
		if Transport_Server(trapName) then
			for j, i in pairs(Transport_Server:GetKeys(trapName, "陷阱")) do
				local TransInfo = Transport_Server(trapName, "陷阱", i.."")
				local EndSceneType = Scene_Common[TransInfo("EndScene")] and Scene_Common[TransInfo("EndScene")].SceneType
				if (TransInfo("BeginScene") == "" or sceneName == TransInfo("BeginScene") )
					and TransInfo("EndScene") ~= sceneName and (EndSceneType == 1 or EndSceneType == 2)
					and (TransInfo("Camp") == 0 or TransInfo("Camp") == camp)  then
					g_GameMain.m_TransportTrap[IntObj] = Transport_Server(trapName)
					break
				end
			end
		end
	end
end





function CIntObjCallbackHandler:OnObjDoActionEnd(IntObjFollower)
--print("OnObjDoActionEnd")
--	if IntObjFollower.m_NextAction then
--		local ModelName = IntObjFollower.m_Properties:GetModelName()	
--		local ActionName = IntObjFollower.m_NextAction
--		IntObjFollower.m_NextAction = nil
--		ObjFollowerDoAction(IntObjFollower, ModelName, ActionName)
--	end
end

--function Gas2Gac:NotifyIntObjPlayDieEffect(Conn, EntityID, ObjEffectName)
--	IntObjPlayEffect(EntityID, ObjEffectName)
--	Gac2Gas:IntObjPlayDieEffectEnd(g_Conn, EntityID)
--end

function CIntObjCallbackHandler:OnDestroy(Obj)
--	print("-----------------------OnDestroy")
	if  g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingIntObj == Obj then
		g_MainPlayer:UnLockIntObj()
	end
	--local OldModelName = Obj.m_Properties:GetModelName()	
	local Type = Obj.m_Properties:GetType()
	if Type == ECharacterType.IntObj then
		--local OldPieceName = IntObjRes_Client(OldModelName, "PieceName")
		Obj:GetRenderObject():RemovePiece(nil)
	elseif Type == ECharacterType.Trap then
		Obj:GetRenderObject():RemovePiece("Body")
		if g_GameMain.m_TransportTrap[Obj]then
			g_GameMain.m_TransportTrap[Obj] = nil
		end
		
	end
end	

function CIntObjCallbackHandler:OnLeaveSyncAoi(Obj)
--	print("-----------------------OnLeaveSyncAoi")
	if not IsCppBound(g_MainPlayer) then
		return
	end
	Obj.m_Properties:OutOfSync()
	if  g_MainPlayer.m_LockCenter.m_LockingIntObj == Obj then
		g_MainPlayer:UnLockIntObj()
	end

--	print(Obj:GetEntityID().." OnLeaveSyncAoi")
	local Type = Obj.m_Properties:GetType()
	if Type == ECharacterType.IntObj then
		--local OldModelName = Obj.m_Properties:GetModelName()
		--local OldPieceName = IntObjRes_Client(OldModelName, "PieceName")
		Obj:GetRenderObject():RemovePiece(nil)
		if g_MainPlayer ~= nil 
			and g_MainPlayer.m_IntObjInVisionTbl 
			and table.maxn(g_MainPlayer.m_IntObjInVisionTbl) ~= 0 then
			g_MainPlayer:IntObjOutOfVision(Obj)
		end
		--任务npc(obj) 头顶标记
		if g_GameMain.m_NpcHeadSignMgr then
			g_GameMain.m_NpcHeadSignMgr:NpcAndObjOutView(Obj)
		end
		if g_GameMain.m_CharacterInSyncMgr then
			g_GameMain.m_CharacterInSyncMgr:CharacterOutSync(Obj) --头顶信息包括
		end
	elseif Type == ECharacterType.Trap then
--		if g_GameMain.m_NpcHeadSignMgr then
--			g_GameMain.m_NpcHeadSignMgr:NpcAndObjOutView(Obj)
--		end
		
		if g_GameMain.m_TransportTrap[Obj]then
			g_GameMain.m_TransportTrap[Obj] = nil
		end
		local Name = Obj.m_Properties:GetCharName()
		local EffectName = Trap_Common(Name, "ObjEffectName")
		if EffectName == "" then
			return
		end
	end
	if Obj.m_EffectId then
		Obj:GetRenderObject():DelEffect( Obj.m_EffectId )
		Obj.m_EffectId = nil
	end
	if Obj.m_CanClickFxId then
		Obj:GetRenderObject():DelEffect( Obj.m_CanClickFxId )
		Obj.m_CanClickFxId = nil
	end
end

function CIntObjCallbackHandler:OnAnimationEnd(Obj,actionName)
	if Obj.IsNeedCallback then
		Obj.IsNeedCallback = false
		local ModelName = IntObj_Common(Obj.m_Properties:GetCharName(), "ModelName")
		ObjFollowerDoAction(Obj, ModelName, "stand0"..Obj.NextActionNum)
		Obj:GetRenderObject():Update()
	end
end

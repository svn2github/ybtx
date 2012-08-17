-- 更新某角色头盔显隐，只影响 头盔、头发、面部的模型
function UpdateHeadModel(Character)
	if Character == nil then
		return
	end
	if Character.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end

	local RenderObj = Character:GetRenderObject()
	RenderObj:RemovePiece("armet")
	RenderObj:RemovePiece("hair")
	RenderObj:RemovePiece("face")
	local ResTbl = {}
	ResTbl["Class"]				= Character:CppGetClass()
	ResTbl["Sex"]				= Character.m_Properties:GetSex()
	ResTbl["Camp"]				= Character:CppGetCamp()
	ResTbl["HairResID"] 		= Character.m_Properties:GetHairResID()
	ResTbl["FaceResID"] 		= Character.m_Properties:GetFaceResID()
	ResTbl["ArmetResID"] 		= g_GetResIDByEquipPart(EEquipPart.eHead,Character)
	InitPlayerCustomRes(
		RenderObj,
		{		
			ResTbl["FaceResID"],
			ResTbl["HairResID"]
		},
		ResTbl["Class"],ResTbl["Sex"]
	)
	
	InitPlayerModel(
		RenderObj,
		{		
			ResTbl["ArmetResID"]
		},
		ResTbl["Class"],ResTbl["Sex"],ResTbl["Camp"]	
	)
	-- 装备特效
	local EquipPhase =  g_GetEquipPhaseByEquipPart(EEquipPart.eHead,Character)
	InitPlayerPiecePhaseFX(RenderObj,"armet" ,EquipPhase)
end

function UpdateModel(Character)
	if Character.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	local ResTbl = {}
	ResTbl["Class"]				= Character:CppGetClass()
	ResTbl["Camp"]				= Character:CppGetCamp()
	ResTbl["Sex"]				= Character.m_Properties:GetSex()
	ResTbl["HairResID"] 		= Character.m_Properties:GetHairResID()
	ResTbl["FaceResID"] 		= Character.m_Properties:GetFaceResID()
	ResTbl["ArmetResID"] 		= g_GetResIDByEquipPart(EEquipPart.eHead,Character)
	ResTbl["BodyResID"] 		= g_GetResIDByEquipPart(EEquipPart.eWear,Character)
	ResTbl["ArmResID"] 			= g_GetResIDByEquipPart(EEquipPart.eHand,Character)
	ResTbl["ShoeResID"] 		= g_GetResIDByEquipPart(EEquipPart.eShoe,Character)
	ResTbl["ShoulderResID"] 	= g_GetResIDByEquipPart(EEquipPart.eShoulder,Character)
	ResTbl["WeaponResID"] 		= g_GetResIDByEquipPart(EEquipPart.eWeapon,Character)
	ResTbl["OffWeaponResID"] 	= g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,Character)
	ResTbl["BackResID"] 		= g_GetResIDByEquipPart(EEquipPart.eBack,Character) 
	
	ResTbl["WeaponItemID"] 		= Character.m_Properties:GetWeaponIndexID()
	ResTbl["OffWeaponItemID"] 	= Character.m_Properties:GetOffWeaponIndexID()
	ResTbl["OffWeaponBigID"] 	= Character.m_Properties:GetOffWeaponBigID()
	ResTbl["HorseResID"] 		= Character.m_Properties:GetHorseResID()
	ResTbl["HandsBehaviorName"] = Character.m_Properties:GetHandsBehaviorName()
	ResTbl["HorseType"] 		= Character.m_Properties:GetHorseType()
	ResTbl["BehaviorType"] 		= Character.m_Properties:GetBehaviorType()
	
	ResTbl["ArmetPhase"] 		= g_GetEquipPhaseByEquipPart(EEquipPart.eHead,Character)
	ResTbl["BodyPhase"] 		= g_GetEquipPhaseByEquipPart(EEquipPart.eWear,Character)
	ResTbl["ArmPhase"] 			= g_GetEquipPhaseByEquipPart(EEquipPart.eHand,Character)
	ResTbl["ShoePhase"] 		= g_GetEquipPhaseByEquipPart(EEquipPart.eShoe,Character)
	ResTbl["ShoulderPhase"] 	= g_GetEquipPhaseByEquipPart(EEquipPart.eShoulder,Character)
	ResTbl["WeaponPhase"] 		= g_GetEquipPhaseByEquipPart(EEquipPart.eWeapon,Character)
	ResTbl["OffWeaponPhase"] 	= g_GetEquipPhaseByEquipPart(EEquipPart.eAssociateWeapon,Character)
	
	UpdatePlayerModel(Character,ResTbl)
end

function UpdatePlayerModel(Character,ResTbl)
	if Character == nil then
		return
	end
	local RenderObj = Character:GetRenderObject()
	if Character.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	
	local WeaponItemID = ResTbl["WeaponItemID"]
	local OffWeaponBigID = ResTbl["OffWeaponBigID"]
	local OffWeaponItemID = ResTbl["OffWeaponItemID"]
	local HorseResID = ResTbl["HorseResID"]
	local HorseType = ResTbl["HorseType"]
	local HandsBehaviorName = ResTbl["HandsBehaviorName"]
	local BehaviorType = ResTbl["BehaviorType"]
	local WeaponInfo = {}
	local WeaponTransformType,OffWeaponTransformType = 0,0
	RenderObj:RemoveAllPiece()	
	RenderObj:ClearAddSke()
	Character:GetMainHandRenderObj():ClearAllEfx()
	Character:GetMainHandRenderObj():RemoveAllPiece()
	Character:GetMainHandRenderObj():Delframework()
	Character:SetWeaponPlayType(0)
	Character:SetOffWeaponPlayType(0)
	RenderObj.WeaponBindBone = ""
	RenderObj.weaponeffectname = ""
	RenderObj.weaponRenderObj = nil
	local EquipType
	if WeaponItemID ~= 0 and WeaponItemID ~= nil then
		local itemtype,itemname = g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[WeaponItemID]
		EquipType = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"EquipType")
		local WeaponTransformType = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"WeaponTransformType")
		if WeaponTransformType~= 0 and WeaponTransformType ~= nil then
			RenderObj.weaponRenderObj = Character:GetMainHandRenderObj()
			Character:SetWeaponPlayType(WeaponTransformType)
		end
	end
	Character:GetOffHandRenderObj():ClearAllEfx()
	Character:GetOffHandRenderObj():RemoveAllPiece()
	Character:GetOffHandRenderObj():Delframework()
	Character:SetOffWeaponPlayType(0)
	RenderObj.OffWeaponBindBone = ""
	RenderObj.offweaponeffectname = ""
	RenderObj.offweaponRenderObj = nil
	if OffWeaponBigID ~= nil and OffWeaponBigID ~= 0 and  g_ItemInfoMgr:IsStaticWeapon(OffWeaponBigID) then
		local OffWeaponTransformType = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[OffWeaponItemID],"WeaponTransformType")
		if OffWeaponTransformType~= 0 and OffWeaponTransformType ~= nil then
			RenderObj.offweaponRenderObj = Character:GetOffHandRenderObj()
			Character:SetOffWeaponPlayType(OffWeaponTransformType)
		end
	end

	local AniType=""
	if BehaviorType ~= "" and BehaviorType ~= nil then 
		AniType = "wf"..","..BehaviorType
		
	elseif HorseType ~= "" and HorseType ~= nil then
		if Character:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) then
			AniType = "jt"..","..HorseType
		elseif Character:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse) then	
			InitPlayerCommonAni(RenderObj,ResTbl["Class"],ResTbl["Sex"],HorseType)
			if  EquipType ~= nil then	
				AniType = "zq,"..EquipType..","..HorseType
			else
				AniType = "zq,,"..HorseType
			end
		elseif EquipType ~= nil then
--			print("SyncVariant's HorseType is wrong!")
			AniType = EquipType
		end
	else
		AniType = EquipType
	end

-- 初始化模型资源
	InitPlayerCustomRes(
		RenderObj,
		{		
			ResTbl["FaceResID"],
			ResTbl["HairResID"],
		},
		ResTbl["Class"],ResTbl["Sex"]	
	)
	
	InitPlayerModel(
		RenderObj,
		{		
			ResTbl["ArmetResID"],
			ResTbl["BodyResID"],
			ResTbl["ArmResID"],
			ResTbl["ShoeResID"],
			ResTbl["ShoulderResID"],
			ResTbl["BackResID"]
		},
		ResTbl["Class"],ResTbl["Sex"],ResTbl["Camp"]
	)
	
	local tblEquipPhase = {}
	tblEquipPhase["armet"] 	= ResTbl["ArmetPhase"]
	tblEquipPhase["body"] 	= ResTbl["BodyPhase"]
  	tblEquipPhase["arm"] 	= ResTbl["ArmPhase"]
	tblEquipPhase["shoe"] 	= ResTbl["ShoePhase"] 
	tblEquipPhase["shoulder"] = ResTbl["ShoulderPhase"] 
	
	-- 初始化动作
	if HorseResID == 0 then
		InitPlayerBaseAni(RenderObj, ResTbl["Class"], ResTbl["Sex"], nil)
	end
	InitPlayerFinalAni(RenderObj, ResTbl["Class"], ResTbl["Sex"], AniType)
	InitWeaponRender(RenderObj, ResTbl["WeaponResID"], ResTbl["Class"],ResTbl["Sex"])
	InitWeaponRender(RenderObj, ResTbl["OffWeaponResID"], ResTbl["Class"],ResTbl["Sex"])
	Character:SetWeaponInfo(RenderObj.WeaponBindBone,RenderObj.OffWeaponBindBone,RenderObj.weaponeffectname,RenderObj.offweaponeffectname)
	InitPlayerEquipPhaseFX(RenderObj,tblEquipPhase)
	Character:DoNowAni()
	if WeaponItemID ~= 0 and WeaponItemID ~= nil then
		CUseItemRpcMgr.SetAttackEffect(g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[WeaponItemID],EEquipPart.eWeapon,Character,ResTbl["HorseType"],ResTbl["BehaviorType"],ResTbl["Class"],ResTbl["Sex"])
	elseif OffWeaponItemID ~= 0 and OffWeaponItemID ~= nil then
		CUseItemRpcMgr.SetAttackEffect(g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[OffWeaponItemID],EEquipPart.eAssociateWeapon,Character,ResTbl["HorseType"],ResTbl["BehaviorType"],ResTbl["Class"],ResTbl["Sex"])
	end
	if HorseResID ~= 0 and HorseResID ~= nil then
		InitHorseRender(Character:GetHorseRenderObj(), HorseResID, ResTbl["Class"], ResTbl["Sex"])
		Character:DoNowAni()
		Character:GetHorseRenderObj():AddSkeletal("zw01")
		Character:GetHorseRenderObj():SetTerrainInfluence(true);
		local HorseRes = HorseRes_Common(g_sParseHorseTblClient[HorseResID])
		Character:SetHorseName(HorseRes("Name"))
	elseif HorseResID == 0  then
		Character:GetHorseRenderObj():SetTerrainInfluence(false);
		Character:SetHorseName("")
		Character:GetHorseRenderObj():ClearAllEfx()
		Character:GetHorseRenderObj():RemoveAllPiece()
		Character:GetHorseRenderObj():Delframework()
	end
	if HandsBehaviorName ~= "" and HandsBehaviorName ~= nil then
		local pos = CFPos:new(0,0)
		RenderObj.BehaviorRenderObj = Character:GetAdditionRenderObj()
		local BehaviorRes = NpcRes_Common(HandsBehaviorName)
		if BehaviorRes == nil then
			BehaviorRes = IntObjRes_Client(HandsBehaviorName)
			if BehaviorRes ~= nil then
				IntObjLoadModel(RenderObj.BehaviorRenderObj, HandsBehaviorName)
			else
--				print("无此模型")	
			end		
		else
			InitRenderNpc(RenderObj.BehaviorRenderObj, HandsBehaviorName, ECharSex.eCS_Male)
--			for i, NpcEvent in pairs( NpcTriggerEvent_Common) do 
--				if NpcEvent.NpcName == HandsBehaviorName then
--					for _, Arg in pairs( NpcEvent.ScriptArg) do
--						g_GameMain.m_HoldThingType = Arg[1]
--						break
--					end
--					break
--				end
--			end
			local ScriptInfo = GetNpcTriggerArg(HandsBehaviorName, "点击")
			if ScriptInfo then
				for i, v in pairs(ScriptInfo) do
					g_GameMain.m_HoldThingType = i
					break
				end
			end
		end
		RenderObj.BehaviorRenderObj:DoAni("wf",true, -1.0)
	else
		-----------------
		if g_MainPlayer and g_MainPlayer:GetEntityID() == Character:GetEntityID() then
			
			if not g_MainPlayer.m_ActionTimeOver then
				if GroundSelector:IsActive() then
					GroundSelector:CancelSelectGround()
				end
			end
			
			g_MainPlayer.m_ActionTimeOver = nil
			g_MainPlayer.m_ActionSkillName = nil
		end
		-----------------------
		Character:GetAdditionRenderObj():RemoveAllPiece()
		Character:GetAdditionRenderObj():Delframework()
	end
end

function CCharacterFollower:InitCharacterFollowerOnCaughtViewSightOf()
	self.m_NpcInVisionTbl = {}             --用于热键自动拾取掉落物品 
	self.m_IntObjInVisionTbl = {}             --用于更新交互对象可用状态的外观显示
	self.m_NameColor = ECharNameColor.eCNC_Blue	
	self:InitLockCenter()	
end

function CCharacterFollower:InitLockCenter()
	self.m_LockCenter						= {}
	self.m_LockCenter.m_LockingObj			= nil	
end

function Gas2Gac:UpdateModel(Connection,uEntityID)
	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
	if	Character==nil or Character.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	UpdateModel(Character)
end

function Gas2Gac:UpdateAni(Connection, uEntityID)
	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
	if	Character == nil then
		return
	end
	local RenderObj = Character:GetRenderObject()
	InitPlayerCommonAni(RenderObj, Character:CppGetClass(), Character.m_Properties:GetSex(), nil)
end

function Gas2Gac:UpdateCoreInfo(Connection,uEntityID)
	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
	if Character==nil then
		return
	end
	UpdateFollowerCoreInfo(Character)
end

function UpdateFollowerCoreInfo(Character)
	if Character == nil or Character:IsBattleHorse() then
		return
	end
	if Character == g_MainPlayer then
		g_GameMain.m_PlayerInfo:Show(true)
	elseif Character == g_MainPlayer.m_LockCenter.m_LockingObj then
		g_GameMain.m_TargetInfo:UpdatePortrait(g_MainPlayer.m_LockCenter.m_LockingObj)
	elseif g_MainPlayer.m_LockCenter.m_LockingObj and g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter and Character == g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj then
		g_GameMain.m_TargetOfTargetInfo:UpdatePortrait(g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj)
	end
end

function GetActionDir(CharacterForm,CharacterTo)
	
	local posFrom 	= CFPos:new()
	local posTo		= CFPos:new()
	
	CharacterForm:GetPixelPos(posFrom)
	CharacterTo:GetPixelPos(posTo)
	
	local dx = posFrom.x - posTo.x
	local dy = posFrom.y - posTo.y
	
	local Dir = CDir:new()
	local vecDir = CVector2f:new() 
	vecDir.x = dx
	vecDir.y = dy
	Dir:Set( vecDir )
	
	return Dir.uDir
end

function CCharacterFollower:DoRespondActionBegin(player)
	if self:CppGetCtrlState( EFighterCtrlState.eFCS_ForbitTurnAround ) or self:IsMoving() then
		return
	end
	local udir = GetActionDir(player,self)
	
	self:DoAction(EActionState.eAS_Respond_Enter, udir)
	self.OnRespond = true
end

--设置NPC回原因的方向,并改变动作
function Gas2Gac:RetSetNpcDirection(Conn, uEntityID, Direction)
	local Target = CCharacterFollower_GetCharacterByID(uEntityID)
	if IsCppBound(Target) and Target:CppIsAlive() and not Target.OnRespond then
		Target:DoAction(EActionState.eAS_Respond_Leave, Direction)
	end
end

--从服务端得到方向
function CCharacterFollower:DoRespondActionEnd(player)
	if self.OnRespond and not self:CppGetCtrlState( EFighterCtrlState.eFCS_ForbitTurnAround ) then
		Gac2Gas:GetNpcDirection(g_Conn, self:GetEntityID())
		self.OnRespond = nil
	end
end

--功能NPC转向从服务端得到方向
function CCharacterFollower:FunNpcActionEnd(player)
	if not self:IsMoving() and self:CppIsAlive() and player:CppIsAlive() then
		if CQuest.IsObjectContainFun(self) then
			self:DoRespondActionEnd(player)
		end
	end
end

function Gas2Gac:OtherPlayFacial(Connection, uCharID, FacialName)
	local Character = CCharacterFollower_GetCharacterByID(uCharID)
	local ActionState = CActorCfg_GetEnumByString(FacialName)
	Character:DoActionState(ActionState)
	--Character:DoFunActionState(FacialName)
end

function Gas2Gac:PlayFacialActionFailed(Connection, str)
	MsgClient(120001, str)
end

function Gas2Gac:RetHoldThingName(Connction ,SelfEntityID, TargetName)
	if TargetName and g_ObjActionArgTbl[TargetName] then
		g_MainPlayer.m_ActionSkillName = g_ObjActionArgTbl[TargetName][1]--技能的名称
	end
	g_GameMain.m_HoldThingType = IntObj_Common(TargetName, "TriggerAction")
end

function Gas2Gac:RetHoldThingTimeOver(Connction ,SelfEntityID)
	if SelfEntityID ~= 0 then
		g_MainPlayer.m_ActionTimeOver = true
	end
	if GroundSelector:IsActive() then
		GroundSelector:CancelSelectGround()
	end
end

function Gas2Gac:SetNpcAlphaValue(EntityID, Value)
	local Npc = CCharacterFollower_GetCharacterByID(EntityID)
	if Npc then
		if Npc:GetRenderObject() then
			Npc:GetRenderObject():SetAlphaValue(Value)
		end
	end
end

function Gas2Gac:UpdateShadowModel(Connection,uEntityID)
	local ShadowNpc = CCharacterFollower_GetCharacterByID(uEntityID)
	if ShadowNpc then
		ShadowNpc:OnCaughtSyncSightOfShadow()
	end
end

function CCharacterFollower:OnCaughtSyncSightOfShadow()
	local ResTbl = {}
	ResTbl["Class"]				= self.m_Properties:GetMasterClass()
	ResTbl["Sex"]				= self.m_Properties:GetMasterSex()
	ResTbl["Camp"]				= self:CppGetCamp()
	ResTbl["HairResID"] 		= self.m_Properties:GetHairResID()
	ResTbl["FaceResID"] 		= self.m_Properties:GetFaceResID()
	ResTbl["WeaponItemID"] 		= self.m_Properties:GetWeaponIndexID()
	ResTbl["OffWeaponItemID"] 	= self.m_Properties:GetOffWeaponIndexID()
	ResTbl["OffWeaponBigID"] 	= self.m_Properties:GetOffWeaponBigID()
	ResTbl["HorseResID"] 		= self.m_Properties:GetHorseResID()
	ResTbl["HandsBehaviorName"] = self.m_Properties:GetHandsBehaviorName()
	ResTbl["HorseType"] 		= self.m_Properties:GetHorseType()
	ResTbl["BehaviorType"] 		= self.m_Properties:GetBehaviorType()
	ResTbl["ArmetResID"] 		= self.m_Properties:GetHeadResID()
	ResTbl["BodyResID"] 		= self.m_Properties:GetBodyResID()
	ResTbl["ArmResID"] 			= self.m_Properties:GetArmResID()
	ResTbl["ShoeResID"] 		= self.m_Properties:GetShoeResID()
	ResTbl["ShoulderResID"] 	= self.m_Properties:GetShoulderResID()
	ResTbl["WeaponResID"] 		= self.m_Properties:GetWeaponResID()
	ResTbl["OffWeaponResID"] 	= self.m_Properties:GetOffWeaponResID()
	ResTbl["BackResID"] 		= self.m_Properties:GetBackResID()
	
	ResTbl["ArmetPhase"] 		= self.m_Properties:GetHeadPhase()
	ResTbl["BodyPhase"] 		= self.m_Properties:GetBodyPhase()
	ResTbl["ArmPhase"] 			= self.m_Properties:GetArmPhase()
	ResTbl["ShoePhase"] 		= self.m_Properties:GetShoePhase()
	ResTbl["ShoulderPhase"] 	= self.m_Properties:GetShoulderPhase()
	ResTbl["WeaponPhase"] 		= self.m_Properties:GetWeaponPhase()
	ResTbl["OffWeaponPhase"] 	= self.m_Properties:GetOffWeaponPhase()
	
	UpdatePlayerModel(self,ResTbl)
	--local ShadowNpc = CCharacterFollower_GetCharacterByID(uShadowID)
	--if ShadowNpc then
		--UpdatePlayerModel(ShadowNpc,ResTbl)
	--end
end

--上线后发送玩家身上的积分
function Gas2Gac:ReturnPlayerPoint(Conn,NaturePoint,EvilPoint,DestroyPoint,DaTaoShaPoint,JiFenSaiPoint,ArmorPoint, KillPoint,BaseAttackPoint,PvpPoint,Profferpoint,TeamProffer)
	g_MainPlayer.m_NaturePoint		= NaturePoint
	g_MainPlayer.m_EvilPoint		= EvilPoint
	g_MainPlayer.m_DestroyPoint		= DestroyPoint
	g_MainPlayer.m_ArmorPoint		= ArmorPoint
	g_MainPlayer.m_DaTaoShaPoint	= DaTaoShaPoint
	g_MainPlayer.m_JiFenSaiPoint	= JiFenSaiPoint
	g_MainPlayer.m_KillPoint		= KillPoint
	g_MainPlayer.m_BaseAttackPoint 	= BaseAttackPoint
	g_MainPlayer.m_PvpPoint 		= PvpPoint
	g_MainPlayer.m_TongProffer 		= Profferpoint
	g_MainPlayer.m_TeamProffer		= TeamProffer
end

function CCharacterFollower:GetJiFenCountByType(jiFenType)
    local PlayerHaveJiFenCount = {}
    PlayerHaveJiFenCount[1] = g_MainPlayer.m_NaturePoint
    PlayerHaveJiFenCount[2] = g_MainPlayer.m_EvilPoint
    PlayerHaveJiFenCount[3] = g_MainPlayer.m_DestroyPoint
    PlayerHaveJiFenCount[4] = g_MainPlayer.m_DaTaoShaPoint
    PlayerHaveJiFenCount[5] = g_MainPlayer.m_JiFenSaiPoint
    PlayerHaveJiFenCount[6] = g_MainPlayer.m_ArmorPoint
    PlayerHaveJiFenCount[7] = g_MainPlayer.m_KillPoint
    PlayerHaveJiFenCount[8] = g_MainPlayer.m_BaseAttackPoint
    PlayerHaveJiFenCount[9] = g_MainPlayer.m_PvpPoint
    PlayerHaveJiFenCount[10] = g_MainPlayer.m_TongProffer 
    return PlayerHaveJiFenCount[jiFenType]
end

function CCharacterFollower:UpdateJiFenCountByType(jiFenType, modifiedJiFenCount)
    if jiFenType == 1 then
        g_MainPlayer.m_NaturePoint = g_MainPlayer.m_NaturePoint + modifiedJiFenCount
    elseif jiFenType == 2 then
        g_MainPlayer.m_EvilPoint = g_MainPlayer.m_EvilPoint + modifiedJiFenCount
    elseif jiFenType == 3 then
        g_MainPlayer.m_DestroyPoint = g_MainPlayer.m_DestroyPoint + modifiedJiFenCount
    elseif jiFenType == 4 then
        g_MainPlayer.m_DaTaoShaPoint = g_MainPlayer.m_DaTaoShaPoint + modifiedJiFenCount
    elseif jiFenType == 5 then
        g_MainPlayer.m_JiFenSaiPoint = g_MainPlayer.m_JiFenSaiPoint + modifiedJiFenCount
    elseif jiFenType == 6 then
        g_MainPlayer.m_ArmorPoint = g_MainPlayer.m_ArmorPoint + modifiedJiFenCount
    elseif jiFenType == 7 then
        g_MainPlayer.m_KillPoint = g_MainPlayer.m_KillPoint + modifiedJiFenCount
        if g_MainPlayer.m_KillPoint < 0 then
       	 g_MainPlayer.m_KillPoint = 0
        end
    elseif jiFenType == 8 then
        g_MainPlayer.m_BaseAttackPoint = g_MainPlayer.m_BaseAttackPoint + modifiedJiFenCount
    elseif jiFenType == 9 then
        g_MainPlayer.m_PvpPoint = g_MainPlayer.m_PvpPoint + modifiedJiFenCount
    elseif jiFenType == 10 then
        g_MainPlayer.m_TongProffer  = g_MainPlayer.m_TongProffer  + modifiedJiFenCount
    end
end

--设置玩家的显示效果(是否有透明的效果)
function Gas2Gac:SetPlayerAlphaValue(Conn,AlphaValue)
	local RenderObj = g_MainPlayer:GetRenderObject()
	RenderObj:SetAlphaValue(AlpHaValue)
end

function Gas2Gac:DoAlertPhaseEffect(Connection, AlertNpcID, uLastTime)
	--print("DoAlertPhaseEffect :", AlertNpcID, uLastTime)
	local AlertNpc = CCharacterFollower_GetCharacterByID(AlertNpcID)
	if IsCppBound(AlertNpc) then
		local RenderObj = AlertNpc:GetRenderObject()
		if IsCppBound(RenderObj) then
			if RenderObj.m_AlertObject ~= nil then
				--RenderScene=g_CoreScene:GetRenderScene()
				RenderObj:DestroyRenderObject(RenderObj.m_AlertObject)
			end
			RenderObj.m_AlertObject = nil
			RenderObj.m_AlertObject = RenderObj:CreateRenderObject(eLinkType.LT_SKELETAL_OFFSET, "Bip01 Head")
			RenderObj.m_AlertObject:SetCanBeSelect(false)
			AlertNpc:ClearAlertEffect()
			if uLastTime ~= 0 then
				RenderObj.m_AlertObject:SetRenderVelocity( 9/uLastTime, -1)
				AlertNpc.m_AlertFx = RenderObj.m_AlertObject:AddEffect("fx/setting/jingjie.efx", "jingjie/create", 0, nil)
			end
		end
	end
end

function Gas2Gac:DoAlertEndEffect(Connection, AlertNpcID)
	--print("DoAlertEndEffect : ", AlertNpcID)
	local AlertNpc = CCharacterFollower_GetCharacterByID(AlertNpcID)
	if IsCppBound(AlertNpc) then
		local RenderObj = AlertNpc:GetRenderObject()
		if IsCppBound(RenderObj) and RenderObj.m_AlertObject then
			AlertNpc:ClearAlertEffect()
			RenderObj.m_AlertObject:SetRenderVelocity( 1, -1)
			RenderObj.m_AlertObject:AddEffect("fx/setting/jingjie.efx", "jingjie/zhandou", 0, nil)
		end
	end
end

function Gas2Gac:ClearNpcAlertEffect(Connection, AlertNpcID)
	local AlertNpc = CCharacterFollower_GetCharacterByID(AlertNpcID)
	if IsCppBound( AlertNpc ) then
		AlertNpc:ClearAlertEffect()
		local RenderObj = AlertNpc:GetRenderObject()
		if RenderObj.m_AlertObject then
			RenderObj:DestroyRenderObject(AlertNpc.m_AlertObject)
			AlertNpc.m_AlertObject = nil
		end
	end
end

function CCharacterFollower:ClearAlertEffect()
	if self ~= nil and IsCppBound(self) then
		local RenderObj = self:GetRenderObject()
		if RenderObj.m_AlertObject then
			if self.m_AlertFx then
				RenderObj.m_AlertObject:DelEffect(self.m_AlertFx)
				self.m_AlertFx = nil
			end
		end
	end
end



cfg_load "model_res/NpcRes_Common"
cfg_load"npc/Npc_Common"

CCharacterFollowerCallbackHandler = class( CCharacterClientCallbackHandler, ICharacterFollowerCallbackHandler )

function CCharacterFollowerCallbackHandler:OnCreated(Character, IsBattleHorse)
--	print("CCharacterFollowerCallbackHandler:OnCreated")
	local EntityID = Character:GetEntityID()
	SetEvent( Event.Test.ObjectCreated , true )
	local Type = Character:GetViewVariantHolder(ECodeTableIndex.eCTI_Static):GetUInt8("Attribute/Type")
--	Character:GetHorseRenderObj():SetDirection( CDir:new( 160 ) )
	Character:GetHorseRenderObj():SetLapseTime(2000)	
	Character:GetRenderObject():SetLapseTime(2000)	
	if Type == ECharacterType.Player then
		if(IsBattleHorse) then
			Character.m_Properties = CBattleHorseProperties:new()
			Character.m_Properties:Init(Character)
			Character:SetIsPlayer(false)
			Character:InitLockCenter()
		else
			Character.m_Properties = CPlayerProperties:new()
			Character.m_Properties:Init(Character)
			Character.m_uID	= Character.m_Properties:GetCharID()
			Character:SetIsPlayer(true)
			local ArpFileName =  MakeAniFileName(Character:CppGetClass(),Character.m_Properties:GetSex(),nil)
			Character:SetSpliceName(ArpFileName.."_skill/")
			Character:SetSkillFxFile("fx/setting/skill/"..ArpFileName.."_skill/")
			Character:SetActionFxFile("fx/setting/wf/"..GetRaceSexTypeString(Character:CppGetClass(), Character.m_Properties:GetSex()) .. "/")
			--在这个函数里面会绑定Handler	 
			Character:InitCharacterFollowerOnCaughtViewSightOf()
			g_AddPlayerInfo(Character.m_uID, Character.m_Properties:GetCharName(), Character)
			local PlayerName = Character.m_Properties:GetCharName()
			g_GameMain.m_SmallMapBG.m_Static:AddEntity(EntityID,PlayerName,-1)
			g_GameMain.m_CharacterInViewMgr:CharacterInView(Character)
		end
		return
	end
	if Type == ECharacterType.Npc then
		Character.m_Properties = CNpcProperties:new()
		Character.m_Properties:Init(Character)
		Character:SetIsPlayer(false)
		local npcName = Character.m_Properties:GetCharName()
		local npcShowName = GetNpcDisplayName(npcName)
		local AniFile = NpcRes_Common(npcName, "AniFileName")
		if  AniFile ~= nil then
			Character:SetAttackFxFile("fx/setting/npcfx/"..AniFile .. "/")
			Character:SetActionFxFile("fx/setting/npcfx/"..AniFile .. "/")
			Character:SetSkillFxFile("fx/setting/npcfx/"..AniFile .. "/")
		end
		local NpcType = GetFuncTypeByNpcName(npcName)
		g_GameMain.m_SmallMapBG.m_Static:AddEntity(EntityID,npcShowName,NpcType)
		local NpcCfg = Npc_Common(npcName)
		--显血类型为1或者2的Npc不可以被选中
		if NpcCfg("ShowBloodType") == 1 and NpcCfg("ShowBloodType") == 2 then
			Character:SetCanBeSelected(false)
		end
		Character:InitLockCenter()
		if NpcCfg("Boss") == 1 then
			Character:SetIsBoss(true)
		end
		return
	end
end

function CCharacterFollowerCallbackHandler:OnIntoSyncAoi(Character)
	Character.m_Properties:InitSync()
	g_GameMain.m_HideFollowerMgr:SetFriendCanNotBeSelected(Character)
	
	local Type = Character.m_Properties:GetType()
	if Type == ECharacterType.Player then
		UpdateModel(Character)
		UpdateFollowerCoreInfo(Character)
		if not Character:CppIsAlive() and not g_MainPlayer.m_CanLookDead then
			Character:SetCanBeSelected(false)
		end
	elseif Type == ECharacterType.Npc and Character:GetNpcType() ~= ENpcType.ENpcType_BattleHorse then
		
		local NpcName = Character.m_Properties:GetCharName()
		local Sex = Character.m_Properties:GetSex()
		
		if Character.m_Properties:GetIsShadow() then
			Character:OnCaughtSyncSightOfShadow()
			InitPlayerBaseAni(Character:GetRenderObject(), Character.m_Properties:GetMasterClass(), 
				Character.m_Properties:GetMasterSex(), Character.m_Properties:GetHorseType())
		else
			InitRenderNpc(Character:GetRenderObject(), NpcName, Sex)
			Character:DoNowAni()
		end
		Character:GetRenderObject():SetAlphaValueEx(255)
		
		if NpcRes_Common(NpcName, "AdhereTerrain") == 1 then
			Character:GetRenderObject():SetTerrainInfluence(true) -- 设置贴地
		end
		
		if not Character:CppIsAlive() then
			CheckCanClickDeadNpc(Character)
		end
		
		g_MainPlayer:NpcIntoVision(Character)
		
		local NpcCfg = Npc_Common(NpcName)
		--显血类型为1或者2的Npc不可以被选中
		if NpcCfg("ShowBloodType") == 1 or NpcCfg("ShowBloodType") == 2 then
			Character:SetCanBeSelected(false)
		end
		
		--斗中的NPC,不可被同阵营的玩家点到(国战地图中的NPC会用到)
		if NpcCfg("ShowBloodType") == 4 then
			local PlayerCamp = g_MainPlayer:CppGetBirthCamp()
			local TargetCamp = Character:CppGetCamp()
			if PlayerCamp == TargetCamp and Character:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
				Character:SetCanBeSelected(false)
			end
		end
		
		--任务npc(obj) 头顶标记
		g_GameMain.m_NpcHeadSignMgr:NpcAndObjInView(Character)
		
		g_MainPlayer:JudgementServantCanBeSelected(Character)
	end
	
	g_GameMain.m_CharacterInSyncMgr:CharacterInSync(Character) --对进入视野的对象做相关操作
	g_GameMain.m_TargetInfo:ReLockObj(Character) --重新关联离开视野的锁定目标
	g_GameMain.m_TargetOfTargetInfo:ReLockTargetOfTarget(Character)
	g_MainPlayer:DealHideFollowerIntoSync(Character)
	
	--如果Npc配置表中显血类型是2的话则不显示血条和Npc名字
	if Type ~= ECharacterType.Npc 
		or Npc_Common(Character.m_Properties:GetCharName(),"ShowBloodType") ~= 2 then
			Character:AddHeadBloodRendler(g_MainPlayer, g_GameMain.m_BloodRenderDialogPool, g_GameMain, g_GameMain.BloodRenderWndIsShow)
			g_GameMain.m_CharacterInSyncMgr:SetHeadBloodTransparentFun(Character,true)
	end
	if PlayerName == g_GameMain.m_TargetInfo.m_TargetName and g_GameMain.m_TargetInfo:IsShow() and Character:CanBeSelected() then
		--如果玩家下线再上线，uEntityID会变化，但目标消失又不取消目标投向，导致目标玩家再上线时UI数值不对的问题
		g_MainPlayer:LockObj( Character )
	end
end

function CCharacterFollowerCallbackHandler:OnDestroy(Character)
	if Character:GetRemovedImmediately() then
		Character:GetHorseRenderObj():SetLapseTime(0)
		Character:GetRenderObject():SetLapseTime(0)
	end
	local EntityID = Character:GetEntityID()
	local Type = Character.m_Properties:GetType()
	g_GameMain.m_SmallMapBG.m_Static:RemoveEntity(EntityID)
	if Character==g_MainPlayer:GetServant(ENpcType.ENpcType_BattleHorse) then--战斗坐骑
		g_GameMain.m_BattleHorseBuffWnd:ClearAllBuffState()
	elseif Character==g_MainPlayer:GetServant(ENpcType.ENpcType_Summon) then--召唤兽
		g_GameMain.m_ServantBuffWnd:ClearAllBuffState()
	end
	if Character==g_MainPlayer.m_LockCenter.m_LockingObj then
		CTargetBuffWnd.GetWnd():ClearAllBuffState()
		g_GameMain.m_TargetInfo:UnRegisterTick()
	end
	if g_GameMain.m_TargetOfTargetInfo:IsShow() and 
		( (g_MainPlayer.m_LockCenter.m_LockingObj == nil or g_MainPlayer.m_LockCenter.m_LockingObj == Character) or 
		( g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter and g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj == Character) ) then
			g_GameMain.m_TargetOfTargetInfo:Close()
	end
	
	if g_GameMain.m_AimStatusWnd ~= nil and Character == g_GameMain.m_AimStatusWnd.m_Target then
		g_GameMain.m_AimStatusWnd.m_Target = nil
		local viewinfoMgr = CFighterViewInfoMgr.GetWnd()
		viewinfoMgr:DelViewInfoByID(Character:GetEntityID())
	end
	
	--检查NPC头顶冒泡的Tick是否被析构s
	PopNpcHeadUpDialog(Character)
	
	g_GameMain.m_CharacterInViewMgr:CharacterOutView(Character)
	
	self:OnLeaveSyncAoi(Character)
	if(Character.m_Properties:GetType() == ECharacterType.Player and not Character:IsBattleHorse()) then
		g_DelPlayerInfo(Character.m_uID, Character.m_Properties:GetCharName())
	end
	SetEvent( Event.Test.ObjectDestroied , true )
	Character.m_StaticPerfomer = nil
	if ( Character.m_Properties ~= nil ) then
		Character.m_Properties.m_Character = nil
	end
	Character:ClearAlertEffect()
	Character.m_Properties = nil
	Character:SetCallbackHandler(nil)
	Character.m_CallbackHandler = nil
	Character = nil
	g_GameMain.m_NavigationMap:DelRoleData(Object)
end	



function CCharacterFollowerCallbackHandler:OnLeaveSyncAoi(Character)
	Character.m_Properties:OutOfSync()
	
	if  g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj == Character then
		g_MainPlayer:UnLockObj( Character:CppIsAlive() )
	end
	
	g_GameMain.m_CharacterInSyncMgr:CharacterOutSync(Character) --头顶信息
	
	local Type = Character.m_Properties:GetType()
	if Type ~= ECharacterType.Npc 
		or Npc_Common(Character.m_Properties:GetCharName(),"ShowBloodType") ~= 2 then
		if not Character:IsBattleHorse() then
			Character:DelHeadBloodRendler(g_GameMain.m_BloodRenderDialogPool)
		end
	end
	
	if Type == ECharacterType.Npc then
		if Character.m_CanClickFxId then
			Character:GetRenderObject():DelEffect( Character.m_CanClickFxId )
			Character.m_CanClickFxId = nil
		end
		--任务npc(obj) 头顶标记
		g_GameMain.m_NpcHeadSignMgr:NpcAndObjOutView(Character)
		if Character.m_CanClickDeadNpcTick then
			UnRegisterTick(Character.m_CanClickDeadNpcTick)
			Character.m_CanClickDeadNpcTick = nil
		end
		if g_MainPlayer ~= nil 
			and g_MainPlayer.m_NpcInVisionTbl 
			and table.maxn(g_MainPlayer.m_NpcInVisionTbl) ~= 0 then
			g_MainPlayer:NpcOutOfVision(Character)
		end
	end
end

function CCharacterFollowerCallbackHandler:OnLevelUp(uEntityID, nLevelChanged)
--	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
--	if(not Character) then return end
--	if g_MainPlayer then
--		g_MainPlayer:ChangeCharacterFollowerSetCamp(Character)
--	end
end

function CCharacterFollowerCallbackHandler:OnDead(uEntityID)
	--print("CCharacterFollowerCallbackHandler:OnDead ", uEntityID)
	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
	if(not Character) then return end
		
	if(g_MainPlayer.m_LockCenter.m_LockingObj  == Character)then
		g_MainPlayer:UnLockObj()
	elseif (g_GameMain.m_TargetOfTargetInfo:IsShow() and g_MainPlayer.m_LockCenter.m_LockingObj ~= nil and
		g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter and g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj == Character) then
			g_GameMain.m_TargetOfTargetInfo:Close()
	end
	if(Character.m_Properties:GetType() ~= ECharacterType.Player) then
		g_GameMain.m_CharacterInSyncMgr:DestroyHeadInfoWnd(Character)
		g_GameMain.m_TeamMarkSignMgr:CharacterOutView(Character)
	end
	if(Character.m_Properties:GetType() == ECharacterType.Player) then
		if not g_MainPlayer.m_CanLookDead  then
			Character:SetCanBeSelected(false)
		end
	end

	PopNpcHeadUpDialog(Character)
end

function CCharacterFollowerCallbackHandler:OnReborn(uEntityID)
--print("CCharacterFollowerCallbackHandler:OnReborn ", uEntityID)
	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
	if(not Character) then return end
		
	if(Character.m_Properties:GetType() == ECharacterType.Player) then
		SetCanBeSelectedForLua(Character,true)
	end
end

function CCharacterFollowerCallbackHandler:OnMaterialNumChange(Character, MaterialNum, bDirector)
	if(bDirector) then
		if(g_MainPlayer:GetServant(ENpcType.ENpcType_Truck) == Character) then
			local wnd = CTruckInfo.GetWnd()
			wnd:SetMPValue(MaterialNum, wnd.m_nMaxMP)
		end
	else
		if(g_MainPlayer.m_LockCenter.m_LockingObj == Character) then
			local wnd = g_GameMain.m_TargetInfo
			wnd:SetMPValue(MaterialNum, wnd.m_nMaxMP)
		end
	end
end

function CCharacterFollowerCallbackHandler:OnExpOwnerChange(Character)
	local Target = g_MainPlayer.m_LockCenter.m_LockingObj
	if(Target == Character) then
		if(Character.m_HeadInfoWnd and IsCppBound(Character.m_HeadInfoWnd)) then
			Character.m_HeadInfoWnd:TempColor()
		end
	end
end

gac_require "message/message_box/ClientMsg"
gac_require "fight/target_info/TargetInfo"
gac_gas_require "player/PlayerMgr"
gac_require "world/npc/NpcClient"
gac_require "world/int_obj/IntObjClient"
gac_require "fight/buff_wnd/BaseBuff"
gac_require "world/area/AreaClient"

--function Gas2Gac:Message(Conn,MessageID)
--	MsgClient(MessageID)
--	SetEvent( Event.Test.MsgToClient,true,sMessage )
--end

function Gas2Gac:CreatePlayerEnded(Conn,roleName, ChangeType)	
--	print("CreatePlayerEnded ----------------------------------")
	SaveLoginFlow("Gas2Gac:CreatePlayerEnded " .. roleName)
	
	SetEvent( Event.Test.GasCreatePlayerEnded, true )

	if (g_App.m_CreateCharSuc) then
		g_GameMain:GameMainRegisterAccelerator()
		g_App.m_CreateCharSuc = nil
	end
	if IsCppBound(Conn) then
		Conn:SetRoleName(roleName)
	end

	if ChangeType == 0 then
		if g_GameMain.m_LeadLeanSkillWnd then
			g_GameMain.m_LeadLeanSkillWnd:InitData()
		end
		-- 弹出活动提示面板
		if g_GameMain.m_FbActionPanel then
			local PlayerLevel = g_MainPlayer:CppGetLevel()
			if PlayerLevel >= 15 then
				g_GameMain.m_FbActionPanel:PopupActionPanel("Login")
			end
		end
	end
	
	local RenderScene = g_CoreScene:GetRenderScene()
	SaveLoginFlow("InitSceneRes Begin ")
	RenderScene:InitSceneRes()
	SaveLoginFlow("InitSceneRes End ")
	
	if g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn then
		if string.find(g_GameMain.m_SceneName,"树屋酒吧") then
			Gas2Gac:SetCompassHeadDir(g_Conn,2, 308*EUnits.eGridSpanForObj, 281*EUnits.eGridSpanForObj)
		end
		g_GameMain.m_InformationWnd:ShowText()
	end
	
end
--跳转场景通知客户端
function NotifyChangeScene()
	g_GameMain:ResetCameraRotate()
	--ShowLoadingWnd()
	PlayerMoveChangeAreaInfo()
end

---------------------------------------------以下是角色初始化代码---------------------------------------------------
function Gas2Gac:InitMoneyInfo( Connection,nMoney,nBindMoney,nBindTicket)
	if g_MainPlayer then
		g_MainPlayer:InitMoney(nMoney,0,nBindMoney,nBindTicket)
		g_GameMain.m_WndMainBag:InitMoneyInMainBag()
	end
end

-- 状态初始化
function Gas2Gac:InitMainPlayer( Connection,uCurExp,uLevelExp ,SceneName, SceneType)
	local Player = g_MainPlayer
	local nFromSceneType = g_GameMain.m_SceneType
	Player.m_MoveSpeed = g_MainPlayer:GetMaxSpeed()
	g_GameMain.m_SceneName = SceneName
	if SceneType == 25 then
		g_GameMain.m_SceneName = Scene_Common[SceneName].BasicName
	else
		g_GameMain.m_SceneName = SceneName
	end
	
	g_GameMain.m_SceneType = SceneType
	if g_GameMain.m_IsTransfer then
		g_GameMain:EndTransfer()
	else
		PlayerMoveChangeAreaInfo()
		g_GameMain:ResetCameraRotate()
	end
	g_GameMain.m_PlayerInfo:UpdateHeadInfo()
	g_GameMain.m_PkSwitchWnd:IsShowWnd()
	g_MainPlayer.m_uID = g_MainPlayer.m_Properties:GetCharID()
	g_MainPlayer.m_sName = g_MainPlayer.m_Properties:GetCharName()
	g_AddPlayerInfo(g_MainPlayer.m_uID, g_MainPlayer.m_sName, g_MainPlayer)
	
	if uCurExp==0 and uLevelExp==0 then
		g_GameMain.m_ExpBar:ShowWnd(false)
	else	
		g_GameMain.m_ExpBar:SetExpLong( uCurExp,uLevelExp)
		g_GameMain.m_RoleStatus:SetExpLong(uCurExp,uLevelExp)
	end
	g_GameMain.m_QuestTraceBack:InitNotifyActionTick()
	g_GameMain.m_PlayerInfo:SetClassPic() --主角阵营头像
	-- 临时注销ping值功能
	--Player._m_PingTick = RegisterTick( Player.DisplayPingValue, 1000 )
	g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit()	--刷新主角头顶信息
	g_GameMain.m_TeamMarkSignMgr:PlayerHeadTeamMarkInit()	--主角队伍标记
	
	g_GameMain.m_CharacterInSyncMgr:UpdateHeadModelInSync()
	g_GameMain.m_SysSetting.m_MouseSettingWnd:ChangeMouseSettingEnd()
	g_GameMain.m_GuideBtn:InitShow()
	g_GameMain.m_GuideBtn:OnTransfered(nFromSceneType, SceneType)
end

-- ping值功能
function CCharacterDirector:DisplayPingValue()
	assert(g_Conn.GetLatency ~= nil)
--	print ("当前的ping值为:", g_Conn:GetLatency())
end

function CCharacterDirector:Init()
	CCharacterClient.Init( self )
	CCharacterClient.OnCreated(self)
	--self:SetHandler( self )
	--self.m_CancelFSTbl = {}
end

function CCharacterDirector:Unit()
	SetEvent( Event.Test.ObjectDestroied , true )
	self:MoveOnTime_UnRegisterTick()
	self:RunToMouse_UnRegisterTick()
	self:ActionTurn_UnRegisterTick()
	self:MoveByGlide_UnRegisterTick()
	self:CharInfoDebug_UnRegisterTick()
	CSoundCDTimeMgr_OnDestroy()
	if self.m_CharInfoDlg then
		self.m_CharInfoDlg:DestroyWnd()
	end
	local RenderObj = self:GetRenderObject()
	if RenderObj then
		RenderObj.weaponRenderObj=nil
		RenderObj.offweaponRenderObj=nil
	end
	g_MainPlayer.m_NpcInVisionTbl = {}
	g_MainPlayer.m_IntObjInVisionTbl = {}
	g_MainPlayer.m_StaticPerfomer = nil
	g_MainPlayer.m_Properties.m_Character = nil
	g_MainPlayer.m_Properties = nil
	g_MainPlayer:SetCallbackHandler(nil)
	g_MainPlayer.m_CallbackHandler = nil
	--Gac2Gas:GetTeamMembers(g_Conn)

	if( g_MainPlayer.m_LockCenter.m_LockingObj and g_MainPlayer.m_LockCenter.m_LockingObj ~= nil) then
		g_MainPlayer:UnLockObj()
	end
	
	if g_MainPlayer.m_LockCenter.m_LockingIntObj then
		g_MainPlayer:UnLockIntObj()
	end
end

-----------------------------------------以下是角色操作代码----------------------------------------------
-- 创建鼠标特效
function CCharacterDirector:CreateMouseFx()
--print("CCharacterDirector:CreateMouseFx()")
	if (self:CppIsAlive() == false or g_MainPlayer.m_bHideMouseFX == 1) then
		return
	end
	
	local pos = CVector3f:new()
	g_MainPlayer:UpdateSelectPos()
	g_CoreScene:GetRenderScene():GetSelectedPos (pos)
	local Pos = CFPos:new()
	Pos.x = pos.x
	Pos.y = pos.z
	CPositionFX_CppPlayPosFX("fx/setting/shubiaoxiangguan.efx", "鼠标点击/create",Pos)
end

function CCharacterDirector:CreateLockObjFx(LockObj)
	if LockObj ~= nil then 
		--LockObj:GetRenderObject():CreateDefault()
		self.m_LockCenter.m_RenderChild=LockObj:GetRenderObject():CreateRenderObject(eLinkType.LT_CENTER,"")
		local Type = LockObj.m_Properties:GetType()
		if Type == ECharacterType.IntObj then
			local ModelName = IntObj_Common(LockObj.m_Properties:GetCharName(), "ModelName")
			local IntObjRes = IntObjRes_Client(ModelName)
			self.m_LockCenter.m_RenderChild:SetScale( IntObjRes("FootSize")/IntObjRes("Scaling") )
		elseif Type == ECharacterType.Npc then
			local NpcName = LockObj.m_Properties:GetCharName()
			local NpcRes = NpcRes_Common(NpcName)
			self.m_LockCenter.m_RenderChild:SetScale( NpcRes("FootSize")/NpcRes("Scaling") )
		end
		CAniHandler = class ( IModelHandler )
		local aniHandler = CAniHandler:new()
		aniHandler.LockCenter = self.m_LockCenter
		aniHandler.LockObj = LockObj
		aniHandler.Player = self
		self.m_LockCenter.m_RenderChild:SetModelHandler( aniHandler )
		local FxName 
		if LockObj:GetGameEntityType() == EGameEntityType.eGET_CharacterFollower then
			if self:IsEnemy(LockObj) then 
				FxName = "选中目标/create03"
			else
				FxName = "选中目标/create01"
			end
		else
			FxName = "选中目标/create01"
		end
		self.m_LockCenter.m_RenderChild:AddEffect( "fx/setting/mubiaoxuanze.efx", FxName, 0, NULL )
		function CAniHandler:OnLinkAnimationEnd(actionName)
			self.LockCenter.m_RenderChild:SetModelHandler(nil)
			local FxName 
			if LockObj:GetGameEntityType() == EGameEntityType.eGET_CharacterFollower then
				if self.Player:IsEnemy(self.LockObj) then 
					FxName = "选中目标/create04"
				else
					FxName = "选中目标/create02"
				end
			else
				FxName = "选中目标/create02"
			end
			self.LockCenter.m_RenderChild:RemovePiece(nil)
			self.LockCenter.m_RenderChild:AddEffect( "fx/setting/mubiaoxuanze.efx", FxName, -1, NULL )
		end
	end
end

function CCharacterDirector:DestroyLockObjFx(LockObj)
	local RenderObj = LockObj:GetRenderObject()
	RenderObj:RemovePiece("")
	RenderObj:DestroyRenderObject(self.m_LockCenter.m_RenderChild)
	self.m_LockCenter.m_EffectId	= nil
	self.m_LockCenter.m_RenderChild = nil		
end

local function UpdateCursorKeyBoardMove()
	if g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
		if g_WndMouse:CheckSelWnd( g_GameMain) then
			g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
		end	
		g_WndMouse:UpdateCursor()
	end
end

-- 更新选中位置
function CCharacterDirector:UpdateSelectPos()
	--local rtRootWnd = CFRect:new()
	--g_App:GetRootWnd():GetWndRect( rtRootWnd )
	local posCursor = CFPos:new()
	g_App:GetRootWnd():GetCursorPos (posCursor)
	local x = posCursor.x
	local y = posCursor.y
	if g_CoreScene == nil or g_GameMain.FreedomCameraCtrl == true then
		return
	end
	if g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveState then
		g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveWnd:CheckMouseLeaveWnd(posCursor)
	end
	assert(g_CoreScene:GetRenderScene())
	--g_CoreScene:GetRenderScene():SelectPosByCursor( x, y )
	if g_GameMain.m_DoSmallGame then
		local vecPos = CVector3f:new()
		--g_CoreScene:GetRenderScene():GetSelectedPos(vecPos)
		vecPos.x = posCursor.x
		vecPos.z = posCursor.y
		g_GameMain.m_SGMgr:RecMousePosSmallGame(vecPos)
--		print("m_DoSmallGame m_Do SmallGame m_DoSmallGame m_DoSmallGame")
	else
		local SelectObj = CEntityClient_GetSelectedEntity(g_CoreScene)
		local isCanSelect = true
		if SelectObj ~= nil then
			if g_GameMain.m_ForbidSelectOtherPlayer and SelectObj.m_Properties:GetType() == ECharacterType.Player then
				isCanSelect = false
			end
		else
			isCanSelect = false
		end
	
		if isCanSelect then
			local target = SelectObj
			local Type = target.m_Properties:GetType()
			if SelectObj:GetGameEntityType() == EGameEntityType.eGET_CharacterDirector or SelectObj:GetGameEntityType() == EGameEntityType.eGET_CharacterFollower then
				if g_GameMain:IsInWndArea(x, y) then
					if not target:CppIsAlive() then
						if Type == ECharacterType.Npc then
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_PickUp )
						else
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
						end
						if g_WndMouse:CheckSelWnd( g_GameMain) then
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
						end	
						UpdateCursorKeyBoardMove()
						return
					end
					local Camp = target:CppGetCamp()
					if Type == ECharacterType.Npc then
						if (Camp ~= ECamp.eCamp_None and Camp ~= ECamp.eCamp_Passerby and Camp ~= self:CppGetCamp()) then
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_PK )
						elseif CQuest.IsObjectContainFun(target) then
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Talk )
						end
						if g_WndMouse:CheckSelWnd( g_GameMain) then
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
						end
						UpdateCursorKeyBoardMove()
						return
					end
					if target ~= g_MainPlayer and Type ~= nil then	
						--临时	
						--if (Camp == ECamp.eCamp_None or Camp == g_MainPlayer:CppGetCamp()) then
						if (Camp == ECamp.eCamp_None or g_WndMouse:CheckSelWnd( g_GameMain) or Camp == self:CppGetCamp()) then
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
							--return
						else
							g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_PK )
							--return
						end
						UpdateCursorKeyBoardMove()
					end
				else
					--g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
					g_WndMouse:ResetSkillState()
				end
			end
		else
			g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
			UpdateCursorKeyBoardMove()
		end
	end
end

function CCharacterDirector:OnSelectGround()
	if g_MainPlayer:IsPassive() then
		g_MainPlayer:CancelCoerceMove(self:GetEntityID())
	end

--	if( GroundSelector:IsActive() ) then
--		GroundSelector:CancelSelectGround()
--	else
		
		-- 这里做键盘移动优先级高于鼠标，如需平等，将IF判断去掉即可
		if( not self:IsMovingByKeyboard() ) then
			self:CreateMouseFx()
			self:BeforeMoveToSelectPos()
			self:MoveOnTime_UnRegisterTick()
			if not self.RunToMouseTick then
				self.MoveOnTime_Tick = RegClassTick( "MoveOnTime_OnTick", CCharacterDirector.MoveOnTime_OnTick , 1500, self)
			end
		end
--	end
end

function CCharacterDirector:ClickTargetSetDir(Target)
	local charactorPos = CPos:new() 
	self:GetGridPos( charactorPos)
	-- 得到NPC当前Pos
	local TargetPos = CPos:new()
	Target:GetGridPos( TargetPos)				
	local Dir = CDir:new()
	local PosDir = CVector2f:new()
	PosDir.x = TargetPos.x - charactorPos.x
	PosDir.y = TargetPos.y - charactorPos.y
	if (PosDir.x <= -0.001 or PosDir.x >= 0.001)
		or (PosDir.y <= -0.001 or PosDir.y >= 0.001) then
		Dir:Set(PosDir)
		self.m_uDir = Dir.uDir;
		self:TurnAndSyncDir(Dir.uDir)
	end
end

function CCharacterDirector:OnLButtonClickObj(Target)
	local targetID = Target:GetEntityID()
	local Type = Target.m_Properties:GetType()
	local moveDis = GetAutoTrackDistance(Target)
	if Type == ECharacterType.IntObj then
		if g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint then
			ClearAutoTrackState()
		end
		if g_GetDistance(Target, self) <= moveDis then
			if self:IsMoving() and not g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
				self:StopMoving()
			end
			OnLClickIntObj(Target)
			return true
		else
			if not g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
				self:InitAutoTracker(self:GetEntityID(), targetID, moveDis, AutoTrackerHandlerForIntObj:new(), false)
				return true
			end
		end
		return false
	elseif Type == ECharacterType.Npc then
		if g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint then
			ClearAutoTrackState()
		end
		local bNeedNormalNpcAutoTracker = false
		if not g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
			bNeedNormalNpcAutoTracker = (not self:CheckDoNormalAttack(targetID)) and self:CppIsAlive() 
		end

		if g_GetDistance(Target, self) <= moveDis then
			if not self:IsMoving() and not self:CppGetCtrlState( EFighterCtrlState.eFCS_ForbitTurnAround ) 
				and not self:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
				self:ClickTargetSetDir(Target)
			end
		
			if not g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
				self:StopMoving()
			end
			if Target:CppIsAlive() then
				return OnRClickNpc(Target)
			else
				OnRClickDeadNpc(Target)
				return true
			end			
		else
			if Target:CppIsAlive() then
				if bNeedNormalNpcAutoTracker then
					--print("Npc AutoTracker")
					self:LockObj( Target )
					self:InitAutoTracker(self:GetEntityID(), targetID, moveDis, AutoTrackerHandlerForNpc:new(), false)
					return true
				end
			else
				self:LockObj( Target )
				--print("DeadNpc AutoTracker")
				if not g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
					self:InitAutoTracker(self:GetEntityID(), targetID, moveDis, AutoTrackerHandlerForDeadNpc:new(), false)
					return true
				end
			end
		end
	end
	return false
end

function CCharacterDirector:OnSelectObj(Target, bDoNormalAttack)
	--print("CCharacterDirector:OnSelectObj")
	g_MainPlayer.TabAgain = true
	local targetID = Target:GetEntityID()
	-- 同步选中目标的属性
	local Type = Target.m_Properties:GetType()
	if Type == ECharacterType.Trap then
		return
	end
	if( (bDoNormalAttack == false and Target ~= self.m_LockCenter.m_LockingObj ) or Type == ECharacterType.IntObj or Type == ECharacterType.Trap) then
		--处于战斗状态下才执行放弃普攻
		if self:IsInBattleState() then
			self:CancelNormalAttack()
		end
	end
	if Type ~= ECharacterType.IntObj then
		self:LockObj( Target )
	else
		self:LockIntObj(Target)
	end
			
	self.m_LockCenter.m_StopDistance	= 1
	self.m_LockCenter.m_FirstLock		= true			
	self.m_LockCenter.m_Speed			= self.m_MoveSpeed
	
	if Type == ECharacterType.IntObj then
		return
	end
	
	--这里判断 如果选中目标是NPC
	if(Target:CppIsAlive() and bDoNormalAttack) and g_MainPlayer:IsEnemy(Target) 
		and (self.m_LockCenter.m_LockingObj.m_CanNormalAttack or self:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle))then
		if not g_MainPlayer:IsNormalAttacking() then
			if( self.m_LockCenter.m_RenderChild and
				self.m_LockCenter.m_LockingObj ) then
				self:DestroyLockObjFx(self.m_LockCenter.m_LockingObj)
			end
			self:CreateLockObjFx(self.m_LockCenter.m_LockingObj)
		end

		local bIsNeedAutoTracker = true
		if g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
			bIsNeedAutoTracker = false
		end
		if not g_MainPlayer.m_PickOre then
			self:DoNormalAttack(targetID, bIsNeedAutoTracker)
		end
	end		
end
-- 鼠标左键按下
function CCharacterDirector:OnLButtonDown()
	--print("CCharacterDirector:OnLButtonDown")
	self:UpdateSelectPos()
	local state, context = g_WndMouse:GetCursorState()
	if context ~= nil then --手上有物品
		return
	end
	
	if( GroundSelector:IsActive() ) then
		if g_GameMain.m_ShootProgressWnd.m_Tick then
			g_GameMain.m_ShootProgressWnd:BeginAccumulatePower()
		end
		return
	end
	
	if self.m_IsWaitShowWnd then
		return
	end
	
	if g_GameMain.m_XLProgressWnd.m_IsShowWnd then
		g_GameMain.m_XLProgressWnd:BeginXLToProgress()
		return
	elseif g_GameMain.m_XLProgressWnd.m_IsButtonDown then
		return
	end
	
	self:RunToMouse_UnRegisterTick()
	assert(g_CoreScene ~= nil )
	local Target = CEntityClient_GetSelectedEntity(g_CoreScene)
	local isCanSelect = true
	if Target ~= nil then
		if g_GameMain.m_ForbidSelectOtherPlayer and Target.m_Properties:GetType() == ECharacterType.Player then
			isCanSelect = false
		end
	else
		isCanSelect = false
	end
	if isCanSelect then
		if g_GameMain.m_SysSetting.m_MouseSettingWnd:OpenAttackOnLButtonDown() then --开启攻击
			self:OnSelectObj(Target, true)
		elseif g_GameMain.m_SysSetting.m_MouseSettingWnd:OpenSelectOnLButtonDown() then--开启选择
			self:OnSelectObj(Target, false)
		end
		if g_GameMain.m_SysSetting.m_MouseSettingWnd:CanLButtonClickObj() then
			local bSucc = self:OnLButtonClickObj(Target)
		end
		if (not bSucc and g_GameMain.m_SysSetting.m_MouseSettingWnd:OpenMoveOnLButtonDown() )then --仅开启移动
			self:OnSelectGround()
		end
	else
		if (g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag)then --开启移动
			self:OnSelectGround()
		end
	end
end

--鼠标左键弹起
function CCharacterDirector:OnLButtonUp()
	if( GroundSelector:IsActive() ) then
		--if g_GameMain.m_HoldThingName ~= nil then
		--	Gac2Gas:ActiveBehavior(g_Conn)
		--else
			GroundSelector:OnLButtonUp()
		--end
	end
	if g_GameMain.m_XLProgressWnd.m_IsButtonDown then
		g_GameMain.m_XLProgressWnd:SendXLNum()
	end
	
	--如果抱着东西的话,就放下
	if not g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
		if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= ""	then
			if g_GameMain.m_HoldThingType == "抱" then
				g_MainPlayer:StopMoving()
				Gac2Gas:ActiveBehavior(g_Conn)
				return
			end
		end
	end
	
	if g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
		self:MoveOnTime_UnRegisterTick()
	end
	if g_GameMain.m_SysSetting.m_MouseSettingWnd:CanUnLockTargetOnLButtonUp() then
		self:UnLockObj()
		self:UnLockIntObj()
	end
end

--鼠标右键按下
function CCharacterDirector:OnRButtonDown()
	self:UpdateSelectPos()
	
	if self.m_IsWaitShowWnd then
		return
	end
	local state, context = g_WndMouse:GetCursorState()
	if context ~= nil then --手上有物品
		return
	end
	
	if GroundSelector:IsActive() then
		return
	end
	-- 判断是否被动，以发送取消被动Move的Notify (by wb 10.24)
	self:RunToMouse_UnRegisterTick()
	
	local Target = CEntityClient_GetSelectedEntity(g_CoreScene)
	if( Target ~= nil ) then
		if g_GameMain.m_SysSetting.m_MouseSettingWnd:OpenAttackOnRButtonDown() then		--开启攻击
			self:OnSelectObj(Target, true)
		elseif g_GameMain.m_SysSetting.m_MouseSettingWnd:OpenSelectOnRButtonDown() then	--开启选择
			self:OnSelectObj(Target, false)
		end
		if g_GameMain.m_SysSetting.m_MouseSettingWnd:CanRButtonClickObj() then
			local bSucc = self:OnLButtonClickObj(Target)
		end
		if (not bSucc and g_GameMain.m_SysSetting.m_MouseSettingWnd:OpenMoveOnRButtonDown()) then	--开启移动
			self:OnSelectGround()
		end
	else
		if (not g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag) then --开启移动
			self:OnSelectGround()
		end
	end
end

--鼠标右键弹起
function CCharacterDirector:OnRButtonUp()
	if not g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
		self:MoveOnTime_UnRegisterTick()
	end
			
	if g_GameMain.m_SysSetting.m_MouseSettingWnd:CanUnLockTargetOnRButtonUp() then
		self:UnLockObj()
		self:UnLockIntObj()
	end
	
	if GroundSelector:IsActive() then
		GroundSelector:CancelSelectGround()
		return
	else
		
		--如果正要踢,就取消
		if g_GameMain.m_XLProgressWnd.m_IsShowWnd then
			g_GameMain.m_XLProgressWnd:StopXLFromProgress()
			return
		elseif g_GameMain.m_XLProgressWnd.m_IsButtonDown then
			return
		end
		--如果抱着东西的话,就放下
		if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= ""	then
			if g_GameMain.m_HoldThingType == "抱" then
				if g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
					g_MainPlayer:StopMoving()
					Gac2Gas:ActiveBehavior(g_Conn)
					return
				end
			else
				if g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
					Gac2Gas:QuitFitoutState(g_Conn)
					return
				end
			end
		end
		
	end
end

function CompareSlope(x1, y1, x2, y2, CompRet, K)
	local Result

	if x1-x2 == 0 then
		if y1-y2 > 0 then -- CompK为正无穷
			if CompRet == 1 then -- K为正无穷, K == CompK
				Result = 0
			else -- K非正无穷, K < CompK
				Result = -1
			end
		else -- CompK为负无穷或0
			if CompRet == -1 then -- K也为负无穷, K == CompK
				Result = 0
			else -- K非负无穷, K > CompK
				Result = 1
			end
		end
	else
		if CompRet == 0 then
			CompK = (y1-y2) / (x1-x2)
			if K > CompK then
				Result = 1
			else
				Result = -1
			end
		end
	end

	return Result
end

-- 得到键盘移动目的坐标
function CCharacterDirector:GetDestPosByDirAndArc( Direction, Arc, ArcRotate )
	local Xmax = EUnits.eGridSpan * g_CoreScene:GetMetaScene():GetWidthInGrid()
	local Ymax = EUnits.eGridSpan * g_CoreScene:GetMetaScene():GetHeightInGrid()
	local SinA = math.sin(Arc)
	local CosA = math.cos(Arc)
	local SinOA = math.sin(ArcRotate)
	local CosOA = math.cos(ArcRotate)
	local StepX, StepY
	local DestPos = CFPos:new()
	local PlayerPos = CFPos:new()
	self:GetPixelPos(PlayerPos)
	
	if Direction == EDirectionWord.Up or Direction == EDirectionWord.LeftUp then
		StepX = 0
		StepY = EUnits.eGridSpan * 10
	elseif Direction == EDirectionWord.Down or Direction == EDirectionWord.RightDown then
		StepX = 0
		StepY = EUnits.eGridSpan * -10
	elseif Direction == EDirectionWord.Left or Direction == EDirectionWord.LeftDown then
		StepX = EUnits.eGridSpan * -10
		StepY = 0
	elseif Direction == EDirectionWord.Right or Direction == EDirectionWord.RightUp then
		StepX = EUnits.eGridSpan * 10
		StepY = 0
	else
		StepX = 0
		StepY = 0
	end
	
	if Direction == EDirectionWord.Up or Direction == EDirectionWord.Down 
		or Direction == EDirectionWord.Left or Direction == EDirectionWord.Right then
		DiffX = StepX * CosA + StepY * SinA
		DiffY = StepY * CosA - StepX * SinA
	else
		DiffX = StepX * CosOA + StepY * SinOA
		DiffY = StepY * CosOA - StepX * SinOA
	end

	-- 判断终点所处象限
	-- 求斜率，判断与哪条直线相交
	local CompRet = 0 -- K与CompK的比较结果, 0为==, 1为>, -1为<
	local K
	local CompK
	-- DiffX与DiffY不会同为0
	if DiffX == 0 then
		if DiffY > 0 then -- K正无穷
			CompRet = 1
		else -- K负无穷
			CompRet = -1
		end
	else
		K = DiffY / DiffX
	end

	if DiffX > 0 and DiffY >= 0 then
		-- CompK = (Ymax-PlayerPos.y) / (Xmax-PlayerPos.x)
		CompRet = CompareSlope(Xmax, Ymax, PlayerPos.x, PlayerPos.y, CompRet, K);

		if CompRet == 1 then -- 和(0,y),(x,y)相交
			if DiffY == 0 then
				DestPos.x = 0
				DestPos.y = PlayerPos.y
			else
				DestPos.x = DiffX * (Ymax-PlayerPos.y) / DiffY + PlayerPos.x
				DestPos.y = Ymax - 1
			end
		else -- 和(x,0),(x,y)相交
			DestPos.x = Xmax - 1
			DestPos.y = (Xmax-PlayerPos.x) * DiffY / DiffX + PlayerPos.y
		end
	elseif DiffX < 0 and DiffY <= 0 then
		-- CompK = (0-PlayerPos.y) / (0-PlayerPos.x)
		CompRet = CompareSlope(0, 0, PlayerPos.x, PlayerPos.y, CompRet, K);

		if CompRet == 1 then -- (0,-y),(-x,-y)
			if DiffY == 0 then
				DestPos.x = Xmax - 1
				DestPos.y = PlayerPos.y
			else
				DestPos.x = DiffX * (0-PlayerPos.y) / DiffY + PlayerPos.x
				DestPos.y = 0
			end
		else -- (-x,0),(-x,-y)
			DestPos.x = 0
			DestPos.y = (0-PlayerPos.x) * DiffY / DiffX + PlayerPos.y
		end
	elseif DiffX <= 0 and DiffY > 0 then
		-- CompK = (Ymax-PlayerPos.y) / (0-PlayerPos.x)
		CompRet = CompareSlope(0, Ymax, PlayerPos.x, PlayerPos.y, CompRet, K);

		if CompRet == 1 then -- (0,0),(0,y)
			if DiffX == 0 then
				DestPos.x = PlayerPos.x
				DestPos.y = Ymax - 1
			else
				DestPos.x = 0
				DestPos.y = (0-PlayerPos.x) * DiffY / DiffX + PlayerPos.y
			end
		else -- (x,y),(0,y)
			DestPos.x = DiffX * (Ymax-PlayerPos.y) / DiffY + PlayerPos.x
			DestPos.y = Ymax - 1
		end
	else -- DiffX => 0 and DiffY < 0
		-- CompK = (0-PlayerPos.y) / (Xmax-PlayerPos.x)
		CompRet = CompareSlope(Xmax, 0, PlayerPos.x, PlayerPos.y, CompRet, K);

		if CompRet == 1 then -- (x,y),(x,0)
			if DiffX == 0 then
				DestPos.x = PlayerPos.x
				DestPos.y = Ymax - 1
			else
				DestPos.x = Xmax - 1
				DestPos.y = (Xmax-PlayerPos.x) * DiffY / DiffX + PlayerPos.y
			end
		else -- (0,0),(x,0)
			DestPos.x = DiffX * (0-PlayerPos.y) / DiffY + PlayerPos.x
			DestPos.y = 0
		end
	end
	
	return DestPos
end

function CCharacterDirector:GetDestPosKeyBorad( Direction )
	local RenderScene = g_CoreScene:GetRenderScene()
	local Arc = RenderScene:GetCameraYDegree() + RenderScene:GetCameraYRotate()
	local ArcRotate = RenderScene:GetCameraYRotate()
	local DestPos = self:GetDestPosByDirAndArc( Direction, Arc, ArcRotate )
	return DestPos
end

-- 键盘寻路 --
function CCharacterDirector:MoveByDir( Direction )
	local DestPos = self:GetDestPosKeyBorad( Direction )
	local Result = self:MoveToInLinePath( DestPos, EFindPathType.eFPT_HypoLine, EBarrierType.eBT_LowBarrier)
	if Result ~= EMoveToResult.eMTR_Success then
		self:MoveByGlide()
	end
	return Result
end

function CCharacterDirector:MoveByGlide_UnRegisterTick()
	if( self.MoveByGlideTick ) then
		UnRegisterTick( self.MoveByGlideTick )
		self.MoveByGlideTick = nil
	end
end

function CCharacterDirector:GetModifyDirection()
	local NumToDirection = {
		[0] = EDirectionWord.RightUp,
		[1] = EDirectionWord.RightDown,
		[2] = EDirectionWord.LeftDown,
		[3] = EDirectionWord.LeftUp
	}
	local CircleArc		= math.pi * 2
	local QuarterArc	= CircleArc / 4
	local EighthArc		= CircleArc / 8
	local Direction		= g_GameMain.m_SysSetting.m_KPmap:GetMoveDirection()
	local RenderScene	= g_CoreScene:GetRenderScene()
	local ArcRotate		= RenderScene:GetCameraYRotate()
	if Direction == EDirectionWord.Right or Direction == EDirectionWord.Down or
		Direction == EDirectionWord.Left or Direction == EDirectionWord.Up then
		ArcRotate = ArcRotate + EighthArc
	end
	if ArcRotate >= 0 then
		ArcRotate = ArcRotate - CircleArc * math.floor(ArcRotate/CircleArc)
	else
		ArcRotate = ArcRotate + CircleArc * math.floor((-ArcRotate)/CircleArc + 1)
	end
	local QuarterCount = math.floor(ArcRotate/QuarterArc)
	
	local index
	if Direction == EDirectionWord.RightUp then
		index = 0 + QuarterCount
	elseif Direction == EDirectionWord.Right then
		index = 0 + QuarterCount
	elseif Direction == EDirectionWord.RightDown then
		index = 1 + QuarterCount
	elseif Direction == EDirectionWord.Down then
		index = 1 + QuarterCount
	elseif Direction == EDirectionWord.LeftDown then
		index = 2 + QuarterCount
	elseif Direction == EDirectionWord.Left then
		index = 2 + QuarterCount
	elseif Direction == EDirectionWord.LeftUp then
		index = 3 + QuarterCount
	elseif Direction == EDirectionWord.Up then
		index = 3 + QuarterCount
	else
		return -1
	end
	if index >= 4 then
		index = index - 4
	end
	return NumToDirection[index]
end

function CCharacterDirector:GetOtherModifyDirection( Direction )
	if Direction == EDirectionWord.RightUp then
		return EDirectionWord.RightDown
	elseif Direction == EDirectionWord.RightDown then
		return EDirectionWord.LeftDown
	elseif Direction == EDirectionWord.LeftDown then
		return EDirectionWord.LeftUp
	elseif Direction == EDirectionWord.LeftUp then
		return EDirectionWord.RightUp
	else
		--print("Direction有误")
		return -1	
	end
end

function CCharacterDirector:MoveByGlide()
	local Direction = self:GetModifyDirection()
	if Direction == -1 then
		return
	end
	local RenderScene = g_CoreScene:GetRenderScene()
	local Arc = RenderScene:GetCameraYDegree()
	local DestPosA = self:GetDestPosByDirAndArc(Direction, Arc, 0)
	local eResult = self:MoveToInLinePath( DestPosA, EFindPathType.eFPT_HypoLine, EBarrierType.eBT_LowBarrier )
	if EMoveToResult.eMTR_Success ~= eResult then
		Direction = self:GetOtherModifyDirection(Direction)
		if Direction == -1 then
			return
		end
		local DestPosB = self:GetDestPosByDirAndArc(Direction, Arc, 0)
		eResult = self:MoveToInLinePath( DestPosB, EFindPathType.eFPT_HypoLine, EBarrierType.eBT_LowBarrier)
	end
	if EMoveToResult.eMTR_Success == eResult then
		self:MoveByGlide_UnRegisterTick()
		self.MoveByGlideTick = RegClassTick( "MoveByGlide_OnTick", CCharacterDirector.MoveByGlide_OnTick , 500, self)
	else
		self:MoveByGlide_UnRegisterTick()
	end
end

function CCharacterDirector:MoveByGlide_OnTick( Tick )
	if self ~= nil then
		local Direction = g_GameMain.m_SysSetting.m_KPmap:GetMoveDirection()
		DestPos = self:GetDestPosKeyBorad( Direction )
		local eResult = self:MoveToInLinePath( DestPos, EFindPathType.eFPT_HypoLine, EBarrierType.eBT_LowBarrier)
		if EMoveToResult.eMTR_Success == eResult then
			self:MoveByGlide_UnRegisterTick()
		end
	end
end

------------------------------------------------------------ 以下是主角状态 ------------------------------------------------------------
-- 在移动前做转向
function CCharacterDirector:BeforeMoveToSelectPos( InPos, Direction, bForce)
	if (not self:CppIsAlive()) or self.m_IsWaitShowWnd then
		return
	end
	if(self:CppGetCtrlState(EFighterCtrlState.eFCS_AllowChangeDirection)) then
		local vecPos = CVector3f:new()	
		self:MoveToSelectPosAllowChangeDirection( InPos, vecPos )
		return
	end

	if self:GetActionState() == EActionState.eAS_Cast then
		self:MoveToSelectPos( InPos, Direction )
		return
	end

	if self:CppGetCtrlState( EFighterCtrlState.eFCS_InBattle ) and self.m_LockCenter.m_LockingObj then
		self:MoveToSelectPos( InPos, Direction )
	else
		self:ActionTurn_UnRegisterTick()
		
		if self:CppGetCtrlState( EFighterCtrlState.eFCS_FeignDeath ) then
			self:MoveToSelectPos( InPos, Direction )
			return
		end
		
		local vecPosSelect = CVector3f:new()
		local posSelect = CFPos:new()
		if InPos == nil then
			local RenderScene = g_CoreScene:GetRenderScene()
			RenderScene:GetSelectedPos (vecPosSelect)
			local ratio = EUnits.eGridSpanForObj / EUnits.eGridSpan
			posSelect.x = vecPosSelect.x*ratio
			posSelect.y = vecPosSelect.z*ratio
		else
			posSelect.x = InPos.x
			posSelect.y = InPos.y
		end
		
		g_MainPlayer.TabAgain = true

		local dirSelect = CDir:new()	
		if Direction ~= nil then
			dirSelect.uDir = self:GetDirByKeyboard(Direction)
		else
			local posSelf = CFPos:new() 
			self:GetPixelPos( posSelf )
			local vecPos = CVector2f:new()
			vecPos.x = posSelect.x - posSelf.x
			vecPos.y = posSelect.y - posSelf.y
			dirSelect:Set(vecPos)
		end
		
		if g_MainPlayer.m_PickOre then
			g_MainPlayer.m_uDir = dirSelect.uDir
		end
		
		if self:IsStopBeforeTurn(dirSelect.uDir) and (not bForce) then
			self:StopMoving()
			self:TurnAndSyncDir(dirSelect.uDir)
			self.ActionTurn = RegClassTick( "ActionTurn_OnTick", CCharacterDirector.ActionTurn_OnTick , 33, self, posSelect, Direction)
		else
			self:MoveToSelectPos( posSelect, Direction )
		end
	end
end

function CCharacterDirector:GetDirByKeyboard(Direction)
	local RenderScene = g_CoreScene:GetRenderScene()
	local YRotate = RenderScene:GetCameraYRotate()
	local DirFix = YRotate * 256 / (3.1415926 * 2)
	local Dir	
	if Direction == EDirectionWord.Up then
		Dir = 32
	elseif Direction == EDirectionWord.Down then
		Dir = 160
	elseif Direction == EDirectionWord.Left then
		Dir = 224
	elseif Direction == EDirectionWord.Right then
		Dir = 96 
	elseif Direction == EDirectionWord.LeftUp then
		Dir = 0
	elseif Direction == EDirectionWord.LeftDown then
		Dir = 192
	elseif Direction == EDirectionWord.RightUp then
		Dir = 64
	elseif Direction == EDirectionWord.RightDown then
		Dir = 128
	end
	Dir = (Dir + DirFix) % 256 
	return Dir
end

function CCharacterDirector:ActionTurn_UnRegisterTick()
	if( self.ActionTurn ) then
		UnRegisterTick( self.ActionTurn )
		self.ActionTurn = nil
	end
end

function CCharacterDirector:ActionTurn_OnTick(Tick, InPos, Direction )
	if self ~= nil then
		if self:ActionIsTurnOver() then 
			self:ActionTurn_UnRegisterTick()
			self:MoveToSelectPos( InPos, Direction )
		end
	end
end

-- 移动到选定位置
function CCharacterDirector:MoveToSelectPos( InPos, Direction )
	g_GameMain.m_LastMoveToPos = nil
	if g_GameMain.m_PrepareChangeTick or g_App.m_Loading:IsShow() then
		return
	end
	if g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint then
		ClearAutoTrackState()
	end
	local vecPos = CVector3f:new()
	local pos = CFPos:new()
	if( InPos == nil ) then -- 走该路线
		local RenderScene = g_CoreScene:GetRenderScene()
		RenderScene:GetSelectedPos (vecPos)
		local ratio = EUnits.eGridSpanForObj / EUnits.eGridSpan
		pos.x = vecPos.x*ratio
		pos.y = vecPos.z*ratio
	else
		pos.x = InPos.x
		pos.y = InPos.y
		
	end
	if self:IsPassive() == true then
		if(self:CppGetCtrlState(EFighterCtrlState.eFCS_ForbitMove)) then -- 没执行
			if g_GameMain.m_skillProgress_Load:IsShow() and self:IsCastingTypeSkill(g_GameMain.m_skillProgress_Load.m_Skillname) then
				g_MainPlayer:CancelCastingProcess()--即时禁止移动，试图移动的操作也要打断流程
			end
		end
	else -- 鼠标、键盘均走该分支
		self:LockObj_UnRegisterTick()
		self:CancelAutoTracker(false)
		
		local MoveResult	
		if Direction == nil then -- 鼠标寻路
			MoveResult = self:MoveToInAStarPath(pos, EFindPathType.eFPT_HypoAStarOpti, EBarrierType.eBT_LowBarrier, 0, 100)
			g_GameMain.m_LastMoveToPos = {}
			g_GameMain.m_LastMoveToPos.SceneName = g_GameMain.m_SceneName
			g_GameMain.m_LastMoveToPos.Pos = InPos
		else -- 键盘方向键直线寻路
			MoveResult = self:MoveByDir(Direction)
		end
		if MoveResult ~= eMTR_Success then --移动不成功，比如遇到障碍，试图移动的操作也要打断流程
			if g_GameMain.m_skillProgress_Load:IsShow() and self:IsCastingTypeSkill(g_GameMain.m_skillProgress_Load.m_Skillname) then
				g_MainPlayer:CancelCastingProcess()
			end
		end
	end
	g_MainPlayer.TabAgain = true
	return vecPos	
end

function CCharacterDirector:MoveToSelectPosAllowChangeDirection( InPos, vecPos )
	local pos = CFPos:new()
	if( InPos == nil ) then
		local RenderScene = g_CoreScene:GetRenderScene()
		RenderScene:GetSelectedPos (vecPos)
		local ratio = EUnits.eGridSpanForObj / EUnits.eGridSpan
		pos.x = vecPos.x*ratio
		pos.y = vecPos.z*ratio
	else
		pos.x = InPos.x
		pos.y = InPos.y
	end

	--print("		CCharacterDirector:MoveToSelectPos(" .. pos.x .. ", " .. pos.y .. ")\n") 

	self:LockObj_UnRegisterTick()
	--if(self:IsNormalAttacking()) then
	self:CancelAutoTracker(false)
	--end			
	local curSpeed = self:GetCurSpeed()
	self:NotifyServerChangeDir( pos )
	--local MoveResult = self.m_CoreObj:MoveTo( pos, EFindPathType.eFPT_HypoLine, EBarrierType.eBT_LowBarrier, 0 )
		
	return vecPos;
end

---------------------------------------------------- 锁定/追踪 目标 ----------------------------------------------------

-- yy 添加新的功能 
-- 当玩家选中OBJ 等的时候 面向目标
function CCharacterDirector:LockIntObj(SelectObj)	
	self.m_IsShowListWnd = nil
	if not self:IsMoving() and not self:CppGetCtrlState( EFighterCtrlState.eFCS_ForbitTurnAround ) 
		and not self:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
		self:ClickTargetSetDir(SelectObj)
	end
	if( self.m_LockCenter.m_LockingIntObj ) then
		if( self.m_LockCenter.m_LockingIntObj == SelectObj ) then
			return
		end
		self:UnLockIntObjExceptFace()
	end
	
	if( self.m_LockCenter.m_LockingObj ) then
		local TargetType = self.m_LockCenter.m_LockingObj.m_Properties:GetType()
		if TargetType == ECharacterType.Npc then
			self.m_LockCenter.m_LockingObj:FunNpcActionEnd(self)
		end
		self:UnLockObjSimple()	
	end
	
	self.m_LockCenter.m_LockingIntObj			= SelectObj
	self.m_LockCenter.m_TouchAttack = false	
	
	local renderScene = g_CoreScene:GetRenderScene()

	self:CreateLockObjFx(SelectObj)
	
--[[
	if( SelectObj ~= self ) then
		self:CreateFacingPair( self, SelectObj )
	else
		self:RemoveFacingPair()
	end
--]]	
	
	--更新交互对象头像窗口
	g_GameMain.m_TargetInfo:UpdateIntObj()
	Gac2Gas:OnSelectIntObj(g_Conn,SelectObj:GetEntityID())
	return true
end

function CCharacterDirector:UnLockIntObj()
	Gac2Gas:UnLockIntObj( g_Conn )
	self:RemoveFacingPair()
	self:UnLockIntObjExceptFace()
end

-- LockIntObj()调用 不控制 面部朝向
function CCharacterDirector:UnLockIntObjExceptFace()
	--关闭小游戏
	if g_GameMain.m_SmallGame or g_GameMain.m_DoSmallGame then
		MsgClient(29)
		g_GameMain.m_SGMgr:CancelSmallGame()
	end
		
	if(self.m_LockCenter.m_LockingObj) then --去掉剪影
		g_GameMain.m_TargetInfo:SetLockObjRanderStyle(self.m_LockCenter.m_LockingObj, false)
	end
	
	g_GameMain.m_TargetOfTargetInfo:Close()
	g_GameMain.m_TargetInfo:ClosePanel()
	
	if( self.m_LockCenter.m_LockingIntObj ) then
		if( self.m_LockCenter.m_RenderChild ) then
			self:DestroyLockObjFx(self.m_LockCenter.m_LockingIntObj)
		end		
		self.m_LockCenter.m_LockingIntObj		= nil
		
		self.m_LockCenter.m_TouchAttack = false	
	end
end

function CCharacterDirector:LockObj( SelectObj )
	self.m_IsShowListWnd = nil
	--print( "--------------------------- CCharacterDirector:LockObj(", SelectObj, ") -----------------------------" )	
	if( self.m_LockCenter.m_LockingObj ) then
		if( self.m_LockCenter.m_LockingObj == SelectObj ) then
			self.m_LockCenter.m_LockingObj.m_CanNormalAttack = true
			return
		end
		
		local TargetType = self.m_LockCenter.m_LockingObj.m_Properties:GetType()
		if TargetType == ECharacterType.Npc then
			self.m_LockCenter.m_LockingObj:FunNpcActionEnd(self)
		end
		
		self:UnLockObjSimple()
	else
		g_GameMain.m_TargetInfo:ClosePanel()
		g_GameMain.m_TargetOfTargetInfo:Close()
		CTargetBuffWnd.GetWnd():ClearAllBuffState()
	end
	
	if( self.m_LockCenter.m_LockingIntObj ) then
		self:UnLockIntObjExceptFace()
	end
	
	self.m_LockCenter.m_LockingObj			= SelectObj
	--远程职业需要2次选中目标才开始普通攻击
	local selfClass = g_MainPlayer:CppGetClass()
	if selfClass == EClass.eCL_NatureElf or			--法师
		selfClass == EClass.eCL_EvilElf or			--邪魔
		selfClass == EClass.eCL_Priest or			--牧师
		selfClass == EClass.eCL_ElfHunter then		--精灵弓箭手
		self.m_LockCenter.m_LockingObj.m_CanNormalAttack = false
	else --大剑士,魔剑士,兽人战士,骑士,矮人骑士
		self.m_LockCenter.m_LockingObj.m_CanNormalAttack = true
	end
	
	local renderScene = g_CoreScene:GetRenderScene()
	local selectedObj = CEntityClient_GetSelectedEntity(g_CoreScene)
	local targetID

	self.m_LockCenter.m_TouchAttack = false

	if(SelectObj == nil and selectedObj ~= nil) then
		self:CreateLockObjFx(selectedObj)
		targetID = selectedObj:GetEntityID()
	elseif(SelectObj ~= nil) then
		local selectedEntityType = SelectObj:GetGameEntityType()
		if(selectedEntityType == EGameEntityType.eGET_CharacterDirector or selectedEntityType == EGameEntityType.eGET_CharacterFollower) then
			targetID = SelectObj:GetEntityID()
			self:CreateLockObjFx(SelectObj)
			SelectObj:SetAgileValueBeCare(true)
		elseif(selectedEntityType == EGameEntityType.eGET_IntObject) then
			--print("交互对象")
		end
	else
		return
	end

	--设定目标，技能条件判断时使用
	g_MainPlayer:SetTarget( targetID)
	local TargetChar = CCharacterFollower_GetCharacterByID(targetID)
	if TargetChar and g_MainPlayer:IsEnemy(TargetChar) then
		g_MainPlayer:OpenObjPanel(targetID, g_GameMain.m_TargetInfo.pViewInfo, true, true)		
	end

	--头部朝向	
	if( SelectObj ~= self ) then
		self:CreateFacingPair( self, SelectObj )
	else
		self:RemoveFacingPair()
	end
	
	--更新目标头像窗口
	g_GameMain.m_TargetInfo:ShowAndUpdate()
	g_GameMain.m_TargetInfo:SetLockObjRanderStyle(SelectObj, true)
	
	--目标头顶文字变色
	if(SelectObj.m_HeadInfoWnd) then
		SelectObj.m_HeadInfoWnd:TempColor()
		local camp = SelectObj:CppGetCamp()
		if( g_GameMain.m_CharacterInSyncMgr:IsNeedShowNameNpc(camp) ) then
			SelectObj.m_HeadInfoWnd:ShowWnd(true)
		end
		g_GameMain.m_CharacterInSyncMgr:SetHeadInfoTransparent(SelectObj, 2)
	end
	
	--更新目标的目标
	self:GetTargetOfTarget(targetID)
	
	--更新目标BuffDebuff ， 调用C++的函数刷新
	if( self.m_LockCenter.m_LockingObj ) then
		self.m_LockCenter.m_LockingObj:SetTargetBuff()
	end
	--更新目标CastingProcess
	g_GameMain.m_TargetInfo:OnCastEnd()
	self:GetTargetCastingProcess(targetID)
			
	return true
end

function CCharacterDirector:UnLockTarget()
	if self.m_LockCenter.m_LockingObj then
		self:UnLockObj()
	elseif self.m_LockCenter.m_LockingIntObj then
		self:UnLockIntObj()
	elseif IsFunction(g_GameMain.m_TargetInfo.IsShow) 
		and g_GameMain.m_TargetInfo:IsShow() then
		CTargetBuffWnd.GetWnd():ClearAllBuffState()
		g_GameMain.m_TargetInfo:ClosePanel()
	end
end

function CCharacterDirector:MoveToLockObj()
	-- 如果当前已经在追逐一个对象，则先终止这个追逐
	--self:LockObj_UnRegisterTick()
	--self.m_LockCenter.m_Ticker		= RegisterTick( "LockObj_OnTick", CCharacterDirector.LockObj_OnTick , 500, self)
	if not(self.m_LockCenter.m_LockingObj) then
		return
	end
	
	local TargetType = self.m_LockCenter.m_LockingObj.m_Properties:GetType()
	if TargetType == ECharacterType.Player then--只能跟随玩家
		self:CancelAutoTracker(false)
		MsgClient(3053,self.m_LockCenter.m_LockingObj.m_Properties:GetCharName())
		self:InitAutoTracker(self:GetEntityID(), self.m_LockCenter.m_LockingObj:GetEntityID(), 1.5, AutoTrackerHandlerForFollowingPlayer:new(), false)
	end
end

function CCharacterDirector:UnLockObj(bKeepFigure)
	local Target = self.m_LockCenter.m_LockingObj
	if Target ~= nil then
		local TargetType = Target.m_Properties:GetType()
		if TargetType == ECharacterType.Npc then
			Target:FunNpcActionEnd(self)
		end
		Gac2Gas:UnLockObj( g_Conn )
		self:SetTarget(0)
	end
	self:RemoveFacingPair()
	self:UnLockObjSimple(bKeepFigure)
end

-- LockObj()调用 不控制 面部朝向
function CCharacterDirector:UnLockObjSimple(bKeepFigure)
	-- 去掉脚下光圈
	if( self.m_LockCenter.m_RenderChild and
		self.m_LockCenter.m_LockingObj ) then
		self:DestroyLockObjFx(self.m_LockCenter.m_LockingObj)
	end
	--关闭小游戏状态
	if g_GameMain.m_SmallGame or g_GameMain.m_DoSmallGame then
		MsgClient(29)
		g_GameMain.m_SGMgr:CancelSmallGame()
	end
	
	if(self.m_LockCenter.m_LockingObj) then
		--去掉剪影
		g_GameMain.m_TargetInfo:SetLockObjRanderStyle(self.m_LockCenter.m_LockingObj, false)
		local targetID = self.m_LockCenter.m_LockingObj:GetEntityID()
		local TargetChar = CCharacterFollower_GetCharacterByID(targetID)
		if TargetChar and self:IsEnemy(TargetChar) then
			self:OpenObjPanel(targetID, g_GameMain.m_TargetInfo.pViewInfo, true, false)
			self:CancelNormalAttack()
		end
		--恢复头顶文字颜色
		if(self.m_LockCenter.m_LockingObj.m_HeadInfoWnd) then
			self.m_LockCenter.m_LockingObj.m_HeadInfoWnd:SelfColor()
			local camp = self.m_LockCenter.m_LockingObj:CppGetCamp()
			if(self.m_LockCenter.m_LockingObj ~= g_GameMain.m_CharacterInSyncMgr.m_SelectObj) then
				if( g_GameMain.m_CharacterInSyncMgr:IsNeedShowNameNpc(camp) ) then
					self.m_LockCenter.m_LockingObj.m_HeadInfoWnd:ShowWnd(false)
					self.m_LockCenter.m_LockingObj.m_HeadInfoWnd:ShowWndForQuestNeed(TargetChar.m_Properties:GetCharName(), ECharacterType.Npc)
				end
				g_GameMain.m_CharacterInSyncMgr:SetHeadInfoTransparent(self.m_LockCenter.m_LockingObj, 1)
			end
		end
	end
	
	g_GameMain.m_TargetOfTargetInfo:Close()
	local TargetBuff = CTargetBuffWnd.GetWnd()
	TargetBuff:ClearAllBuffState()
	g_GameMain.m_TargetInfo:OnCastEnd()
	-- 去掉头像显示
	if not bKeepFigure then
		g_GameMain.m_TargetInfo:ClosePanel()
	end
	if g_GameMain.m_AimPlayerManageWnd:IsShow() then
		g_GameMain.m_AimPlayerManageWnd:ShowWnd(false)
	end
	
	if self.m_LockCenter.m_LockingObj and self.m_LockCenter.m_LockingObj ~= self and
		self.m_LockCenter.m_LockingObj~=self:GetServant(ENpcType.ENpcType_BattleHorse) and
		self.m_LockCenter.m_LockingObj~=self:GetServant(ENpcType.ENpcType_Summon) then
		local EntityType = self.m_LockCenter.m_LockingObj:GetGameEntityType()
		if(EntityType == EGameEntityType.eGET_CharacterDirector or EntityType == EGameEntityType.eGET_CharacterFollower) then
			local lockObj = self.m_LockCenter.m_LockingObj
			if(not g_GameMain.m_TeamBase:IsTeamMember(lockObj:GetEntityID())) then
				lockObj:SetAgileValueBeCare(false)
			end
		end
	end
	
	self.m_LockCenter.m_TouchAttack = false
	self.m_LockCenter.m_LockingObj	= nil
end

function CCharacterDirector:LockObj_UnRegisterTick()
	if( self.m_LockCenter.m_Ticker ) then
		UnRegisterTick( self.m_LockCenter.m_Ticker )
		self.m_LockCenter.m_Ticker		= nil
	end
end

function CCharacterDirector:LockObj_OnTick()

	local MyLockCenter = self.m_LockCenter
	
	if( MyLockCenter.m_LockingObj == nil ) then
		self:UnLockObj()
		return
	end
	if( self:IsPassive() == false and not self:IsMoving() and
		(not self:IsNormalAttacking())) then
		
		local pos1
		local pos2
		
		pos1 = CFPos:new()
		self:GetPixelPos (pos1)
		pos2 = CFPos:new()
		MyLockCenter.m_LockingObj:GetPixelPos (pos2)
		
		local dx = pos2.x - pos1.x
		local dy = pos2.y - pos1.y
		
		pos1 = CPos:new()
		pos2 = CPos:new()		
		self:GetGridPos( pos1 )
		MyLockCenter.m_LockingObj:GetGridPos( pos2 )
		local Distance = math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )
		
		if( Distance <= MyLockCenter.m_StopDistance ) then 
			local bDoTouch = false
			if( self:IsMoving() ) then
				self:StopMoving()
			
				local Dir = CDir:new()
				local vecDir = CVector2f:new() 
				vecDir.x = dx
				vecDir.y = dy
				Dir:Set( vecDir )
				self.m_uDir = Dir.uDir
				self:SetDirection( Dir , false )
				bDoTouch = true
			end
			if( self ~= MyLockCenter.m_LockingObj and bDoTouch or MyLockCenter.m_FirstLock == true ) then
				MyLockCenter.m_FirstLock = false
				self:Touch( MyLockCenter.m_LockingObj )
				if( MyLockCenter.m_LockingObj.OnTouch ) then
					MyLockCenter.m_LockingObj.OnTouch( MyLockCenter.m_LockingObj, self )
				end
			end
			
		else
			local pos3 = CFPos:new()
			MyLockCenter.m_LockingObj:GetPixelPos (pos3)
			local distPos = CFPos:new()
			local srcPos = CFPos:new()
			self:GetPixelPos(srcPos)
			distPos = GetReachDist(srcPos, pos3, 1 * EUnits.eGridSpanForObj)
			
			local r = self:MoveToInAStarPath( distPos, EFindPathType.eFPT_HypoAStarOpti, EBarrierType.eBT_LowBarrier,0,100)
		end
	end
end

function CCharacterDirector:SetDeadFriendCanBeSelected(CanBeSelected)
	g_MainPlayer.m_CanLookDead = CanBeSelected
	local tbl = g_GameMain.m_CharacterInSyncMgr.m_tblCharacter
	for i, p in pairs(tbl) do
		if p and IsCppBound(p) then
			local Type = p.m_Properties:GetType()
			if Type == ECharacterType.Player and not g_MainPlayer:IsEnemy(p) and not p:CppIsAlive() then
				SetCanBeSelectedForLua(p,CanBeSelected)
			end
		end
	end
end

function CCharacterDirector:MoveOnTime_UnRegisterTick()
	if( self.MoveOnTime_Tick ) then
		UnRegisterTick( self.MoveOnTime_Tick )--注销3s的Tick
		self.MoveOnTime_Tick = nil
	end
end

function CCharacterDirector:MoveOnTime_OnTick()
	if( self.MoveOnTime_Tick ) then
		UnRegisterTick( self.MoveOnTime_Tick )--注销3s的Tick
		self.MoveOnTime_Tick = nil
		--注册跟踪鼠标的Tick
		self.RunToMouseTick = RegClassTick( "RunToMouse_OnTick", CCharacterDirector.RunToMouse_OnTick , 200, self)
	end
end

function CCharacterDirector:RunToMouse_UnRegisterTick()
	if( self.RunToMouseTick ) then
		UnRegisterTick( self.RunToMouseTick )--注销鼠标移动Tick
		self.RunToMouseTick = nil
	end
end

function CCharacterDirector:RunToMouse_OnTick()
	if( self.RunToMouseTick and self:CppIsAlive() ) then
		--模拟鼠标点击地面的操作
		self:OnSelectGround()
	else
		self:RunToMouse_UnRegisterTick()
	end
end

function CCharacterDirector:NpcIntoVision(Npc)
	if self.m_NpcInVisionTbl[Npc:GetEntityID()] then
		return
	end
	self.m_NpcInVisionTbl[Npc:GetEntityID()] = Npc
end

function CCharacterDirector:NpcOutOfVision(Npc)
	self.m_NpcInVisionTbl[Npc:GetEntityID()] = nil
end

function CCharacterDirector:IntObjIntoVision(IntObj)
	if self.m_IntObjInVisionTbl[IntObj:GetEntityID()] then
		return
	end
	self.m_IntObjInVisionTbl[IntObj:GetEntityID()] = IntObj
end

function CCharacterDirector:IntObjOutOfVision(IntObj)
	self.m_IntObjInVisionTbl[IntObj:GetEntityID()] = nil
end

--短暂CD或流程内 记录的技能
function CCharacterDirector:RecordSkillName(SkillName, SkillLevel)
	local uCoolDownTime = self:GetSkillLeftCoolDownTime(SkillName)
	if g_GameMain.m_skillProgress_Load:IsShow() then
		if g_GameMain.m_skillProgress_Load:GetCastingProcessLeftTime() <= 0.5 and uCoolDownTime <= 500 then
			self.szRecordSkillName = SkillName
			self.uRecordSkillLevel =SkillLevel
		else
			self.szRecordSkillName = nil
			self.uRecordSkillLevel =nil
		end
	elseif uCoolDownTime <= 500 then
		self.szRecordSkillName = SkillName
		self.uRecordSkillLevel =SkillLevel
	else
		self.szRecordSkillName = nil
		self.uRecordSkillLevel =nil
	end
end
--CD或流程结束立即释放记录的技能
function CCharacterDirector:DoRecordSkill(SkillName)
	if self.szRecordSkillName ~= nil and	self.uRecordSkillLevel ~= nil and g_GameMain.m_skillProgress_Load:IsShow()==false then
		if SkillName == self.szRecordSkillName or SkillName == nil then
			local flag = self:FightSkill(self.szRecordSkillName, self.uRecordSkillLevel)
			if flag == EDoSkillResult.eDSR_InCDTime then
				self:RecordSkillName(self.szRecordSkillName, self.uRecordSkillLevel)
			else
				self.szRecordSkillName = nil
				self.uRecordSkillLevel =nil
			end
		end
	end
end

function CCharacterDirector:StopMovingBeforeDoCastingSkill(SkillName)
	local bFlag1 = g_MainPlayer:IsCastingTypeSkill(SkillName)
	local bFlag2 = g_MainPlayer:CanUseNonFightSkill(SkillName, 1)
	if bFlag1 and bFlag2 then
		if g_MainPlayer:IsMoving() then
			g_MainPlayer:StopMoving()
		end
		g_MainPlayer:CancelAutoTracker(false)
		ClearAutoTrackState()
	end
end
------------------------------------------------------------ 以下是移动回调代码 ------------------------------------------------------------

function CCharacterDirector:Touch(	Obj )
	--print( "CCharacterDirector : Touch |", Obj )
	if( self.m_LockCenter.m_TouchAttack == true ) then
		self.m_LockCenter.m_TouchAttack = false
		self.m_LockCenter.m_StopDistance = 1
	else
		self.m_LockCenter.m_TouchCallBack = false
		--self:DoWeaponAttack( Obj )
	end
end

-------------------------------------------以下是角色物品代码------------------------------------------------
--返回装备武器的信息
function Gas2Gac:RetCommonWeaponInfo(Conn, nItemId, ViewName, BindingType, BaseLevel, CurLevel, DPS,AttackSpeed,DPSFloorRate, intensifyQuality, maxDuraValue, curDuraValue,IntensifyRoot)
	g_DynItemInfoMgr:CreateWeaponInfo( nItemId, ViewName, BindingType, tonumber(BaseLevel), tonumber(CurLevel), DPS, AttackSpeed,DPSFloorRate, intensifyQuality, maxDuraValue, curDuraValue,IntensifyRoot)
end
--返回装备护甲的信息
function Gas2Gac:RetCommonArmorInfo(Conn, nItemId, ViewName, BindingType, BaseLevel, CurLevel, OutputAttr1, OutputAttr2, OutputAttr3,StaticProValue, intensifyQuality, maxDuraValue, curDuraValue,IntensifyRoot) 
	g_DynItemInfoMgr:CreateArmorInfo( nItemId, ViewName, BindingType, tonumber(BaseLevel), tonumber(CurLevel), OutputAttr1, OutputAttr2, OutputAttr3,StaticProValue, intensifyQuality, maxDuraValue, curDuraValue,IntensifyRoot)
end

--返回装备指环的信息
function Gas2Gac:RetCommonRingInfo(Conn, nItemId, ViewName, BindingType, BaseLevel, CurLevel, DPS,StaticProValue, intensifyQuality, maxDuraValue, curDuraValue,IntensifyRoot)
	g_DynItemInfoMgr:CreateRingInfo( nItemId, ViewName, BindingType, tonumber(BaseLevel), tonumber(CurLevel), DPS,StaticProValue, intensifyQuality, maxDuraValue, curDuraValue,IntensifyRoot)
end

--返回装备盾牌的信息
function Gas2Gac:RetCommonShieldInfo(Conn, nItemId, ViewName, BindingType, BaseLevel, CurLevel,intensifyQuality,uOutputAttrValue1,uOutputAttrValue2,uOutputAttrValue3,uOutputAttrValue4, maxDuraValue, curDuraValue,IntensifyRoot)
	g_DynItemInfoMgr:CreateShieldInfo( nItemId, ViewName, BindingType, tonumber(BaseLevel), tonumber(CurLevel),intensifyQuality, uOutputAttrValue1,uOutputAttrValue2,uOutputAttrValue3,uOutputAttrValue4, maxDuraValue, curDuraValue,IntensifyRoot)
end

--------------装备强化信息---------
function Gas2Gac:RetEquipIntensifyInfo(Conn, nItemId, ItensifySoul, AddAttr1, AddAttrValue1, AddAttr2, AddAttrValue2, IntensifyPhase, SuitName, intensifyTimes, intensifyBackTimes, intenAddTalentIndex)
	g_DynItemInfoMgr:CreateEquipIntensifyInfo(nItemId, ItensifySoul, AddAttr1, AddAttrValue1, AddAttr2, AddAttrValue2, IntensifyPhase, SuitName, intensifyTimes, intensifyBackTimes, intenAddTalentIndex)
end

--返回装备强化（4~9阶段）的附加属性和值的信息
function Gas2Gac:RetEquipAddAttrInfo(Conn, nItemId,sAddAttr4, uAddAttr4Value, sAddAttr5, uAddAttr5Value, sAddAttr6, uAddAttr6Value, sAddAttr7, uAddAttr7Value, sAddAttr8, uAddAttr8Value, sAddAttr9, uAddAttr9Value)
	g_DynItemInfoMgr:CreateEquipAddAttrInfo( nItemId,sAddAttr4, uAddAttr4Value, sAddAttr5, uAddAttr5Value, sAddAttr6, uAddAttr6Value, sAddAttr7, uAddAttr7Value, sAddAttr8, uAddAttr8Value, sAddAttr9, uAddAttr9Value)
end

--返回装备进阶属性
function Gas2Gac:RetEquipAdvanceInfo(Conn,nItemId,CurAdvancePhase,MaxAdvancePhase,AdvanceSkillPiece1,AdvanceSkillPiece2,AdvanceSkillPiece3,AdvanceSkillPiece4,ChoosedAdvanceSkillPiece,JingLingType,AdvanceSoul,AdvancedTimes, AdvancedAttrValue1,AdvancedAttrValue2,AdvancedAttrValue3,AdvancedAttrValue4,AdvancedRoot,AdvancedAttrName1,AdvancedAttrName2,AdvancedAttrName3,AdvancedAttrName4)
	g_DynItemInfoMgr:CreateEquipAdvanceInfo(nItemId,CurAdvancePhase,MaxAdvancePhase,AdvanceSkillPiece1,AdvanceSkillPiece2,AdvanceSkillPiece3,AdvanceSkillPiece4,ChoosedAdvanceSkillPiece,JingLingType,AdvanceSoul,AdvancedTimes, AdvancedAttrValue1,AdvancedAttrValue2,AdvancedAttrValue3,AdvancedAttrValue4,AdvancedRoot,AdvancedAttrName1,AdvancedAttrName2,AdvancedAttrName3,AdvancedAttrName4)
end

--返回装备护甲属性
function Gas2Gac:RetEquipEnactmentInfo(Conn,nItemId,ArmorPieceAttr1,ArmorPiece1AddTimes,ArmorPieceAttr2,ArmorPiece2AddTimes,ArmorPieceAttr3,ArmorPiece3AddTimes,ArmorPieceAttr4,ArmorPiece4AddTimes)
	g_DynItemInfoMgr:CreateEquipEnactmentInfo(nItemId,ArmorPieceAttr1,ArmorPiece1AddTimes,ArmorPieceAttr2,ArmorPiece2AddTimes,ArmorPieceAttr3,ArmorPiece3AddTimes,ArmorPieceAttr4,ArmorPiece4AddTimes)
end

--返回阵法书的阵型信息
function Gas2Gac:RetCommonBattleArrayBookInfo(Conn, nItemId, nPos1, nPos2, nPos3, nPos4, nPos5)
	g_DynItemInfoMgr:CreateBattleArrayBookInfo(nItemId, nPos1, nPos2, nPos3, nPos4, nPos5)
end

--返回邮件文本附件信息
function Gas2Gac:RetMailTextAttachmentInfo(Conn, nItemId, sMailTitle,sSenderName)
	g_DynItemInfoMgr:CreateMailTextAttachmentInfo(nItemId, sMailTitle,sSenderName)
end

function Gas2Gac:RetEquipSuperaddRate(Conn, equipID, curSuperaddPhase, superaddRate)
    g_DynItemInfoMgr:CreateEquipSuperaddInfo(equipID, curSuperaddPhase, superaddRate)
end

--返回魂珠信息
function Gas2Gac:RetSoulPearlItemInfo(Conn, nItemId, nSoulNum)
	g_DynItemInfoMgr:CreateSoulPearlItemInfo(nItemId, nSoulNum)
	g_GameMain.m_GMConsole:SetResult("AddSoul(" .. tostring(uItemId).. "):" .. tostring(uSoulNum))
	SetEvent( Event.Test.AddSoul,true, "AddSoulSuccess" )
end

--返回矿藏图信息
function Gas2Gac:RetOreMapItemInfo(Conn, nItemId, sceneName)
	g_DynItemInfoMgr:CreateOreMapItemInfo(nItemId, sceneName)
end

--返回采矿工具信息
function Gas2Gac:RetPickOreItemInfo(Conn, nItemId, MaxDura, CurDura)
	g_DynItemInfoMgr:CreatePickOreItemInfo(nItemId, MaxDura, CurDura)
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(true,nItemId)
end

function Gas2Gac:RetBoxitemOpened(Conn,nItemId,  openedFlag)
	g_DynItemInfoMgr:CreateBoxItemInfo(nItemId,  openedFlag)
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(true,nItemId)
end

--返回经验瓶或者魂瓶动态信息
function Gas2Gac:ReturnExpOrSoulBottleInfo(Conn,nItemId,uState,uStorageNum)
	g_DynItemInfoMgr:CreateExpOrSoulBottleInfo(nItemId,uState,uStorageNum)
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(true,nItemId)
end 	


function Gas2Gac:ReturnBottleState(Conn,itemId,uState)
	local tbl = g_DynItemInfoMgr.m_tblGlobalItemInfo[itemId]
	local info = tbl or {}
	info.uState = uState
	
	g_DynItemInfoMgr.m_tblGlobalItemInfo[itemId] = info
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(true,itemId)
end
----------------------------------------以下是主角属性操作代码------------------------------------------------

function CCharacterDirector:SetLevel( uLevel )
	self:CppSetLevel( uLevel )
	g_GameMain.m_PlayerInfo:UpdateHeadInfo()
end

function CCharacterDirector:SetLevelExp( uLevelExp )
	self.m_uLevelExp = uLevelExp
	g_GameMain.m_PlayerInfo:UpdateHeadInfo()
end

function CCharacterDirector:IsInBattleState()
	return self:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle)
end

function CCharacterDirector:IsInForbitUseWeaponState()
	return self:CppGetCtrlState(EFighterCtrlState.eFCS_ForbitUseWeapon)
end

--佣兵等级
function Gas2Gac:RetSetMercenaryLevel(Connection, uLevel, IsLevelUp)
	if g_MainPlayer and IsCppBound(g_MainPlayer) then
		g_MainPlayer.m_uMercenaryLevel = uLevel
		if IsLevelUp then
			g_GameMain.m_NpcHeadSignMgr:UpdateNpcAndObjInView()
			g_MainPlayer:GetRenderObject():AddEffect("fx/setting/other/other/levelup/create.efx","levelup/create",0,nil)
		end
	end
end

--佣兵积分
function Gas2Gac:RetSetMercenaryIntegral(Connection, uIntegral )
	if g_MainPlayer and IsCppBound(g_MainPlayer) then
		g_MainPlayer.m_uMercenaryIntegral = uIntegral
--		if g_GameMain.m_RoleStatus
--			and IsFunction(g_GameMain.m_RoleStatus.IsShow)
--			and g_GameMain.m_RoleStatus:IsShow() then
--				g_GameMain.m_RoleStatus:ShowPlayerMercenary()
--		end
	end
end

----------------------------------------------------

--------------------------------金钱操作----------------------
--金
function CCharacterDirector:GetGold( )
	return math.floor (self.m_nMoney / 10000)
end

--银
function CCharacterDirector:GetArgent( )
	return math.floor ( (self.m_nMoney - self:GetGold() * 10000) / 100)
end

--铜
function CCharacterDirector:GetCopper( )
	return math.floor( (self.m_nMoney - self:GetGold() * 10000 - self:GetArgent() * 100))
end

--绑定的金
function CCharacterDirector:GetBindingGold( )
	return math.floor (self.m_nBindMoney / 10000)
end

--绑定的银
function CCharacterDirector:GetBindingArgent( )
	return math.floor ( (self.m_nBindMoney - self:GetBindingGold() * 10000) / 100)
end

--绑定的铜
function CCharacterDirector:GetBindingCopper( )
	return math.floor( (self.m_nBindMoney - self:GetBindingGold() * 10000 - self:GetBindingArgent() * 100))
end
--金钱
function CCharacterDirector:AddMoney(nMoney)
	self.m_nMoney = nMoney
end

function CCharacterDirector:GetMoney()
	return self.m_nMoney
end
--佣兵币
function CCharacterDirector:AddTicket(nTicket)
	self.m_nTicket = nTicket
end

function CCharacterDirector:GetTicket()
	return self.m_nTicket
end
--绑定的金钱
function CCharacterDirector:AddBindMoney(nMoney)
	self.m_nBindMoney = nMoney
end

function CCharacterDirector:GetBindMoney()
	return self.m_nBindMoney
end
--绑定的佣兵币
function CCharacterDirector:AddBindTicket(nTicket)
	self.m_nBindTicket = nTicket
end

function CCharacterDirector:GetBindTicket()
	return self.m_nBindTicket
end
--初始化金钱，只有在client有，因为是高数逻辑
function CCharacterDirector:InitMoney(nMoney,nTicket,nBindMoney,nBindTicket)
	self.m_nMoney		= nMoney
	self.m_nTicket		= nTicket
	self.m_nBindMoney	= nBindMoney
	self.m_nBindTicket	= nBindTicket
end

function CCharacterDirector:SetMoneyType(MoneyType)
	self.m_nMoneyType		= MoneyType
end

function CCharacterDirector:GetMoneyType()
	return self.m_nMoneyType
end

------------------------------------------------------------------------------

function CCharacterDirector:DoFacialAction(ID, FacialAction, FacialActionName, Continuance, SpecialActionName, IsActive, ContinuanceSpecial)
	if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) or g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse) then
		MsgClient(30)
		return
	end
	if FacialActionName then
		Gac2Gas:PlayActionCyc(g_Conn, ID, FacialAction, FacialActionName, Continuance, IsActive, SpecialActionName, ContinuanceSpecial)
	end
end

function Gas2Gac:LoadSpecialEffect(g_Conn, FacialActionName, SpecialActionName)
	local str = "fx/setting/other/other/shenmingfuti/create.efx" 
	--local str = "fx/setting/biaoqing/"..GetRaceSexTypeString(Character:CppGetClass(), Character.m_Properties:GetSex()) .. "/"
	local RenderObj = g_MainPlayer:GetRenderObject()	
	if IsCppBound(g_MainPlayer) then		
		RenderObj:AddEffect(str, SpecialActionName, 0, nil)		--得到玩家的职业如人类等
		RenderObj:DoAni(FacialActionName, false, -1.0)
	end
end

local function CancelSpecialEffect(Continuance, ContinuanceSpecial)
	local RenderObj = g_MainPlayer:GetRenderObject()		
	local str = "fx/setting/other/other/shenmingfuti/create.efx" 
	if IsCppBound(g_MainPlayer) and ContinuanceSpecial then
		RenderObj:DelEffect(g_MainPlayer.m_SpecialEffect)
	end
end

function Gas2Gac:LoadContinuancelEffect(g_Conn, Continuance, ContinuanceSpecial)
	local str = "fx/setting/other/other/shenmingfuti/create.efx" 
	--通过职业和性别获得表情包
	--local str = "fx/setting/biaoqing/"..GetRaceSexTypeString(Character:CppGetClass(), Character.m_Properties:GetSex()) .. "/"
	local RenderObj = g_MainPlayer:GetRenderObject()	
	if IsCppBound(g_MainPlayer) then		
		if g_MainPlayer.m_SpecialEffect then
			RenderObj:DelEffect(g_MainPlayer.m_SpecialEffect)
		end
		g_MainPlayer.m_SpecialEffect = RenderObj:AddEffect(str, ContinuanceSpecial, -1, nil)	--得到玩家的职业如人类等
		RenderObj:DoAni(Continuance, true, -1.0)
	end
	RegisterOnceTick(g_GameMain, "CancelEffect", CancelSpecialEffect, 5000, Continuance, ContinuanceSpecial)
end



--------------------------------------------------------------------------------------------------

function CCharacterDirector:CharInfoDebug_RegisterTick()
	if self.m_CharInfoDlg == nil then	
		self.m_CharInfoDlg	= SQRRichText:new()
		local param = WndCreateParam:new()
		param.x = g_GameMain:GetWndOrgWidth()/2
		param.y = g_GameMain:GetWndOrgHeight()/2
		param.uStyle = 0x60000000
		param.width = 250
		param.height = 60
		param:SetParentWnd(g_GameMain)
		param.font_size = 15
		self.m_CharInfoDlg:Create( param )
		self.m_CharInfoDlg:SetLockFlag( BitOr( LOCK_LEFT, LOCK_RIGHT, LOCK_TOP, LOCK_BOTTOM ) )
		local Image = WND_IMAGE_LIST:new()
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE
		self.m_CharInfoDlg:SetWndBkImage( Flag, Image )
	end

	self:CharInfoDebug_OnTick()
	self:CharInfoDebug_UnRegisterTick()
	self.CharInfoDebugTick = RegClassTick( "CharInfoDebug_OnTick", CCharacterDirector.CharInfoDebug_OnTick , 200, self)
end

function CCharacterDirector:CharInfoDebug_UnRegisterTick()
	if self.CharInfoDebugTick then
		self.m_CharInfoDlg:SetWndText("")
		UnRegisterTick( self.CharInfoDebugTick )
		self.CharInfoDebugTick = nil
	end
end

function CCharacterDirector:CharInfoDebug_OnTick()
	if self ~= nil then
		local strCharInfo = string.format("Move Speed: %.2f grid per second", self:GetMaxSpeed())
		local angle = string.format("%.2f", self:GetActionDir()/256 * 360)
		strCharInfo = strCharInfo .. "#rDirection: "..angle.." degree"
		if self:GetTarget() == nil then
			strCharInfo = strCharInfo .. "#rNo Target"
		else
			local Dist = string.format("%.2f", self:GetEntityDistInGrid(self:GetTarget()))
			strCharInfo = strCharInfo .. "#rDistance: "..Dist.." grid"
		end
		self.m_CharInfoDlg:SetWndText(strCharInfo)
	end
end

function CCharacterDirector:OpenCharInfoSwitch()
	if not CAppConfigClient_Inst():IsTestClient() then
		return
	end
	
	if self.CharInfoDebugTick ~= nil then
		self:CharInfoDebug_UnRegisterTick()
	else
		self:CharInfoDebug_RegisterTick()
	end
end

function CCharacterDirector:JudgementServantCanBeSelected(Character)
	local Type = Character.m_Properties:GetType()
	local DirectorTeamID		= g_MainPlayer.m_Properties:GetTeamID()
	local DirectorTongID		= g_MainPlayer.m_Properties:GetTongID()
	if Type == ECharacterType.Npc then
		local NpcAIType			= Character:GetAITypeID()
		local SelfTeamID		= Character.m_Properties:GetMasterTeamID()
		local SelfTroopID		= Character.m_Properties:GetMasterTroopID()
		local SelfTongID		= Character.m_Properties:GetMasterTongID()
		if NpcAIType == ENpcAIType.ENpcAIType_Servant then
			local bSelect = true
			if Character:GetMaster() == g_MainPlayer then
				bSelect = g_bSelectServant
			elseif (DirectorTeamID ~= 0 and DirectorTeamID == SelfTeamID)	or
					(DirectorTongID ~= 0 and DirectorTongID == SelfTongID)		then
				bSelect = g_bSelectServant
			end
			Character:SetCanBeSelected(bSelect)
		end
	end
end

function CCharacterDirector:OnServantSelectConditionChange()
	for id, Npc in pairs(self.m_NpcInVisionTbl) do
		self:JudgementServantCanBeSelected(Npc)
	end
end

function Gas2Gac:SetExp( Connection, uAddExp, uInspirationExp, TeamAwardExp, uCurExp, uLevelExp)
--print("Gas2Gac:SetExp")
	if uAddExp ~= 0 and uAddExp ~= nil then 
		local RenderObj = g_MainPlayer:GetRenderObject()
		local strEffectFile="fx/setting/number1.efx";
		local strEffectName="playernum/exp"
		local strDisplay=GetStaticTextClient(17001)..uAddExp --"获得经验值"
		local strFontName="num"
		CFontSprite_DisplayInfo(RenderObj,strEffectFile,strEffectName,strFontName,strDisplay,99998,false,2);

		if g_MainPlayer ~= nil then
			local FightInfoWnd = CFightInfoWnd.GetWnd()
			local fightInfo = FightInfoWnd:GetExpInfo(29, g_MainPlayer:GetEntityID(), 0, "", uAddExp, uInspirationExp, TeamAwardExp)
			if(fightInfo ~= "") and FightInfoWnd ~= nil then
				local Wnd = g_GameMain.m_CreateChatWnd.m_CChatWnd
				table.insert(Wnd.ChatInfoTbl[1] ,fightInfo .. "#r")
				g_GameMain.m_CreateChatWnd.m_CChatWnd:ChatInfoShow()
			end
		end
		--灵感爆发特效
--		if uInspirationExp ~= 0 and uInspirationExp ~= nil then
--			local playerPos = CFPos:new()
--			g_MainPlayer:GetPixelPos(playerPos)
--			CPositionFX_CppPlayPosFX("fx/setting/other/other/brith/create.efx", "brith/create", playerPos)
--		end
	end
	g_GameMain.m_RoleStatus:SetExpLong(uCurExp,uLevelExp)
	g_GameMain.m_ExpBar:SetExpLong( uCurExp,uLevelExp)
	g_GameMain.m_QuestTraceBack:AddExpUpdateActionNotify()
end

function Gas2Gac:ReturnObjInSight( Connection, ObjID, Type )
	--强烈建议不要再用这个函数CCoreObjectFollower_GetObjectFollower
	--改用CCharacterFollower_GetCharacter函数
	local FolObj = CCoreObjectFollower_GetObjectFollower( ObjID )
	g_GameMain.m_NavigationMap:RecRoleData( FolObj, Type )
end

function Gas2Gac:CancelNormalAttack(Connection)
--	print("Gas2Gac:CancelNormalAttack")
	g_MainPlayer:CancelNormalAttack()
end

------以下是角色死亡后弹出窗口--------------------------------
function Gas2Gac:OnShowDeadMsgBox(Connection, ShowDeadMsgType, RebornCountTime, DeadTimes)
	ClearAutoTrackState()
	if g_MainPlayer:CppIsAlive() then --救赎之魂技能,会在死后重生
		return
	end
	if g_GameMain.IsNowShowRebornWnd == nil then
		g_GameMain.IsNowShowRebornWnd = true
	end
	g_MainPlayer.m_ShowDeadMsgType = ShowDeadMsgType
	if g_GameMain.IsNowShowRebornWnd == false then
		g_GameMain.IsNowShowRebornWnd = true
		return
	end
	
	-----------------------------------
	local function GoBackOrStay(g_GameMain,uButton)
		local bFlag = (uButton == MB_BtnOK)
		Gac2Gas:ExitOrInMode(g_Conn, bFlag)--如果是副本中
		g_GameMain.m_MsgBox = nil
		return true
	end
	------------------------------------
	--print(os.time(),"Gas2Gac:OnShowDeadMsgBox(Connection, ShowDeadMsgType, RebornCountTime)")
--	g_GameMain.m_DeadTimeLog = math.floor(GetProcessTime()/1000)
	g_GameMain.m_DeadTime = math.floor(GetProcessTime()/1000)
	local TickTime = 1500
	if ShowDeadMsgType == 6 or ShowDeadMsgType == 10 then
		TickTime = 3000
	end
	local function TickFun()
		if not IsCppBound(g_MainPlayer) then
			return
		end
		if ShowDeadMsgType == 1 then
			g_GameMain.m_ToDeadMsgBox:ToCharDeadWnd(RebornCountTime)
		elseif ShowDeadMsgType == 2 then
			return
		elseif ShowDeadMsgType == 3 then
			local ShowInfo = GetStaticTextClient(17002)
			g_GameMain.m_MsgBox = MessageBox(g_GameMain, ShowInfo, BitOr( MB_BtnOK, MB_BtnCancel), GoBackOrStay, g_GameMain)
		elseif ShowDeadMsgType == 4 then
			g_GameMain.m_AreaFbDeadMsgBox:ShowDeadWnd(30,RebornCountTime)--如果是区域副本中
		elseif ShowDeadMsgType == 5 then
			g_GameMain.m_AreaFbDeadMsgBox:ShowDeadWnd(10,RebornCountTime)--如果是刷怪活动中
		elseif ShowDeadMsgType == 6 then
			Gac2Gas:GoBackOrStay( g_Conn, true )
		elseif ShowDeadMsgType == 7 then
			g_GameMain.m_WarDeadMsgBox:ShowDeadWnd(60)
		elseif ShowDeadMsgType == 8 then
			g_GameMain.m_FBDeadMsgBox:ShowDeadWnd(10)
		elseif ShowDeadMsgType == 9 then
			g_GameMain.m_ChallengeDeadMsgBox:ShowDeadWnd(10)
		elseif ShowDeadMsgType == 10 then
			Gac2Gas:MatchGameJoinPointReborn(Connection)
		elseif ShowDeadMsgType == 11 then		--抢夺资源点用复活类型
			g_GameMain.m_RobResDeadMsgBox:ShowDeadWnd(30,RebornCountTime)	--30秒之后可以使用
		elseif ShowDeadMsgType == 12 then
			g_GameMain.m_TeamPVPFBDeadMsgBox:ShowDeadWnd(30)
		elseif ShowDeadMsgType == 13 then
			local wnd = COldWarDeadWnd:GetWnd()
			wnd:ShowDeadWnd(DeadTimes)
		else
			error("错误的重生类型"..ShowDeadMsgType)
			return
		end
	end
	RegisterOnceTick(g_GameMain, "RebornTick", TickFun, TickTime)
end

function Gas2Gac:HideTeamPVPFBDeadMsgWnd(Conn)
	g_GameMain.m_TeamPVPFBDeadMsgBox:HideWnd()
end

function Gas2Gac:OnNotifyPermitUseSkill(Connection, szSkillName)
	local function DoSkillOrGoBack(g_GameMain,uButton)
		if g_MainPlayer == nil then
			return
		end
		local bFlag = (uButton == MB_BtnOK)
		if bFlag then
			Gac2Gas:FightSpecialSkill(g_Conn, szSkillName)
		else
			if g_MainPlayer.m_ShowDeadMsgType ~= nil then
				Gas2Gac:OnShowDeadMsgBox(Connection, g_MainPlayer.m_ShowDeadMsgType, 0)
			end
		end
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.IsNowShowRebornWnd = false
	--救赎之魂在用，OnDead的Gas2Gac:OnShowDeadMsgBox不能弹出复活框，但要记录，如果救赎之魂的MsgBox点了Cancel，则弹出复活框
	local ClassID = g_MainPlayer:CppGetClass()
	GiftSkill_Common = loadGeniusfile(ClassID)
	Lan_GiftSkill_Common = loadGeniusfile(ClassID,true)
	local ShowInfo = GiftSkill_Common(szSkillName) and Lan_GiftSkill_Common(MemH64(GiftSkill_Common(szSkillName,"SkillName")),"DisplayName") or szSkillName
	local ShowMsg = GetStaticTextClient(17003)..ShowInfo
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, ShowMsg, BitOr( MB_BtnOK, MB_BtnCancel), DoSkillOrGoBack, g_GameMain)
end

function Gas2Gac:OnPermitReborn(Conn, CharName)
	local function DoReborn(g_GameMain,uButton)
		if(not g_MainPlayer) then return end
		if(uButton == MB_BtnOK) then
			g_GameMain.m_AreaFbDeadMsgBox:HideWnd()
			g_GameMain.m_FBDeadMsgBox:HideWnd()
			g_GameMain.m_TeamPVPFBDeadMsgBox:HideWnd()
			g_GameMain.m_WarDeadMsgBox:HideWnd()
			g_GameMain.m_ChallengeDeadMsgBox:HideWnd()
			g_GameMain.m_RobResDeadMsgBox:HideWnd()
			local wnd = COldWarDeadWnd:GetWnd()
			wnd:HideWnd()
			Gac2Gas:DoReborn(g_Conn)
		else
			if g_MainPlayer.m_ShowDeadMsgType == 1 then
				g_GameMain.m_ToDeadMsgBox:ReShow()
			end
		end
		g_GameMain.m_PermitRebornMsgBox = nil
		return true
	end
	if g_MainPlayer:CppIsAlive() then
		return
	end
	if g_GameMain.m_PermitRebornMsgBox then
		return
	end
	
	if g_MainPlayer.m_ShowDeadMsgType == 1 then
		g_GameMain.m_ToDeadMsgBox:HideWnd()
	end
	
	local ShowMsg = CharName..GetStaticTextClient(17004)
	g_GameMain.m_PermitRebornMsgBox = MessageBox(g_GameMain, ShowMsg, BitOr( MB_BtnOK, MB_BtnCancel), DoReborn, g_GameMain)	
end


--获取自动移动的距离
function GetAutoTrackDistance(Target)
	local Type = Target.m_Properties:GetType()
	local ObjName = Target.m_Properties:GetCharName()
	if Type == ECharacterType.IntObj then
		local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
		if TriggerAction == "可拾取的Obj" or TriggerAction == "非战斗技能学习" then
			return 3
		end
		if ObjName == "鱼" then
			return 6
		end
		return 0.5
	elseif Type == ECharacterType.Npc then
		if Target:CppIsAlive() then
--			if NpcTriggerEvent_Common[ObjName] then
--				for i=1,table.getn(NpcTriggerEvent_Common[ObjName]) do
--					for n=1, table.getn(NpcTriggerEvent_Common[ObjName][i].ScriptArg) do
--						local doName = NpcTriggerEvent_Common[ObjName][i].ScriptArg[n][1]
--						if doName == "抱" or doName == "小游戏" then
--							return 0.5
--						end
--					end
--				end
--			end
			local TriggerInfo = GetNpcTriggerArg(ObjName, "点击")
			if TriggerInfo then
				if TriggerInfo["抱"] or TriggerInfo["小游戏"] then
					return 0.5
				end
			end
			if CQuest.IsObjectContainFun(Target) then
				return 3.5
			else
				return 0.5
			end
		else
			return 0.5
		end
	end
end

--头顶显示
function Gas2Gac:PlayHeadEffect(Connection, color, strDisplay, EntityID)

	local Object = CCharacterFollower_GetCharacterByID(EntityID)
	if Object then
		local RenderObj		= Object:GetRenderObject()
		local strEffectFile	= "fx/setting/number.efx";
		local strEffectName	= "num_UP/"..color
		local strFontName	= color
		CFontSprite_DisplayInfo(RenderObj,strEffectFile,strEffectName,strFontName,strDisplay,99999,false,2);
	end
end


--设置是否能选其他玩家
function Gas2Gac:SetCanBeSelectOtherPlayer(Connection,isCan)
	g_GameMain.m_ForbidSelectOtherPlayer = not isCan
end

function Gas2Gac:SetSceneState(Connection,isDark)
	if g_MainPlayer.m_DarkEffect then
		g_MainPlayer:GetRenderObject():DelEffect(g_MainPlayer.m_DarkEffect)
		g_MainPlayer.m_DarkEffect = nil
	end
	if isDark then
		g_MainPlayer.m_DarkEffect = g_MainPlayer:GetRenderObject():AddEffect("fx/setting/other/other/fanyao09/create.efx","fanyao09/create",-1,nil)
	end
end



function PrepareChangeScene()
	if g_GameMain.m_PrepareChangeTick then
		return
	end
	if g_GameMain.m_PrepareChangeCDTick then
		return
	end
	g_MainPlayer:StopMoving()
	
	local function OnCDTick()
		if g_GameMain and g_GameMain.m_PrepareChangeCDTick then
			UnRegisterTick(g_GameMain.m_PrepareChangeCDTick)
			g_GameMain.m_PrepareChangeCDTick = nil
		end
	end
	
	local function OnPrepareChangeTick()
		if g_GameMain and g_GameMain.m_PrepareChangeTick then
			UnRegisterTick(g_GameMain.m_PrepareChangeTick)
			g_GameMain.m_PrepareChangeTick = nil
			if g_MainPlayer then
				g_MainPlayer:GetRenderObject():DoAni("stand01",true,-1.0)
			end
		end
		if g_MainPlayer then
			g_GameMain.m_PrepareChangeCDTick = RegisterTick("PrepareChangeCDTick", OnCDTick, 5000)
		end
	end
	g_GameMain.m_PrepareChangeTick = RegisterTick("PrepareChangeTick", OnPrepareChangeTick, 5000)
end

function CCharacterDirector:ChangeHideModeOnRelationChanged(pCharacter)
	g_GameMain.m_HideFollowerMgr:ChangeHideModeOnRelationChanged(pCharacter)
end

function CCharacterDirector:ChangeSelectedModeOnRelationChanged(pCharacter)
	if pCharacter.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	local bFriendCanSelected = g_UISettingMgr:GetOnePropState("友方不可选中")
	if g_MainPlayer ~= pCharacter then
		g_GameMain.m_HideFollowerMgr:SetFriendCanNotBeSelected(pCharacter)
	elseif g_MainPlayer == pCharacter then
		g_GameMain.m_CharacterInSyncMgr:UpdateFriendCanSelectedInSync()
	end
end

function CCharacterDirector:DealHideFollowerIntoSync(pCharacter)
	g_GameMain.m_HideFollowerMgr:DealHideFollowerIntoSync(pCharacter)
end

function CCharacterDirector:DealHideFollowerOnTeamChanged(pCharacter, bInTeam)
	g_GameMain.m_HideFollowerMgr:DealHideFollowerOnTeamChanged(pCharacter, not bInTeam)
end

function CCharacterDirector:ChangeWarModel()
	UpdateModel(g_MainPlayer)
	local tbl = g_GameMain.m_CharacterInSyncMgr.m_tblCharacter
	for i, p in pairs(tbl) do
		if p and IsCppBound(p) then
			local Type = p.m_Properties:GetType()
			if Type == ECharacterType.Player then
				UpdateModel(p)
			end
		end
	end
end

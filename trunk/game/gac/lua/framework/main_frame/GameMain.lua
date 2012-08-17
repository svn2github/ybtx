gac_require "framework/main_frame/PropRes"
gac_require "framework/main_frame/GameState"
gac_gas_require "activity/scene/LoadPositionCfg"
gac_require "framework/display_common/DisplayCommon"
gac_require "fight/player_info/PlayerInfo"
gac_require "fight/role_status/StateCommon"
gac_require "information/head_info/HeadInfo"
gac_require "framework/gm_console/GMConsole"
gac_require "framework/mouse_hook/MouseHook" 
gac_require "framework/ground_selector/GroundSelector"
gac_require "message/message_box/MsgBoxMsg"
gac_require "toolbar/main_skills_toolbar/MainSkillsToolBar"
gac_require "toolbar/master_skill_area/MasterSkillArea"
gac_require "toolbar/panel_menu/PanelMenu"
gac_require "relation/team/TeamClient"
gac_gas_require "player/PlayerMgr"
gac_require "message/chat_wnd/ChatWnd"
gac_require "activity/npc_dialog_box/NpcDialogBox"
gac_require "activity/npc_dialog_box/NpcDialogBoxCopy"
gac_require "world/npc/NpcBornPointMgr"
gac_gas_require "areamgr/AreaMgr"

gac_require "activity/quest/QuestGas2GacFunc"
gac_require "activity/quest/NpcHeadSign"
gac_require "activity/quest/TeamMarkSign"
gac_require "activity/quest/QuestRecordWnd"
gac_gas_require "activity/npc/CheckNpcTriggerAction"
gac_gas_require "activity/Trigger/NpcTrigger"
gac_require "activity/quest/Quest"
gac_require "activity/playerautotrack/PlayerAutoTrackMgr"
gac_require "activity/quest/QuestTraceBack"
gac_require "activity/direct/DirectWnd"

gac_require "message/text_item_wnd/TextItemWnd"
gac_require "message/message_list/MsgListWnd"

gac_require "fight/turn_table/Turntable"
gac_require "fight/skill_loading/SkillLoadiNg" 
gac_require "fight/exp_bar/ExpBarWnd"
gac_require "activity/talkwith_npc_wnd/TalkWithNpcWnd"
--gac_require "activity/npc_dialog_box/SpecialNpcWnd"

gac_require "fight/burst_soul/BurstSoul"

gac_require "relation/team/TeamClient"
gac_require "relation/tong/TongBase"                  --在佣兵团的最前面require
gac_require "relation/tong/tong_building/TongProductionBase"
gac_require "relation/tong/TongChallengeStatisticsWnd"
--gac_require "relation/tong/TongRobResourceWnd"
--gac_require "relation/tong/TongRobResExitWnd"
--gac_require "relation/tong/TongSignQueryWnd"
--gac_require "relation/tong/TongRobOverExit"

gac_require "activity/direct/DirectWnd"
gac_require "activity/direct/DirectShowWnd"

gac_require "relation/association/AssociationBase"
gac_require "relation/association/Association"
gac_require "relation/association/AssociationFind"

gac_require "setting/function_area/ExcludeWnd"

gac_require "activity/fb_action/FbAction_Mgr"

gac_require "activity/MercenaryMonsterFbWnd/MercenaryMonsterFbWnd"
gac_require "fight/pet/GacPetInfo"

gac_require "resource/productive_profession/LiveSkillExpertNPC"
gac_require "resource/productive_profession/LiveSkillProdMain"

gac_require "smallgames/ProgressWnd/CProgressWnd"
gac_require "smallgames/SmallGameMgr"

gac_require "activity/quest/PickupItem"
gac_require "world/player/PlayerActionWnd"
gac_require "smallgames/ClickPicClient/ClickPicWnd/CClickPicWnd"
gac_require "smallgames/GetHorseMedicine/CGetHorseMedicineWnd"
gac_require "smallgames/smallgame_msgwnd/SmallGameMsgWnd"
gac_require "activity/ride_camel/RideCamelWnd"
gac_require "toolbar/temp_skill_area/temp_skill"
--gac_require "activity/essayquestionwnd/EssayQuestionWnd"
gac_require "world/item/UseItemPlayEffect"
gac_gas_require "activity/scene/AreaFbSceneMgr"
gac_require "activity/tong_war_wnd/WarZoneSelWnd"
--gac_require "character/pk_switch/PkSwitchWnd"
gac_require "fight/target_info/TargetOfTargetInfo"
gac_require "toolbar/servant_skill_area/servant_skill"
gac_require "information/select_obj_tooltips/SelectObjTooltips"
gac_require "world/area/AreaIntroductionWnd"
gac_require "fight/role_status/PlayerFightInfo"
gac_require "message/system_msg/SystemMsgWnd"
gac_require "toolbar/hide_player/HidePlayer"
gac_gas_require "framework/message/message"

gac_gas_require "framework/common/OnceTickMgr"
gac_require "smallgames/HeadGames/HeadGameProgressWnd"
gac_require "smallgames/HeadGames/HeadFishing"
gac_require "smallgames/HeadGames/HeadHunt"
gac_require "relation/tong/TongBattlePersonalCountWnd"
gac_require "relation/tong/TongWarBattleCountWnd"
gac_require "relation/tong/TongChallengeBattleCountWnd"
gac_require "relation/tong/TongMonsAttackCountWnd"
gac_require "relation/tong/TongDownTimeWnd"
--gac_require "relation/tong/TongShowRulesWnd"
gac_require "message/bag_shapeeffect_wnd/BagShapeEffectWnd"
gac_require "activity/MercenaryLevelWnd/MercenaryLevelTrace"
gac_require "fight/target_info/TargetInfo"

g_LoginFlowStepId = 0

function SaveLoginFlow(stepName)   ---临时查卡 5% bug 加的函数
	if g_MyCharIdTemp == 100574 then
		LogErr("客户端登录流程",  g_LoginFlowStepId .. "  " .. stepName)
		g_LoginFlowStepId = g_LoginFlowStepId + 1
	end
end

function CGameAccelerator:OnAccelerator( Msg, wParam, lParam )
	if( g_MainPlayer == nil ) then
		return false
	end
	if Msg == WM_MOUSEWHEEL then
		local MouseDelta=math.floor(wParam/65536)
				
		if MouseDelta > 32768 then
			MouseDelta = MouseDelta - 65536
		end
		
		RenderScene=g_CoreScene:GetRenderScene()
		
		local CamOffset = RenderScene:GetCameraOffset()
		
--		print("攝像機距離:"..tostring(CamOffset/EUnits.eGridSpan).."米")
		
		CamOffset = CamOffset - MouseDelta / 1.5
		
		RenderScene:SetCameraOffset( CamOffset )	
		
		return
	elseif Msg == WM_MOUSEMOVE then
		g_GameMain:OnTurnRunCamera(wParam, lParam)
	end
	if(Msg == WM_ACTIVATE) then
		g_GameMain.m_SysSetting.m_KPmap:ClearAllKeyTbl()
		g_GameMain.m_SysSetting.m_KPmap:MoveEndStep()
		UnShowAllName()
		g_GameMain:OnTurnRunCameraEnd()
		if g_MainPlayer and IsCppBound(g_MainPlayer) then
			g_MainPlayer:MoveOnTime_UnRegisterTick()
		end
	end
	if(Msg == WM_KEYDOWN or Msg == WM_SYSKEYDOWN) then
		g_GameMain.m_SysSetting.m_KPmap:AllKeySet(Msg,wParam)
	end
	if(Msg == WM_KEYUP or Msg == WM_SYSKEYUP) then
		g_GameMain.m_SysSetting.m_KPmap:AllKeySet(Msg,wParam)
	end

	return true
end

function CGameAccelerator:OnAcceleratorIsModule(Msg, wParam, lParam) --模态窗口回调
	g_GameMain.m_SysSetting.m_KPmap:ClearAllKeyTbl()
	g_GameMain.m_SysSetting.m_KPmap:MoveEndStep()
	--打开聊天输入框
	if(GetFunctionNameByChar(wParam) == "StartChat()" and Msg == WM_KEYDOWN) then
		g_GameMain.m_SysSetting.m_KPmap:AllKeySet(Msg,wParam)
	end	
end

function CGameAccelerator:OnAcceleratorInEdit() --有输入焦点时的回调
	g_GameMain.m_SysSetting.m_KPmap:ClearAllKeyTbl()
	g_GameMain.m_SysSetting.m_KPmap:MoveEndStep()
end

function CGameMain:ReSetUIOnTransfered()
	if g_MainPlayer then
		--重新绑定
		if g_MainPlayer.m_NpcHeadUpDialog then
			g_MainPlayer:GetRenderObject():AddChild( g_MainPlayer.m_NpcHeadUpDialog, eLinkType.LT_SKELETAL, "Bip01 Head" )
		end
		g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit()	--主角头顶信息
		g_GameMain.m_TeamMarkSignMgr:PlayerHeadTeamMarkInit()	--主角队伍标记
		g_MainPlayer:AddHeadBloodRendler(g_MainPlayer, g_GameMain.m_BloodRenderDialogPool, g_GameMain, g_GameMain.BloodRenderWndIsShow)
		g_GameMain.m_CharacterInSyncMgr:SetHeadBloodTransparentFun(g_MainPlayer,false)
		
		local bSet = g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle)
		g_GameMain.m_PlayerInfo:FlashShow(bSet)
	end
	self:GameMainRegisterAccelerator()
end

function CGameMain:IsWndOpenState(Wnd)
	if Wnd and IsFunction(Wnd.IsShow)	and Wnd:IsShow() then
		return true
	else
		return false
	end
end

function CGameMain:ClearFrameBeginTransfer()--换场景等情况,导致角色重新创建,要清空客户端UI数据
	g_WndMouse:ClearCursorSpecialState()
	self.m_skillProgress_Load:ShowWnd(false)
	if self.m_Buff then
		self.m_Buff:ClearAllBuffState() --角色的buff数据清空
	end
	if self.m_TargetBuff then
		self.m_TargetBuff:ClearAllBuffState()
	end
	if self.m_TargetOfTargetBuff then
		self.m_TargetOfTargetBuff:ClearAllBuffState()
	end
	if self.m_ServantBuffWnd then
		self.m_ServantBuffWnd:ClearAllBuffState()
	end
	if self.m_BattleHorseBuffWnd then
		self.m_BattleHorseBuffWnd:ClearAllBuffState()
	end
	if self.m_Servant then
		self.m_Servant:ShowWnd(false)
	end
	if self.m_OrdnanceMonsterInfo then
		self.m_OrdnanceMonsterInfo:ShowWnd(false)
	end
	if self.m_CannonInfo then
		self.m_CannonInfo:ShowWnd(false)
	end
	if self.m_FbConfirmSecWnd then
		self.m_FbConfirmSecWnd:ShowWnd(false)
	end
	if self.m_FbConfirmWnd then
		self.m_FbConfirmWnd:ShowWnd(false)
	end
	if self.m_ConfirmWnd then
		self.m_ConfirmWnd:ShowWnd(false)
	end
	if self.m_GameDownTimeWnd then
		self.m_GameDownTimeWnd:ShowWnd(false)
	end
	self.m_SysSetting.m_KPmap:ClearAllKeyTbl()
	self.m_SysSetting.m_KPmap:MoveEndStep()
	--self.m_RoleStatus:ShowWnd(false)
	local PromptWnd = g_GameMain.m_GamePromptWnd
	if PromptWnd:IsShow() then
		PromptWnd:ShowWnd(false)
	end
	local StoneWnd = CStone.GetWnd()
	StoneWnd.StonePartUsing:Close()
	local FbGameStatisticsWnd = CFbGameStatisticsWnd.GetWnd()
	FbGameStatisticsWnd:Close()
	local JiFenSaiInfoWnd = CJiFenSaiInfoWnd.GetWnd()
	JiFenSaiInfoWnd:ShowWnd(false)
	if self.m_RobResourceWnd then
		self.m_RobResourceWnd:ShowWnd(false)
	end
	if self.m_SignQueryWnd then
		self.m_SignQueryWnd:ShowWnd(false)
	end
	if self.m_SpecialNpcWnd then 
		self.m_SpecialNpcWnd:ShowWnd(false)
	end
	if self.m_TongRobResExitWnd then
		self.m_TongRobResExitWnd:ShowWnd(false)
	end
	if self.m_TongRobResOverExit then
		self.m_TongRobResOverExit:ShowWnd(false)
	end
	if self.m_RobResCountDown then
		self.m_RobResCountDown:ShowWnd(false)
	end
	
	self.m_FbActionMgrWnd:ShowWnd(false)
	if self.m_WarTransportWnd then
		self.m_WarTransportWnd:ShowWnd(false)
	end
	if self:IsWndOpenState(self.m_MsgBox) then
		self.m_MsgBox:ShowWnd(false)
		self.m_MsgBox = nil
	end
	
	if self.m_PlayerManageWnd and self.m_PlayerManageWnd:IsShow() then
		self.m_PlayerManageWnd:ShowWnd(false)
	end
	--清楚玩家头上的表情信息
	self.m_NpcHeadSignMgr:ClearPlayerFace()
	
	--切场景后,关闭所有打开着的有自动关闭功能的窗口
	CAutoCloseWnd.TransferCloseAutoCloseWnd()
	----------------------
	local DpsWnd = CDpsOutPutWnd.GetWnd()
	DpsWnd:ShowWnd(false)
	
	local StoneWnd = CStone.GetWnd()
	StoneWnd.StonePartUsing:Close()
	
	if self.m_ChangeOutMsgBox then
		self.m_ChangeOutMsgBox:Close()
		self.m_ChangeOutMsgBox = nil
	end
	
	if self:IsWndOpenState(self.m_AreaFbDeadMsgBox) then
		self.m_AreaFbDeadMsgBox:HideWnd()
	end
	
	if self:IsWndOpenState(self.m_RobResDeadMsgBox) then
		self.m_RobResDeadMsgBox:HideWnd()
	end
	
	
	if self:IsWndOpenState(self.m_WarDeadMsgBox) then
		self.m_WarDeadMsgBox:HideWnd()
	end
	
	if self:IsWndOpenState(self.m_ChallengeDeadMsgBox) then
		self.m_ChallengeDeadMsgBox:HideWnd()
	end
	
	if self:IsWndOpenState(self.m_FBDeadMsgBox) then
		self.m_FBDeadMsgBox:HideWnd()
	end
	
	if self:IsWndOpenState(self.m_TeamPVPFBDeadMsgBox) then
		self.m_TeamPVPFBDeadMsgBox:HideWnd()
	end
	
	if self:IsWndOpenState(self.m_OldWarDeadWnd) then
		self.m_OldWarDeadWnd:HideWnd()
	end
	
	if self.m_AreaInfoWnd then
		self.m_AreaInfoWnd.m_IsGetWarZoneState = false
		self.m_AreaInfoWnd.m_WarZoneState = nil
		if IsFunction(self.m_AreaInfoWnd.IsShow) and self.m_AreaInfoWnd:IsShow() then
			self.m_AreaInfoWnd:CloseAreaInfoWnd()
		end
	end
	
	if IsCppBound(g_MainPlayer) then
		g_MainPlayer:DelHeadBloodRendler(g_GameMain.m_BloodRenderDialogPool)
		PopNpcHeadUpDialog(g_MainPlayer)
		--切场景后关闭选择着的对象
		g_MainPlayer:UnLockTarget()
	end
	
	--关闭区域面板
	if self:IsWndOpenState(self.m_AreaInfoWnd) then
		self.m_AreaInfoWnd:CloseAreaInfoWnd()
	end

	if g_MainPlayer and g_MainPlayer.HeadFishingTick then
		EndHeadFishing()
	end
	if g_MainPlayer and g_MainPlayer.HeadHuntTick then
		EndHeadHunt()
	end
	
	if g_GameMain.m_ShootProgressWnd.m_Tick then
		g_GameMain.m_ShootProgressWnd:EndShoot()
	end
	
	g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
	g_GameMain.m_PlayersTradeRequestWndInGameMain:ShowWnd(false)
end

function CGameMain:EndTransfer()
	if g_GameMain.m_PrepareChangeTick then
		UnRegisterTick(self.m_PrepareChangeTick)
		g_GameMain.m_PrepareChangeTick = nil
	end
	self:ReSetUIOnTransfered()
	self.m_NavigationMap:UpdatePlayerPosInSmallMap()
	if not g_App.m_Loading:IsShow() then
		IsSpanSceneAutoTrackState()
	end
	self.m_IsTransfer = false
	g_MainPlayer:MoveOnTime_UnRegisterTick()
	NotifyChangeScene()
end

function CGameMain:GameMainRegisterAccelerator()
	if self.m_KeyAccelerator.GMain == nil then
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_SYSKEYDOWN )
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_SYSKEYUP )
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_ACTIVATE )
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_KEYDOWN )
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_KEYUP )
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_MOUSEWHEEL )
		SQRWnd_RegisterAccelerator( self.m_KeyAccelerator, WM_MOUSEMOVE )
		self.m_KeyAccelerator.GMain	= self
	else
--		print("重复RegisterAccelerator！！！")
	end
end

function CGameMain:GameMainUnRegisterAccelerator()
	SQRWnd_UnRegisterAccelerator( self.m_KeyAccelerator )
	self.m_KeyAccelerator.GMain = nil
end

local ShowInitWndTrueTotalTime = 1500 --单位为毫秒
local InitWndTransparentScale = 0.1
local function ShowInitWndSlowly()
	local wnd = g_GameMain
	wnd.m_MainSkillsToolBar:SetTransparent(InitWndTransparentScale)	--技能快捷栏
	wnd.m_SmallMapBG:SetTransparent(InitWndTransparentScale)
	wnd.m_NavigationMap:SetTransparent(InitWndTransparentScale)
	wnd.m_PlayerInfo:SetTransparent(InitWndTransparentScale)
	wnd.m_ExpBar:SetTransparent(InitWndTransparentScale)
	wnd.m_CreateChatWnd.m_CChatWnd:SetTransparent(InitWndTransparentScale)
	wnd.m_QuestTraceBack:SetTransparent(InitWndTransparentScale)
	wnd.m_MainSkillsToolBar:SetTransparentObj(ShowInitWndTrueTotalTime,false)	--技能快捷栏
	wnd.m_SmallMapBG:SetTransparentObj(ShowInitWndTrueTotalTime,false)
	wnd.m_NavigationMap:SetTransparentObj(ShowInitWndTrueTotalTime,false)
	wnd.m_PlayerInfo:SetTransparentObj(ShowInitWndTrueTotalTime,false)
	wnd.m_ExpBar:SetTransparentObj(ShowInitWndTrueTotalTime,false)
	wnd.m_CreateChatWnd.m_CChatWnd:SetTransparentObj(ShowInitWndTrueTotalTime,false)
	wnd.m_QuestTraceBack:SetTransparentObj(ShowInitWndTrueTotalTime,false)
end

--是否用新建角色进入游戏，决定着像聊天窗口、战斗信息窗口、小地图等是否显示
--为了防止进入游戏卡，这几个窗体显示隐藏，忽略材质管理的函数，调用ShowWndIgnTex
function CGameMain:ShowInitWndOrNot( showOrNot)
	self.m_MainSkillsToolBar:ShowWndIgnTex(showOrNot)	--技能快捷栏
	self.m_SmallMapBG:ShowWndIgnTex(showOrNot)
	self.m_NavigationMap:ShowWndIgnTex(showOrNot)
	self.m_PlayerInfo:ShowWndIgnTex(showOrNot)
	self.m_ExpBar:ShowWndIgnTex(showOrNot)
	self.m_CreateChatWnd.m_CChatWnd:ShowWndIgnTex(showOrNot)
	self.m_QuestTraceBack:ShowWndIgnTex(showOrNot)
	if showOrNot == true then	
		ShowInitWndSlowly()
	end
end

function CGameMain:OnInit()
	CppGameClientInit()
	-- 初始化Entity Manager Handler
	g_GameEntityMgrHandler = CEntityClientMgrHandler:new()
	CEntityClientManager_GetInst():SetHandler(g_GameEntityMgrHandler)

	local nWidth = g_App:GetRootWnd():GetWndWidth()
	local nHeight = g_App:GetRootWnd():GetWndHeight()
	
	local param = WndCreateParam:new()
	param.uStyle = 0x40000000
	param.width = 1024
	param.height = 768
	param:SetParentWnd(g_App:GetRootWnd())
	param.font_size = 12
	self:Create( param )--"I'm GameMain"
	self:SetLockFlag( BitOr( LOCK_LEFT, LOCK_RIGHT, LOCK_TOP, LOCK_BOTTOM ) )
	local Image = WND_IMAGE_LIST:new()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE
	self:SetWndBkImage( Flag, Image )
	self:ShowWnd( false )
	--***************************************************
	self.m_DisplayCommonObj = CDisplayCommon:new()
	_,self.m_SelectObjTooltips = apcall(CreateSelectObjTooltips,self) --对象ToopTips
	
	apcall(self.InitTaskWnd,self)
	--***************************************************
	--一级界面最好都挪到这里(主角头像、目标头像、召唤兽头像、小队头像、小地图、技能快捷栏、聊天框、经验条、功能菜单等)
	_,self.m_MainSkillsToolBar		= apcall(CreateMainSkillsToolBar,self)				--技能快捷栏
	_,self.m_MasterSkillArea		= apcall(CreateMasterSkillArea, self, 2)			--浮动快捷栏
	_,self.m_AddMasterSkillArea		= apcall(CreateMasterSkillArea, self, 3)			--浮动快捷栏
	_,self.m_PlayerCore				= apcall(CreatePlayerCoreInfo,self)					--主角头像脸部
	_,self.m_PlayerInfo				= apcall(CreatePlayerInfo,self)						--主角头像
	_,self.m_TargetCore				= apcall(CreateTargetCoreInfo,self)					--目标头像脸部
	_,self.m_TargetInfo				= apcall(CreateTargetInfo,self)						--目标头像
	_,self.m_TargetOfTargetInfo		= apcall(CreateTargetOfTargetInfo,self)				--目标的目标头像
	_,self.m_CreateChatWnd			= apcall(CreateChatWnd,self)						--聊天系统
	self.m_SmallMapBG				= CSmallMapBG:new(self)								--小地图边框
	self.m_NavigationMap			= CSmallMapWnd:new(self)							--小地图显示
	_,self.m_BurstSoulWnd			= apcall(CreatBurstSoulWnd,self.m_MainSkillsToolBar)--爆魂条
	_,self.m_ExpBar					= apcall(CreateExpBar,self.m_MainSkillsToolBar)		--经验条
	_,self.m_GuideBtn				= apcall(CGuideBtn.new, CGuideBtn, self)
	------------------------------------------------------
	--准一级界面
	_,self.m_PanelMenu				= apcall(CreatePanelMenu, self)
	apcall(self.InitTeam, self)			--初始化小队
	--***************************************************

	self.m_KeyAccelerator 	= CGameAccelerator:new()
	
	--Npc对话框窗口
	_,self.m_NpcDlg = apcall(CreateNpcDlg,self)
	_,self.m_NpcDlgCopy = apcall(CreateNpcDlgCopy,self)
	
	--Npc交互对话窗口
	_, self.m_NpcTalkWnd = apcall( CNpcTalkWnd.new, CNpcTalkWnd )
	_, self.m_AreaFbSelWndNew = apcall( CAreaFbSelWndNew.new, CAreaFbSelWndNew )
	--特殊Npc窗口
	--self.m_SpecialNpcWnd = CreateSpecialNpcWnd(self)
	
	--区域系统面板
	--_,self.m_AreaInfoWnd    	= apcall(CreateAreaInfoWnd,self)
	_, self.m_AreaInfoWnd = apcall(CAreaInfoWnd.new, CAreaInfoWnd)
	_, self.m_GamePromptWnd = apcall(CGamePromptWnd.new, CGamePromptWnd)
	
	--第一次进入区域时弹出的区域介绍面板
	_,self.m_AreaIntroductionWnd = apcall(CreateAreaIntroductionWnd,self)
	
	--排行榜窗口
	_,self.m_SortListWnd 	= apcall(CSortList.new, CSortList)
	
	self:InitEmail()	--邮件
	self:InitCommerce()	--商业类系统
	
	--物品拾取窗口
	_,self.m_PickupItemWnd = apcall(CreatePickupItemWnd,self)
	
	--玩家报bug窗口
	gac_require "message/UserAdvice/UserAdvice"
	_,self.m_UserAdviceWnd = apcall(CUserAdviceWnd.CreateUserAdviceWnd,self)
	
	--物品拾取的拍卖模式窗口
	self.m_AcutionAssignTbls = {}
	--需求分配方式的窗口可弹出多个
	self.m_NeedAssignTbls = {}
	
	--技能吟唱进度条显示
	_,self.m_skillProgress_Load = apcall(CreateSkillLoading,self)
	
	--创建头顶冒泡窗口池
	_,self.m_NpcHeadUpDialogTbl = apcall(SaveNpcHeadUpDialog)	
	
	--角色信息
	_,self.m_PlayerManageWnd 		= apcall(CPlayerManageWnd.new,CPlayerManageWnd)
	_,self.m_AimPlayerManageWnd 	= apcall(CAimPlayerManageWnd.new,CAimPlayerManageWnd)
	_,self.m_AimStatusWnd			= apcall(CAimStatus.new,CAimStatus)
	_,self.m_RoleStatus				= apcall(CPlayerStatus.new,CPlayerStatus)
	_,self.m_PlayerFightInfoWnd 	= apcall(CreatePlayerFightInfoWnd,self )
	self.m_FightingEvaluationWnd 	= CFightingEvaluation:new()
	_,self.m_JiFenDetailWnd	 		= apcall(CJiFenWnd.new,CJiFenWnd)
	self.m_AimFightingEvaluationWnd = CAimFightingEvaluation:new()
	self.FollowerTbl = {}
	
	
	--轮盘界面
	_,self.m_turntable = apcall(turntableCreate,self ) 
		
	--装备升级及强化
	_,self.m_EquipUpIntensifyWnd = apcall(CEquipUpIntensify.new,CEquipUpIntensify)
	
	
	--初始化连击提示面板
	apcall(self.InitComboHitsWnd,self)
	
	--技能系统
	_,self.m_SelectSeriesWnd = apcall(CSelectSeriesWnd.new,CSelectSeriesWnd)
	_,self.m_NpcSkillLearnWnd = apcall(CNpcSkillLearnWnd.new,CNpcSkillLearnWnd)
	_,self.m_CommonSkillWnd = apcall(CCommonSkillWnd.new,CCommonSkillWnd)
	_,self.m_SkillJingLingWnd = apcall(CSkillJingLingWnd.new,CSkillJingLingWnd)
	_,self.m_CreateTalentSkillPartWnd = apcall(CTalentSkillPartWnd.new,CTalentSkillPartWnd)
	_,self.m_LearnCommonSkillWnd = apcall(CLearnCommonSkillWnd.new,CLearnCommonSkillWnd)
	_,self.m_LeadLeanSkillWnd = apcall(CLeadLeanSkillWnd.new,CLeadLeanSkillWnd)
	_,self.m_LeadLeanSkillShowWnd = apcall(CLeadLeanSkillShowWnd.new,CLeadLeanSkillShowWnd)
	
	_,self.m_ShowSkillDescWnd = apcall(CTalentSkillDescWnd.new,CTalentSkillDescWnd)
	
	--临时技能
	_,self.m_TempSkill = apcall(CreateTempSkill,self)
	
	apcall(self.InitTong,self)			--初始化帮会
	apcall(self.InitAssociation,self)	--初始化社群
	apcall(self.InitLiveSkill,self)		--初始化生活技能
	
	InitShowWndFun()
	--头顶信息
	self.m_CharacterInViewMgr = CCharacterInViewMgr:new()
	self.m_CharacterInSyncMgr = CCharacterInSyncMgr:new()
	--队伍标记
	self.m_TeamMarkSignMgr = CTeamMarkSignMgr:new()
	
	--任务Npc或Obj 头顶标志信息 
	_,self.m_NpcHeadSignMgr = apcall(CreateNpcHeadSignWndTbl,self)
	--可以进行采集的Obj的显示窗口
	_,self.m_CollObjWnd = apcall(CCollObjShowWnd.new, CCollObjShowWnd)
	--交互对象系统用的进度条
	_,self.m_ProgressBarWnd = apcall(CProgressBarWnd.new, CProgressBarWnd)
	_,self.m_NormalProgressBarWnd = apcall(CNormalProgressBarWnd.new, CNormalProgressBarWnd)
	_,self.m_ResourceProgressWnd = apcall(CNormalProgressBarWnd.new, CNormalProgressBarWnd)
	_,self.m_UseItemProgressWnd = apcall(CUseItemProgressWnd.new, CUseItemProgressWnd) 
	_,self.m_SandGlassWnd = apcall(CSandGlassWnd.new, CSandGlassWnd) 
	
	--Pk开关
	
	self.m_PkSwitchWnd = CPkSwitch:new(g_GameMain)
	--_,self.m_PkSwitchWnd = apcall(CreatePkSwitch,self)

	--帮会战相关窗口
	_,self.m_WarZoneSelWnd = apcall(CreateWarZoneSelWnd,self)
	
	--动作面板
	_,self.m_WndAction = apcall(CreatePlayerActionWnd,self)
	
	_,self.m_TextItemWnd = apcall(CreateTextItemWnd,self) --文本物品阅读窗口
	
	--答题窗口
	--_,self.m_EssayQuestionWnd = apcall(CreateEssayQuestionWnd,self)
	
	_,self.m_DirectShowWnd = apcall(CreateDirectShowWnd,self)
	
	--小副本记分信息显示板
	_,self.m_MatchGameWnd = apcall(CMatchGameCountWnd.new, CMatchGameCountWnd)
	_,self.m_MatchGameSelWnd = apcall(CMatchGameSelWnd.new, CMatchGameSelWnd)
	--指南针对象
	_,self.m_ChallengeCompass = apcall(CChallengeCompass.new, CChallengeCompass)
	--非战斗行为的蓄力
	_,self.m_XLProgressWnd = apcall(CXLProgressWnd.new, CXLProgressWnd)
	_,self.m_AuctionBasePriceWnd = apcall(CChangeAuctionBasePrice.new, CChangeAuctionBasePrice)
		
	--装备设定属性已满的提示窗口
	_,self.m_EquipEnactmentAttrWnd = apcall(CArmorPieceEnactment.new,CArmorPieceEnactment)
	
	self.m_BloodRenderDialogPool = CBloodRenderDialogPool:new()
	self.m_BloodRenderDialogPool:CreatePool(self)
	
	apcall(self.InitSmallGamesWnd,self)		--初始化小游戏
	apcall(self.InitFbActionWnd,self)		--初始化副本玩法
	apcall(self.InitPlayerBag,self)			--初始化包裹
	apcall(self.PlayerDead,self)			--初始化玩家死亡
	apcall(self.InitMercenaryWnd,self)		--初始佣兵信息窗口
	
	_,self.m_NonFightSkillWnd = apcall(CNonFightSkill.new,CNonFightSkill) 		--初始化非战斗技能面板
	_,self.m_NetworkDelayTimeWnd = apcall(CreateNetworkDelayTimeWnd,self)		--初始化显示网络状态面板
	self.m_NetworkDelayTimeTick = RegisterTick("GetDelayTime", GetDelayTime , 15000)	-- C++代码15秒更新一次延时，没必要这么短时间内刷
	
	_,self.m_SystemMsgInfo = apcall(CreateSystemMsgInfo,self)
	_,self.m_SystemMsgWnd = apcall(CreateSystemMsgWnd,self)
	
	_,self.m_AppellationAndMoralWnd = apcall(CAppellationAndMoralWnd.new,CAppellationAndMoralWnd)

	_,self.m_HidePlayerWnd = apcall(CreateHidePlayerWnd,self)
	_,self.m_HidePlayerExceptTeammateWnd = apcall(CreateHidePlayerExceptTeammateWnd,self)
	
	_,self.m_GMConsole	= apcall(CreateGMConsole,self)			--GM面板
	_,self.m_SysSetting	= apcall(CSystemSettingParent.new,CSystemSettingParent)	--系统设置
	_,self.m_ExitGame		= apcall(CExitGame.new, CExitGame)			--退出游戏
	
	self.m_EquipDuraWnd = CEquipDuraWnd:new() --装备耐久窗口
--	self.m_FixEquipWnd = CFixEquipWnd:new()
	self.m_NewFixEquipWnd = CNewFixEquipWnd:new()
	_,self.m_CEquipInRoleWnd = apcall(CEquipInRoleWnd.new,CEquipInRoleWnd)
	_,self.m_CEquipInBagWnd = apcall(CEquipInBagWnd.new,CEquipInBagWnd)
	_,self.m_FlashMsgWnd = apcall(CFlashMsgWnd.new, CFlashMsgWnd)
	_,self.m_BagShapeEffectWnd = apcall(CreateBagShapeEffectWnd, self)
	_,self.m_FlashSkillMsgWnd = apcall(CFlashSkillMsgWnd.new, CFlashSkillMsgWnd)
	_,self.m_ToolsMallWnd = apcall(CToolsMallMainWnd.new, CToolsMallMainWnd)
	_,self.m_ToolsMallBuyWnd = apcall(CToolsMallBuyWnd.new, CToolsMallBuyWnd)
	_,self.m_ToolsMallGiveItemWnd = apcall(CToolsMallGiveItemWnd.new, CToolsMallGiveItemWnd)
	_,self.m_MessageShapeEffectWnd = apcall(CMessageShapeEffectWnd.new,CMessageShapeEffectWnd)
	_,self.m_MessageShapeEffectSecondWnd =  apcall(CMessageShapeEffectSecondWnd.new,CMessageShapeEffectSecondWnd)
	_,self.m_MessageMiniTextWnd = apcall(CMessageMiniTextWnd.new,CMessageMiniTextWnd)
	
	_,self.m_DirectWnd			= apcall(CreateDirectWnd, self)
	_,self.m_PetInfoWnd			= apcall(CreatePetInfoWnd, self)
	
	self:InitTeamList()
	self.m_HideFollowerMgr = CHideFollowerMgr:new()
	
	self.m_TransportTrap = {} --存附近可传送的trap
	
	
	_, self.m_EquipSuperaddWnd = apcall(CEquipSuperaddWnd.new, CEquipSuperaddWnd)

	_,self.m_ContractManufactureMarketWnd  = apcall(CContractManufactureMarketWnd.new, CContractManufactureMarketWnd)
	
	_,self.m_IdentityAffirmWnd = apcall(CIdentityAffirmWnd.new,CIdentityAffirmWnd)
	
	_,self.m_ItemBagLockWnd = apcall(CItemBagLockWnd.new,CItemBagLockWnd)
	
	_,self.m_ItemBagTimeLockWnd = apcall(CItemBagTimeLockWnd.new,CItemBagTimeLockWnd)
	self:InitGuide()
	self:InitMessageWnd()				--初始化提示类面板
	
----------------------------------------------------------------------------------------
	if g_App.m_re ~= EGameState.eToNewRoleGameMain then 	--非创建新角色时
		self:GameMainRegisterAccelerator()
	else
		self:ShowInitWndOrNot(false)
	end
	
	--鼠标旋转摄像机控制
	self.FreedomCameraCtrl = false
	self.m_NetDelayImageFlag = 0	--网络延时贴图标识
	
	
----------------------------------------------------------------------------------------
	g_App.m_Loading:SetPos(5)
	
	SaveLoginFlow("GameMain:OnInit")
end

function CGameMain:InitGuide()
	_,self.m_GuideData	= apcall(CGuideData.new, CGuideData)
	_,self.m_GuideWnd	= apcall(CGuide.new, CGuide)
end

--提示信息类面板都放到这里
function CGameMain:InitMessageWnd()
	_,self.m_MsgListParentWnd	= apcall(CreateMsgListParentWnd, self)
	_,self.m_SystemFriendMsgBox	= apcall(CSystemFriendMsgBox.new, CSystemFriendMsgBox)
	_,self.m_CenterMsg			= apcall(CCenterMsg.new, CCenterMsg)
end

--邮件
function CGameMain:InitEmail()
	_,self.m_EmailBox		= apcall(CEmailBoxWnd.new,CEmailBoxWnd)	--邮件系统窗口
	_,self.m_SendBox		= apcall(CSendEmailWnd.new,CSendEmailWnd)	--发件箱窗口
	_,self.m_ReceiveBox		= apcall(CGetEmailWnd.new,CGetEmailWnd)	--收件箱窗口
end

--商业类窗口
function CGameMain:InitCommerce()
	--货币兑换窗口
	_,self.m_Agiotage = apcall(CAgiotage.new,CAgiotage)
	--NPC商店购回窗口
	
	--gm补偿奖励
	_,self.m_GmActivity = apcall(CGMActivityWnd.new,CGMActivityWnd)
	_,self.m_GmCompensateItemWnd = apcall(CGMCompensateWnd.new,CGMCompensateWnd)
	
	_,self.m_NPCShopPlayerSold 	= apcall(CNPCShopPlayerSoldWnd.new,CNPCShopPlayerSoldWnd)
	--NPC商店购买窗口
	
	_,self.m_NPCShopSell = apcall(CNPCShopSellWnd.new,CNPCShopSellWnd)
	self.m_NPCShopSell.m_NPCShopGoodsList = {}
	--玩家交易窗口
	
	_,self.m_PlayersTradeWnd = apcall(CPlayersTradeWnd.new,CPlayersTradeWnd)
	--玩家交易请求btn
	_,self.m_PlayersTradeRequestWndInGameMain = apcall(CTradeRequestTipsWnd.new, CTradeRequestTipsWnd)
	--寄售交易所窗口
	
	_,self.m_CSMSellOrderWnd = apcall(CConsignmentSellListWnd.new,CConsignmentSellListWnd)

	_,self.m_CSMSellOrderWnd.TreeListMap = apcall(CConsignmentTreeListMap.CreateCSMTreeListMap)
	self.m_CSMSellOrderWnd:CheckCSMTreeListInfoValidate(self.m_CSMSellOrderWnd.m_TreeDataInPro)
	
	_,self.m_BuyCouponsWnd = apcall(CBuyCouponsWnd.new, CBuyCouponsWnd)
end

--寻求组队
function CGameMain:InitTeamList()
	gac_require "relation/team/TeamClientList"
	gac_require "relation/team/TeamClientListOneItem"
	_,self.m_TeamAppUnderList	= apcall(CreateTeamListUnderWnd, self)
	_,self.m_TeamAppActList		= apcall(CreateTeamListActWnd, self)
	_,self.m_TeamAppTaskList	= apcall(CreateTeamListTaskWnd, self)
	_,self.m_TeamListOneItem	= apcall(CreatTeamListOneItemWnd, self)  
	_,self.m_TeamApplicationListMini = apcall(CreateTeamListMiniWnd, self)
	
end

--佣兵指引功能窗口
function CGameMain:InitMercenaryWnd()
	_, self.m_MercenaryLevelTrace = apcall( CMercenaryLevelTrace.new, CMercenaryLevelTrace )
	--_, self.m_MercenaryLevelAward = apcall( CMercenaryLevelAward.new, CMercenaryLevelAward )
	--_, self.m_MercenaryAssess = apcall( CYbAssessWnd.new, CYbAssessWnd )
	--佣兵指引的日志窗口
	_, self.m_InformationWnd = apcall( CInformationWnd.new, CInformationWnd )
	_, self.m_MercenaryDirWnd = apcall( CYBDirTempFxWnd.new, CYBDirTempFxWnd )
end

--小游戏
function CGameMain:InitSmallGamesWnd()
	--小游戏使用
	self.m_ProgressWnd = CreateProgressWnd(self)
	--脑电波小游戏
	self.m_HeadProgressWnd = CreateHeadProgressWnd(self)
	--得治疗疯马药(水槽)小游戏
	self.m_GetHorseMedicine = CreateGetMedicineWnd(self)
	--和德鲁伊萨兰谈谈小游戏
	self.m_ClickPicWnd = CreateClickPicWnd(self)
	--骑骆驼任务的体力窗口
	self.m_CRideCamelWnd = CreateRideCamelWnd(self)
	
	--小游戏管理类
	self.m_SmallGemeMsgWnd = CreateSmallGameMsgWnd(self)
	self.m_SGMgr = CreatSmallGame()
	self.m_SmallGame = nil    --m_SmallGame = nil ：正常状态    m_SmallGame = "游戏名" ：小游戏状态
end

--副本玩法
function CGameMain:InitFbActionWnd()
	if not self.m_FbActionTick then
		self.m_FbActionTick = {}
	end
	if not self.m_FbActionMsgWnd then
		self.m_FbActionMsgWnd = {}
	end
	if not self.m_AreaFbMsgWnd then
		self.m_AreaFbMsgWnd = {}
	end
	
	--酒吧游戏窗口
	self.m_DrinkWnd = CreateDrinkWnd(self)
	
	--帮会挑战统计窗口
	self.m_TongChallengeStatisticsWnd = CreateTongChallengeStatisticsWnd(self)
	self.m_TongBattlePersonalCountWnd = CreateTongBattlePersonalCountWnd(self)
	self.m_TongWarExitBtnWnd = CreateTongWarExitBtnWnd(self)
	self.m_TongWarBattleCountWnd = CreateTongWarBattleCountWnd(self)
	self.m_TongChallengeExitBtnWnd = CreateTongChallengeExitBtnWnd(self)
	self.m_TongChallengeBattleCountWnd = CreateTongChallengeBattleCountWnd(self)
	self.m_TongMonsAttackCountWnd = CreateTongMonsAttackCountWnd(self)
	self.m_TongMonsAttackExitBtnWnd = CreateTongMonsAttackExitBtnWnd(self)
	self.m_TongDownTimeWnd = CreateTongDownTimeWnd(self)
	--佣兵训练活动
	_, self.m_YbEducateActionWnd = apcall(CYbEducateActionWnd.new, CYbEducateActionWnd)
	_, self.m_YbEducateActInfoWnd = apcall(CYbEducateActionInfoWnd.new, CYbEducateActionInfoWnd)
	_, self.m_YbEducateActAwardWnd = apcall(CYbEducateActionAwardWnd.new, CYbEducateActionAwardWnd)

	_, self.m_YbMercCardBagWnd = apcall(CMercCardBagWnd.new, CMercCardBagWnd) --CMercCardBagWnd:new()
	
	--佣兵团刷怪本
	self.m_MercenaryMonsterFbWnd = CreateMercenaryMonsterFbWnd(self)
	
	--Boss争夺战
	_, self.m_BossBattleWnd = apcall(CBossBattleWnd.new, CBossBattleWnd)
	
	--龙穴活动窗口
	_, self.m_DragonCaveWnd = apcall( CDragonCaveWnd.new, CDragonCaveWnd)
	
	--喝酒射箭副本
	_, self.m_ShootProgressWnd = apcall(CShootProgressWnd.new, CShootProgressWnd)
	
	--副本中部分显示的窗口
	CreateFbActionScoreWnd(self)
	--副本中的信息面板
	self.m_FbActionMgrWnd = CreateFbActionMgrWnd(self)
	_,self.m_EquipRefineWnd = apcall(CEquipRefine.new, CEquipRefine)
	
	--倒计时窗口
	_, self.m_DownTimeWnd = apcall(CDownTimeWnd.new, CDownTimeWnd)
	_, self.m_GameDownTimeWnd = apcall(CCountdown.new, CCountdown)
	
end

--任务
function CGameMain:InitTaskWnd()
	_, self.m_FbActionPanel = apcall(CFbActionPanelWnd.new, CFbActionPanelWnd) --活动信息窗口
	
	_, self.m_takeTask = apcall( CTakeTaskWnd.new, CTakeTaskWnd )	--任务接取窗口
	_, self.m_finishTask = apcall( CFinishTaskWnd.new, CFinishTaskWnd )	--任务交付窗口
	self.m_QuestTraceBack = CreateQuestTraceBack(self) --任务追踪窗口
	self.m_QuestTraceBack:ShowWnd(true)
	self.m_QuestRecordWnd = CreateQuestRecordWnd(self) --任务记录面板
	self.m_HandBookWnd = CHandBookWnd:new() --佣兵手册
	_, self.m_QihunjiZhiyinWnd = apcall( CQihunjiZhiyinWnd.new, CQihunjiZhiyinWnd )	--气魂击指引窗口
	_, self.m_HeadHelpWnd = apcall( CHeadHelpWnd.new, CHeadHelpWnd )	--新手帮助窗口
end

--包裹
function CGameMain:InitPlayerBag()
	--初始化包裹、附背包栏、仓库等容器
	self.m_MainItemBag = CWndItemMainBag:new()
	self.m_MainItemBag:InitBagRoom( self )
	--初始化主背包、附背包栏中的格子
	self.m_MainItemBag:InitBagGrids()
	_, self.m_Depot = apcall(CDepot.new, CDepot)
end

--生活技能
function CGameMain:InitLiveSkill()
	self.m_LiveSkillExpertNPC	= CreateLiveSkillExpertNPCWnd(self)	--学习技能专精的NPC面板
	gac_require "resource/LiveSkillMain"
	self.m_LiveSkillMainWnd		= CreateLiveSkillMainWnd(self)
	self.m_LiveSkillProdMain	= CreateLiveSkillProdMainWnd(self)	--锻造的主面板(其中创建了2个子面板)
	_,self.m_LiveSkillFlower	= apcall(CFlowerWnd.new, CFlowerWnd)
	_,self.m_FlowerMiniWnd		= apcall(CFlowerMiniWnd.new, CFlowerMiniWnd)
end

--小队
function CGameMain:InitTeam()
	local firstX	= 180
	local dis		= 60
	self.m_TeamBase			= CreateTeamBase(self)
	self.m_tblTeamMsgBtn	= {}
	self.m_tblTeamIcons		= {}
	
	for i = 1, 4 do
		self.m_tblTeamMsgBtn[i]				= CreateTeamMsgBtn(self)
		self.m_tblTeamIcons[i]				= CreateTeamIcon(self.m_tblTeamMsgBtn[i])
		self.m_tblTeamIcons[i].m_BuffWnd	= CTeammateBuff:new(self.m_tblTeamIcons[i])
		self.m_tblTeamMsgBtn[i].m_TeamIcon	= self.m_tblTeamIcons[i]
		self.m_tblTeamMsgBtn[i]:SetPos(nil, (firstX + i*dis))
	end
end

--佣兵团
function CGameMain:InitTong()
	self.m_TongBase		= CreateTongBase(self)			--佣兵小队基类
	self.m_TongMainPan	= CTongMainPanWnd:new()			--佣兵小队主面板
	self.m_TongDepot	= CTongDepotWnd:new()			--佣兵小队仓库
	self.m_TongScience	= CTongScienceWnd:new()			--佣兵小队科技
	self.m_TongLearnScience = CTongLearnScience:new()	--佣兵小队科技主动学习
	
--	self.m_TongRobResourceWnd = CreateRobResourceWnd(self)  
--	self.m_TongSignQueryWnd = CreateSignQueryWnd(self)
--	self.m_TongRobResExitWnd = CreateTongRobResExitWnd(self)
--	self.m_TongRobResOverExit = CreateTongRobResOverExit(self)
end

--社群
function CGameMain:InitAssociation()
	self.m_AssociationBase			= CreateAssociationBase()
	self.m_AssociationWnd			= CreateAssociationWnd(self)
	self.m_AssociationFindWnd		= CreateAssociationFindWnd(self)
	self.m_tblAssociationPriChat	= {}
	self.m_tblAssociationPubChat	= {}
	self.m_tblPerInfoShowWnd		= {}
	self.m_tblAddToClassCB			= {}
end

--初始化连击提示面板
function CGameMain:InitComboHitsWnd()
	self.m_ComboHitsWndTbl = {}
	self.ComboHitsInfo = {}
	self.ComboHitsInfo.m_IsCrazyState = false
	for i = 1, 3 do 	
		table.insert(self.m_ComboHitsWndTbl,CComboHitsWnd:new())
	end
	_,self.m_ComboHitsRectWnd = apcall(CComboHitsWnd.new,CComboHitsWnd)
end

--玩家死亡
function CGameMain:PlayerDead()
	self.m_ToDeadMsgBox = CWndDeadMsg:new()
	self.m_AreaFbDeadMsgBox = CAreaFbDeadMsgWnd:new()
	self.m_FBDeadMsgBox = CFBDeadMsgWnd:new()
	self.m_TeamPVPFBDeadMsgBox = CTeamPVPFBDeadMsgWnd:new()
	self.m_WarDeadMsgBox = CWarDeadMsgWnd:new()
	self.m_ChallengeDeadMsgBox = CChallengeDeadMsgWnd:new()
	self.m_RobResDeadMsgBox = CRobResDeadMsgWnd:new()
end
--**********************************************************************************************

function CGameMain:RemoveFile()
	--self.m_AssociationBase:RemoveChatRecord()
end

function CGameMain:OnExit()
	-- 注销Character Manager Handler
	CEntityClientManager_GetInst():SetHandler(nil)
	g_GameEntityMgrHandler = nil
	-- 注销Intobj Manager Handler
--	CIntObjClientManager_Get():SetHandler(nil)
--	g_IntObjMgrHandler = nil

	self:UnRegisterTickOnDestroyGameMain() --注销Tick
	self.m_CharacterInSyncMgr:CloseFollowerHeadInfoWnd()
	g_WndMouse:ClearCursorSpecialState()
	
	self:DestroyWnd()
	self:RemoveFile()
	g_ExcludeWndMgr:Clear()
	
	self.m_MainSkillsToolBar		= nil
	self.m_MasterSkillArea			= nil
	self.m_AddMasterSkillArea		= nil
	self.m_PlayerCore				= nil
	self.m_PlayerInfo				= nil
	self.m_TargetCore				= nil
	self.m_TargetInfo				= nil
	self.m_TargetOfTargetInfo		= nil
	self.m_CreateChatWnd			= nil
	self.m_SmallMapBG				= nil
	self.m_NavigationMap			= nil
	self.m_BurstSoulWnd				= nil
	------------------------------------------------------
	--准一级界面
	self.m_PanelMenu				= nil

	self.m_KeyAccelerator			= nil

	--区域系统面板
	self.m_AreaInfoWnd				= nil
	--第一次进入区域时弹出的区域介绍面板
	self.m_AreaIntroductionWnd		= nil

	--召唤兽,战斗坐骑,攻城器械
	self.m_Servant					= nil
	self.m_BattleHorseWnd 			= nil
	self.m_TruckWnd					= nil
	self.m_OrdnanceMonsterInfo		= nil
	self.m_CannonInfo		 		= nil
	self.m_ServantBuffWnd 			= nil
	self.m_BattleHorseBuffWnd		= nil
	self.m_OrdnanceMonsterBuffWnd	= nil
	self.m_CannonBuffWnd			= nil
	
	--排行榜窗口
	self.m_SortListWnd 	= nil
	
	--邮件
	self.m_EmailBox		= nil
	self.m_SendBox		= nil
	self.m_ReceiveBox	= nil

	--NPC商店购回窗口
	self.m_NPCShopPlayerSold 	= nil

	--NPC商店购买窗口
	self.m_NPCShopSell = nil

	--玩家交易窗口
	self.m_PlayersTradeWnd = nil
	--玩家交易请求btn
	self.m_PlayersTradeRequestWndInGameMain = nil

	--寄售交易所窗口
	self.m_CSMSellOrderWnd 	= nil
	--物品拾取窗口
	self.m_PickupItemWnd 		= nil
	--物品拾取的拍卖模式窗口
	self.m_AcutionAssignTbls = {}
	--需求分配方式的窗口可弹出多个
	self.m_NeedAssignTbls = nil
	
	--宝石,打孔材料合成界面
	self.m_stone = nil
	self.m_stonecompound = nil
	--白宝石鉴定
	self.m_whitestone = nil
	--技能吟唱进度条显示
	self.m_skillProgress_Load = nil
	
	--创建头顶冒泡窗口池
	self.m_NpcHeadUpDialogTbl = nil
	
	--角色信息
	self.m_AimStatusWnd	= nil
	self.m_RoleStatus	= nil
	self.m_PlayerFightInfoWnd = nil
	self.FollowerTbl = nil
	
	--轮盘界面
	self.m_turntable = nil
	
	--装备升级及强化
	self.m_EquipUpIntensifyWnd = nil
	
	--测试显示面板
	self.m_FightInfoWnd = nil
	
	--技能系统
	self.m_SkillJingLingWnd = nil
	
	--临时技能
	self.m_TempSkill = nil
	--Buff Debuff
	self.m_Buff = nil
	self.m_TargetBuff = nil
	self.m_TargetOfTargetBuff = nil

	--装备中提取魂 面板(需要点击相应的NPC)
	self.m_EquipToSoul = nil
	--经验条
	self.m_ExpBar = nil

	--货币兑换窗口
	self.m_Agiotage = nil
	--头顶信息
	self.m_CharacterInViewMgr = nil
	self.m_CharacterInSyncMgr = nil
	--队伍标记
	self.m_TeamMarkSignMgr = nil
	--任务Npc或Obj 头顶标志信息 
	_,self.m_NpcHeadSignMgr = nil
	--Npc对话框窗口
	self.m_NpcDlg = nil
	self.m_NpcDlgCopy = nil
	self.m_SpecialNpcWnd = nil
	--碑文，路牌等类型Obj内容显示窗口
	self.m_ShowContentWnd = nil
	--可以进行采集的Obj的显示窗口
	self.m_CollObjWnd = nil
	--交互对象系统用的进度条
	self.m_ProgressBarWnd = nil
	self.m_NormalProgressBarWnd = nil
	self.m_ResourceProgressWnd = nil
	self.m_UseItemProgressWnd = nil
	self.m_SandGlassWnd = nil
	--Pk开关
	self.m_PkSwitchWnd = nil

	--区域副本报名窗口
	self.m_AreaFbInfoWnd = nil
	self.m_AreaFbSelWnd = nil
	self.m_DareFbListWnd = nil
	
	--帮会战相关窗口
	self.m_WarZoneSelWnd = nil
	
	--召唤功能的的提示窗口
	self.m_TransportHintWnd = nil
	--动作面板
	self.m_WndAction = nil
	
	self.m_TextItemWnd = nil
	
	--答题窗口
	self.m_EssayQuestionWnd = nil
	
	--小副本记分信息显示板
	self.m_MatchGameWnd = nil
	self.m_MatchGameSelWnd = nil
	self.m_FbGameStatisticsWnd = nil
	
	--指南针对象
	self.m_ChallengeCompass = nil
	--非战斗行为的蓄力
	self.m_XLProgressWnd = nil
	--佣兵任务池窗口
	self.m_MercenaryQuestPoolWnd = nil
	--装备设定属性已满的提示窗口
	self.m_EquipEnactmentAttrWnd = nil
	
	self.m_NonFightSkillWnd = nil
	self.m_NetworkDelayTimeWnd = nil
	self.m_SystemMsgInfo = nil
	self.m_SystemMsgWnd = nil
	self.m_AppellationAndMoralWnd = nil

	self.m_HidePlayerWnd = nil
	self.m_HidePlayerExceptTeammateWnd = nil
	
	self.m_GMConsole	= nil
	self.m_SysSetting	= nil
	self.m_ExitGame		= nil
	
	self.m_BloodRenderDialogPool:Release()
	
	self.m_TransportTrap = nil
	self.m_FightingEvaluationWnd = nil
	self.m_RoleLevelAttentionWnd = nil
	self.m_AimFightingEvaluationWnd = nil
	self.m_HideFollowerMgr = nil
	self.m_IdentityAffirmWnd = nil
	self.m_ItemBagLockWnd = nil
	self.m_ItemBagTimeLockWnd = nil
end

function CGameMain:UnRegisterTickOnDestroyGameMain()
	UnRegisterObjOnceTick(g_GameMain)  	 		--注销全局生命周期的一次性tick
		
	--注销Buff图标的Tick
	if self.m_Buff then
		self.m_Buff:ClearAllBuffState() --角色的buff数据清空
	end
	if self.m_TargetBuff then
		self.m_TargetBuff:ClearAllBuffState()
	end
	if self.m_TargetOfTargetBuff then
		self.m_TargetOfTargetBuff:ClearAllBuffState()
	end
	if self.m_ServantBuffWnd then
		self.m_ServantBuffWnd:ClearAllBuffState()
	end
	if self.m_BattleHorseBuffWnd then
		self.m_BattleHorseBuffWnd:ClearAllBuffState()
	end
	if self.m_OrdnanceMonsterBuffWnd then
		self.m_OrdnanceMonsterBuffWnd:ClearAllBuffState()
	end
	if self.m_CannonBuffWnd then
		self.m_CannonBuffWnd:ClearAllBuffState()
	end
	if self.m_FbActionTick then
		for FbType,_ in pairs(self.m_FbActionTick) do
			UnRegisterTick(self.m_FbActionTick[FbType])
		end
	end
	self.m_skillProgress_Load:UnRegisterTick()	--自身流程Tick
	self.m_TargetInfo:UnRegisterTick()			--目标头像，流程Tick
	self.m_TargetOfTargetInfo:UnRegisterTick()	--目标的目标头像，流程Tick
	
	g_GameMain.m_SysSetting.m_KPmap:UnRegisterAccelKeyKey()
	UnRegisterTick( self.m_ProgressBarWnd.m_LoadTime )
	UnRegisterTick( self.m_NormalProgressBarWnd.m_LoadTime )
	UnRegisterTick( self.m_ResourceProgressWnd.m_LoadTime )
	
	
	CTongRobResExitWnd.UnRegisterRobResExitTick()
	CTongRobOverExit.UnRegisterRobResOverExitTick()
	CTongItemUseTrans.UnRegisterItemUsetTick()
	CConfirmWnd.UnRegisterUseItemTick()
	CTongChangeByPos.UnRegisterChangeByPosTick()
	
	CRobResCountDown.UnRegisterRobResDownTick()
	CCountdown.UnRegisterGameDownTimeTick()
	UnRegisterTick( self.m_UseItemProgressWnd.m_LoadTime )
	UnRegisterTick( self.m_SandGlassWnd.m_LoadTime )
	if self.m_FbActionScoreTick then
		UnRegisterTick(self.m_FbActionScoreTick)
	end
	
	self:GameMainUnRegisterAccelerator()
	if self.m_whitestone then
		UnRegisterTick( self.m_whitestone.ontimer )
		UnRegisterTick( self.m_whitestone.AnthorTimer )
	end
	--注销临时技能Tick
	self.m_TempSkill:UnRegisterTempSkill()
	
	--注销强化升级的Tick
	if self.m_EquipUpIntensifyWnd ~= nil then
		self.m_EquipUpIntensifyWnd:UnRegisterSelfTick()
	end
	
	--技能学习升级的Tick
	if self.m_LearnCommonSkillWnd.m_CommonSkillItemTbl then
		for i = 1,#(self.m_LearnCommonSkillWnd.m_CommonSkillItemTbl) do
			local CommonSkillItem =  self.m_LearnCommonSkillWnd.m_CommonSkillItemTbl[i]
			if CommonSkillItem and CommonSkillItem.m_EventBtn.m_UpdateSkillTick then
				UnRegisterTick(CommonSkillItem.m_EventBtn.m_UpdateSkillTick)
				CommonSkillItem.m_EventBtn.m_UpdateSkillTick = nil
			end
		end
	end
	
	--干掉指南针中的Tick
	g_GameMain.m_ChallengeCompass:CancelCompassState()
	--小游戏中动画窗口的Tick
	if self.m_SGMgr.m_ShowEffectTick then
		UnRegisterTick(self.m_SGMgr.m_ShowEffectTick)
		self.m_SGMgr.m_ShowEffectTick = nil
	end
	if self.m_DownTimeTick then
		UnRegisterTick(self.m_DownTimeTick)
		self.m_DownTimeTick = nil
	end

	--脑电波小游戏Tick
	if self.HeadFishingTick then
		UnRegisterTick(self.HeadFishingTick)
		self.HeadFishingTick = nil
	end
	if self.HeadHuntTick then
		UnRegisterTick(self.HeadHuntTick)
		self.HeadHuntTick = nil
	end
	
	--矿洞退出倒计时Tick
	if self.m_IsExitSceneMsgBox and self.m_IsExitSceneMsgBox.m_CountDownTick then
		UnRegisterTick(self.m_IsExitSceneMsgBox.m_CountDownTick)
		self.m_IsExitSceneMsgBox = nil
	end
	
	if self.m_MatchGameWnd.m_ShowTimeTick then
		UnRegisterTick(self.m_MatchGameWnd.m_ShowTimeTick)
		self.m_MatchGameWnd.m_ShowTimeTick = nil
	end
	if self.m_FbActionMgrWnd.m_DownTimeTick then
		UnRegisterTick(self.m_FbActionMgrWnd.m_DownTimeTick)
		self.m_FbActionMgrWnd.m_DownTimeTick = nil
	end
	
	if self.m_EssayQuestionWnd and self.m_EssayQuestionWnd.m_AnswerWndTick then
		UnRegisterTick(self.m_EssayQuestionWnd.m_AnswerWndTick)
		self.m_EssayQuestionWnd.m_AnswerWndTick = nil 
	end
	
	self.m_YbEducateActionWnd:ClearAllTick()
	
	self.m_NpcHeadSignMgr:ClearNpcShowQuestFaceTick()
	
	--倒计时窗口
	CCountdownWnd.ClearAllCountdownWndTick()
	
	--倒计时任务的窗口
	UnRegisterAllQuestTick()
	
	--注销定时管理器的Tick
	g_AlarmClock:ClearTick()
	--注销结束日常玩法活动通知的Tick
	ClearEndActionNotifyTick()
	
	--注销客户端自动关闭窗口的Tick
	CAutoCloseWnd.ClearAutoCloseWndTick()
	
	--注销延时关闭区域介绍面板的Tick
	if self.m_AreaIntroductionWnd.m_CloseWndTick then
		UnRegisterTick(self.m_AreaIntroductionWnd.m_CloseWndTick)
		self.m_AreaIntroductionWnd.m_CloseWndTick = nil
	end
	
	if self.m_StopBackgroundMusicTick then
		UnRegisterTick(self.m_StopBackgroundMusicTick)
		self.m_StopBackgroundMusicTick = nil
	end
	
	if self.m_PlayBackgroundMusicTick then
		UnRegisterTick(self.m_PlayBackgroundMusicTick)
		self.m_PlayBackgroundMusicTick = nil
	end
	
	for _, musicCDTick in pairs(g_MusicCDTbl) do
		UnRegisterTick(musicCDTick)
	end
	
	--注销聊天系统的Tick
	if self.m_CreateChatWnd.m_CSysRollArea.m_ChatWndTick then
		UnRegisterTick(self.m_CreateChatWnd.m_CSysRollArea.m_ChatWndTick)
	end
	
	--注销闪烁提示面板tick
	if self.m_FlashMsgWnd then
		if self.m_FlashMsgWnd.m_ShowFlashMsgWndTick then
			UnRegisterTick(self.m_FlashMsgWnd.m_ShowFlashMsgWndTick)	
			self.m_FlashMsgWnd.m_ShowFlashMsgWndTick = nil			
		end
		if self.m_FlashMsgWnd.m_StartTransTick then
			UnRegisterTick(self.m_FlashMsgWnd.m_StartTransTick)	
			self.m_FlashMsgWnd.m_StartTransTick = nil			
		end		
	end
	
	--注销技能闪烁提示面板tick
	if self.m_FlashSkillMsgWnd then
		if self.m_FlashSkillMsgWnd.m_StartTransTick then
			UnRegisterTick(self.m_FlashSkillMsgWnd.m_StartTransTick)	
			self.m_FlashSkillMsgWnd.m_StartTransTick = nil			
		end		
	end
	
	if self.m_BagShapeEffectWnd then
		if self.m_BagShapeEffectWnd.m_StartbagShapeTick then
			UnRegisterTick(self.m_BagShapeEffectWnd.m_StartbagShapeTick)
			self.m_BagShapeEffectWnd.m_StartbagShapeTick = nil	
		end
	end
	
	if self.m_MessageShapeEffectWnd 
		and self.m_MessageShapeEffectWnd.m_StartMessageShapeTick then
			UnRegisterTick(self.m_MessageShapeEffectWnd.m_StartMessageShapeTick)
			self.m_MessageShapeEffectWnd.m_StartMessageShapeTick = nil
			self.m_MessageShapeEffectWnd.m_RichText:SetWndText("")
	end
	
	if self.m_MessageMiniTextWnd 
		and self.m_MessageMiniTextWnd.m_StartMessageShapeTick then
			UnRegisterTick(self.m_MessageMiniTextWnd.m_StartMessageShapeTick)
			self.m_MessageMiniTextWnd.m_StartMessageShapeTick = nil
			self.m_MessageMiniTextWnd.m_RichText:SetWndText("")
	end
	
	if self.m_MessageShapeEffectSecondWnd 
		and self.m_MessageShapeEffectSecondWnd.m_StartMessageShapeTick then
			UnRegisterTick(self.m_MessageShapeEffectSecondWnd.m_StartMessageShapeTick)
			self.m_MessageShapeEffectSecondWnd.m_StartMessageShapeTick = nil
			self.m_MessageShapeEffectSecondWnd.m_RichText:SetWndText("")
	end	
	
	if self.m_CreateChatWnd.m_CSysRollArea.m_CheckHaveMsgRollTick  then
		UnRegisterTick(self.m_CreateChatWnd.m_CSysRollArea.m_CheckHaveMsgRollTick )
		self.m_CreateChatWnd.m_CSysRollArea.m_CheckHaveMsgRollTick  = nil
	end
	
	if self.m_ShowSkillDescWnd and self.m_ShowSkillDescWnd.m_CloseSkillDescWndTick then
		UnRegisterTick( self.m_ShowSkillDescWnd.m_CloseSkillDescWndTick )
		self.m_ShowSkillDescWnd.m_CloseSkillDescWndTick = nil
	end
	
	--清佣兵指引日志的tick
	if self.m_ShowMercDirTick then
		UnRegisterTick(self.m_ShowMercDirTick)
		self.m_ShowMercDirTick = nil
	end
	
	CShareQuestWnd.UnRegisterShareQuestTick()
	
	--注销倒计时退出游戏的tick
	if ( self.m_ExitGameMsgBox and self.m_ExitGameMsgBox.m_CountDownTick ) then
		UnRegisterTick(self.m_ExitGameMsgBox.m_CountDownTick)
		self.m_ExitGameMsgBox.m_CountDownTick = nil
	end
	
	--注销返回登陆界面的tick
	if ( self.m_ToLoginMsgBox and self.m_ToLoginMsgBox.m_CountDownTick ) then
		UnRegisterTick(self.m_ToLoginMsgBox.m_CountDownTick)
		self.m_ToLoginMsgBox.m_CountDownTick = nil
	end
	
	--注销角色死亡界面tick
	if g_GameMain.m_ToDeadMsgBox ~= nil  then
		if g_GameMain.m_ToDeadMsgBox.m_CountDownTick ~= nil then
			UnRegisterTick(g_GameMain.m_ToDeadMsgBox.m_CountDownTick)
			g_GameMain.m_ToDeadMsgBox.m_CountDownTick = nil
		end
		if g_GameMain.m_ToDeadMsgBox.m_BeforeCountDownTick ~= nil then
			UnRegisterTick(g_GameMain.m_ToDeadMsgBox.m_BeforeCountDownTick)
			g_GameMain.m_ToDeadMsgBox.m_BeforeCountDownTick = nil
		end
	end
	
	if self.m_AreaFbDeadMsgBox then
		if self.m_AreaFbDeadMsgBox.m_CountDownTick then
			UnRegisterTick(self.m_AreaFbDeadMsgBox.m_CountDownTick)
			self.m_AreaFbDeadMsgBox.m_CountDownTick = nil
		end
		self.m_AreaFbDeadMsgBox = nil
	end
	
	if self.m_RobResDeadMsgBox then
		if self.m_RobResDeadMsgBox.m_CountDownTick then
			UnRegisterTick(self.m_RobResDeadMsgBox.m_CountDownTick)
			self.m_RobResDeadMsgBox.m_CountDownTick = nil
		end
		self.m_RobResDeadMsgBox = nil
	end
	
	if g_GameMain.m_FbActionMgrWnd.m_RobResDownTimeTick then
		UnRegisterTick(g_GameMain.m_FbActionMgrWnd.m_RobResDownTimeTick)
		g_GameMain.m_FbActionMgrWnd.m_RobResDownTimeTick = nil
	end
	
	if g_GameMain.m_FbActionMgrWnd.m_RobResourceDownTime then
		UnRegisterTick(g_GameMain.m_FbActionMgrWnd.m_RobResourceDownTime)
		g_GameMain.m_FbActionMgrWnd.m_RobResourceDownTime = nil
	end
	
	if g_GameMain.m_QihunjiZhiyinWnd.m_CloseTick then
		UnRegisterTick(g_GameMain.m_QihunjiZhiyinWnd.m_CloseTick)
		g_GameMain.m_QihunjiZhiyinWnd.m_CloseTick = nil
	end
	
	
	if self.m_FBDeadMsgBox then
		if self.m_FBDeadMsgBox.m_CountDownTick then
			UnRegisterTick(self.m_FBDeadMsgBox.m_CountDownTick)
			self.m_FBDeadMsgBox.m_CountDownTick = nil
		end
		self.m_FBDeadMsgBox = nil
	end
	
	if self.m_TeamPVPFBDeadMsgBox then
		if self.m_TeamPVPFBDeadMsgBox.m_CountDownTick then
			UnRegisterTick(self.m_TeamPVPFBDeadMsgBox.m_CountDownTick)
			self.m_TeamPVPFBDeadMsgBox.m_CountDownTick = nil
		end
		self.m_TeamPVPFBDeadMsgBox = nil
	end
	
	if self.m_WarDeadMsgBox then
		if self.m_WarDeadMsgBox.m_CountDownTick then
			UnRegisterTick(self.m_WarDeadMsgBox.m_CountDownTick)
			self.m_WarDeadMsgBox.m_CountDownTick = nil
		end
		self.m_WarDeadMsgBox = nil
	end
	if self.m_NewRole then
		self.m_NewRole:UnRegisterNewRoleTick() 
	end
	
	if self.m_CameraMoveTick then
		UnRegisterTick(self.m_CameraMoveTick)
		self.m_CameraMoveTick = nil
	end
	
	--注销包裹中的物品剩余时间tick
	UnRegisterTick(g_GameMain.m_MainItemBag.m_UpdateItemLeftTimeInBagTick)
	g_GameMain.m_MainItemBag.m_UpdateItemLeftTimeInBagTick = nil 
	
	if g_GameMain.m_ExitTick ~= nil then
		UnRegisterTick(g_GameMain.m_ExitTick)
		g_GameMain.m_ExitTick = nil 
	end
	
	--注销selectObjTooltips剩余时间的tick
	if( self.m_SelectObjTooltips.m_SelectObjTooltipsTick ) then
		UnRegisterTick( self.m_SelectObjTooltips.m_SelectObjTooltipsTick )
		self.m_SelectObjTooltips.m_SelectObjTooltipsTick = nil
	end
	
	if (MoveCameraTick) then
		UnRegisterTick(MoveCameraTick)
		MoveCameraTick = nil
	end
	
	if (SetPageUpTick) then
		UnRegisterTick(SetPageUpTick)
		SetPageUpTick = nil
	end
	
	if (SetPageDownTick) then
		UnRegisterTick(SetPageDownTick)
		SetPageDownTick = nil
	end
	
	if g_GameMain.m_TurnLeftCameraTick then
		UnRegisterTick(g_GameMain.m_TurnLeftCameraTick)
		g_GameMain.m_TurnLeftCameraTick = nil
	end
	if g_GameMain.m_TurnRightCameraTick then
		UnRegisterTick(g_GameMain.m_TurnRightCameraTick)
		g_GameMain.m_TurnRightCameraTick = nil
	end
	
	--注销寻求组队tick
	if g_GameMain.m_TeamApplicationListMini.m_FreshAppListTick then
		UnRegisterTick(g_GameMain.m_TeamApplicationListMini.m_FreshAppListTick)
		g_GameMain.m_TeamApplicationListMini.m_FreshAppListTick = nil
	end
	 if g_GameMain.m_TeamApplicationListMini.m_ShowUgTimeTick then 
	 	UnRegisterTick(g_GameMain.m_TeamApplicationListMini.m_ShowUgTimeTick)
	 	g_GameMain.m_TeamApplicationListMini.m_ShowUgTimeTick = nil
	end
	if g_GameMain.m_TeamApplicationListMini.m_ShowActTimeTick then
		UnRegisterTick(g_GameMain.m_TeamApplicationListMini.m_ShowActTimeTick)
		g_GameMain.m_TeamApplicationListMini.m_ShowActTimeTick = nil
	end
	if g_GameMain.m_TeamApplicationListMini.m_ShowTaskTimeTick then
		UnRegisterTick(g_GameMain.m_TeamApplicationListMini.m_ShowTaskTimeTick)
		g_GameMain.m_TeamApplicationListMini.m_ShowTaskTimeTick = nil
	end
	
	
	
	 
	self:UnRegisterLiveSkillTick()	--注销生活技能tick
	self:UnRegisterTongTick()		--注销佣兵团tick
	
	if self.m_NetworkDelayTimeTick then
		UnRegisterTick( self.m_NetworkDelayTimeTick )
		self.m_NetworkDelayTimeTick = nil
	end

	if self.m_NeedAssignTbls then
		for i , v in pairs(self.m_NeedAssignTbls) do
			UnRegisterTick(v.m_CountDownTick)
		end
	end
	if self.m_AcutionAssignTbls then
		for i , v in pairs(self.m_AcutionAssignTbls) do
			UnRegisterTick(v.m_CountDownTick)
		end
	end
	
	if g_GameMain.m_JoinChallengeTick then
		UnRegisterTick(g_GameMain.m_JoinChallengeTick)
		g_GameMain.m_JoinChallengeTick = nil
	end
	
	--社群
	local assoWnd = self.m_AssociationWnd
	if(assoWnd and assoWnd.m_AssociationListWnd and assoWnd.m_AssociationListWnd.UpdateOnlineFriendTick) then
		UnRegisterTick(assoWnd.m_AssociationListWnd.UpdateOnlineFriendTick)
		assoWnd.m_AssociationListWnd.UpdateOnlineFriendTick = nil
	end
	
	--活动面板
	self.m_FbActionPanel:VirtualExcludeWndClosed()
	
	-- 修行塔奖励钱
	if self.m_XiuXingTaAddMoneyTick then
		UnRegisterTick(self.m_XiuXingTaAddMoneyTick)
		self.m_XiuXingTaAddMoneyTick = nil
	end
	-- 修行塔奖励魂值
	if self.m_XiuXingTaAddSoulTick then
		UnRegisterTick(self.m_XiuXingTaAddSoulTick)
		self.m_XiuXingTaAddSoulTick = nil
	end
	
	UnRegisterTurnLeftCameraTick()
	UnRegisterTurnRightCameraTick()
	UnRegisterTurnTopCameraTick()
	UnRegisterTurnBottomCameraTick()
	UnRegisterMoveCameraNearTick()
	UnRegisterMoveCameraFarTick()
	if self.m_PrepareChangeTick then
		UnRegisterTick(self.m_PrepareChangeTick)
		self.m_PrepareChangeTick = nil
	end
	if self.m_PrepareChangeCDTick then
		UnRegisterTick(self.m_PrepareChangeCDTick)
		self.m_PrepareChangeCDTick = nil
	end
	self:OnTurnRunCameraEnd()
	
	if g_GameMain.m_StatisticFightInfoWnd and g_GameMain.m_StatisticFightInfoWnd.m_pStatisticTick then
		UnRegisterTick(g_GameMain.m_StatisticFightInfoWnd.m_pStatisticTick)
	end
	if self.m_FightInfoWnd then
		self.m_FightInfoWnd:UnRegisterFightInfoTick()
	end
	
	if self.m_CreateChatWnd.m_CCHMsgWnd.m_CheckHaveMsgTick  then
		UnRegisterTick(self.m_CreateChatWnd.m_CCHMsgWnd.m_CheckHaveMsgTick)
		self.m_CreateChatWnd.m_CCHMsgWnd.m_CheckHaveMsgTick  = nil
	end
	
	if self.m_ItemBagTimeLockWnd.m_ShowTimeTick then
		UnRegisterTick(self.m_ItemBagTimeLockWnd.m_ShowTimeTick)
		self.m_ItemBagTimeLockWnd.m_ShowTimeTick = nil
	end
	
	if self.m_TongNeedFireActivityItemWnd then
		if self.m_TongNeedFireActivityItemWnd.m_CoolDownTick then
			UnRegisterTick(self.m_TongNeedFireActivityItemWnd.m_CoolDownTick)
			self.m_TongNeedFireActivityItemWnd.m_CoolDownTick = nil
		end
	end
	
	ClearDb2GacTimeOutTick()
end

--注销生活技能tick
function CGameMain:UnRegisterLiveSkillTick()
	--种花Tick
	if self.m_FlowerCooldownTick then
		UnRegisterTick(self.m_FlowerCooldownTick)
		self.m_FlowerCooldownTick = nil
	end
--	--锻造、铸甲
--	if(self.m_LiveSkillProdMain.m_LiveSkillProduction and self.m_LiveSkillProdMain.m_LiveSkillProduction.ProdTickFun) then
--		UnRegisterTick(self.m_LiveSkillProdMain.m_LiveSkillProduction.ProdTickFun)
--		self.m_LiveSkillProdMain.m_LiveSkillProduction.ProdTickFun = nil
--	end
--	--淬炼
--	if(self.m_LiveSkillProdMain.m_LiveSkillProdRefine and self.m_LiveSkillProdMain.m_LiveSkillProdRefine.RefineTickFun) then
--		UnRegisterTick(self.m_LiveSkillProdMain.m_LiveSkillProdRefine.RefineTickFun)
--		self.m_LiveSkillProdMain.m_LiveSkillProdRefine.RefineTickFun = nil
--	end
	if self.m_RoleLevelAttentionWnd then
		if self.m_RoleLevelAttentionWnd.m_MoveLevelUpWndTick then
			UnRegisterTick( self.m_RoleLevelAttentionWnd.m_MoveLevelUpWndTick )
			self.m_RoleLevelAttentionWnd.m_MoveLevelUpWndTick = nil
		end
	end
end

--注销佣兵团tick
function CGameMain:UnRegisterTongTick()
	if self.m_TongDownTimeWnd.m_DownTimeTick then
		UnRegisterTick(self.m_TongDownTimeWnd.m_DownTimeTick)
		self.m_TongDownTimeWnd.m_DownTimeTick = nil
	end
	if self.m_TongDownTimeWnd.m_ShowTimeTick then
		UnRegisterTick(self.m_TongDownTimeWnd.m_ShowTimeTick)
		self.m_TongDownTimeWnd.m_ShowTimeTick = nil
	end
	if self.m_TongBattlePersonalCountWnd.m_ShowTimeTick then
		UnRegisterTick(self.m_TongBattlePersonalCountWnd.m_ShowTimeTick)
		self.m_TongBattlePersonalCountWnd.m_ShowTimeTick = nil
	end
	if self.m_TongBattlePersonalCountWnd.m_DownTimeTick then
		UnRegisterTick(self.m_TongBattlePersonalCountWnd.m_DownTimeTick)
		self.m_TongBattlePersonalCountWnd.m_DownTimeTick = nil
	end
	if self.m_TongMonsAttackCountWnd.m_InfoUpdateTick then
		UnRegisterTick(self.m_TongMonsAttackCountWnd.m_InfoUpdateTick)
		self.m_TongMonsAttackCountWnd.m_InfoUpdateTick = nil
	end
	if self.m_TongWarBattleCountWnd.m_InfoUpdateTick then
		UnRegisterTick(self.m_TongWarBattleCountWnd.m_InfoUpdateTick)
		self.m_TongWarBattleCountWnd.m_InfoUpdateTick = nil
	end
	if self.m_TongChallengeBattleCountWnd.m_InfoUpdateTick then
		UnRegisterTick(self.m_TongChallengeBattleCountWnd.m_InfoUpdateTick)
		self.m_TongChallengeBattleCountWnd.m_InfoUpdateTick = nil
	end
	if(self.m_TongBuildingItemCreate and self.m_TongBuildingItemCreate.CreatingBuildingItemTick) then
		UnRegisterTick(self.m_TongBuildingItemCreate.CreatingBuildingItemTick)
		self.m_TongBuildingItemCreate.CreatingBuildingItemTick = nil
	end
	
	if(self.m_TongProductionCenter and self.m_TongProductionCenter.CreatingBuildingItemTick) then
		UnRegisterTick(self.m_TongProductionCenter.CreatingBuildingItemTick)
		self.m_TongProductionCenter.CreatingBuildingItemTick = nil
	end
	

	if(self.m_TongScience and self.m_TongScience.CreatingBuildingItemTick) then
		UnRegisterTick(self.m_TongScience.CreatingBuildingItemTick)
		self.m_TongScience.CreatingBuildingItemTick = nil
	end

	--注销连击提示Tick
	CComboHitsWnd.HideComboHitsWnd()
	
	--活动面板
	self.m_FbActionPanel:VirtualExcludeWndClosed()

	
	--活动面板
	self.m_FbActionPanel:VirtualExcludeWndClosed()
	
	-- 中地图
	if self.m_AreaInfoWnd then
		self.m_AreaInfoWnd:ClearAllTick()
	end
  
	self:OnTurnRunCameraEnd()
	
end

function CGameMain:OnCommand( wParam, lParam )--注册RegisterAccelKeyCommand的响应函数
	g_GameMain.m_SysSetting.m_KPmap:OnCommand( wParam, lParam )
end

--从游戏状态退出到其他状态（登录、角色选择等）不要直接使用coroutine.resume而应该用这个函数
function CGameMain:ExitToState( eGameState )
	CEffectLoader_Inst():Clear()
	if ( eGameState == EGameState.eExitGame ) then
		
		if(g_Conn.ShutDown) then
			g_Conn:ShutDown()
		end
		
	end
	
	if (g_CoreScene ~= nil) then
		g_CoreSceneMgr:DestroyMainScene()
	end
		

	local function ExitTickFun()
		if( g_CoreScene == nil ) then
			UnRegisterTick( self.m_ExitTick )
			g_App:GetRootWnd():DelScreenFx("hpguodi/create")
			StopCue("heartbeat") 
			coroutine.resume( g_App.MainRoutine, eGameState )
		end
	end

	--注册退出的Tick
	if self.m_ExitTick == nil then
	    self.m_ExitTick = RegisterTick( "ExitTickFun", ExitTickFun, 100)
    end
end
	
function CGameMain:main()
	--self:OnInit()
	local re = self:WaitMessage() 
	self:OnExit()
	return re	
end

function CGameMain:OnMouseLeave()
	if g_MainPlayer and IsCppBound(g_MainPlayer) then
		g_MainPlayer:MoveOnTime_UnRegisterTick()
		g_MainPlayer:RunToMouse_UnRegisterTick()
	end
	if g_GameMain and g_GameMain.m_MousePosX and g_GameMain.m_MousePosY and 
		g_GameMain.FreedomCameraCtrl and g_GameMain.bMouseLastActionIsButtonDown == false then
		g_App:GetRootWnd():SetSysCursorPos(g_GameMain.m_MousePosX,g_GameMain.m_MousePosY)
	end
end

function CGameMain:OnMLButtonUpAnyCase()
	if g_MainPlayer and IsCppBound(g_MainPlayer) and g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
		g_MainPlayer:MoveOnTime_UnRegisterTick()
	end
end

function CGameMain:OnMRButtonUpAnyCase()
	if g_MainPlayer and IsCppBound(g_MainPlayer) and not g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag then
		g_MainPlayer:MoveOnTime_UnRegisterTick()
	end
	self:OnTurnRunCameraEnd()
end

function CGameMain:OnLButtonDown( nFlags, x, y )
	if not IsCppBound(g_MainPlayer) or self.m_IsTransfer then --无主角或正在小游戏状态
		return
	end
	if self.m_SmallGame ~= nil or self.m_DoSmallGame then   --小游戏状态
		self.m_SGMgr:RunSmallGameFun(1)
		return 
	end
	if g_GameMain.m_NavigationMap.m_SelectShowWnd:IsShow() then
		g_GameMain.m_NavigationMap.m_SelectShowWnd:ShowWnd(false)
	end
	if g_GameMain.m_ProgressBarWnd.m_IsInPerfectCollState then
		g_GameMain.m_ProgressBarWnd:StopPerfetCollState()
		return
	end
	if( g_MainPlayer and IsCppBound(g_MainPlayer) ) then
		-- 有技能状态则优先处理技能状态，否则再判断是否有鼠标状态，有就不传递鼠标消息给主角
		local State, Context = g_WndMouse:GetCursorState()
		if State == ECursorState.eCS_Split then
			return
		elseif State == ECursorState.eCS_PickupItem then
			return
		elseif State == ECursorState.eCS_PickupEquip then
			return
		else
			if( not g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag ) then
				g_GameMain:OnTurnRunCameraInit()
			end
			g_MainPlayer:OnLButtonDown()
		end
	end
end

--双击主角模型
function CGameMain:OnLButtonDblClk(nFlags, x, y )
	g_GameMain:OnLButtonDown( nFlags, x, y )
end

--按住shift点击地面
function CGameMain:OnShiftLButtonDown( nFlags, posx, posy )
	if not g_MainPlayer then
		return
	end
	if self.m_CreateChatWnd.m_ChatSendArea:IsShow() then
		local EditMessageWnd = self.m_CreateChatWnd.m_ChatSendArea.m_CEditMessage
		local pos = CPos:new()
		g_MainPlayer:GetGridPos(pos)
		local scenename = g_GameMain.m_SceneName or ""
		local str = "[" .. GetSceneDispalyName(scenename) .. "("  .. pos.x .. "," .. pos.y .. ")" .. "]"
		local content = EditMessageWnd:GetWndText()
		--local maxTextLenght = EditMessageWnd:GetMaxTextLenght()*2
		--if string.len(content) < maxTextLenght then
			EditMessageWnd:SetWndText(content .. str)
			EditMessageWnd:SetFocus()
		--end
	else
		self:OnLButtonDown( nFlags, posx, posy )
	end
end

function CGameMain:OnMouseMove( nFlags, posx, posy )
	--self.m_SelectObjTooltips:OnMouseMoveInGameMain()
	self.m_CharacterInSyncMgr:OnMouseMoveInGameMain()
end

function CGameMain:OnLButtonClick( nFlags, x, y )
	self:OnLButtonUp( nFlags, x, y )
end

function CGameMain:OnLButtonUp( nFlags, x, y )
	if not IsCppBound(g_MainPlayer) or self.m_IsTransfer then --无主角或正在小游戏状态
		return
	end
	
	local cssstate, csscontext = g_WndMouse:GetCursorSkillState()
	if( cssstate ~= ECursorSkillState.eCSS_Normal ) then
		g_MainPlayer:OnLButtonUp()
	else
		local State, Context = g_WndMouse:GetCursorState()
		if State == ECursorState.eCS_Split then
			return--切分状态不让动
		elseif State == ECursorState.eCS_PickupEquip then
			local nItemID, eEquipPart, EquipWnd = Context[1], Context[2],Context[3]
			local function DelEquip(EquipWnd, nIndex)
				if nIndex == MB_BtnOK then
					g_WndMouse:ClearCursorAll()
					Gac2Gas:DelEquip(g_Conn,eEquipPart)
				else
					g_WndMouse:ClearCursorAll()
				end
				return true
			end
			if g_MainPlayer:IsInBattleState() then
				g_WndMouse:ClearCursorAll()
				MsgClient(1024)
				return
			end
			EquipWnd.m_MsgBox = MessageBox(g_GameMain,MsgBoxMsg(13008),BitOr(MB_BtnOK,MB_BtnCancel),DelEquip,EquipWnd)
		elseif State == ECursorState.eCS_PickupItem then
			--手上有物品(从物品栏上拾取)
			local fromRoom, fromrow, fromcol, count = Context[1], Context[2],Context[3], Context[4]
			local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc(fromRoom)
			local SrcPos = SrcCt:GetPosByIndex(fromrow, fromcol, fromRoom)
			local Grid = SrcCt:GetGridByIndex(fromrow, fromcol, fromRoom)
			local Type, index = Grid:GetType()
			
			if(g_Client_IsSlotIndex(fromRoom)) then return end --如果是扩展栏上的4个格子中的
				
			--只有拆分状态count才有值，拿去就是获得格子上的全部物品
			if count == nil then
				local SrcGrid = SrcCt:GetGridByIndex(fromrow, fromcol, fromRoom)
				local SrcSize = SrcGrid:Size()
				count = SrcSize
			end
				
			local function DelItem(SrcCt, nIndex)
				if nIndex == MB_BtnOK then
					if g_MainPlayer.m_ItemBagLock then
						MsgClient(160013)
						return
					end		
					g_WndMouse:ClearCursorAll()
					Gac2Gas:OnLButtonUpDelItemByPos(g_Conn,fromRoom,SrcPos,count)
				else
					g_WndMouse:ClearCursorAll()
				end
				return true
			end
			if g_QuestPropMgr[index] then
				local proptype = g_QuestPropMgr[index][1]
				local QuestName = g_QuestPropMgr[index][2]
				if proptype == Type 
					and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName] 
					and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName].m_State ~= QuestState.finish then
					local DisplayName = g_ItemInfoMgr:GetItemLanInfo(proptype,index,"DisplayName")
					local str=MsgBoxMsg(13009, DisplayName, g_GetLanQuestName(QuestName))
					SrcCt.m_MsgBox = MessageBox(g_GameMain,str, BitOr(MB_BtnOK,MB_BtnCancel),DelItem,SrcCt)
				else
					SrcCt.m_MsgBox = MessageBox(g_GameMain,MsgBoxMsg(13008), BitOr(MB_BtnOK,MB_BtnCancel),DelItem,SrcCt)
				end
			elseif g_WhereGiveQuestMgr["Goods"][index]~=nil and Type == 16 then
				local QuestName=g_WhereGiveQuestMgr["Goods"][index][1]
				if g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName] 
					and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName].m_State ~= QuestState.finish then
					local DisplayName = g_ItemInfoMgr:GetItemLanInfo(Type,index,"DisplayName")
					local str=MsgBoxMsg(13009,DisplayName, g_GetLanQuestName(QuestName))
					SrcCt.m_MsgBox = MessageBox(g_GameMain,str, BitOr(MB_BtnOK,MB_BtnCancel),DelItem,SrcCt)
				else
					SrcCt.m_MsgBox = MessageBox(g_GameMain,MsgBoxMsg(13008), BitOr(MB_BtnOK,MB_BtnCancel),DelItem,SrcCt)
				end
			else
				SrcCt.m_MsgBox = MessageBox(g_GameMain,MsgBoxMsg(13008), BitOr(MB_BtnOK,MB_BtnCancel),DelItem,SrcCt)
			end
		elseif State == ECursorState.eCS_PickupItemShortcut then    --手上有物品（从快捷栏上拾取）
			g_WndMouse:ClearCursorAll()
		elseif State == ECursorState.eCS_PickupSkill then           --手上有技能
			g_WndMouse:ClearCursorAll()
		elseif State == ECursorState.eCS_PickupItemFromNPCShop then --手上的物品是从NPC商店获得的
			g_WndMouse:ClearCursorAll()
			--g_WndMouse:SetCursorState( ECursorState.eCS_Normal )
		elseif State == ECursorState.eCS_PickupBattleArrayIcon then --手上是阵法
			g_WndMouse:ClearCursorAll()
			--g_WndMouse:SetCursorState( ECursorState.eCS_Normal )
		else --缺省处理
			g_MainPlayer:OnLButtonUp()
		end
	end	

end

function CGameMain:OnRButtonClick( nFlags, x, y )
	if not IsCppBound(g_MainPlayer) or  self.m_IsTransfer then
		return
	end
	if self.m_SmallGame ~= nil or self.m_DoSmallGame then   --小游戏状态
		return 
	end
	local State, Context = g_WndMouse:GetCursorState()
	if Context ~= nil and Context[1] == g_AddRoomIndexClient.PlayerExpandBag then
		g_WndMouse:ClearCursorAll() 
	elseif( g_MainPlayer and self.bMouseLastActionIsButtonDown == true) then 
		g_MainPlayer:OnRButtonUp()
	elseif State ~= ECursorState.eCS_Normal then
		return
	end
end

function CGameMain:OnRButtonDown( nFlags, x, y )
	if(self.m_Menu) then
		self.m_Menu:Destroy()
		self.m_Menu = nil
	end
	if not IsCppBound(g_MainPlayer) or self.m_IsTransfer then
		return
	end
	if self.m_SmallGame ~= nil or self.m_DoSmallGame then   --小游戏状态
		self.m_SGMgr:RunSmallGameFun(2)
		return 
	end
	
	
	local SkillState = g_WndMouse:GetCursorSkillState()
	--判断右键点击地面的时候鼠标的状态是不是装备鉴定状态或者护甲片设定状态，是的话取消鼠标当前的状态
	if SkillState == ECursorSkillState.eCSS_EquipIdentify or SkillState == ECursorSkillState.eCSS_ArmorPieceEnactment or SkillState == ECursorSkillState.eCSS_EquipSmeltSoul
	    or SkillState == ECursorSkillState.eCSS_RenewEquip  or SkillState == ECursorSkillState.eCSS_BreakItem or
	     SkillState == ECursorSkillState.eCSS_EquipIntenBack then
		g_WndMouse:ClearCursorSpecialState()
	end
	
	local State, Context = g_WndMouse:GetCursorState()
	if State ~= ECursorState.eCS_Normal then
		g_WndMouse:ClearCursorAll()
		return
	elseif( g_MainPlayer ) then
		if (g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag ) then
			g_GameMain:OnTurnRunCameraInit()
		end
		g_MainPlayer:OnRButtonDown()
	end
end

function CGameMain:OnTurnRunCamera(wParam, lParam)
	if g_MainPlayer == nil then
		return
	end
	if self.FreedomCameraCtrl then
		if (BitAnd(wParam,MK_RBUTTON) ~= 0 and g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag) or
			(BitAnd(wParam,MK_LBUTTON) ~= 0 and not g_GameMain.m_SysSetting.m_MouseSettingWnd.m_MoveLeftFlag) then
			local posCursor = CFPos:new()
			g_App:GetRootWnd():GetSysCursorPos(posCursor)
			if GacConfig.SetCameraMouseMoveRange then
				if (self.LastActionCursorPos.x - posCursor.x) > GacConfig.SetCameraMouseMoveRange or
					(self.LastActionCursorPos.x - posCursor.x) < (0-GacConfig.SetCameraMouseMoveRange) or
					(self.LastActionCursorPos.y - posCursor.y) > GacConfig.SetCameraMouseMoveRange or 
					(self.LastActionCursorPos.y - posCursor.y) < (0-GacConfig.SetCameraMouseMoveRange) then
					self.PermitInRangeMouseMove = false
				end
			else
				self.PermitInRangeMouseMove = false
			end
			if self.PermitInRangeMouseMove then
				return
			end
			if self.bMouseLastActionIsButtonDown then
				self:OnTurnRunCameraBegin()
				self.bMouseLastActionIsButtonDown = false
				return
			end
			local posCursorChangedY = posCursor.y - self.m_MousePosY
			local posCursorChangedX = posCursor.x - self.m_MousePosX
			if( posCursorChangedX ==0 and posCursorChangedY == 0 ) or (self.mMouseLastPos == lParam) then
				return
			end
			--why self.mMouseLastPos == lParam 没这个不行。。。
			self.mMouseLastPos = lParam
			RenderScene=g_CoreScene:GetRenderScene()
			RenderScene:SetCameraXRotate( RenderScene:GetCameraXRotate() + posCursorChangedY*0.003 )
			RenderScene:SetCameraYRotate( RenderScene:GetCameraYRotate() + posCursorChangedX*0.003 )
			g_GameMain.m_SysSetting.m_KPmap:Moving(true)
			g_App:GetRootWnd():SetSysCursorPos(g_GameMain.m_MousePosX,g_GameMain.m_MousePosY)
		else
			g_GameMain:OnTurnRunCameraEnd()
		end
	end
end

function CGameMain:OnTurnRunCameraInit()
	self.bMouseLastActionIsButtonDown = true
	if self.FreedomCameraCtrl == false and not self.bIsForbidFreedomCamera then
		self.FreedomCameraCtrl = true
		local posCursor = CFPos:new()
		g_App:GetRootWnd():GetSysCursorPos(posCursor)
		self.LastActionCursorPos = posCursor
		
		local nWidth = g_App:GetRootWnd():GetWndWidth()
		local nHeight = g_App:GetRootWnd():GetWndHeight()
		local GameWndRect = CIRect:new()
		g_App:GetRootWnd():GetClientRect(GameWndRect)
		self.m_MousePosX = tonumber(string.format("%d", nWidth/2+GameWndRect.left))
		self.m_MousePosY = tonumber(string.format("%d", nHeight/2+GameWndRect.top))
		g_App:GetRootWnd():ClipCursor(GameWndRect)
		self.PermitInRangeMouseMove = true
	end
end

local function IsForbidFreedomCamera()
	local scenename = g_GameMain.m_SceneName or ""
	local areaname = g_MainPlayer.m_AreaName or ""
	local keys1 = Area_CameraRule_Client:GetKeys()
	for i=1,#(keys1) do
		if keys1[i] == scenename then
			local Keys2 = Area_CameraRule_Client:GetKeys(keys1[1])
			for k=1, #(Keys2) do
				if Area_CameraRule_Client(scenename,Keys2[k]) == areaname then
					return true
				end
			end
		end
	end
	return false
end

function CGameMain:ResetCameraRotate()
	--设置bIsForbidFreedomCamera
	local bIsForbid = IsForbidFreedomCamera()
	if self.bIsForbidFreedomCamera == bIsForbid then
		return
	end
	self.bIsForbidFreedomCamera = bIsForbid
	if self.bIsForbidFreedomCamera then
		CRenderSystemClient_Inst():SetFreeCameraEnabled(false)
		--MsgClient(198001)
	else
		CRenderSystemClient_Inst():SetFreeCameraEnabled(true)
		--MsgClient(198002)
	end
end

function CGameMain:OnTurnRunCameraBegin()
	g_App:GetRootWnd():ShowCursor(false)
	g_App:GetRootWnd():SetLockCursorMessage(true)
	g_App:GetRootWnd():SetSysCursorPos(self.m_MousePosX,self.m_MousePosY)
	g_App:GetRootWnd():SetCapture()
	CRenderSystemClient_Inst():SetSelectByCursor(false)
	g_MainPlayer:MoveOnTime_UnRegisterTick()
	g_MainPlayer:RunToMouse_UnRegisterTick()
end

function CGameMain:OnTurnRunCameraEnd()
	if self.FreedomCameraCtrl and self.bMouseLastActionIsButtonDown == false then
		g_App:GetRootWnd():SetSysCursorPos(g_GameMain.LastActionCursorPos.x,g_GameMain.LastActionCursorPos.y)
		g_App:GetRootWnd():ShowCursor(true)
		g_App:GetRootWnd():SetLockCursorMessage(false)
		self.LastActionCursorPos = nil
		self.m_MousePosX = nil
		self.m_MousePosY = nil
	end
	self.FreedomCameraCtrl = false
	g_App:GetRootWnd():ClipCursor(nil)
	g_App:GetRootWnd():ReleaseCapture()
	CRenderSystemClient_Inst():SetSelectByCursor(true)
end

local function OnAutoTrackTick()
	IsSpanSceneAutoTrackState()
end

function CGameMain:OnLoading(fPercent)
	if( fPercent >= 100  ) then
		g_App.m_Loading:ShowWnd( false )
		g_App:GetRootWnd():SetLimitedState( true )
		g_GameMain:ShowWnd( true )
		StopCue("music19") 
		StopCue("music20") 
		SetEvent( Event.Test.EnterGame,true )
		RegisterOnceTick(g_App, "WaitingAutoTrack", OnAutoTrackTick, 350)
	else
		if g_App.m_Loading:IsShow() == false then
			g_App.m_Loading:SetRandomText()
		end
		g_App:GetRootWnd():SetLimitedState( false )
		g_App.m_Loading:ShowWnd( true )
		g_GameMain:ShowWnd( false )
		g_App.m_Loading:SetPos( fPercent )
		CRenderSystemClient_Inst():Refresh()
		--这个地方要刷新两次屏幕上显示的才是正确的值
		CRenderSystemClient_Inst():Refresh()
	end
end

function CGameMain:OnDestroyMainScene()
	g_GameMain:GameMainUnRegisterAccelerator()
	g_GameMain:ClearFrameBeginTransfer()
	--UnRegisterNewRoleTick()
end

function CGameMain:OnMainSceneCreated(CoreScene, appGacHandler)
	local pHandler = CGameSceneHandler:new()
	CoreScene:SetHandler(pHandler)
	g_CoreScene = CoreScene
	g_CoreScene.m_MouseMoveTick = RegClassTick( "MouseMoveTick", appGacHandler.OnSelectPosTick, 100, appGacHandler )
	--初始化小地图
	local sSceneName = g_CoreScene:GetMetaScene():GetSceneName()
	local nWidthInPixel = g_CoreScene:GetMetaScene():GetWidthInGrid() * 64 --按像素算的，每个格子64个像素
	local nHeightInPixel = g_CoreScene:GetMetaScene():GetHeightInGrid() * 64 --按像素算的，每个格子64个像素
	g_GameMain.m_NavigationMap:InitData()
	g_GameMain.m_NavigationMap:SetSceneData(500, 500, nWidthInPixel, nHeightInPixel, 45, sSceneName)
	
	--只有创建角色时，才会进入这里，而且角色创建完毕并且传入角色xpos、ypos
	if ( g_App.m_CreateCharSuc ) then
		local newRolePos = CFPos:new(g_GameMain.NewPlayerXPos, g_GameMain.NewPlayerYPos)
		CPositionFX_CppPlayPosFX("fx/setting/other/other/brith/create.efx", "brith/create", newRolePos)
	end
	SetEvent( Event.Test.SceneCreated , true )
	SetEvent( Event.Test.SceneDestroied , false )
end

function CGameMain:OnMainSceneDestroyed()
	--g_GameMain:ClearFrameBeginTransferAfterMainSceneDestroyed()
	UnRegisterTick( g_CoreScene.m_MouseMoveTick )
	--g_CoreScene:SetHandler(nil)
	g_CoreScene = nil
	
	SetEvent( Event.Test.SceneCreated , false )
	SetEvent( Event.Test.SceneDestroied , true )
end



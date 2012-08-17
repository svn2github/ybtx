gac_require "fight/player_info/PlayerInfoInc"
gac_gas_require "team/TeamMgr"
gac_require "framework/texture_mgr/DynImageMgr"
gac_require "relation/team/TeamClient"

local PlayHPPointFxNormal = 0
local PlayHPPointFxRepeat = 1
local PlayHPPointFxScale = 0.2
local PlayHPPoinFxOnceScale = 0.2
local FxName =  "hpguodi/create"

--[[	
	GetCurEG// 能量值
	GetCurRG// 怒气值
	GetCurPE// 潜行能量值
	GetCP//连击点
--]]

function CreatePlayerInfo( parent )
	local wnd = CPlayerInfo:new()
	wnd:CreateFromRes("PlayerInfo", parent)
	wnd:SetIsBottom(true)
	wnd:ShowWnd( true )
	--Wnd.m_AssignMode = EAssignMode.eAM_FreePickup --默认为自由分配
	return wnd
end

function CPlayerInfo:OnChildCreated()
	self.m_WndName			= self:GetDlgChild("WndName")
	self.m_ProgressHP		= self:GetDlgChild("ProgressHP")
	self.m_WndLV			= self:GetDlgChild("WndLV")
	self.m_ProgessOne		= self:GetDlgChild("ProgessOne")
	self.m_WndCaptain		= self:GetDlgChild("WndCaptain")
	self.m_WndShoolParent	= self:GetDlgChild("PlayerShoolParent")
	self.m_Flash			= self:GetDlgChild("FlashWnd")
	self.m_FightingEvaluation = self:GetDlgChild("FightingEvaluationBtn")
	self.m_ProgressHP:SetProgressMode(0)
	self.m_ProgessOne:SetProgressMode(0)
	self.m_ProgessOne:SetRange(100)
	self.m_ProgessOne:SetPos(0)
	self:SetCaptain(false)
	self:FlashShow(false)
	
	self.m_WndShool = CPlayerShoolWnd:new()
	self.m_WndShool:CreateFromRes("PlayerShoolWnd", self.m_WndShoolParent)
	self.m_WndShool:ShowWnd( true )
	self.m_LastTimeHPPoint = 0
	self.m_PlayHPFxFlag = false
end

function CPlayerInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if (Child == self.m_FightingEvaluation) then
			if not g_GameMain.m_PlayerManageWnd:IsShow() then
				g_GameMain.m_PlayerManageWnd:ShowWnd(true)
				g_GameMain.m_FightingEvaluationWnd:ShowWnd(true)
				g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[2]:SetCheck(true)
				g_GameMain.m_JiFenDetailWnd:ShowWnd(false)
				g_GameMain.m_RoleStatus:ShowWnd(false)			
				g_GameMain.m_PlayerManageWnd.m_PlayerNameStatic:SetWndText(g_MainPlayer.m_Properties:GetCharName())		
			elseif not g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[2]:GetCheck() then
				g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[2]:SetCheck(true)
				g_GameMain.m_FightingEvaluationWnd:ShowWnd(true)
				g_GameMain.m_JiFenDetailWnd:ShowWnd(false)
				g_GameMain.m_RoleStatus:ShowWnd(false)
				g_GameMain.m_PlayerManageWnd.m_PlayerNameStatic:SetWndText(g_MainPlayer.m_Properties:GetCharName())					
			else
				g_GameMain.m_PlayerManageWnd:ShowWnd(false)
			end
				
		end
	end
end

--主角创建完成后初始化头像
function CPlayerInfo:InitHeadInfo()
	self.m_nClass			= g_MainPlayer:CppGetClass()
	self.m_WndName:SetWndText( g_MainPlayer.m_Properties:GetCharName() )
	if self.m_nClass == EClass.eCL_Warrior or self.m_nClass == EClass.eCL_OrcWarrior or self.m_nClass == EClass.eCL_DwarfPaladin then
		local strTex = g_ImageMgr:GetImagePath(1349)
		g_DynImageMgr:AddImageByIP(self.m_ProgessOne, strTex, IP_FILL)
	else
		local strTex = g_ImageMgr:GetImagePath(1350)
		g_DynImageMgr:AddImageByIP(self.m_ProgessOne, strTex, IP_FILL)
	end
end

--刷动态信息(血量、魔量等)
function CPlayerInfo:UpdateHeadInfo()
	if(not g_MainPlayer or g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_FeignDeath ))then
		return
	end
	self.m_WndLV:SetWndText( g_MainPlayer:CppGetLevel() )
	
	local nCurMP, nMaxMP = 0, 100
	if(self.m_nClass) then
		if self.m_nClass == EClass.eCL_Warrior or self.m_nClass == EClass.eCL_OrcWarrior then
			nCurMP = g_MainPlayer:CppGetPropertyValueByName("RagePointAgile")
		elseif self.m_nClass == EClass.eCL_DwarfPaladin then
			nCurMP = g_MainPlayer:CppGetPropertyValueByName("EnergyPointAgile")
		else
			nCurMP = g_MainPlayer:CppGetPropertyValueByName("ManaPointAgile")
			nMaxMP = g_MainPlayer:CppGetPropertyValueByName("ManaPoint")
		end
	end
	
	local nCurHP = g_MainPlayer:CppGetPropertyValueByName("HealthPointAgile")
	local nMaxHP = g_MainPlayer:CppGetPropertyValueByName("HealthPoint")
	self:SetHP(nCurHP, nMaxHP)
	self:SetMP(nCurMP, nMaxMP)
	self:HPProgressChange(nCurHP)
	self:HPScreenFxCtrl(nCurHP, nMaxHP)
	
	self.m_LastTimeHPPoint = nCurHP
end

function CPlayerInfo:HPProgressChange(nCurHP)
	local dif = 0 --nCurHP - self.m_LastTimeHPPoint
	if(0 == dif or nCurHP == dif) then return end
	self.m_ProgressHP:SetChangeDiff(dif, 10000)
end

--濒死的全屏特效
function CPlayerInfo:HPScreenFxCtrl(curHPPoint, totalHPPoint)
	if curHPPoint/totalHPPoint < PlayHPPointFxScale then
		if self.m_PlayHPFxFlag == false and  g_App.m_Loading:IsShow() == false then
			g_App:GetRootWnd():PlayScreenFxEx(FxName, PlayHPPointFxRepeat)
			self.m_PlayHPFxFlag = true
			PlayCue("heartbeat") --播放角色濒死状态时的音乐
		end
	else
		self.m_PlayHPFxFlag = false
		g_App:GetRootWnd():DelScreenFx(FxName)
		StopCue("heartbeat") 
	end
	if curHPPoint < self.m_LastTimeHPPoint  then
		local decreasingPoint = self.m_LastTimeHPPoint - curHPPoint
		if decreasingPoint/totalHPPoint > PlayHPPoinFxOnceScale then
			g_App:GetRootWnd():PlayScreenFxEx(FxName, PlayHPPointFxNormal)
		end
	end
end

function CPlayerInfo:SetHP(nCur, nMax)
	self.m_ProgressHP:SetRange(nMax)
	self.m_ProgressHP:SetPos(nCur)
	self.m_ProgressHP:SetWndText( nCur .. "/" .. nMax)
end

function CPlayerInfo:SetMP(nCur, nMax)
	self.m_ProgessOne:SetRange(nMax)
	self.m_ProgessOne:SetPos(nCur)
	self.m_ProgessOne:SetWndText(nCur .. "/" .. nMax)
end

function CPlayerInfo:SetClassPic()
	local nCamp		= g_MainPlayer:CppGetBirthCamp()
	local nClass	= g_MainPlayer:CppGetClass()
	local sEnableImg, sDisableImg, sCamp, sClass = g_DynImageMgr:GetClassPic(nCamp, nClass)
	g_DynImageMgr:AddImageByIP(self.m_WndShool, sEnableImg, IP_ENABLE)
	self.m_WndShool:SetMouseOverDescAfter(sCamp .. "#r" ..sClass)
end

function CPlayerInfo:Show(showOrNot)
	self:ShowWnd(showOrNot)
	--更新头像
	if showOrNot then
		g_GameMain.m_PlayerCore:ShowWnd(true)
		local tblResID = {

				g_GetResIDByEquipPart(EEquipPart.eWear,g_MainPlayer),
				g_GetResIDByEquipPart(EEquipPart.eHead,g_MainPlayer),
		}
		local tblCustomResID = {
				g_MainPlayer.m_Properties:GetFaceResID(),
				g_MainPlayer.m_Properties:GetHairResID(),
		}
		local RenderObject 	= g_GameMain.m_PlayerCore.m_RenderObj
		local uClass 		= g_MainPlayer:CppGetClass()
		local uSex 			= g_MainPlayer.m_Properties:GetSex()
		RenderObject:RemoveAllPiece()
		RenderObject:Delframework()
		InitPlayerCommonAni(RenderObject, uClass, uSex, nil)
		InitCustomCharPortrait(RenderObject, tblCustomResID, uClass, uSex)
		InitCharPortrait(RenderObject, tblResID, uClass, uSex)
		RenderObject:SetPortraitPiece("body")
		RenderObject:DoAni( "stand01_w", true, -1.0 )
		g_GameMain.m_PlayerCore.m_PortraitDlg:AddChild(g_GameMain.m_PlayerCore.m_RenderObj, eLinkType.LT_UNKNOW, "")
	end
end

function CPlayerInfo:FlashShow(bFlag)
	self.m_Flash:ShowWnd(bFlag)
end

function CPlayerInfo:SetRedName()
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	self.m_WndName:SetWndTextColor(Flag,"0xffff0000" )
end

function CPlayerInfo:SetNameColor( Color )
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	self.m_WndName:SetWndTextColor(Flag, Color )
end

function CPlayerInfo:SetCaptain( bFlag )
	self.m_WndCaptain:ShowWnd( bFlag )
end

function CPlayerInfo:OnCancel()
--	print("OnCancel")
end

--离开队伍
function CPlayerInfo:OnLeaveTeam()
	Gac2Gas:LeaveTeam(g_Conn)
end

--解散队伍
function CPlayerInfo:OnDismissTeam()
  Gac2Gas:BreakTeam(g_Conn)
end

function CPlayerInfo:OnTradeClose()
	
end

--[[
function CPlayerInfo:OnTurnPickup()
	self.m_AssignMode = EAssignMode.eAM_TurnPickup
	Gac2Gas:SetAssignMode(g_Conn, self.m_AssignMode)
end
--]]

--	eAM_FreePickup 		   = 0, --自由拾取
--	eAM_AverageAssign 	 = 1, --平均分配
--	eAM_RespectivePickup = 2, --各自拾取
--	eAM_AuctionAssign 	 = 3, --拍卖模式

--各自拾取
function CPlayerInfo:OnRespectivePickup()
	g_GameMain.m_PlayerInfo.m_AssignMode = EAssignMode.eAM_RespectivePickup
	Gac2Gas:SetAssignMode(g_Conn, g_GameMain.m_PlayerInfo.m_AssignMode)
	CPlayerInfo:Init_menu()
end
--自由拾取
function CPlayerInfo:OnFreePickup()
	g_GameMain.m_PlayerInfo.m_AssignMode = EAssignMode.eAM_FreePickup
	Gac2Gas:SetAssignMode(g_Conn, g_GameMain.m_PlayerInfo.m_AssignMode)
	CPlayerInfo:Init_menu()
end
--平均分配
function CPlayerInfo:OnAverageAssign()
	g_GameMain.m_PlayerInfo.m_AssignMode = EAssignMode.eAM_AverageAssign
	Gac2Gas:SetAssignMode(g_Conn, g_GameMain.m_PlayerInfo.m_AssignMode)
	CPlayerInfo:Init_menu()
end
--拍卖模式
function CPlayerInfo:OnAuctionAssign()
	g_GameMain.m_PlayerInfo.m_AssignMode = EAssignMode.eAM_AuctionAssign
	Gac2Gas:SetAssignMode(g_Conn, g_GameMain.m_PlayerInfo.m_AssignMode)
	CPlayerInfo:Init_menu()
end
--需求分配
function CPlayerInfo:OnNeedAssign()
	g_GameMain.m_PlayerInfo.m_AssignMode = EAssignMode.eAM_NeedAssign
	Gac2Gas:SetAssignMode(g_Conn, g_GameMain.m_PlayerInfo.m_AssignMode)
	CPlayerInfo:Init_menu()
end

--	eAS_GrayStandard   = 0, --灰色
--	eAS_WhiteStandard  = 1, --白色
--	eAS_GreenStandard  = 2, --绿色
--	eAS_BlueStandard   = 3, --蓝色
--	eAS_PurpleStandard = 4, --紫色
--	eAS_OrangeStandard = 5, --橙色
--	eAS_YellowStandard = 6, --黄色
--	eAS_CyanStandard   = 7, --青色
--设置拍卖物品的品阶
--灰色
function CPlayerInfo:OnGrayStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_GrayStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--白色
function CPlayerInfo:OnWhiteStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_WhiteStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--黑色
function CPlayerInfo:OnBlackStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_BlackStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--红色
function CPlayerInfo:OnRedStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_RedStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--橙色
function CPlayerInfo:OnOrangeStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_OrangeStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--黄色
function CPlayerInfo:OnYellowStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_YellowStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--绿色
function CPlayerInfo:OnGreenStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_GreenStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--青色
function CPlayerInfo:OnCyanStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_CyanStandard		
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end
--蓝色
function CPlayerInfo:OnBlueStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_BlueStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end

--紫色
function CPlayerInfo:OnPurpleStandard()
	g_GameMain.m_PlayerInfo.m_AuctionStandard = EAuctionStandard.eAS_PurpleStandard	
	Gac2Gas:SetAuctionStandard(g_Conn, g_GameMain.m_PlayerInfo.m_AuctionStandard)
end


function CPlayerInfo:OnAssignMode()
	
end

function CPlayerInfo:OnMenuMsg(func)
	func(self)
	g_GameMain.m_Menu:Destroy()
	g_GameMain.m_Menu = nil
end
function CPlayerInfo:OnClickAssignMode()

end
function CPlayerInfo:OnClickAuctionStandard()

end
function CPlayerInfo:OnClickAuctionBasePrice()
	local bCaptain = g_GameMain.m_TeamBase.m_bCaptain
	if bCaptain then
		g_GameMain.m_AuctionBasePriceWnd:SetAuctionBasePrice()
		g_GameMain.m_AuctionBasePriceWnd:ShowWnd(true)
	end
end
function CPlayerInfo:OnChildMenuMsg(func,temp)
	func(self, temp)
	g_GameMain.m_ChildMenu:Destroy()
	g_GameMain.m_ChildMenu = nil
	g_GameMain.m_Menu:Destroy()
	g_GameMain.m_Menu = nil
end

function CPlayerInfo:OpenTrade()
	g_GameMain.m_ChildMenu = CMenu:new("TargetMenu",g_GameMain.m_Menu)
	local ChildMenu = g_GameMain.m_ChildMenu
	--开启
	ChildMenu:InsertItem(GacMenuText(1301), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnTradeOpen)
	--关闭
	ChildMenu:InsertItem(GacMenuText(1302), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnTradeClose)
	--取消
	ChildMenu:InsertItem(GacMenuText(1), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnCancel)
	
	local Rect = g_GameMain.m_Menu:GetItemRect(1)
	ChildMenu:SetPos(Rect.right, Rect.top)
end

function CPlayerInfo:OpenAssignMode()
	g_GameMain.m_ChildMenu = CMenu:new("TargetMenu",g_GameMain.m_Menu)
	local ChildMenu = g_GameMain.m_ChildMenu
	--轮流拾取
	--ChildMenu:InsertItem(GacMenuText(1405), CPlayerInfo.OnChildMenuMsg, true, false, nil, self,CPlayerInfo.OnTurnPickup)
	--自由拾取
	ChildMenu:InsertItem(GacMenuText(1401), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnFreePickup)
	--各自拾取
	ChildMenu:InsertItem(GacMenuText(1402), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnRespectivePickup)
	--平均分配
	ChildMenu:InsertItem(GacMenuText(1403), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnAverageAssign)
	--需求分配
	ChildMenu:InsertItem(GacMenuText(1406), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnNeedAssign)
	--拍卖模式
	ChildMenu:InsertItem(GacMenuText(1404), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnAuctionAssign)
	--取消
	ChildMenu:InsertItem(GacMenuText(1), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnCancel)
	
	local Rect = g_GameMain.m_Menu:GetItemRect(2)
	ChildMenu:SetPos(Rect.right, Rect.top)
end

function CPlayerInfo:OpenAuctionStandard()
	g_GameMain.m_ChildMenu = CMenu:new("TargetMenu", g_GameMain.m_Menu)
	local ChildMenu = g_GameMain.m_ChildMenu
	--轮流拾取
	--ChildMenu:InsertItem(GacMenuText(1401), CPlayerInfo.OnChildMenuMsg, true, false, nil, self,CPlayerInfo.OnTurnPickup)
	--灰色
	ChildMenu:InsertItem(GacMenuText(1501), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnGrayStandard)
	--白色
	ChildMenu:InsertItem(GacMenuText(1502), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnWhiteStandard)
	--绿色
	ChildMenu:InsertItem(GacMenuText(1503), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnGreenStandard)
	--蓝色
	ChildMenu:InsertItem(GacMenuText(1504), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnBlueStandard)
	--紫色
	ChildMenu:InsertItem(GacMenuText(1505), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnPurpleStandard)
	--橙色
	ChildMenu:InsertItem(GacMenuText(1506), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnOrangeStandard)
	--黄色
	ChildMenu:InsertItem(GacMenuText(1507), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnYellowStandard)
	--青色
	ChildMenu:InsertItem(GacMenuText(1508), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnCyanStandard)	
--	--黑色
--	ChildMenu:InsertItem(GacMenuText(1503), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnBlackStandard)
--	--红色
--	ChildMenu:InsertItem(GacMenuText(1504), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnRedStandard)
	--取消
	ChildMenu:InsertItem(GacMenuText(1), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnCancel)
	
	local Rect = g_GameMain.m_Menu:GetItemRect(2)
	ChildMenu:SetPos(Rect.right, Rect.top)
end

function CPlayerInfo:OpenDice()
	g_GameMain.m_ChildMenu = CMenu:new("TargetMenu",g_GameMain.m_Menu)
	local ChildMenu = g_GameMain.m_ChildMenu
	--无
	ChildMenu:InsertItem(GacMenuText(1601), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnNone)
	--稀有物品
	ChildMenu:InsertItem(GacMenuText(1602), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnXiYou)
	--珍稀物品
	ChildMenu:InsertItem(GacMenuText(1603), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnZhenXi)
	--宝贵物品
	ChildMenu:InsertItem(GacMenuText(1604), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnBaoGui)
	--独有物品
	ChildMenu:InsertItem(GacMenuText(1605), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnDuYou)
	--取消
	ChildMenu:InsertItem(GacMenuText(1), CPlayerInfo.OnChildMenuMsg, nil, true, false, nil, self,CPlayerInfo.OnCancel)
	
	local Rect = g_GameMain.m_Menu:GetItemRect(4)
	ChildMenu:SetPos(Rect.right, Rect.top)
end

function CPlayerInfo:OnChangeName()
	local Wnd = CChangeNameWnd.GetWnd()
	Wnd:ShowAndSetText(GetStaticTextClient(1155))
end

--初始菜单
function CPlayerInfo:Init_menu()

	local PlayerInfo = g_GameMain.m_PlayerInfo
	if nil ~= g_GameMain.m_Menu then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu=nil
	end
	
	local Menu = CMenu:new("TargetMenu",g_GameMain)

	local TeamID = g_MainPlayer.m_Properties:GetTeamID()
	Menu:InsertItem(g_MainPlayer.m_Properties:GetCharName(), CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo, CPlayerInfo.OnTest)
--	if g_MainPlayer:CppGetLevel() >= 15 then
		if g_MainPlayer.m_ChallengeFlag then
			Menu:InsertItem(GacMenuText(113),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnRefChallenge) 
		else 
			Menu:InsertItem(GacMenuText(112),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnAccChallenge) 
		end
--	end
	if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) then
		Menu:InsertItem(GacMenuText(114),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnGiveUpChallenge) 
	end
	Menu:EnableItem(0, true)
	if(TeamID == 0) then --不在队伍中
		
	else --在队伍中
		local bCaptain = g_GameMain.m_TeamBase.m_bCaptain
		local eAM =  g_GameMain.m_PlayerInfo.m_AssignMode
		local Standard = g_GameMain.m_PlayerInfo.m_AuctionStandard or 0
		local BasePrice = g_GameMain.m_PlayerInfo.m_AuctionBasePrice or 0
		if(bCaptain) then
			local str = GacMenuText(1701, AMID2NameMap[eAM] or "") .. " >>"
			Menu:InsertItem(str, CPlayerInfo.OnMenuMsg, nil, false, true, CPlayerInfo.OpenAssignMode, PlayerInfo,CPlayerInfo.OnAssignMode )
			if eAM == EAssignMode.eAM_AuctionAssign or eAM == EAssignMode.eAM_NeedAssign then
				str = GacMenuText(1702, AAMID2NameMap[Standard]) .. " >>"
				Menu:InsertItem(str, CPlayerInfo.OnMenuMsg, nil, false, true, CPlayerInfo.OpenAuctionStandard, PlayerInfo,CPlayerInfo.OnClickAuctionStandard )
			end
			if eAM == EAssignMode.eAM_AuctionAssign then
				gac_gas_require "framework/common/CMoney"
				local moneyMgr = CMoney:new()
				local str = moneyMgr:ChangeMoneyToString(BasePrice, EGoldType.GoldCoin)
				str = GacMenuText(1707, str)
				Menu:InsertItem(str, CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnClickAuctionBasePrice )
			end
			Menu:InsertItem(GacMenuText(1703),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnLeaveTeam )
			Menu:InsertItem(GacMenuText(1704),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnDismissTeam )
		else
			Menu:InsertItem(GacMenuText(1701, AMID2NameMap[eAM] or ""), CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnClickAssignMode )
			if eAM == EAssignMode.eAM_AuctionAssign or eAM == EAssignMode.eAM_NeedAssign then
				Menu:InsertItem(GacMenuText(1702, AAMID2NameMap[Standard]), CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnClickAuctionStandard )
			end
			if eAM == EAssignMode.eAM_AuctionAssign then
				gac_gas_require "framework/common/CMoney"
				local moneyMgr = CMoney:new()
				local str = moneyMgr:ChangeMoneyToString(BasePrice, EGoldType.GoldCoin)
				str = GacMenuText(1707, str)
				Menu:InsertItem(str, CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnClickAuctionStandard )
			end
			Menu:InsertItem(GacMenuText(1703),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnLeaveTeam )
		end
	
		local PlayerInfo = g_GameMain.m_PlayerInfo
		
		if(Menu == nil) then
			return
		end
	end
	if g_MainPlayer:GetServant(ENpcType.ENpcType_LittlePet) then
			local Wnd = CChangeNameWnd.GetWnd()
			Wnd:Init(ENpcType.ENpcType_LittlePet)
			Menu:InsertItem(GacMenuText(1706),CPlayerInfo.OnMenuMsg, nil, false, false, nil, PlayerInfo,CPlayerInfo.OnChangeName)
	end
	local Rect = CFRect:new()
	PlayerInfo:GetLogicRect(Rect)
	Menu:SetPos( Rect.left+70 , Rect.top+70 )
	g_GameMain.m_Menu = Menu
end

function CPlayerInfo:WndProc( uMsgID, uParam1, uParam2 )
	if not g_MainPlayer then
		return--出现这种情况g_MainPlayer还没创建好，出现在新建角色时出发了CPlayerInfo:WndProc
	end
	if ( uMsgID == WM_LBUTTONDOWN ) then
		g_MainPlayer:CancelNormalAttack()
		g_MainPlayer:LockObj(g_MainPlayer)		
	elseif( uMsgID == WM_RBUTTONUP )then
		self:Init_menu()
	end
end

function CPlayerInfo:OnTest()
end

--接受挑战
function CPlayerInfo:OnAccChallenge()
	Gac2Gas:CreateChallengeFlag(g_Conn)--创建挑战旗
end

--结束挑战
function CPlayerInfo:OnRefChallenge()
	Gac2Gas:DestroyChallengeFlag(g_Conn)--销毁挑战旗
end

--放弃挑战(认输)
function CPlayerInfo:OnGiveUpChallenge()
	Gac2Gas:JudgeDistance(g_Conn)
end

function CreateChallengeFlag()
	if g_MainPlayer.m_ChallengeFlag then
		Gac2Gas:DestroyChallengeFlag(g_Conn)
	else
		Gac2Gas:CreateChallengeFlag(g_Conn)
	end
end

---------------------------------------------------------------------------------

function CreatePlayerCoreInfo( Parent )
	local Wnd = CPlayerCoreInfo:new();
	Wnd:CreateFromRes( "PlayerInfoCore", Parent )
	Wnd:SetIsBottom(true)
	Wnd:InitPlayerCoreInfo()
	return Wnd
end

function CPlayerCoreInfo:InitPlayerCoreInfo()
	--print("初始化头像UI资源")
	self.m_PortraitDlg	= CRenderSystemClient_Inst():CreateObjectDialog()
	local param		= WndCreateParam:new()
	param.uStyle	= 0x60000000
	param.x			= 0
	param.y			= 0
	param.width		= self:GetWndOrgWidth()
	param.height	= self:GetWndOrgHeight()
	param:SetParentWnd(self)
	self.m_PortraitDlg:Create(param)
	self.m_PortraitDlg:SetUseSceneDepth( true )
	
	self.m_PortraitDlg:SetViewPortrait( true , g_ImageMgr:GetImagePath(20000),g_ImageMgr:GetImagePath(20001) )
	self.m_RenderObj = self.m_PortraitDlg:CreateRenderObject()
end

--注意销毁C++对象
function CPlayerCoreInfo:OnDestroy()
	self.m_PortraitDlg:DestroyRenderObject(self.m_RenderObj)
	CRenderSystemClient_Inst():DestroyObjectDialog(self.m_PortraitDlg)
end

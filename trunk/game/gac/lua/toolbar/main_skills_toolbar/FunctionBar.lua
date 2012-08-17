gac_require "framework/texture_mgr/DynImageMgr"
cfg_load "toolbar/Funcbar_Client"

CFunctionBar			= class(SQRDialog)
CFunctionBarListItem	= class(SQRDialog)

--打开主角属性面板
--把这个界面挂在主界面上，是为了一开始初始化装备信息的时候就能使用
function OpenRoleStatusPanel()
	if not g_MainPlayer then return end
	
	if 	g_GameMain.m_PlayerManageWnd:IsShow() then
		g_GameMain.m_PlayerManageWnd:ShowWnd(false)
	else
		g_GameMain.m_PlayerManageWnd.m_PlayerNameStatic:SetWndText(g_MainPlayer.m_Properties:GetCharName())	
		g_GameMain.m_PlayerManageWnd:ShowWnd(true)
		
		if g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[1]:GetCheck() then
			g_GameMain.m_RoleStatus:OpenSelfWnd(true )
		elseif g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[2]:GetCheck() then
			g_GameMain.m_FightingEvaluationWnd:ShowWnd(true)
		elseif g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[3]:GetCheck() then
			g_GameMain.m_JiFenDetailWnd:ShowWnd(true)
			g_GameMain.m_JiFenDetailWnd:UpdateJiFenWnd()
			Gac2Gas:GetTongProfferCount(g_Conn)
		end		
	end
end

--打开包裹
function OpenWndMainBag()
	if g_GameMain.m_WndMainBag:IsShow() then
		if g_WndMouse:GetCursorState() == ECursorState.eCS_BreakItem then
			g_WndMouse:ClearCursorSpecialState()
		end
	end
	g_GameMain.m_WndMainBag:ShowWnd(not g_GameMain.m_WndMainBag:IsShow())
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	g_GameMain.m_EquipUpIntensifyWnd:SetWndFlashInfo()
end

--社群
function OpenAssociationWnd()
	local associationWnd = g_GameMain.m_AssociationWnd
	associationWnd:ShowWnd(not associationWnd:IsShow())
end

-- 任务记事本
function OpenHandBookWndByKey()
	g_GameMain.m_HandBookWnd:BeOpenWnd()
end

function UpdateSkillAllWndRect()
	local TempRect = CFRect:new()
	g_GameMain.m_CommonSkillWnd:GetLogicRect(TempRect)
	TempRect.top = TempRect.bottom
	TempRect.bottom = TempRect.top + g_GameMain.m_NonFightSkillWnd:GetWndOrgHeight()
	g_GameMain.m_NonFightSkillWnd:SetLogicRect(TempRect)
	g_GameMain.m_NonFightSkillWnd:ShowWnd(true)
	if g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind and g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind > 0 then
		TempRect.top = TempRect.bottom
		TempRect.bottom = TempRect.top + g_GameMain.m_CreateTalentSkillPartWnd:GetWndOrgHeight()
		g_GameMain.m_CreateTalentSkillPartWnd:SetLogicRect(TempRect)
		g_GameMain.m_CreateTalentSkillPartWnd:ShowWnd(true)
	else
		g_GameMain.m_CreateTalentSkillPartWnd:ShowWnd(false)
	end
	
	g_GameMain.m_SkillJingLingWnd:SetJingLingBtnInfo()
	if g_GameMain.m_SkillJingLingWnd.m_IsShow then
		TempRect.top = TempRect.bottom
		TempRect.bottom = TempRect.top + g_GameMain.m_SkillJingLingWnd:GetWndOrgHeight()		
		g_GameMain.m_SkillJingLingWnd:SetLogicRect(TempRect)
		g_GameMain.m_SkillJingLingWnd:ShowWnd(true)
	else
		g_GameMain.m_SkillJingLingWnd:ShowWnd(false)
	end
end

function OpenSkillAllWnd()
	local Wnd = g_GameMain.m_CommonSkillWnd
	if Wnd.m_InitWnd == nil then
		Wnd.IsOpenWnd = true
		Gac2Gas:RequestSkillSeries(g_Conn)
	else
		if Wnd:IsShow() then
			g_GameMain.m_CommonSkillWnd:ClearAllBtnFlash()
			g_GameMain.m_NonFightSkillWnd:ClearAllBtnFlash()
			g_GameMain.m_CreateTalentSkillPartWnd:ClearAllBtnFlash()	
			CFunctionBar.DelToolBarSkillBtnFlash()	
			Wnd:ShowWnd(false)
		else
			UpdateSkillAllWndRect()
			Wnd:ShowWnd(true)
		end
	end
	if g_GameMain.m_SelectSeriesWnd then
		g_GameMain.m_SelectSeriesWnd:SetTalentNumber()
	end
end

--中地图
function OpenAreaInfoWnd()
	g_GameMain.m_AreaInfoWnd:OpenAreaInfoWnd()
end

--佣兵小队面板
function OpenTongWnd()
	g_GameMain.m_TongMainPan:BeShowTongMain()
end

--修行塔卡片
function OpenMercCardWnd()
	g_GameMain.m_YbMercCardBagWnd:OpenMercCardBag()
end

--打开系统设置面板
function OpenSysPanel()
	if(not g_MainPlayer) then return end
	if( not g_GameMain.m_ExitGame:IsShow() ) then
		if( next(g_ExcludeWndMgr.m_OpenWndRecordTbl) or next(g_ExcludeWndMgr.m_OpenWndRecordTblUnExclude) ) then
			g_ExcludeWndMgr:CloseAllActiveWndExclude()
			g_ExcludeWndMgr:CloseAllActiveWndUnExclude()
		else 
			g_GameMain.m_ExitGame:ShowWnd( true )
		end
	else
		g_GameMain.m_ExitGame:ShowWnd( false )
	end 
	if( g_GameMain.m_ExitGame:IsShow() ) then
		g_GameMain.m_ExitGame:SetActive()
	end
end

--按住I键打开系统菜单
function OpenPanelMenuWnd()
	g_GameMain.m_PanelMenu:OpenOrClosePanel()
end

local tblFunc = {}
tblFunc["角色"]		= OpenRoleStatusPanel
tblFunc["背包"]		= OpenWndMainBag
tblFunc["好友"]		= OpenAssociationWnd
tblFunc["任务"]		= OpenHandBookWndByKey
tblFunc["技能"]		= OpenSkillAllWnd
tblFunc["地图"]		= OpenAreaInfoWnd
tblFunc["佣兵团"]	= OpenTongWnd
tblFunc["收藏"]		= OpenMercCardWnd
tblFunc["商城"]		= CToolsMallMainWnd.OpenToolsMallMainWnd
tblFunc["设置"]		= OpenSysPanel
tblFunc["菜单"]		= OpenPanelMenuWnd

function CreateFunctionBar(parent, nType, dependWnd)
	local wnd = CFunctionBar:new(parent, nType, dependWnd)
	wnd:ShowWnd(true)
	wnd:SetIsBottom(true)
	return wnd
end

function CFunctionBar:Ctor(parent, nType, dependWnd)
	self:InitData(nType, dependWnd)
	self:CreateFromRes("FunctionBar", parent)
	self.m_List = self:GetDlgChild("List")
	self:SetRect()
	self:DrawList()
end

function CFunctionBar:InitData(nType, dependWnd)
	self.m_nType		= nType
	self.m_dependWnd	= dependWnd
	self.m_Shaping		= false
	self.m_bingHide		= false
end

function CFunctionBar:SetKeyRect()
	self.m_EndRect		= CFRect:new()
	self.m_StartRect	= CFRect:new()
	self:GetWndRect(self.m_EndRect)
	local width = self.m_EndRect.right - self.m_EndRect.left
	if(self.m_nType == 1) then
		self.m_StartRect.left	= self.m_EndRect.right
		self.m_StartRect.right	= self.m_StartRect.left + width
	else
		self.m_StartRect.left	= self.m_EndRect.left - width
		self.m_StartRect.right	= self.m_EndRect.left
	end
	self.m_StartRect.top	= self.m_EndRect.top
	self.m_StartRect.bottom	= self.m_EndRect.bottom
end

function CFunctionBar:SetShowPos()
	self:ShowWnd(true)
	self.m_bingHide = false
	self:SetKeyRect()
	self:SetShapeEffect(600, self.m_StartRect, self.m_EndRect)
end

function CFunctionBar:SetHidePos()
	self:ShowWnd(true)
	self.m_bingHide = true
	self:SetKeyRect()
	self:SetShapeEffect(600, self.m_EndRect, self.m_StartRect)
end

function CFunctionBar:DoShapeEffect()
	if(self.m_Shaping) then return end
		
	if(self.m_bingHide) then
		self:SetShowPos()
	else
		self:SetHidePos()
	end
	self.m_Shaping = true
end

function CFunctionBar:SetRect()
	local RectDepend = CFRect:new()
	self.m_dependWnd:GetLogicRect(RectDepend)

	local Rect		= CFRect:new()
	local nWidth	= #( Funcbar_Client(tostring(self.m_nType)):GetKeys() )* 37
	self:GetLogicRect(Rect)
	local height = Rect.bottom - Rect.top
	Rect.left	= self.m_nType == 1 and RectDepend.left - nWidth or RectDepend.right
	Rect.right	= Rect.left + nWidth
	Rect.bottom	= RectDepend.bottom - 2
	Rect.top	= Rect.bottom - height
	self:SetLogicRect(Rect)
end

function CFunctionBar:DrawList()
	local num = #( Funcbar_Client(tostring(self.m_nType)):GetKeys() )
	for i = 1, num do
		self.m_List:InsertColumn(i-1, 37)
	end
	self.m_List:InsertItem(0, 40)
	
	self.m_tblFunctionBarBtn = {}
	for i = 1, num do
		local itemWnd	= self:CreateBarItem(self.m_List:GetSubItem(0, i-1), self.m_nType, i)
		self.m_tblFunctionBarBtn[i] = itemWnd
	end
end

function CFunctionBar:OnShapeEffectEnded()
	if(self.m_bingHide) then
		self:ShowWnd(false)
	end
	self.m_Shaping = false
end

function CFunctionBar:CreateBarItem(parent, nType, nIndex)
	local wnd = CFunctionBarListItem:new(parent, nType, nIndex)
	wnd:ShowWnd(true)
	return wnd
end

function CFunctionBar.SetToolBarSkillBtnFlash()
	g_GameMain.m_tblFunctionBar[1].m_tblFunctionBarBtn[5]:AddFlashInfoByName("toolBarSkillBtn")	
end

function CFunctionBar.DelToolBarSkillBtnFlash()
	local Tbl = {
					g_GameMain.m_NonFightSkillWnd,
					g_GameMain.m_CommonSkillWnd,
					g_GameMain.m_CreateTalentSkillPartWnd,
				}
	
	local Number = 0
	for i = 1,#(Tbl) do 
		if Tbl[i] and Tbl[i].m_FlashBtnNumber then
			if Tbl[i].m_FlashBtnNumber <= 0 then
				Tbl[i].m_FlashBtnNumber = 0
			end
			Number = Tbl[i].m_FlashBtnNumber + Number
		end
	end

	if Number == 0 then
		g_GameMain.m_tblFunctionBar[1].m_tblFunctionBarBtn[5]:DelFlashAttention()
	end
end

function CFunctionBarListItem:Ctor(parent, nType, nIndex)
	self.m_nType	= nType
	self.m_Index	= nIndex
	self:CreateFromResEx("FunctionBarListItem", parent, true, true)
	self.m_Btn		= self:GetDlgChild("Btn")
	self:SetBackImage()
	
	local nTooltipsID	= Funcbar_Client(tostring(self.m_nType), tostring(self.m_Index), "DefaultTooltips")
	self.m_Btn:SetMouseOverDescAfter(GetStaticTextClient(nTooltipsID))
end

function CFunctionBarListItem:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Btn) then
			local sFunc = Funcbar_Client(tostring(self.m_nType), tostring(self.m_Index), "Function")
			tblFunc[sFunc]()
		end
	end
end

function CFunctionBarListItem:SetBackImage()
	local info	= Funcbar_Client(tostring(self.m_nType), tostring(self.m_Index))
	local tbl	=	{	{	IP_ENABLE,		info("ImgEnableX"),		info("ImgEnableY")		},
						{	IP_MOUSEOVER,	info("ImgMouseoverX"),	info("ImgMouseoverY")	},
						{	IP_CLICKDOWN,	info("ImgClickdownX"),	info("ImgClickdownY")	}	}
	for i = 1, 3 do
		local rt = CFRect:new()
		rt.left		= tbl[i][2]
		rt.top		= tbl[i][3]
		rt.right	= rt.left + info("ImgWidth")
		rt.bottom	= rt.top + info("ImgHeight")
		g_DynImageMgr:AddImageNeedCut(self.m_Btn, info("ImgPath"), tbl[i][1], rt, CFPos:new())
	end
end


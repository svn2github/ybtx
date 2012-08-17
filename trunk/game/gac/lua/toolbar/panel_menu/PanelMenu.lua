gac_require "toolbar/panel_menu/PanelMenuInc"

local function SetPos(wnd, x, y)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x+uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y+uHeight
	end
	wnd:SetLogicRect(Rect)
end

function CreatePanelMenu(Parent)
	local Wnd = CPanelMenuWnd:new()
	Wnd:Init(Parent)
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, "·Ç»¥³â")
	return Wnd
end

function CPanelMenuWnd:Init(Parent)
	self:CreateFromRes("PanelMenu", Parent)
end

function CPanelMenuWnd:OnChildCreated()
	self.m_Btn2 = self:GetDlgChild("Btn2")
	self.m_Btn3 = self:GetDlgChild("Btn3")
	self.m_Btn5 = self:GetDlgChild("Btn5")
	self.m_Btn6 = self:GetDlgChild("Btn6")
	self.m_Btn7 = self:GetDlgChild("Btn7")
	self.m_Btn8 = self:GetDlgChild("Btn8")
end

--ÏûÏ¢
function CPanelMenuWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if( Child == self.m_Btn2 ) then
			CAfflatusValueWnd.OpenAfflatusValueWnd()
		elseif( Child == self.m_Btn3 ) then
			OpenStonePanel()
		elseif( Child == self.m_Btn5 ) then
			OpenLiveSkillMainWnd()
		elseif( Child == self.m_Btn6 ) then
			OpenStatisticFightInfoWnd()
		elseif( Child == self.m_Btn7 ) then
			local wnd = CStoneCompound.GetWnd()
			wnd:ShowWnd(not wnd:IsShow())
		elseif( Child == self.m_Btn8 ) then
			local wnd = g_GameMain.m_AppellationAndMoralWnd
			wnd:ShowWnd(true)
		end
	end
end

function CPanelMenuWnd:OpenOrClosePanel()
	self:ShowWnd(not self:IsShow())
	self:SetFocus()
	local rt = CFRect:new()
	g_GameMain.m_tblFunctionBar[2].m_tblFunctionBarBtn[5]:GetLogicRect(rt)
	local wndHeight = self:GetWndOrgHeight()
	SetPos(self, rt.left, rt.top - wndHeight)
end

function CPanelMenuWnd:OnActive( bFlag, wnd )
	if (not bFlag and self:IsShow() and wnd ~= g_GameMain.m_tblFunctionBar[2].m_tblFunctionBarBtn[6]) then
		self:ShowWnd(false)
	end
end

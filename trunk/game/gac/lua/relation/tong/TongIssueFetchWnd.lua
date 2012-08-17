CTongIssueFetchWnd = class(SQRDialog)

function CTongIssueFetchWnd:Ctor(parent)
	self:CreateFromRes("IssueFetchForageWnd", parent)
	self.m_Text = self:GetDlgChild("Text")
	self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
--	self:GetWnd()
end

function CTongIssueFetchWnd:GetWnd()
	if (not g_GameMain.m_FetchWnd) then
		g_GameMain.m_FetchWnd = CTongIssueFetchWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_FetchWnd
	wnd:ShowWnd(true)
	return wnd 
end

function CTongIssueFetchWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_CancelBtn == Child then
			self:ShowWnd(false)
		elseif self.m_XBtn == Child then
			self:ShowWnd(false)
		elseif self.m_ConfirmBtn == Child then
			Gac2Gas:IssueTongInfo(g_Conn)
			self:ShowWnd(false)
		end
	end
end






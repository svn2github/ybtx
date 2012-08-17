CTongCancelSignWnd = class(SQRDialog)

function CTongCancelSignWnd:Ctor(parent)
	self:CreateFromRes("CancelSignWnd",parent) 
	self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("Info")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CTongCancelSignWnd:GetWnd()
	if not g_GameMain.m_CancelSignWnd then
		g_GameMain.m_CancelSignWnd = CTongCancelSignWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_CancelSignWnd
	wnd:ShowWnd(true)
	return Wnd
end

function CTongCancelSignWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_ConfirmBtn then
			Gac2Gas:CancelSign(g_Conn)
		end
		self:ShowWnd(false)
	end
end

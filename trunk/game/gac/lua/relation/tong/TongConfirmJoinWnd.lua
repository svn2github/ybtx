CConfirmJoinWnd = class(SQRDialog)

function CConfirmJoinWnd:Ctor(parent, tongName)
	self:CreateFromRes("ConfirmJoinWnd",parent) 
	self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("info")
--	self.m_FbGameInfo:SetWndText()
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CConfirmJoinWnd:GetWnd(tongName)
	if not g_GameMain.m_ConfirmJoinWnd then
		g_GameMain.m_ConfirmJoinWnd = CConfirmJoinWnd:new(g_GameMain,tongName)
	end
	local wnd = g_GameMain.m_ConfirmJoinWnd
	wnd.m_FbGameInfo:SetWndText(GetStaticTextClient(160006, tongName, tongName))
	wnd:ShowWnd(true)
	return wnd
end

function CConfirmJoinWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ConfirmBtn then
			Gac2Gas:ConfirmJoin(g_Conn)
			--确定报名
		end
		self:ShowWnd(false)
	end
end

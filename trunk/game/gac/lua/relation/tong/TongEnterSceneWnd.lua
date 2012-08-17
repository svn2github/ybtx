CTongEnterSceneWnd = class(SQRDialog)

function CTongEnterSceneWnd:Ctor(parent)
	self:CreateFromRes("EnterSceneWnd",parent) 
	self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("Info")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CTongEnterSceneWnd:GetWnd()
	if not g_GameMain.m_EnterSceneWnd then
		g_GameMain.m_EnterSceneWnd = CTongEnterSceneWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_EnterSceneWnd
	wnd:ShowWnd(true)
	return wnd
end

function CTongEnterSceneWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ConfirmBtn then
			Gac2Gas:EnterRobScene(g_Conn, false)
		end
		self:ShowWnd(false)
	end
end

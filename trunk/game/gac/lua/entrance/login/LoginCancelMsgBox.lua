CLoginCancelMsgBox = class( SQRDialog )

function CLoginCancelMsgBox:Ctor(parent)
	self:CreateFromRes("LoginCancel", parent)
	self.m_KeyHook = CLoginCancelMsgBoxHook:new()
end

function CLoginCancelMsgBox:OnChildCreated()
	self.m_wndInfo		= self:GetDlgChild("MsgRT")
	self.m_btnCancel	= self:GetDlgChild("CancelBtn")
end

function CLoginCancelMsgBox:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == VK_ESCAPE) then
		if(g_Login.m_nConnStatus == 1) then
			g_Login.m_Socket:ShutDown()
			g_Login.m_nConnStatus = 0
		elseif(g_Login.m_nConnStatus == 2) then
			g_Conn:ShutDown()
			g_Login.m_nConnStatus = 0
		else
			if g_Conn and g_Conn.ShutDown then
				g_Conn:ShutDown()
			end
		end
		self:ShowWnd(false)
		g_Login.m_IsConnecting = false
		coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
	end
end

function CLoginCancelMsgBox:OnDestroy()
	SQRWnd_UnRegisterHook(self.m_KeyHook)
end
--gac_require "character/pk_switch/PkSwitchMsgWndInc"

CPkSwitchMsg = class (SQRDialog)

function CPkSwitchMsg:Ctor(Parent)
	self:CreateFromRes("WndDeadMsg", Parent)
	self.m_WndInfo			 	= self:GetDlgChild("MsgInfo")
	self.m_WndInfo2			 	= self:GetDlgChild("MsgInfo2")
	self.m_WndInfo3			 	= self:GetDlgChild("MsgInfo3")
	self.m_BtnOk					= self:GetDlgChild("BtnOK")
	self.m_BtnCancel 			= self:GetDlgChild("BtnCancel")	
	self.m_BtnOk:SetWndText(GetStaticTextClient(12024))
	self.m_WndInfo:SetWndText("")
	self.m_WndInfo2:SetWndText(GetStaticTextClient(12021))
	self.m_WndInfo3:SetWndText("")
	self.m_BtnCancel:SetWndText(GetStaticTextClient(1025))
	
end

function CPkSwitchMsg:GetWnd()
	local Wnd = g_GameMain.m_PkSwitchMsgWnd 
	if not Wnd then
		Wnd = CPkSwitchMsg:new(g_GameMain)
		g_GameMain.m_PkSwitchMsgWnd = Wnd
	end
	Wnd:ShowWnd(true)
	return Wnd
end

function CPkSwitchMsg:Destroy()
	self:ShowWnd(false)
	self:DestroyWnd()
	g_GameMain.m_PkSwitchMsgWnd = nil
end

function CPkSwitchMsg:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnCancel == Child then
			self:Destroy()
		elseif self.m_BtnOk == Child then
			self:Destroy()
			Gac2Gas:OpenPkSwitch(g_Conn, false)
		end
	end
end

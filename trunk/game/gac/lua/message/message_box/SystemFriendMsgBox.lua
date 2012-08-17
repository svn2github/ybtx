CSystemFriendMsgBox = class(SQRDialog) --导出类

function CSystemFriendMsgBox:Ctor()
	self:CreateFromRes("SystemFriendChat", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, "非互斥")
end

function CSystemFriendMsgBox:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_OutPutText	= self:GetDlgChild("OutPutText")
end

function CSystemFriendMsgBox:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CSystemFriendMsgBox:OpenPanel(sText)
	self.m_OutPutText:SetWndText(sText)
	self:ShowWnd(true)
end
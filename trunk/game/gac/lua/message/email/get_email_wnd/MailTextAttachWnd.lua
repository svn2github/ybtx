CMailTextAttachWnd = class(SQRDialog)

function CMailTextAttachWnd.CreateMailTextAttachWnd(Parent)
	local wnd = CMailTextAttachWnd:new()
	wnd:CreateFromRes("MailTextAttach",Parent)
	wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 3)
	return wnd
end

--用成员变量存储查看邮件面板中的子控件
function CMailTextAttachWnd:OnChildCreated()
	self.m_Close		=	self:GetDlgChild("Close")
	self.m_MailText		=	self:GetDlgChild("MailText")
	self.m_MailText:SetMouseWheel(true)
end

function CMailTextAttachWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if Child == self.m_Close then
			self:ShowWnd(false)
		end
	end
end

gac_require "message/text_item_wnd/TextItemWndInc"

function CreateTextItemWnd(Parent)
	local wnd = CTextItemWnd:new()
	wnd:CreateFromRes("TextItemWnd", Parent)
	wnd:Init()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	return wnd
end

--用成员变量存储查看邮件面板中的子控件
function CTextItemWnd:Init()
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_Text = self:GetDlgChild("Text")
	self.m_Text:SetMouseWheel(true)
end

function CTextItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end
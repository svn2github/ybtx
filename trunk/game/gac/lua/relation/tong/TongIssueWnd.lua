CIssueWnd = class(SQRDialog)

function CIssueWnd:Ctor(parent, time)
	self:CreateFromRes("IssueWnd", parent)
	self.m_IssueInfo = self:GetDlgChild("IssueInfo")
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_IssueInfo:SetWndText(GetStaticTextClient(320002,time))
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
	self:GetWnd()
end

function CIssueWnd:GetWnd(time)
	g_GameMain.m_IssueWnd = self
	self.m_IssueInfo:SetWndText(GetStaticTextClient(320002,time))
	self:ShowWnd(true)
	return self
end

function CIssueWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_OkBtn == Child then
			self:ShowWnd(false)
		end
	end
end
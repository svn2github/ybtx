CWaitAcceptChallengeWnd = class(SQRDialog)

function CWaitAcceptChallengeWnd.CreateWaitAcceptChallWnd(Parent,TargetName)

	local str = GetStaticTextClient(12502, TargetName)
	local Wnd = CWaitAcceptChallengeWnd:new()
	Wnd:Init(Parent)
	Wnd.m_Text:SetWndText(str)
	Wnd:ShowWnd(true)
	return Wnd
end

function CWaitAcceptChallengeWnd:Init(Parent)
	self:CreateFromRes("WaitAcceptChallenge", Parent)
	self:ShowWnd( true )
end

function CWaitAcceptChallengeWnd:OnChildCreated()
	self.m_CancelBtn = self:GetDlgChild("Cancel")
	self.m_Text = self:GetDlgChild("Text")
end

function CWaitAcceptChallengeWnd:Destroy()
	self:ShowWnd(false)
	self:DestroyWnd()
	g_GameMain.m_WaitAcceptMsg = nil
end

function CWaitAcceptChallengeWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CancelBtn then
			Gac2Gas:JudgeDistance(g_Conn)
			self:Destroy()
		end
	end
end

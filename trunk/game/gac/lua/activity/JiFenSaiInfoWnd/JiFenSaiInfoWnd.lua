CJiFenSaiInfoWnd = class(SQRDialog)

function CJiFenSaiInfoWnd:Ctor(parent)
	self:CreateFromRes("JiFenSaiInfoWnd", parent)
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_KillNum = self:GetDlgChild("KillNum")
	self.m_DeadNum = self:GetDlgChild("DeadNum")
	self.m_WinNum	 = self:GetDlgChild("WinNum")
	self.m_WinPro	 = self:GetDlgChild("WinPro")
	self.m_LoseNum = self:GetDlgChild("LoseNum")
	self.m_RunNum	 = self:GetDlgChild("RunNum")
	self.m_Integral	 = self:GetDlgChild("Integral")
end

function CJiFenSaiInfoWnd:GetWnd()
	local Wnd = g_GameMain.m_JiFenSaiInfoWnd
	if not Wnd then
		Wnd = CJiFenSaiInfoWnd:new(g_GameMain)
		g_GameMain.m_JiFenSaiInfoWnd = Wnd
	end
	return Wnd
end

function CJiFenSaiInfoWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
		end
	end
end

function CJiFenSaiInfoWnd:RetShowJiFenSaiInfoWnd(WinNum, LoseNum, RunNum, WinPro, KillNum, DeadNum, Integral)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self.m_KillNum:SetWndText(KillNum)
	self.m_DeadNum:SetWndText(DeadNum)
	self.m_WinNum :SetWndText(WinNum)
	self.m_WinPro :SetWndText((math.floor(WinPro*10000)/100).."%")
	self.m_LoseNum:SetWndText(LoseNum)
	self.m_RunNum :SetWndText(RunNum)
	self.m_Integral:SetWndText(Integral)
	self:ShowWnd(true)
end

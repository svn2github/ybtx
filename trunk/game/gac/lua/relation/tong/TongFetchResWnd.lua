CTongFetchResWnd  = class(SQRDialog)

function CTongFetchResWnd:Ctor(Parent)
	self:CreateFromRes("TongFetchResWnd", Parent)
	self.m_Total = self:GetDlgChild("Total")
	self.m_Left = self:GetDlgChild("Left")
	self.m_Fetch = self:GetDlgChild("Fetch")
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_IntroLbl = self:GetDlgChild("IntroLbl")
	self.m_IntroLbl:SetWndText(GetStaticTextClient(320004))
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongFetchResWnd.GetWnd(LeftFetchResNum, FetchNum)
	if not g_GameMain.m_TongFetchRes then
		g_GameMain.m_TongFetchRes = CTongFetchResWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_TongFetchRes
	wnd.m_Total:SetWndText(50)
	wnd.m_Left:SetWndText(LeftFetchResNum)
	wnd.m_Fetch:SetWndText(FetchNum)
	if LeftFetchResNum == 0 then
		wnd.m_OkBtn:EnableWnd(false)
	else
		wnd.m_OkBtn:EnableWnd(true)
	end
	wnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
end

--ÏûÏ¢
function CTongFetchResWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_CancelBtn or Child == self.m_XBtn then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
				MsgClient(193526)
				return 
			end
			Gac2Gas:FetchResource(g_Conn)
			self:ShowWnd(false)
		end
	end
end

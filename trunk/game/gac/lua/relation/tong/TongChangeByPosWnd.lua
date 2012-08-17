CTongChangeByPos = class(SQRDialog)

function CTongChangeByPos:Ctor(Parent)
	self:CreateFromRes("ChangeByPosWnd", Parent)
	self.m_Info		= self:GetDlgChild("Info")
	self.m_ExitBtn = self:GetDlgChild("ExitBtn")
end

function CTongChangeByPos.GetWnd()
	if not g_GameMain.m_TongChangeByPos then
		g_GameMain.m_TongChangeByPos = CTongChangeByPos:new(g_GameMain)
	end
	local wnd = g_GameMain.m_TongChangeByPos
	wnd.m_Info:SetWndText(GetStaticTextClient(410001))
	wnd.m_CountDownTime = 3
	wnd.m_ExitBtn:SetWndText(GetStaticTextClient(410002, wnd.m_CountDownTime))
	if wnd.m_ShowTimeTick then
		UnRegisterTick(wnd.m_ShowTimeTick)
		wnd.m_ShowTimeTick = nil
	end
	wnd.m_ShowTimeTick = RegClassTick("MatchGameItemTick", wnd.ShowTimeTick, 1000, wnd)
	wnd:ShowWnd(true)
	return wnd
end

function CTongChangeByPos:ShowTimeTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime < 0 then
		self:Destroy()
		Gac2Gas:PlayerOutFb(g_Conn)
		return
	end
	self.m_ExitBtn:SetWndText(GetStaticTextClient(410002, self.m_CountDownTime))
	self:ShowWnd(true)
end

function CTongChangeByPos:Destroy()
	if self.m_ShowTimeTick then
		UnRegisterTick(self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self:ShowWnd(false)
end

function CTongChangeByPos:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_ExitBtn == Child then
			self:Destroy()
			Gac2Gas:PlayerOutFb(g_Conn) --场景中的所有玩家退出副本
		end
	end
end

function CTongChangeByPos.UnRegisterChangeByPosTick()
	if g_GameMain.m_TongChangeByPos then
		UnRegisterTick(g_GameMain.m_TongChangeByPos.m_ShowTimeTick)
		g_GameMain.m_TongChangeByPos.m_ShowTimeTick = nil
	end
end

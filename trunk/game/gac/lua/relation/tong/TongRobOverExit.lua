CTongRobOverExit = class(SQRDialog)

function CTongRobOverExit:Ctor(Parent)
	self:CreateFromRes("RobOverExit", Parent)
	self.m_ShowTitle		= self:GetDlgChild("ShowTitle")
	self.m_ShowExitTime		= self:GetDlgChild("ShowExitTime")
	self.m_ExitBtn    	= self:GetDlgChild("ExitBtn")
end

function CTongRobOverExit:GetWnd(exitTime)
	if not g_GameMain.m_TongRobResOverExit then
		g_GameMain.m_TongRobResOverExit = CTongRobOverExit:new(g_GameMain, exitTime)
	end
	local wnd = g_GameMain.m_TongRobResOverExit
	wnd.m_CountDownTime = exitTime
	wnd.m_ShowExitTime:SetWndText(wnd.m_CountDownTime)
	if wnd.m_ShowTimeTick then
		UnRegisterTick(wnd.m_ShowTimeTick)
		wnd.m_ShowTimeTick = nil
	end
	wnd.m_ShowTimeTick = RegClassTick("TongRobResExitTick", wnd.ShowTimeTick, 1000, wnd)
	wnd:ShowWnd(true)
end

function CTongRobOverExit:ShowTimeTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime < 0 then
		self:Destroy()
		Gac2Gas:PlayerExitFb(g_Conn)  --所有玩家退出副本
		return
	end
	if self.m_ShowExitTime then
		self.m_ShowExitTime:SetWndText(self.m_CountDownTime)
	end
end

function CTongRobOverExit:Destroy()
	if self.m_ShowTimeTick then
		UnRegisterTick(self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self:ShowWnd(false)
end

function CTongRobOverExit:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_ExitBtn == Child then
			self:Destroy()
			Gac2Gas:PlayerExitFb(g_Conn) --场景中的所有玩家退出副本
		end
	end
end

function CTongRobOverExit.UnRegisterRobResOverExitTick()
	if g_GameMain.m_TongRobResOverExit then
		UnRegisterTick(g_GameMain.m_TongRobResOverExit.m_ShowTimeTick)
		g_GameMain.m_TongRobResOverExit.m_ShowTimeTick = nil
	end
end

--gac_require "relation/tong/TongRobResExitWndInc"
CTongRobResExitWnd = class(SQRDialog)
--m_TongRobResExitWnd
function CTongRobResExitWnd:Ctor(Parent)
	self:CreateFromRes("RobSourceExit", Parent)
	self.m_ShowPanel		= self:GetDlgChild("ShowPanel")
	self.m_ShowTime		= self:GetDlgChild("ShowTime")
	self.m_ExitBtn    	= self:GetDlgChild("Exit")
end

--function CreateTongRobResExitWnd(Parent)
--	local Wnd = CTongRobResExitWnd:new()
--	Wnd:CreateFromRes("RobSourceExit", Parent)
--	Wnd.m_ShowPanel		= Wnd:GetDlgChild("ShowPanel")
--	Wnd.m_ShowTime		= Wnd:GetDlgChild("ShowTime")
--	Wnd.m_ExitBtn    	= Wnd:GetDlgChild("Exit")
--	return Wnd
--end

function CTongRobResExitWnd:GetWnd(exitTime, tongName)
	if not g_GameMain.m_TongRobResExitWnd then
		g_GameMain.m_TongRobResExitWnd = CTongRobResExitWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_TongRobResExitWnd
	wnd.m_CountDownTime = exitTime
	local str = GetStaticTextClient(190004, tongName)
	wnd.m_ShowPanel:SetWndText(str)	
	wnd.m_ShowTime:SetWndText(wnd.m_CountDownTime)
	if wnd.m_ShowTimeTick then
		UnRegisterTick(wnd.m_ShowTimeTick)
		wnd.m_ShowTimeTick = nil
	end
	wnd.m_ShowTimeTick = RegClassTick("TongRobResExitTick", wnd.ShowTimeTick, 1000, wnd)
	wnd:ShowWnd(true)
end

function CTongRobResExitWnd:ShowTimeTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime < 0 then
		self:Destroy()
		Gac2Gas:PlayerExitFb(g_Conn)  --所有玩家退出副本
		return
	end
	self.m_ShowTime:SetWndText(self.m_CountDownTime)
end

--function CTongRobResExitWnd:InitZero(exitTime, tongName)
--	self.m_CountDownTime = exitTime
--	local str = GetStaticTextClient(190004, tongName)
--	self.m_ShowPanel:SetWndText(str)	
--	self.m_ShowTime:SetWndText(self.m_CountDownTime)
--	if self.m_ShowTimeTick then
--		UnRegisterTick(self.m_ShowTimeTick)
--		self.m_ShowTimeTick = nil
--	end
--	self.m_ShowTimeTick = RegClassTick("TongRobResExitTick", self.ShowTimeTick, 1000, self)
--	self:ShowWnd(true)
--end

function CTongRobResExitWnd:Destroy()
	if self.m_ShowTimeTick then
		UnRegisterTick(self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self:ShowWnd(false)
end

function CTongRobResExitWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_ExitBtn == Child then
			self:Destroy()
			Gac2Gas:PlayerExitFb(g_Conn) --场景中的所有玩家退出副本
			return
		end
	end
end

--function Gas2Gac:ShowRobResZeroExitWnd(Conn, tongName)
--	if not IsCppBound(g_MainPlayer) then
--		return
--	end
--	g_GameMain.m_TongRobResExitWnd:InitZero(10, tongName)
--end

function CTongRobResExitWnd.UnRegisterRobResExitTick()
	if g_GameMain.m_TongRobResExitWnd then
		UnRegisterTick(g_GameMain.m_TongRobResExitWnd.m_ShowTimeTick)
		g_GameMain.m_TongRobResExitWnd.m_ShowTimeTick = nil
	end
end

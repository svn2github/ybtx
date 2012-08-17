gac_require "relation/tong/TongDownTimeWndInc"

function CreateTongDownTimeWnd(Parent)
	local Wnd = CTongDownTimeWnd:new()
	Wnd:CreateFromRes("TongDownTimeWnd", Parent)
	Wnd.m_TitleWnd = Wnd:GetDlgChild("Title")
	return Wnd
end

function CTongDownTimeWnd:OpenPanel(nTime, nGameState)
	self:ShowDownTime(nTime, nGameState)
	self:ShowWnd(true)
end

function CTongDownTimeWnd:ClosePanel()
	if self.m_DownTimeTick then
		UnRegisterTick(self.m_DownTimeTick)
		self.m_DownTimeTick = nil
	end
	
	if self.m_ShowTimeTick then
		UnRegisterTick(self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	
	self:ShowWnd(false)
end

function CTongDownTimeWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
end

function CTongDownTimeWnd:SetTime(time)
	if self.m_DownTimeTick then
		UnRegisterTick(self.m_DownTimeTick)
		self.m_DownTimeTick = nil
	end
	
	local nowTime = time
	
	local function TimeTick()
		if g_MainPlayer then
			nowTime = nowTime -1
			local Min = math.floor(nowTime / 60)
			local Sec = math.floor(nowTime % 60)
			
			if nowTime <= 0 then
				MsgClient(9429)
				if self.m_ShowTimeTick then
					UnRegisterTick(self.m_ShowTimeTick)
					self.m_ShowTimeTick = nil
				end
				self:ClosePanel()
			elseif nowTime <= 5 then
				MsgClient(9428, nowTime)
			end
			self.m_TitleWnd:SetWndText(GetStaticTextClient(9122, Min, Sec))
		end
	end
	
	if self.m_ShowTimeTick then
		UnRegisterTick( self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self.m_ShowTimeTick = RegisterTick( "TongMonsShowTimeTick", TimeTick, 1000)
end

function CTongDownTimeWnd:ShowDownTime(time, gameState)
	local nowTime = time
	
	local function DownTime()
		nowTime = nowTime - 1
		local Min = math.floor(nowTime / 60)
		local Sec = math.floor(nowTime % 60)
		if nowTime <= 0 then
			MsgClient(9427)
			if self.m_DownTimeTick then
				UnRegisterTick(self.m_DownTimeTick)
				self.m_DownTimeTick = nil
			end
		elseif nowTime <= 5 then
			MsgClient(9426, nowTime)
		end
		self.m_TitleWnd:SetWndText(GetStaticTextClient(9121, Min, Sec))
	end
	if gameState == 2 then --进场中
		if self.m_DownTimeTick then
			UnRegisterTick(self.m_DownTimeTick)
			self.m_DownTimeTick = nil
		end
		self.m_DownTimeTick = RegisterTick("TongMonsDownTimeTick", DownTime, 1000)
	elseif gameState == 3 then --比赛中
		self:SetTime(time)
	end
end

function Gas2Gac:TongMonsAttackBegin(Conn, time)
	if g_MainPlayer then
		g_GameMain.m_TongDownTimeWnd:SetTime(time)
	end
end

function Gas2Gac:InitTongDownTimeWnd(Conn, nTime, nGameState)
	if g_MainPlayer then
		g_GameMain.m_TongDownTimeWnd:OpenPanel(nTime, nGameState)
	end
end
gac_require "relation/tong/TongBattlePersonalCountWndInc"

function CreateTongBattlePersonalCountWnd(Parent)
	local Wnd = CTongBattlePersonalCountWnd:new()
	Wnd:CreateFromRes("FbActionWnd", Parent)
	Wnd.m_TitleWnd = Wnd:GetDlgChild("Title")
	Wnd.m_ShowInfoBtn = Wnd:GetDlgChild("ShowWndBtn")
	Wnd.m_ShowInfoBtn:SetCheck(true)
	
	Wnd.m_InfoWnd = SQRDialog:new()
	Wnd.m_InfoWnd:CreateFromRes("TongBattleCountInfo", Wnd)
	Wnd.m_InfoWnd:ShowWnd( true )
	Wnd.m_KillNum = Wnd.m_InfoWnd:GetDlgChild("KillNum")
	Wnd.m_ContiKillNum = Wnd.m_InfoWnd:GetDlgChild("ContiKillNum")
	Wnd.m_DeathCount = Wnd.m_InfoWnd:GetDlgChild("DeathTimes")
	return Wnd
end

function CTongBattlePersonalCountWnd:OpenPanel(nTime, nGameState, nKillNum, nContiKillNum, nDeathCount)
	self:DrawCountInfo(nKillNum, nContiKillNum, nDeathCount)
	self:ShowDownTime(nTime, nGameState)
	self:ShowWnd(true)
end

function CTongBattlePersonalCountWnd:DrawCountInfo(nKillNum, nContiKillNum, nDeathCount)
	self.m_KillNum:SetWndText(nKillNum)
	self.m_ContiKillNum:SetWndText(nContiKillNum)
	self.m_DeathCount:SetWndText(nDeathCount)
end

function CTongBattlePersonalCountWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ShowInfoBtn then
			if self.m_InfoWnd then
				if self.m_InfoWnd:IsShow() then
					self.m_ShowInfoBtn:SetCheck(false)
					self.m_InfoWnd:ShowWnd(false)
				else
					self.m_ShowInfoBtn:SetCheck(true)
					self.m_InfoWnd:ShowWnd(true)
				end
			end
		end
	end
end

function CTongBattlePersonalCountWnd:SetTime(time)
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
			self.m_TitleWnd:SetWndText(GetStaticTextClient(9116, Min, Sec))
		end
	end
	
	if self.m_ShowTimeTick then
		UnRegisterTick( self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self.m_ShowTimeTick = RegisterTick( "TongBattleShowTime", TimeTick, 1000)
end

function CTongBattlePersonalCountWnd:ShowDownTime(time, gameState)
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
		self.m_TitleWnd:SetWndText(GetStaticTextClient(9115, Min, Sec))
	end
	if gameState == 2 then --进场中
		if self.m_DownTimeTick then
			UnRegisterTick(self.m_DownTimeTick)
			self.m_DownTimeTick = nil
		end
		self.m_DownTimeTick = RegisterTick("TongBattleDownTime", DownTime, 1000)
	elseif gameState == 3 then --挑战中
		self:SetTime(time)
	end
end

function CTongBattlePersonalCountWnd:ClosePanel()
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

function Gas2Gac:TongChallengeBattleBegin(Conn, time)
	if g_MainPlayer then
		g_GameMain.m_TongBattlePersonalCountWnd:SetTime(time)
	end
end

function Gas2Gac:TongWarBattleBegin(Conn, time)
	if g_MainPlayer then
		g_GameMain.m_TongBattlePersonalCountWnd:OpenPanel(time, 3, 0, 0, 0)
		g_MainPlayer:ChangeWarModel()
	end
end

function Gas2Gac:CloseTongChallengePanel(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongBattlePersonalCountWnd:ClosePanel()
		g_GameMain.m_TongChallengeExitBtnWnd:ClosePanel()
		g_GameMain.m_TongChallengeBattleCountWnd:ClosePanel()
		g_GameMain.m_TongChallengeStatisticsWnd:Close()
	end
end

function Gas2Gac:InitTongBattlePersonalCountWnd(Conn, nTime, nGameState, nKillNum, nContiKillNum, nDeathCount)
	if g_MainPlayer then
		g_GameMain.m_TongBattlePersonalCountWnd:OpenPanel(nTime, nGameState, nKillNum, nContiKillNum, nDeathCount)
		g_MainPlayer:ChangeWarModel()
	end
end

function Gas2Gac:UpdateTongBattlePersonalCountInfo(Conn, nKillNum, nContiKillNum, nDeathCount)
	if g_MainPlayer then
		g_GameMain.m_TongBattlePersonalCountWnd:DrawCountInfo(nKillNum, nContiKillNum, nDeathCount)
	end
end
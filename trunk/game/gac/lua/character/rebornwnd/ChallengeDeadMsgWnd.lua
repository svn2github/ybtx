CChallengeDeadMsgWnd = class(SQRDialog)

function CChallengeDeadMsgWnd:Ctor()
	self:CreateFromRes("ChallengeDeadMsgWnd", g_GameMain)
	self.m_BtnDeadPos = self:GetDlgChild("OkBtn")
end

function CChallengeDeadMsgWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnDeadPos == Child then
			self:HideWnd()
			Gac2Gas:TongChallengeReborn(g_Conn)
		end
	end
end

function CChallengeDeadMsgWnd:CountLeftTimeTick()
	local leftTime = self.m_CountDownNum - 1
	self.m_CountDownNum = leftTime
	
	if leftTime > 0 then
		if not IsCppBound(g_MainPlayer) then
			UnRegisterTick(self.m_CountDownTick)
			self.m_CountDownTick = nil
			return
		end
		self.m_BtnDeadPos:SetWndText(GetStaticTextClient(11013, leftTime))
		self.m_BtnDeadPos:EnableWnd(false)
	else
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		self.m_BtnDeadPos:SetWndText(GetStaticTextClient(11014))
		self.m_BtnDeadPos:EnableWnd(true)
	end
end

function CChallengeDeadMsgWnd:IsToLogin(RebornCountTime)
	if ( g_GameMain.m_ToLoginMsgBox and g_GameMain.m_ToLoginMsgBox.m_CountDownTick ) then
		local CallBackToCharDeadWnd = function()
			self:ShowDeadWnd(RebornCountTime)
		end
		g_GameMain.m_ToLoginMsgBox.CancelAndRunCallBack = CallBackToCharDeadWnd
		return true
	end
	return false
end

function CChallengeDeadMsgWnd:ShowDeadWnd(time)
	if(self:IsToLogin(time)) then
		return
	end
	if self.m_CountDownTick ~= nil then
		return
	end
	self.m_CountDownNum = time
	self.m_BtnDeadPos:SetWndText(GetStaticTextClient(11013, self.m_CountDownNum))
	self.m_BtnDeadPos:EnableWnd(false)
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	self.m_CountDownTick = RegClassTick("ChallengeDeadTick", self.CountLeftTimeTick, 1000, self)	
end

function CChallengeDeadMsgWnd:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end

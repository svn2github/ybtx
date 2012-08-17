CFBDeadMsgWnd = class(SQRDialog)

function CFBDeadMsgWnd:Ctor()
	self:CreateFromRes("WndFbDeadMsg", g_GameMain)
	self:Init()
end

function CFBDeadMsgWnd:Init()
	self.m_BtnOK		= self:GetDlgChild("BtnOK")
end

function CFBDeadMsgWnd:SetBtnOKText(Num)
	self.m_BtnOK:SetWndText(GetStaticTextClient(11004, Num))
end

function CFBDeadMsgWnd:ShowDeadWnd(time)
	if self.m_CountDownTick ~= nil then
		return
	end
	self.m_CountDownNum = time--初始化时间
	
	self:SetBtnOKText(self.m_CountDownNum)
	self.m_BtnOK:EnableWnd(false)
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	self.m_CountDownTick = RegClassTick("FBDeadTick", self.CountLeftTimeTick, 1000, self)	
end

function CFBDeadMsgWnd:CountLeftTimeTick()
	local leftTime = self.m_CountDownNum - 1
	self.m_CountDownNum = leftTime
	
	if leftTime > 0 then	
				
		if not IsCppBound(g_MainPlayer) then
			UnRegisterTick(self.m_CountDownTick)
			self.m_CountDownTick = nil
			return
		end
		
		self:SetBtnOKText(leftTime)
		self.m_BtnOK:EnableWnd(false)
	else
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		self.m_BtnOK:SetWndText(GetStaticTextClient(11005))
		self.m_BtnOK:EnableWnd(true)
	end
end

function CFBDeadMsgWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnOK == Child then
			Gac2Gas:AreaFbRebornToRePoint(g_Conn)
		end
	end
end

function CFBDeadMsgWnd:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end

CTeamPVPFBDeadMsgWnd = class(SQRDialog)

function CTeamPVPFBDeadMsgWnd:Ctor()
	self:CreateFromRes("TeamPVPFbDeadMsg", g_GameMain)
	self:Init()
end

function CTeamPVPFBDeadMsgWnd:Init()
	self.m_MsgInfo		= self:GetDlgChild("MsgInfo")
end

function CTeamPVPFBDeadMsgWnd:SetText(Num)
	self.m_MsgInfo:SetWndText(GetStaticTextClient(11017, Num))
end

function CTeamPVPFBDeadMsgWnd:ShowDeadWnd(time)
	if self.m_CountDownTick ~= nil or g_MainPlayer:CppIsAlive() then
		return
	end
	self.m_CountDownNum = time--初始化时间
	
	self:SetText(self.m_CountDownNum)
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	self.m_CountDownTick = RegClassTick("TeamPVPFBDeadTick", self.CountLeftTimeTick, 1000, self)	
end

function CTeamPVPFBDeadMsgWnd:CountLeftTimeTick()
	local leftTime = self.m_CountDownNum - 1
	self.m_CountDownNum = leftTime
	
	if leftTime > 0 then	
				
		if not IsCppBound(g_MainPlayer) or g_MainPlayer:CppIsAlive() then
			self:HideWnd()
			return
		end
		
		self:SetText(leftTime)
	else
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		self:SetText(0)
	end
end

function CTeamPVPFBDeadMsgWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )

end

function CTeamPVPFBDeadMsgWnd:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end


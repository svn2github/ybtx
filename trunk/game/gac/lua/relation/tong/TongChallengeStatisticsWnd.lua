gac_require "relation/tong/TongChallengeStatisticsWndInc"

------帮会挑战统计面板------

function CreateTongChallengeStatisticsWnd(Parent)
	local Wnd = CTongChallengeStatisticsWnd:new()
	Wnd.m_CountDownTime = 30
	Wnd:Init(Parent)
	return Wnd
end

function CTongChallengeStatisticsWnd:CountDownTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime <= 0 then
		Gac2Gas:ExitTongChallenge(g_Conn)
		self:Close()
		return
	end
	self.m_ExitBtn:SetWndText(GetStaticTextClient(1012, self.m_CountDownTime))
end

function CTongChallengeStatisticsWnd:Init(Parent)
	self:CreateFromRes("TongChallengeStatisticsWnd", Parent)
	self:ShowWnd(false)
end

function CTongChallengeStatisticsWnd:Close()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	self:ShowWnd(false)
end

function CTongChallengeStatisticsWnd:OnChildCreated()
	self.m_ExitBtn = self:GetDlgChild("ExitBtn")
	self.m_WinTongName = self:GetDlgChild("WinTongName")
	self.m_WinKillBuildNum = self:GetDlgChild("WinKillBuildNum")
	self.m_WinKillPersonNum = self:GetDlgChild("WinKillPersonNum")
	self.m_WinHonorNum = self:GetDlgChild("WinHonorNum")
	self.m_FailTongName = self:GetDlgChild("FailTongName")
	self.m_FailKillBuildNum = self:GetDlgChild("FailKillBuildNum")
	self.m_FailKillPersonNum = self:GetDlgChild("FailKillPersonNum")
	self.m_FailHonorNum = self:GetDlgChild("FailHonorNum")
end

--消息
function CTongChallengeStatisticsWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ExitBtn then
			Gac2Gas:ExitTongChallenge(g_Conn)
			self:Close()
		end
	end
end

function CTongChallengeStatisticsWnd:ShowInfo(WinTongName, WinKillBuildNum, WinKillPersonNum, WinHonorNum, FailTongName, FailKillBuildNum, FailKillPersonNum, FailHonorNum)
	if WinTongName == "" then
		WinTongName = GetStaticTextClient(8329)
	elseif FailTongName == "" then
		FailTongName = GetStaticTextClient(8329)
	end
	
	self.m_WinTongName:SetWndText(WinTongName)
	self.m_WinKillBuildNum:SetWndText(WinKillBuildNum)
	self.m_WinKillPersonNum:SetWndText(WinKillPersonNum)
	self.m_WinHonorNum:SetWndText(WinHonorNum)
	self.m_FailTongName:SetWndText(FailTongName)
	self.m_FailKillBuildNum:SetWndText(FailKillBuildNum)
	self.m_FailKillPersonNum:SetWndText(FailKillPersonNum)
	self.m_FailHonorNum:SetWndText(FailHonorNum)
	
	self.m_ExitBtn:SetWndText(GetStaticTextClient(1012, self.m_CountDownTime))
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	self.m_CountDownTick = RegClassTick("StatisticsWndCountDownTick", self.CountDownTick, 1000, self)
	self:ShowWnd(true)
end

function Gas2Gac:ShowTongChallengeStatisticsWnd(Conn, WinTongName, WinKillBuildNum, WinKillPersonNum, WinHonorNum, FailTongName, FailKillBuildNum, FailKillPersonNum, FailHonorNum)
	g_GameMain.m_TongChallengeStatisticsWnd:ShowInfo(WinTongName, WinKillBuildNum, WinKillPersonNum, WinHonorNum, FailTongName, FailKillBuildNum, FailKillPersonNum, FailHonorNum)
end
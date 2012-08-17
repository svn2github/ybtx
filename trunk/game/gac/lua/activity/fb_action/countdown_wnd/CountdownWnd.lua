cfg_load "int_obj/TempBagObj_Common"
lan_load "int_obj/Lan_TempBagObj_Common"

CCountdown = class(SQRDialog)

function CCountdown:Ctor()
	self:CreateFromRes("CountDown", g_GameMain)
	self.m_MinWnd = self:GetDlgChild("Minute")
	self.m_SecWnd = self:GetDlgChild("Second")
	self.m_MSELWnd = self:GetDlgChild("MSEL")
	self:ShowWnd(false)
end

function CCountdown:OpenGameCountdownWnd(Time, Index)
	if self:IsShow() then
		return
	end
	
	if Index ~= 0 then
		self:SetMouseOverDescAfter( GetStaticTextClient(Index) )
	else
		self:SetMouseOverDescAfter("")
	end
	
	self:BeginCountdownTime(Time)
end

function CCountdown:CloseGameCountdownWnd()
	if self.m_DownTimeTick then
		UnRegisterTick(self.m_DownTimeTick)
		self.m_DownTimeTick = nil
	end
	self:ShowWnd(false)
end

function CCountdown:SetCountdownTimeText()
	local function GetNumber(num)
		if num <= 9 then
			return "0"..num
		else
			return num
		end
	end
	local Min = 0
	local Sec = 0
	Min = math.floor((self.m_DownTime) / 60)
	Sec = self.m_DownTime - Min * 60
	
	self.m_MinWnd:SetWndText(GetNumber(Min)..":")
	self.m_SecWnd:SetWndText(GetNumber(Sec).."\"")
	self.m_MSELWnd:SetWndText(GetNumber(self.m_MSELTime))
end

function CCountdown:BeginCountdownTime(SecondNum)
	if SecondNum <= 0 then
		return
	end
	self.m_DownTime = SecondNum
	self.m_MSELTime = 0
	self:SetCountdownTimeText()
	local function DevTime()
		if self.m_MSELTime == 0 then
			self.m_MSELTime = 99
			self.m_DownTime = self.m_DownTime - 1
		else
			self.m_MSELTime = self.m_MSELTime - 1
		end
		self:SetCountdownTimeText()
		if self.m_DownTime <= 0 and self.m_MSELTime <= 0 then
			self:CloseGameCountdownWnd()
		end
	end
	self.m_DownTimeTick = RegisterTick("GameCountdownTick",DevTime,10)
	self:ShowWnd(true)
end

function CCountdown:SetCountdownTime(SecondNum)
	if SecondNum <= 0 then
		return
	end
	self.m_DownTime = SecondNum
end


function CCountdown.UnRegisterGameDownTimeTick()
	if g_GameMain.m_GameDownTimeWnd then
		UnRegisterTick(g_GameMain.m_GameDownTimeWnd.m_DownTimeTick)
		g_GameMain.m_GameDownTimeWnd.m_DownTimeTick = nil
	end
end

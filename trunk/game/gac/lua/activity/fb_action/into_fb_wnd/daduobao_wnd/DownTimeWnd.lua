
CDownTimeWnd = class(SQRDialog)

function CDownTimeWnd:Ctor()
	self:CreateFromRes("Fb_DDB_DownTimeWnd", g_GameMain)
	self.m_DownTime = self:GetDlgChild("DownTime")
end


function CDownTimeWnd:ShowWindow(SecondNum)
	SecondNum = math.floor(SecondNum)
	self.m_CountTimeNum = 0
	if g_GameMain.m_DownTimeTick then
		if SecondNum <= 0 then
			UnRegisterTick(g_GameMain.m_DownTimeTick)
			g_GameMain.m_DownTimeTick = nil
			self:ShowWnd(false)
		else
			self.m_CountTimeNum = SecondNum
		end
	else
		if SecondNum <= 0 then
			return
		else
			self:SetGameTime(SecondNum)
			local function DevTime()
				self.m_CountTimeNum = self.m_CountTimeNum - 1
				self:SetGameTime(self.m_CountTimeNum)
				if self.m_CountTimeNum <= 0 then
					UnRegisterTick(g_GameMain.m_DownTimeTick)
					g_GameMain.m_DownTimeTick = nil
					self:ShowWnd(false)
				end
			end
			self.m_CountTimeNum = SecondNum
			g_GameMain.m_DownTimeTick = RegisterTick("DownTimeTick",DevTime,1000)
			self:ShowWnd(true)
		end
	end
end

function CDownTimeWnd:SetGameTime(SecondNum)
	local Hour = 0
	local Min = 0
	local Sec = 0
	Hour = math.floor(SecondNum / 3600)
	Min = math.floor((SecondNum - Hour * 3600) / 60)
	Sec = SecondNum - Hour * 3600 - Min * 60
	
	local TimeStr = ""
	if Hour > 0 then
		TimeStr = TimeStr..Hour..GetStaticTextClient(1121)
	end
	if TimeStr ~= "" or Min > 0 then
		TimeStr = TimeStr..Min..GetStaticTextClient(1120)
	end
	if TimeStr ~= "" or Sec > 0 then
		TimeStr = TimeStr..Sec..GetStaticTextClient(1119)
	end
	self.m_DownTime:SetWndText(TimeStr)
end


CRobResCountDown = class(SQRDialog)

local function SetWndRect(ParentWnd,ChildWnd)
	local rt = CFRect:new()
	ParentWnd:GetLogicRect(rt)
	local rt_1 = CFRect:new()
	ChildWnd:GetLogicRect(rt_1)
	local width = rt_1.right-rt_1.left
	local height = rt_1.bottom-rt_1.top
	rt_1.right = rt.right - 10
	rt_1.left = rt_1.right - width
	ChildWnd:SetLogicRect(rt_1)
end

function CRobResCountDown:Ctor(parent)
	self:CreateFromRes("RobResCountDown", parent)
	self.m_Msg		= self:GetDlgChild("Msg")
	self.m_Time			= self:GetDlgChild("Time")
end


function CRobResCountDown:GetWnd(tongName, time)
	
	if not g_GameMain.m_RobResCountDown then
		g_GameMain.m_RobResCountDown = CRobResCountDown:new(g_GameMain, tongName, time)
	end
	local wnd = g_GameMain.m_RobResCountDown
	SetWndRect(g_GameMain,wnd)
	wnd.m_Msg:SetWndText(GetStaticTextClient(330003, tongName))
	wnd:ShowTime(time)
	wnd:ShowWnd(true)
	return wnd
end

function CRobResCountDown:GetListGameTime(SecondNum)
	local Hour = 0
	local Min = 0
	local Sec = 0
	Hour = math.floor(SecondNum / 3600)
	Min = math.floor((SecondNum-(Hour*3600)) / 60)
	Sec = SecondNum - Hour*3600 - Min * 60
	local GameTime = Min..GetStaticTextClient(1120)..Sec..GetStaticTextClient(1119)
	return GameTime
end


function CRobResCountDown:ShowTime(time)
	if time <= 0 then
		return
	end
	local function RobResDownTime()
		time = time - 1
		local tempStr = GetStaticTextClient(330004, self:GetListGameTime(time))
		if self.m_Time then
			self.m_Time:SetWndText(tempStr)
		end
		if time <= 0 then
			UnRegisterTick(self.m_RobResourceDownTime)
			self.m_RobResourceDownTime = nil
			self:ShowWnd(false)
		end
	end

	if self.m_RobResDownTime then
		UnRegisterTick(self.m_RobResDownTime)
		self.m_RobResDownTime = nil
	end 
	self.m_RobResDownTime = RegisterTick("RobResWndTick",RobResDownTime,1000)
end

function CRobResCountDown.UnRegisterRobResDownTick()
	if g_GameMain.m_RobResCountDown then
		UnRegisterTick(g_GameMain.m_RobResCountDown.m_RobResDownTime)
		g_GameMain.m_RobResCountDown.m_RobResDownTime = nil
	end
end

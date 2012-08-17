CCountdownWnd = class(SQRDialog)

local index = 0
--showText 包含%d(且只能一个) 将用剩余time 填充
--buttonList 为按钮的显示名字
function CCountdownWnd:Ctor(parent, showText, time, callBackFun, buttonList)
	self:CreateFromRes("CountDownWnd", parent)
	self.m_ShowText = showText
	self.m_Time = time
	self.m_CallBackFun = callBackFun
	self.m_TextCtrl =  self:GetDlgChild("ShowMsg")
	local text = string.format(self.m_ShowText, self.m_Time)
	self.m_TextCtrl:SetWndText(text)
	
	self.m_BtnList = {}
	
	local btnCount = #buttonList
	assert(btnCount <=3)
	
	if btnCount == 1 then
		self.m_BtnList[1] = self:GetDlgChild("Btn2")
		self:GetDlgChild("Btn1"):ShowWnd(false)
		self:GetDlgChild("Btn3"):ShowWnd(false)
	elseif btnCount == 2 then
		self.m_BtnList[1] = self:GetDlgChild("Btn1")
		self.m_BtnList[2] = self:GetDlgChild("Btn3")
		self:GetDlgChild("Btn2"):ShowWnd(false)
	elseif btnCount == 3  then
		self.m_BtnList[1] = self:GetDlgChild("Btn1")
		self.m_BtnList[2] = self:GetDlgChild("Btn2")
		self.m_BtnList[3] = self:GetDlgChild("Btn3")
	end
	
	for i = 1, btnCount do -- 目前最多提供3个按钮
		self.m_BtnList[i]:SetWndText(buttonList[i])
	end
--	if btnCount >= 1 then --设置按钮位置
--		local rt = CFRect:new()
--		self:GetLogicRect(rt)
--		local btnW = 55
--		local blankW = ((rt.right - rt.left) - btnCount * btnW)/(btnCount + 1)
--		local rtTemp = CFRect:new()
--		rtTemp.top = rt.top + 130
--		rtTemp.bottom = rtTemp.top + 30
--		rtTemp.left = rt.left
--		for _, btn in pairs(self.m_BtnList) do
--			rtTemp.left = rtTemp.left + blankW
--			rtTemp.right = rtTemp.left + btnW
--			btn:SetLogicRect(rtTemp)
--			rtTemp.left = rtTemp.right
--		end
--	end
	self:ShowWnd(true)	
	self.m_CountDownTick = RegClassTick("CountDownWndTick", CCountdownWnd.CountDownTick, 1000, self)
	if g_GameMain.m_CountDownWnd == nil then
		g_GameMain.m_CountDownWnd = {}
	end
	g_GameMain.m_CountDownWnd[index] = self
	self.m_CountDownWndIndex = index
	index = index + 1
end

function CCountdownWnd:CountDownTick()
	if self.m_Time <= 0 then
		self:Close()
		self.m_CallBackFun(self, 1)
		return
	end
	self.m_Time = self.m_Time - 1
	local showText = string.format(self.m_ShowText, self.m_Time)
	self.m_TextCtrl:SetWndText(showText)
end

function CCountdownWnd:Close()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		g_GameMain.m_CountDownWnd[self.m_CountDownWndIndex] = nil
	end
	self:ShowWnd(false)
	self:DestroyWnd()
end

function CCountdownWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		for i, btn in pairs(self.m_BtnList) do
			if  Child == btn then
				self.m_CallBackFun(self, i)
				break
			end
		end
	end
end

function CCountdownWnd:ClearAllCountdownWndTick()
	if g_GameMain.m_CountDownWnd then
		for id, wnd in pairs(g_GameMain.m_CountDownWnd ) do
			if wnd.m_CountDownTick then
				UnRegisterTick(wnd.m_CountDownTick)
				wnd.m_CountDownTick = nil
				g_GameMain.m_CountDownWnd[wnd.m_CountDownWndIndex] = nil
--				print ("ClearAllCountdownWndTick " .. id .. "  " .. wnd.m_CountDownWndIndex)
			end
		end
	end
end
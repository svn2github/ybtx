
------拆分重叠物品的窗口------
CWndSplit = class(SQRDialog)

function CWndSplit:OnChildCreated()
	self.m_BtnOK		= self:GetDlgChild("BtnOk")
	self.m_BtnCancel	= self:GetDlgChild("BtnCancel")
	self.m_Scrollbar	= self:GetDlgChild("ScrollMoney")
	self.m_wndCount		= self:GetDlgChild("EditMoney")
	self.m_wndTicket	= self:GetDlgChild("WndYuanbao")
	self.m_wndGold		= self:GetDlgChild("WndGold")
	self.m_wndArgent	= self:GetDlgChild("WndArgent")
	self.m_wndCopper	= self:GetDlgChild("WndCopper")
end



function CWndSplit:VirtualExcludeWndClosed() 
    g_WndMouse:ClearCursorAll()
    self.m_WndWorking:GetSubItem(self.m_PosX, self.m_PosY):EnableWnd(true)
    self:DestroyWnd()
end


function CWndSplit:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if self.m_BtnOK == Child and uMsgID == BUTTON_LCLICK then
		self:ShowWnd(false)
	    self.m_WndWorking:SetPickupSplitItem(self.m_RoomIndex, self.m_nSrcRow, self.m_nSrcCol, self.m_SelCount, ECursorState.eCS_Split)	
	elseif self.m_BtnCancel == Child and uMsgID == BUTTON_LCLICK then
        self:ShowWnd(false)
        
	elseif self.m_Scrollbar == Child then
		self.m_SelCount = self.m_Scrollbar:GetPos() + 1
		self.m_wndCount:SetWndText(tostring(self.m_SelCount))
	elseif uMsgID == WND_NOTIFY and (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
		local input = tonumber(self.m_wndCount:GetWndText())
		if IsNumber(input) == false then
			--self.m_wndCount:SetWndText("")
			return
		end
		if input == 0 or self.m_wndCount:GetWndText() == "" then
			local count	= 1
			self.m_wndCount:SetWndText(count)
			self.m_Scrollbar:SetPos(count-1)
		elseif input > self.m_MaxNum then
			self.m_wndCount:SetWndText(tostring(self.m_MaxNum))
			self.m_Scrollbar:SetPos(self.m_MaxNum)
		else
			self.m_Scrollbar:SetPos(input-1)
		end
	end
end

function CWndSplit:SetSplitItemInfo(Style, MaxNum, RoomIndex, row, col)
	self.m_RoomIndex = RoomIndex
	self.m_MaxNum    = MaxNum
	self.m_nSrcRow   = row
	self.m_nSrcCol   = col
	
	self.m_Scrollbar:SetRange(self.m_MaxNum)
	self.m_Scrollbar:SetPage(1)
	self.m_Scrollbar:SetPos(0)
	self.m_wndCount:SetFocus()
	self.m_wndCount:SetWndText(tostring(1))
	
	--Style为0说明是item，1是ticket（不知道干啥的）
	if Style == 0 then
		self.m_wndTicket:ShowWnd(false)
		self.m_wndGold:ShowWnd(false)
		self.m_wndArgent:ShowWnd(false)
		self.m_wndCopper:ShowWnd(false)
	end
end

function CWndSplit:SetWorkingWnd(wnd, x, y)
	self.m_WndWorking = wnd
	self.m_PosX = x
	self.m_PosY = y
	self.m_WndWorking:GetSubItem(self.m_PosX, self.m_PosY):EnableWnd(false)
end
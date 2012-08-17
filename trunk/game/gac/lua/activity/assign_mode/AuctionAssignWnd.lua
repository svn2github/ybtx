gac_gas_require "framework/common/CMoney"
CAuctionAssignWnd = class ( SQRDialog )

local RefreshTime = 30
AuctionAssignRect = nil

function CAuctionAssignWnd.CreateAuctionPickUpWnd(Parent, BigId, ItemName, ItemCount, ServerId, IntObjID, Time)
	local Wnd = CAuctionAssignWnd:new()
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(BigId,ItemName,"SmallIcon")
	Wnd:Init(Parent)
	Wnd.m_IntObjID = IntObjID
	Wnd.m_ServerId = ServerId
	g_LoadIconFromRes(SmallIcon, Wnd.m_ItemIconWnd, -1, IP_ENABLE, IP_ENABLE)
	Wnd.m_ItemIconWnd:SetWndText(ItemCount)
	g_Tooltips:LoadIconFromRes(Wnd.m_ItemIconWnd, SmallIcon)
	g_SetItemRichToolTips(Wnd.m_ItemIconWnd,BigId, ItemName, 0)
	
	local money = CMoney:new()
	local price = g_GameMain.m_PlayerInfo.m_AuctionBasePrice or 0
	local goldCount, silverCount, tongCount = money:ChangeMoneyToGoldAndArgent(price)
	Wnd.m_Jin:SetWndText(goldCount)
	Wnd.m_Yin:SetWndText(silverCount)
	Wnd.m_Tong:SetWndText(tongCount)
	Wnd.m_TextJin:SetWndText(goldCount)
	Wnd.m_TextYin:SetWndText(silverCount)
	Wnd.m_TextTong:SetWndText(tongCount)
	
	if AuctionAssignRect then
		AuctionAssignRect.top = AuctionAssignRect.top + 10
		AuctionAssignRect.bottom = AuctionAssignRect.bottom + 10
		AuctionAssignRect.left = AuctionAssignRect.left + 10
		AuctionAssignRect.right = AuctionAssignRect.right + 10
		Wnd:SetLogicRect(AuctionAssignRect)
		return Wnd
	end
	AuctionAssignRect = CFRect:new()
	Wnd:GetLogicRect(AuctionAssignRect)
	
	Wnd:ShowCountDownText()
	Wnd:ShowWnd(true)
	return Wnd
end

function CAuctionAssignWnd:Init(Parent)
	self:CreateFromRes("WndDropItemAuction", Parent)
end

function CAuctionAssignWnd:ShowCountDownText()
	
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	self.m_CountTime = RefreshTime
	self.m_CountDownWnd:SetWndText(self.m_CountTime)
	local function CountTime()
		if g_MainPlayer then
			if self.m_CountTime <= 0 then     
				self.m_CountDownWnd:SetWndText(self.m_CountTime)
				self.m_SubmitPriceBtn:EnableWnd(false)
				self:ShowWnd(false)
				UnRegisterTick(self.m_CountDownTick)
				self.m_CountDownTick = nil
			else
				self.m_CountTime = self.m_CountTime - 1
				self.m_CountDownWnd:SetWndText(self.m_CountTime)
			end
		end
	end
	self.m_CountDownTick = RegisterTick("CountTime", CountTime, 1000)
end

function CAuctionAssignWnd:GetInputPrice()
	local goldCount = tonumber( self.m_Jin:GetWndText() )
	local silverCount = tonumber( self.m_Yin:GetWndText() )
	local tongCount = tonumber( self.m_Tong:GetWndText() )
	
	local money = CMoney:new()
	local moneyCount = money:ChangeGoldAndArgentToMoney(goldCount, silverCount, tongCount)

	return moneyCount
end

function CAuctionAssignWnd:SetHighestPrice()
	local money = CMoney:new()
	local goldCount, silverCount, tongCount = money:ChangeMoneyToGoldAndArgent(self.m_NowPrice)
	self.m_TextJin:SetWndText(goldCount)
	self.m_TextYin:SetWndText(silverCount)
	self.m_TextTong:SetWndText(tongCount)
end

function CAuctionAssignWnd:CheckInput( clickedWnd )
	local inputValidate = false
	local money = CMoney:new()
	--如果要输入数字性的东西，则检测输入的是否合法
	if clickedWnd == self.m_Yin or  
		clickedWnd == self.m_Tong then
		money:CheckInputMoneyValidate(clickedWnd, 2)
	elseif clickedWnd == self.m_Jin then
		money:CheckInputMoneyValidate(clickedWnd, 1)
	end
end

function CAuctionAssignWnd:OnChildCreated()
	
	self.m_ItemIconWnd = self:GetDlgChild("ItemIcon")  --显示物品图标和数目
	self.m_Jin = self:GetDlgChild("Jin")
	self.m_Yin = self:GetDlgChild("Yin")
	self.m_Tong = self:GetDlgChild("Tong")
	self.m_TextJin = self:GetDlgChild("TextJin")
	self.m_TextYin = self:GetDlgChild("TextYin")
	self.m_TextTong = self:GetDlgChild("TextTong")
	self.m_SubmitPriceBtn = self:GetDlgChild("SubmitPrice")  --按钮"竞拍",输入出价后可以点击
	self.m_CancelAuctionBtn = self:GetDlgChild("CancelAuction")  --放弃
	self.m_TimeRemainWnd = self:GetDlgChild("TimeRemain")
	self.m_CountDownWnd = self:GetDlgChild("CountDown")   --出价的倒计时
		
	self.m_NowPrice = 0
	
	self.m_SubmitPriceBtn:EnableWnd(true)
	self.m_SubmitPriceBtn:ShowWnd(true)
	
	self.m_CancelAuctionBtn:EnableWnd(true)
	self.m_CancelAuctionBtn:ShowWnd(true)
	
	self:ShowCountDownText()
	self:SetStyle(0x00040000)
	self:ShowWnd(true)
end

function CAuctionAssignWnd:Destroy()
	g_GameMain.m_AcutionAssignTbls[self.m_IntObjID] = nil
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
	end
	AuctionAssignRect = nil
	self:ShowWnd(false)
	self:DestroyWnd()
end

function CAuctionAssignWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_CancelAuctionBtn) then
			Gac2Gas:SelectAcutionAssign(g_Conn, self.m_ServerId, self.m_IntObjID, 0, false)
			return
		elseif(Child == self.m_SubmitPriceBtn) then
			local str = self:GetInputPrice()
			local GivePrice = tonumber(str)
			Gac2Gas:SelectAcutionAssign(g_Conn, self.m_ServerId, self.m_IntObjID, GivePrice, true)
		end
	elseif uMsgID == WND_NOTIFY then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			self:CheckInput( Child )
		end
	end
end

function CAuctionAssignWnd:NotifyChangeAuctionPrice(NewPrice)
	self.m_NowPrice = NewPrice
	self:SetHighestPrice()
	self:ShowCountDownText()
end
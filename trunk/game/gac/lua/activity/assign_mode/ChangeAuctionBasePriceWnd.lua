gac_gas_require "framework/common/CMoney"
CChangeAuctionBasePrice = class( SQRDialog )

function CChangeAuctionBasePrice:Ctor()
	self:CreateFromRes( "ChangeAuctionBasePrice",g_GameMain )
	self.m_Jin						= self:GetDlgChild("Jin")
	self.m_Yin						= self:GetDlgChild("Yin")
	self.m_Tong						= self:GetDlgChild("Tong")
	self.m_BtnOK					= self:GetDlgChild("BtnOK")
	self.m_BtnCancel			= self:GetDlgChild("BtnCancel")
	self:SetStyle(0x00040000)
end

function CChangeAuctionBasePrice:SetAuctionBasePrice()
	local money = CMoney:new()
	local price = g_GameMain.m_PlayerInfo.m_AuctionBasePrice or 0
	local goldCount, silverCount, tongCount = money:ChangeMoneyToGoldAndArgent(price)
	self.m_Jin:SetWndText(goldCount)
	self.m_Yin:SetWndText(silverCount)
	self.m_Tong:SetWndText(tongCount)
end

function CChangeAuctionBasePrice:GetInputPrice()
	local goldCount = tonumber( self.m_Jin:GetWndText() )
	local silverCount = tonumber( self.m_Yin:GetWndText() )
	local tongCount = tonumber( self.m_Tong:GetWndText() )
	
	local money = CMoney:new()
	local moneyCount = money:ChangeGoldAndArgentToMoney(goldCount, silverCount, tongCount)

	return moneyCount
end

function CChangeAuctionBasePrice:CheckInput( clickedWnd )
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

function CChangeAuctionBasePrice:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_BtnOK)then
			local money = self:GetInputPrice()
			Gac2Gas:SetAuctionBasePrice(g_Conn, money)
		end
		self:ShowWnd(false)
	elseif uMsgID == WND_NOTIFY then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			self:CheckInput( Child )
		end
	end
end
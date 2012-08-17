gac_require "relation/tong/TongMsgBoxInc"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"

function CreateTongInputBoxWnd(parent)
	local wnd = CTongInputBoxWnd:new()
	wnd:CreateFromRes("TongMoneyInput", parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "·Ç»¥³â")
	return wnd
end

function CTongInputBoxWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_OKBtn			= self:GetDlgChild("OKBtn")
	self.m_Gold				= self:GetDlgChild("Gold")
	self.m_Silver			= self:GetDlgChild("Silver")
	self.m_Copper			= self:GetDlgChild("Copper")
	self.m_Lable			= self:GetDlgChild("Lable")
	self.m_DisMaxNum 	= self:GetDlgChild("DisMaxNum")
	self.m_DisHaveNum 	= self:GetDlgChild("DisHaveNum")
	self.m_CannelBtn 	= self:GetDlgChild("CannelBtn")
	
	self.m_Gold:SetMaxTextLenght(8)
	self.m_Silver:SetMaxTextLenght(2)
	self.m_Copper:SetMaxTextLenght(2)
end

function CTongInputBoxWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if g_MainPlayer.m_ItemBagLock and (not (Child == self.m_XBtn or Child == self.m_CannelBtn)) then
				MsgClient(160034)
				return 
		end
		if(Child == self.m_XBtn or Child == self.m_CannelBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OKBtn) then
			local money = self:TransMoney()
			self:ShowWnd(false)
			Gac2Gas:ContributeMoney(g_Conn, tonumber(money))
		end
	end
end

function CTongInputBoxWnd:TransMoney()
	local gold		= tonumber( self.m_Gold:GetWndText() ) or 0
	local silver	= tonumber( self.m_Silver:GetWndText() ) or 0
	local copper	= tonumber( self.m_Copper:GetWndText() ) or 0
	return math.floor( gold*10000 + silver*100 + copper ) 
end

function Gas2Gac:ReturnGetMoneyCanContribute(Conn,total_money,have_money)
	local MoneyMgr = CMoney:new()
	g_GameMain.m_ContributeMoney.m_DisMaxNum:SetWndText(GetStaticTextClient(9128,MoneyMgr:ChangeMoneyToString(total_money-have_money,EGoldType.GoldCoin)))
	g_GameMain.m_ContributeMoney.m_DisHaveNum:SetWndText(GetStaticTextClient(9127,MoneyMgr:ChangeMoneyToString(total_money,EGoldType.GoldCoin)))
	g_GameMain.m_ContributeMoney:ShowWnd(true)
	g_GameMain.m_ContributeMoney:SetFocus()
end

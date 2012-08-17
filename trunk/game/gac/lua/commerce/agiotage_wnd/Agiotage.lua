
CAgiotage = class( SQRDialog )
CExchangeWnd = class( SQRDialog )

function CAgiotage:Ctor()
	self:CreateFromRes( "AgiotageWnd", g_GameMain )
	self.m_BtnClose 		= self:GetDlgChild("CloseBtn")
	self.m_BtnExchange 	= self:GetDlgChild("ExchangeBtn")
	self.m_BtnCharge		= self:GetDlgChild("ChargeBtn")
	self.m_ChargeDes 		= self:GetDlgChild("ChargeDescription")
	self.m_ExchangeWnd  = CExchangeWnd:new( self )
	self.m_ChargeDes:SetWndText(GetStaticTextClient(1600))
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
	--self.m_BtnCharge:EnableWnd(false)
end

function CAgiotage:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK )then
		if (Child == self.m_BtnClose) then
			self:ShowWnd(false)
		elseif( Child == self.m_BtnExchange) then
			self:ShowExchangeWnd()
		elseif( Child == self.m_BtnCharge) then
				RunUrl("http://pay.linekong.com/jsp/help/index.jsp")
		end
	end
end

function CAgiotage:OpenWnd()
	Gac2Gas:GetTotalTicket(g_Conn)
end

function CAgiotage:RetGetTotalTicket(Conn,nTotalBalance)
	self.m_nTotalBalance = nTotalBalance
	self:ShowExchangeWnd()
	self:ShowWnd(true)
end

function CAgiotage:RetDelTicket(Conn,nAddBalance)
	self.m_nTotalBalance = self.m_nTotalBalance - nAddBalance
	self.m_ExchangeWnd.m_ExchangeBtn:EnableWnd(true)
	self:ShowExchangeWnd()
	self:ShowWnd(true)
end

function CAgiotage:ShowExchangeWnd()
	self.m_ExchangeWnd.m_TotalYongbingbi:SetWndText(self.m_nTotalBalance)
	self.m_ExchangeWnd.m_ChangeNum:SetWndText("")
	self.m_ExchangeWnd.m_ChangeNumDes:SetWndText(GetStaticTextClient(1602))
	self.m_ChargeDes:ShowWnd(false)
	self:ShowWnd(true)
	self.m_ExchangeWnd:ShowWnd(true)
	self.m_ExchangeWnd.m_ChangeNum:SetFocus()
end

function CAgiotage:ShowChargeWnd()
	self.m_ExchangeWnd:ShowWnd(false)
	self.m_ChargeDes:ShowWnd(true)
end

--兑换窗口
function CExchangeWnd:Ctor( Parent )
	self:CreateFromRes( "ExchangeWnd", Parent )
	self.m_PropertyDes 			= self:GetDlgChild("property")
	self.m_TotalYongbingbi 	= self:GetDlgChild("TotalYongbingbi")
	self.m_ChangeNum				= self:GetDlgChild("ChangeNum")
	self.m_ExchangeBtn 			= self:GetDlgChild("ExchangeBtn")
	self.m_ChargeBtn 				= self:GetDlgChild("ChargeBtn")
	self.m_ChangeNumDes 				= self:GetDlgChild("ChangeCountDes")
	self.m_PropertyDes:SetWndText(GetStaticTextClient(1601))
	self.m_ChangeNum:SetMaxTextLenght(6)
	
	local Rt = CFRect:new()
	Parent.m_ChargeDes:GetLogicRect(Rt)
	self:SetLogicRect(Rt)
end

function CExchangeWnd:CheckInputMoney()
	local	InputText = self.m_ChangeNum:GetWndText()
	local findResult = string.find(InputText, "[^0123456789]")
	if  findResult  then
		InputText = string.sub(InputText, 1, findResult-1)
	end
	local inputCount = tonumber( InputText )
	if not inputCount or inputCount <= 0 then 
		self.m_ChangeNum:SetWndText("")
		self.m_ChangeNumDes:SetWndText(GetStaticTextClient(1602))
		return
	end
	if inputCount > 999999 then
	    InputText = string.sub(InputText, 1, 6 )
  end
  self.m_ChangeNum:SetWndText( InputText )
  local MoneyMgr = CMoney:new()
  self.m_ChangeNumDes:SetWndText(GetStaticTextClient(1603,MoneyMgr:ChangeMoneyToString(tonumber(InputText)*100,EGoldType.GoldCoin)))
end

function CExchangeWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK )then
		if (Child == self.m_ExchangeBtn) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160048)
				return 
			end
			local nTicket = tonumber(self.m_ChangeNum:GetWndText()) or 0
			if nTicket > g_GameMain.m_Agiotage.m_nTotalBalance then
				MessageBox( g_GameMain, MsgBoxMsg(9002), MB_BtnOK)
				return 
			end
			if nTicket == 0 then
				MessageBox( g_GameMain, MsgBoxMsg(9003), MB_BtnOK)
				return 
			end
			if nTicket > 0 then
				Gac2Gas:ChangeTicketToMoney(g_Conn,nTicket)
				self.m_ExchangeBtn:EnableWnd(false)
			end
		elseif( Child == self.m_ChargeBtn) then
			local url = "http://pay.linekong.com/charging.do?method=chargingTotal&gameid=8"
			if g_GameID == 786 then
				--混服yy号
				url = "http://pay.linekong.com/charging.do?method=chargingTotal&gameid=786"
			elseif g_GameID == 785 then
				--yy专区
				local user_id = g_Conn:GetUserName()
				url = "http://pay.duowan.com/DepositFromGameAction.action?userid=" .. user_id .. "&product=YBTX&server=s1"
			end
			RunUrl(url);
		end
	elseif (uMsgID == EDIT_ONCHAR and Child == self.m_ChangeNum) then
			self:CheckInputMoney()
	end
end

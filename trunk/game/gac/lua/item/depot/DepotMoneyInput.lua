gac_require "item/depot/DepotMoneyInputInc"

function CreateDepotMoneyInput(parent)
	local wnd = CDepotMoneyInput:new()
	wnd:Init(parent)
	return wnd
end

function CDepotMoneyInput:Init(parent)
	self.m_Type = 0 --1 ¥Ê«Æ, 2 »°«Æ
	self:CreateFromRes("DepotMoneyInput", parent)
end

function CDepotMoneyInput:OnChildCreated()
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_OKBtn	= self:GetDlgChild("OKBtn")
	self.m_Gold		= self:GetDlgChild("Gold")
	self.m_Silver	= self:GetDlgChild("Silver")
	self.m_Copper	= self:GetDlgChild("Copper")
	self.m_Lable	= self:GetDlgChild("Lable")
	
	self.m_Gold:SetMaxTextLenght(8)
	self.m_Silver:SetMaxTextLenght(2)
	self.m_Copper:SetMaxTextLenght(2)
end

function CDepotMoneyInput:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OKBtn) then
			if(1 == self.m_Type) then
				self:OnSaveMoney()
			elseif(2 == self.m_Type) then
				self:OnGetMoney()
			end
			self:ShowWnd(false)
		end
	end
end

function CDepotMoneyInput:OpenPanel(type)
	self.m_Type = type
	if(1 == type) then
		self.m_Gold:SetWndText(0)
		self.m_Silver:SetWndText(0)
		self.m_Copper:SetWndText(0)
		self.m_Lable:SetWndText(GetStaticTextClient(14001))
	else
		local money		= g_GameMain.m_Depot.m_Money
		local gold		= math.floor( money/10000 )
		local silver	= math.floor( (money - gold*10000)/100 )
		local copper	= money - gold*10000 - silver*100
		self.m_Gold:SetWndText(gold)
		self.m_Silver:SetWndText(silver)
		self.m_Copper:SetWndText(copper)
		
		self.m_Lable:SetWndText(GetStaticTextClient(14002))
	end
	self:ShowWnd(true)
	self:SetFocus()
end

function CDepotMoneyInput:OnSaveMoney()
	local money = self:TransMoney()
	Gac2Gas:SaveMoneyToDepot(g_Conn, money)
end

function CDepotMoneyInput:OnGetMoney()
	local money = self:TransMoney()
	Gac2Gas:GetMoneyFromDepot(g_Conn, money)
end

function CDepotMoneyInput:TransMoney()
	local gold		= tonumber( self.m_Gold:GetWndText() ) or 0
	local silver	= tonumber( self.m_Silver:GetWndText() ) or 0
	local copper	= tonumber( self.m_Copper:GetWndText() ) or 0
	return math.floor( gold*10000 + silver*100 + copper ) 
end
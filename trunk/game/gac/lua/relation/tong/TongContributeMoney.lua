CTongContributeMoneyWnd = class(SQRDialog)

function CTongContributeMoneyWnd.GetWnd()
	local Wnd = g_GameMain.m_TongContributeMoneyWnd
	if not Wnd then
		Wnd = CTongContributeMoneyWnd:new(g_GameMain)
		g_GameMain.m_TongContributeMoneyWnd = Wnd
	end
	return Wnd
end

function CTongContributeMoneyWnd:Ctor(Parent)
	self:CreateFromRes("ContributeMoneyWnd", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongContributeMoneyWnd:OnChildCreated()
	self.m_TitleText	= self:GetDlgChild("TitleText")
	self.m_StaticText	= self:GetDlgChild("StaticText")
	self.m_OkBtn			= self:GetDlgChild("OKBtn")
	self.m_CancelBtn	= self:GetDlgChild("CancelBtn")
	
	self.m_TotallGold		= self:GetDlgChild("TotallGold")
	self.m_TotallSilver	= self:GetDlgChild("TotallSilver")
	self.m_TotallCopper	= self:GetDlgChild("TotallCopper")
	self.m_NowGold			= self:GetDlgChild("NowGold")
	self.m_NowSilver		= self:GetDlgChild("NowSilver")
	self.m_NowCopper		= self:GetDlgChild("NowCopper")
	
	self.m_GoldEdit			= self:GetDlgChild("GoldEdit")
	self.m_SilverEdit		= self:GetDlgChild("SilverEdit")
	self.m_CopperEdit		= self:GetDlgChild("CopperEdit")
	
	self:SetStyle(0x00040000)
	self.m_eIndex = 1
	self.m_MaxContributedMoney = 0
	self.m_NowContributedMoney = 0
end

function CTongContributeMoneyWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if self.m_OkBtn == Child then
			self:OnContrubuteBtn()
		elseif self.m_CancelBtn == Child then
			self:ShowWnd(false)
		end
	elseif uMsgID == WND_NOTIFY then
		if Child == self.m_GoldEdit then
			local money = CMoney:new()
			money:CheckInputMoneyValidate(Child, 1, "ContributeMoney")
		elseif Child == self.m_SilverEdit or Child == self.m_CopperEdit then
			local money = CMoney:new()
			money:CheckInputMoneyValidate(Child, 2, "ContributeMoney")
		end
	end
end

function CTongContributeMoneyWnd:OnContrubuteBtn()
	local uGold = tonumber(self.m_GoldEdit:GetWndText())
	local uSilve = tonumber(self.m_SilverEdit:GetWndText())
	local uCopper = tonumber(self.m_CopperEdit:GetWndText())
	uGold = uGold and uGold or 0
	uSilve = uSilve and uSilve or 0
	uCopper = uCopper and uCopper or 0
			
	local uMoney = uGold*10000 + uSilve*100 + uCopper
	
	if uMoney == 0 then
		MsgClient(9381)
	end
	if self.m_eIndex == 1 then
		Gac2Gas:ContributeMoneyChangeExp(g_Conn,uMoney)
	elseif self.m_eIndex == 2 then
		Gac2Gas:ContributeMoneyChangeSoul(g_Conn,uMoney)
	elseif self.m_eIndex == 3 then
		Gac2Gas:ContributeMoneyChangeResource(g_Conn,uMoney)
	end
end

function CTongContributeMoneyWnd:ShowMaxMoney(money)
	local gold		= math.floor( money/10000 )
	local silver	= math.floor( (money - gold*10000)/100 )
	local copper	= money - gold*10000 - silver*100
	self.m_TotallGold:SetWndText(gold)
	self.m_TotallSilver:SetWndText(silver)
	self.m_TotallCopper:SetWndText(copper)
end

function CTongContributeMoneyWnd:ShowNowMoney(money)
	local gold		= math.floor( money/10000 )
	local silver	= math.floor( (money - gold*10000)/100 )
	local copper	= money - gold*10000 - silver*100
	self.m_NowGold:SetWndText(gold)
	self.m_NowSilver:SetWndText(silver)
	self.m_NowCopper:SetWndText(copper)
end

function CTongContributeMoneyWnd:OpenPanel(eIndex,max_money,sum_money)
	self.m_eIndex = eIndex
	self.m_GoldEdit:SetWndText("")
	self.m_SilverEdit:SetWndText("")
	self.m_CopperEdit:SetWndText("")
	
	if self.m_eIndex == 1 then
		self.m_TitleText:SetWndText(GetStaticTextClient(10072))
		self.m_StaticText:SetWndText(GetStaticTextClient(10075))
	elseif self.m_eIndex == 2 then
		self.m_TitleText:SetWndText(GetStaticTextClient(10073))
		self.m_StaticText:SetWndText(GetStaticTextClient(10076))
	elseif self.m_eIndex == 3 then
		self.m_TitleText:SetWndText(GetStaticTextClient(10074))
		self.m_StaticText:SetWndText(GetStaticTextClient(10077))
	end
	self:ShowMaxMoney(max_money)
	self:ShowNowMoney(sum_money)
	self.m_MaxContributedMoney = max_money
	self.m_NowContributedMoney = sum_money
	if not self:IsShow() then
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
	end
	self:ShowWnd(true)
end

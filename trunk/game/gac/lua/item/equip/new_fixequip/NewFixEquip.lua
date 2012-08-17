CNewFixEquipWnd = class(SQRDialog)

function CNewFixEquipWnd:Ctor()
	self:Init()
end

function CNewFixEquipWnd:Init()
	self:CreateFromRes("NewFixEquipWnd",g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, "·Ç»¥³â")
end

function CNewFixEquipWnd:OnChildCreated()
	self.m_EquipInRoleCheckBtn = self:GetDlgChild("EquipInRoleCheckBtn")
	self.m_EquipInBagCheckBtn = self:GetDlgChild("EquipInBagCheckBtn")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_Static = self:GetDlgChild("Static")
	self.m_EquipInRoleCheckBtn:SetCheck(true)
	self.m_EquipInBagCheckBtn:SetCheck(false)
end

function CNewFixEquipWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
		elseif Child == self.m_EquipInRoleCheckBtn then
			if not self.m_EquipInRoleCheckBtn:GetCheck() then
				self.m_EquipInRoleCheckBtn:SetCheck(true)
				self.m_EquipInBagCheckBtn:SetCheck(false)
			end
			g_GameMain.m_CEquipInRoleWnd:ShowWnd(true)
			g_GameMain.m_CEquipInBagWnd:ShowWnd(false)
		elseif Child == self.m_EquipInBagCheckBtn then
			if not self.m_EquipInBagCheckBtn:GetCheck() then
				self.m_EquipInBagCheckBtn:SetCheck(true)
				self.m_EquipInRoleCheckBtn:SetCheck(false)
			end
			g_GameMain.m_CEquipInRoleWnd:ShowWnd(false)
			g_GameMain.m_CEquipInBagWnd:ShowWnd(true)
		end	
	end		
end





CFlowerMiniWnd = class(SQRDialog)

function CFlowerMiniWnd:Ctor()
	self:CreateFromRes("FlowerMiniWnd", g_GameMain)
	self.m_Btn = self:GetDlgChild("Btn")
	self.m_Btn:SetMouseOverDescAfter(GetStaticTextClient(6015))
	self:ShowWnd(false)
end

function CFlowerMiniWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Btn then
			g_GameMain.m_LiveSkillFlower:ShowWnd(true)
			g_GameMain.m_LiveSkillFlower:SetFocus()
			self:ShowWnd(false)
		end
	end
end

CLeadLeanSkillShowWnd = class(SQRDialog) 

function CLeadLeanSkillShowWnd:Ctor()
	self:CreateFromRes("LeadLeanSkillShowWnd",g_GameMain)
	self.m_LeadLeanSkillShowBtn = self:GetDlgChild("LeadLeanSkillShowBtn")
end

function CLeadLeanSkillShowWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_LeadLeanSkillShowBtn then
			if not g_GameMain.m_LeadLeanSkillWnd:IsShow() then
				g_GameMain.m_LeadLeanSkillWnd:ShowWnd(true)
				self:ShowWnd(false)
			end
		end
	end
end
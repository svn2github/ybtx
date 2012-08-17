CClassIntroVideoBtn = class(SQRDialog)

function CClassIntroVideoBtn:Ctor()
	self:CreateFromRes("ClassIntroVideoBtn", g_NewRole)
end

function CClassIntroVideoBtn:OnChildCreated()
	self.m_PlayBtn	= self:GetDlgChild("PlayBtn")
end

function CClassIntroVideoBtn:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_PlayBtn) then
			g_NewRole.m_ClassIntroVideo:LoadAndPlayMovie()
		end
	end
end
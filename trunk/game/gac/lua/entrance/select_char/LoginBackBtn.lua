CLoginBackBtn = class( SQRDialog )

function CLoginBackBtn:Ctor(parent)
	self:CreateFromRes("LoginBackBtn", parent)
end

function CLoginBackBtn:OnChildCreated()
	self.m_Btn = self:GetDlgChild("Btn")
end

function CLoginBackBtn:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Btn) then
			local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive
			if( next(tbl[1]) or next(tbl[2]) ) then
				g_SelectChar.m_SelectCampWnd:BackUpAnimation()
			else
				g_SelectChar.m_SelectCampWnd:BackUpAnimation(EGameState.eToLogin)
			end
			self:ShowWnd(false)
		end
	end
end
CPkSwitch = class (SQRDialog)

function CPkSwitch:Ctor(Parent)
	self:CreateFromRes("PkValue", Parent)
	self.m_PkSwitch	= self:GetDlgChild("PkSwitch")
	self:GetWnd()
end

function CPkSwitch:GetWnd()
	self:ShowWnd(true)
	return self
end

function CPkSwitch:IsShowWnd()
	local PlayerLevel
	local PlayerMinLevel
	local PlayerMaxLevel
	if IsCppBound(g_MainPlayer) then
		if g_MainPlayer.m_SwitchState == false then    -- false为否定票(和平)
			self.m_PkSwitch:SetCheck(true)
			self.m_PkSwitch:SetMouseOverDescAfter(GetStaticTextClient(12019))
		else -- true为肯定票(战争)
			self.m_PkSwitch:SetCheck(false)
			self.m_PkSwitch:SetMouseOverDescAfter(GetStaticTextClient(12018))
		end
	end
end

function CPkSwitch:ShowPkWnd()
	if IsCppBound(g_MainPlayer) then
		if g_MainPlayer.m_SwitchState == false then    -- false为否定票(和平)
			self.m_PkSwitch:SetCheck(true)
		else -- true为肯定票(战争)
			self.m_PkSwitch:SetCheck(false)
		end
	end
end



function CPkSwitch:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_PkSwitch == Child then
			if not IsCppBound(g_MainPlayer) then
				return
			end
			if g_MainPlayer.m_SwitchState == true then
				Gac2Gas:OpenPkSwitch(g_Conn, g_MainPlayer.m_SwitchState)
			elseif g_MainPlayer.m_SwitchState == false then
				if g_GameMain.m_PkSwitchMsgWnd then
					g_GameMain.m_PkSwitchMsgWnd:Destroy()
				end
				g_GameMain.m_PkSwitchMsgWnd = CPkSwitchMsg.GetWnd()
			end
		end
	end
end


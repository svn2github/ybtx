CTongChangeNameWnd	= class(SQRDialog)

--佣兵小队更名
function CTongChangeNameWnd:GetWnd(parent)
	local Wnd = g_GameMain.m_TongChangeName
	if not Wnd then
		Wnd = CTongChangeNameWnd:new(g_GameMain)
		Wnd:CreateFromRes("TongChangeName", g_GameMain)
		g_GameMain.m_TongChangeName = Wnd
	end
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, "非互斥")
	return Wnd
end

function CTongChangeNameWnd:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_CancelBtn	= self:GetDlgChild("CancelBtn")
	self.m_InputEdit	= self:GetDlgChild("InputEdit")
	self.m_OldName		= self:GetDlgChild("OldName")
	self.m_InputEdit:SetMaxTextLenght(8)
end

function CTongChangeNameWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then
			local sNewTongName = self.m_InputEdit:GetWndText()
			if( "" == sNewTongName ) then
				MsgClient(9166)
				return
			end
			local textFilter = CTextFilterMgr:new()
			if( not textFilter:IsValidName(sNewTongName) ) then
				MsgClient(9198)
				return
			end
			self:ShowWnd(false)
			Gac2Gas:ChangeTongName(g_Conn, sNewTongName)
		elseif(Child == self.m_CancelBtn or Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CTongChangeNameWnd:OpenPanel()
	self:ShowWnd(true)
	local sOldName = string.gsub( g_MainPlayer.m_Properties:GetTongName(), "&", "" )
	self.m_OldName:SetWndText(sOldName)
end

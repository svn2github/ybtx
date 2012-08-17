CArmyCorpsChangeName = class(SQRDialog)

function CArmyCorpsChangeName:GetWnd(parent)
	local Wnd = g_GameMain.m_ArmyCorpsChangeName
	if not Wnd then
		Wnd = CArmyCorpsChangeName:new(g_GameMain)
		Wnd:CreateFromRes("ArmyCorpsChangeName", g_GameMain)
		g_GameMain.m_ArmyCorpsChangeName = Wnd
	end
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, "·Ç»¥³â")
	return Wnd
end

function CArmyCorpsChangeName:OnChildCreated()
	self.m_XBtn			  = self:GetDlgChild("XBtn")
	self.m_Title			= self:GetDlgChild("Title")
	self.m_Notice	    = self:GetDlgChild("Notice")
	self.m_InputWnd		= self:GetDlgChild("InputWnd")
	self.m_Confirm		= self:GetDlgChild("Confirm")
	self.m_Cancel	    = self:GetDlgChild("Cancel")
	self.m_OldName		= self:GetDlgChild("OldName")
--	self.m_InputEdit:SetMaxTextLenght(8)
end

--ÏûÏ¢
function CArmyCorpsChangeName:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Confirm) then
			local sName = self.m_InputWnd:GetWndText()
			if( "" == sName ) then
				MsgClient(8543)
				return
			end
			local textFilter = CTextFilterMgr:new()
			if( not textFilter:IsValidName(sName) ) then
				MsgClient(8540)
				return
			end
			self:ShowWnd(false)
			Gac2Gas:ChangeArmyCorpsName(g_Conn, sName)
		elseif(Child == self.m_Cancel or Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CArmyCorpsChangeName:OpenPanel()
	self:ShowWnd(true)
	local sOldName = string.gsub( g_MainPlayer.m_Properties:GetArmyCorpsName(), "&", "" )
	self.m_OldName:SetWndText(sOldName)
end
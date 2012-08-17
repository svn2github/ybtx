CPlayerChangeNameWnd				= class( SQRDialog )

local function CreatePlayerChangeNameWnd(wnd)
	wnd:CreateFromRes("PlayerChangeName", g_SelectChar)
	wnd:ShowWnd(true)
end

function CPlayerChangeNameWnd.ReturnPreChangePlayerName(bFlag)
	g_SelectChar:MsgControl(false, false)
	if ( bFlag ) then
		g_SelectChar.m_SelectCharWnd:OnMouseSetting(false)
	else
		if(not g_SelectChar.m_PlayerChangeName) then
			g_SelectChar.m_PlayerChangeName = CPlayerChangeNameWnd:new()
		end
		g_SelectChar.m_PlayerChangeName:OpenPanel()
	end
end

function CPlayerChangeNameWnd.ReturnChangePlayerName(bFlag, nMessageID)
	if ( bFlag ) then
		g_SelectChar.m_SelectCharWnd:OnMouseSetting(false)
	else
		g_SelectChar.m_MsgBox = MessageBox( g_SelectChar, MsgBoxMsg(nMessageID), MB_BtnOK )
	end
end

function CPlayerChangeNameWnd:Ctor()
	CreatePlayerChangeNameWnd(self)
end

function CPlayerChangeNameWnd:OnChildCreated()
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_InputEdit	= self:GetDlgChild("InputEdit")
	self.m_OldName		= self:GetDlgChild("OldName")
end

function CPlayerChangeNameWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then
			local oldName = self.m_OldName:GetWndText()
			local sNewName = self.m_InputEdit:GetWndText()
			if oldName == sNewName then
				MessageBox(g_SelectChar, MsgBoxMsg(34), MB_BtnOK, CallBack)
				return
			end
			local textFilter = CTextFilterMgr:new()
			local bType,strRet = textFilter:CheckRoleName(sNewName)
			if ( not bType ) then
				g_SelectChar.m_MsgBox = MessageBox( g_SelectChar, MsgBoxMsg(strRet), MB_BtnOK, CallBack)
				return
			end
			self:ShowWnd(false)
			
			local nIndex	= g_SelectChar.m_SelectCharWnd.m_List:GetSelectItem(-1) + 1
			local role		= g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex]
			Gac2Gas:ChangePlayerName(g_Conn, role.m_id, sNewName)
		end
	end
end

function CPlayerChangeNameWnd:OpenPanel()
	self:ShowWnd(true)
	local nIndex	= g_SelectChar.m_SelectCharWnd.m_List:GetSelectItem(-1) + 1
	local role		= g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex]
	local sOldName = string.gsub( role.m_name, "&", "" )
	self.m_OldName:SetWndText(sOldName)
end
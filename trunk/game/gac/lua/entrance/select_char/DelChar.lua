CWndDelRole							= class( SQRDialog )

--验证用户输入的验证码是否正确
function CWndDelRole:CheckVarifyWord()
	--如果输入的验证码不对，则给出提示信息
	if(self.m_InputVarifyWord:GetWndText() ~= "yes") then
		MessageBox( g_SelectChar, MsgBoxMsg(11007), MB_BtnOK )
		return
	end
	--如果玩家输入的验证码正确，则直接删除
	self:OnOK( false )
	self:ShowWnd( false )
	SQRWnd_RegisterAccelerator( g_SelectChar.m_KeyIAccelerator, WM_KEYDOWN )
end
 
function CWndDelRole:OnOK()
	g_SelectChar.m_SelectCharWnd.m_EnterGame:EnableWnd(false)
	g_SelectChar.m_SelectCharWnd.m_DelRole:EnableWnd(false)
	g_SelectChar.m_DelRoleList.m_Live:EnableWnd(false)
	if(self.m_bDel) then
		if (GacTestConfig.Need == 1) then	--测试运行
			Gac2Gas:CompleteDeleteChar(g_Conn, self.m_nRoleId)
		else								--正常运行
			g_SelectChar:MsgControl(true, true)
			Gac2Gas:DelChar( g_Conn, self.m_nRoleId )
		end
	else
		g_SelectChar:MsgControl(true, true)
		Gac2Gas:GetBackRole(g_Conn, self.m_nRoleId)
	end
end

function CWndDelRole:Ctor()
	self:CreateFromRes( "Login_DelRole", g_SelectChar )
end

function CWndDelRole:OnChildCreated()
	self.m_OkBtn			= self:GetDlgChild("Btn_Ok")
	self.m_CancelBtn		= self:GetDlgChild("Btn_Cancel")
	self.m_Title			= self:GetDlgChild("Wnd_Title")
	self.m_VarifyWord		= self:GetDlgChild("Wnd_VarifyWord")
	self.m_InputVarifyWord	= self:GetDlgChild("Input_VarifyWord")
	self.m_DelInfomation	= self:GetDlgChild("Wnd_Info")	--给出删除该角色的相关提示信息,具体根据角色等级的不同，保留天天数不同，需改进
end

function CWndDelRole:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if( Child == self.m_OkBtn ) then
			 if(self.m_InputVarifyWord:GetWndText() == "") then 
				MessageBox( g_SelectChar, MsgBoxMsg(11007), MB_BtnOK )
				return
			 end
			 self:CheckVarifyWord()
		elseif( Child == self.m_CancelBtn ) then
			self:ShowWnd( false )
			SQRWnd_RegisterAccelerator( g_SelectChar.m_KeyIAccelerator, WM_KEYDOWN ) 
		end
	end
end

function CWndDelRole:OnShow(role, bDel)
	self.m_nRoleId	= role.m_id
	self.m_bDel		= bDel
	SQRWnd_UnRegisterAccelerator( g_SelectChar.m_KeyIAccelerator )
	local nMsg = bDel and 1054 or 1056
	self.m_Title:SetWndText(GetStaticTextClient(nMsg))
	self.m_VarifyWord:SetWndText("yes")
	self.m_InputVarifyWord:SetWndText("")
	local roleLevel = role.m_level
	local delRoleSavedTime = GetDelCharSavedTimeByLevel(roleLevel)
	self.m_DelInfomation:SetWndText(MsgBoxMsg(11006, delRoleSavedTime))
	self:ShowWnd(true)
	self.m_InputVarifyWord:SetFocus()
end

CIdentityAffirmWnd	= class( SQRDialog )

function CIdentityAffirmWnd:Ctor()
	self:CreateFromRes("IdentityAffirmWnd", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CIdentityAffirmWnd:OnChildCreated()
	self.m_CloseBtn					= self:GetDlgChild("CloseBtn")
	self.m_OkBtn 						= self:GetDlgChild("OkBtn")
	self.m_PasswordEdit 		= self:GetDlgChild("PasswordEdit")
end

function CIdentityAffirmWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if( uMsgID == BUTTON_LCLICK ) then
		if (Child == self.m_OkBtn) then
			local strPasswordMD5 = self:Encrypt(self.m_PasswordEdit:GetWndText())
			Gac2Gas:IdentityAffirm(g_Conn,strPasswordMD5)
		elseif (Child == self.m_CloseBtn) then
			self.m_PasswordEdit:SetWndText("")
			self:ShowWnd(false)
		end
	end
end

function CIdentityAffirmWnd:Encrypt(strPassword)
	local Password_to_MD5 = g_Conn:GetUserName() .. " " .. strPassword
	local PasswordMD5 = CreateCBufAcr( CreateCBuf(33) )
	local MD5Digester = CMd5Digester:new()
	MD5Digester:Begin()
	MD5Digester:Append(Password_to_MD5, string.len(Password_to_MD5))
	MD5Digester:GetMD5Base16Str(PasswordMD5)
	local strPasswordMD5 = PasswordMD5:ToString()
	return strPasswordMD5
end

function CIdentityAffirmWnd.RetOpenIdentityAffirmWnd()
	g_GameMain.m_IdentityAffirmWnd:ShowWnd(true)
	g_GameMain.m_IdentityAffirmWnd.m_PasswordEdit:SetFocus()
end

function CIdentityAffirmWnd.ReturnIdentityAffirm(Conn,result)
	if result then
		g_GameMain.m_ItemBagLockWnd:ShowWnd(true)
		g_GameMain.m_IdentityAffirmWnd:ShowWnd(false)
	else
		MsgClient(160022)
		g_GameMain.m_IdentityAffirmWnd.m_PasswordEdit:SetFocus()
	end
	g_GameMain.m_IdentityAffirmWnd.m_PasswordEdit:SetWndText("")
end
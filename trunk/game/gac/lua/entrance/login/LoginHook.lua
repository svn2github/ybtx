CLoginHook = class( IHook )

--托管开关
local function OpenTrusteeLogin()
	if g_Login and g_Login:IsShow() then
		local strFileName = g_RootPath .. "etc/gac/trustee.txt"
		local f = CLuaIO_Open(strFileName, "r")
		if(f ~= nil) then
			if g_Login.m_TrusteeFlag then
				g_Login.m_LoginAccounts.m_btnOK:SetWndText(GetStaticTextClient(1236))
				g_Login.m_LoginAccounts.m_Wnd_ID:SetWndText(GetStaticTextClient(1235))
				g_Login.m_TrusteeFlag = nil
				
			else
				if not g_Login.m_LoginAccounts.m_btnOK:IsShow() then
					g_Login.m_LoginAccounts:SetChildWndShow(true)
					g_Login.m_YY:ShowWnd(false)
					g_Login.m_LoginAccounts.m_BtnYYBack:ShowWnd(false)
				end
				g_Login.m_LoginAccounts.m_btnOK:SetWndText(GetStaticTextClient(1234))
				g_Login.m_LoginAccounts.m_Wnd_ID:SetWndText(GetStaticTextClient(1233))
				g_Login.m_TrusteeFlag = true
			end
			f:Close()
		end
	end
end

function CLoginHook:Ctor()
	self.m_KeyTbl = {}
end

function CLoginHook:KeyIsExist(ukey)
	for i =1, #(self.m_KeyTbl) do
		if (self.m_KeyTbl[i] == ukey) then
			return true
		end
	end
	return false
end

function CLoginHook:SaveKeyToTbl(ukey)
	for i =1, #(self.m_KeyTbl) do
		if (self.m_KeyTbl[i] == ukey) then
			return
		end
	end
	table.insert(self.m_KeyTbl, ukey)
end

function CLoginHook:ReMoveKeyFromTbl(ukey)
	for i =1, #(self.m_KeyTbl) do
		if (self.m_KeyTbl[i] == ukey) then
			table.remove(self.m_KeyTbl,i)
			return
		end
	end
end

function CLoginHook:OnHook( Wnd, Msg, wParam, lParam )
	if g_Login and g_Login.m_DisableInput == true then	
		return
	end

	if (Msg == WM_KEYUP or Msg == WM_SYSKEYUP) then
		self:ReMoveKeyFromTbl(wParam)
	elseif (Msg == WM_KEYDOWN or Msg == WM_SYSKEYDOWN) then
		self:SaveKeyToTbl(wParam)
		if self:KeyIsExist(VK_F12) and self:KeyIsExist(VK_CONTROL) and self:KeyIsExist(VK_SHIFT) then
			OpenTrusteeLogin()
		end
		if wParam == VK_ESCAPE then
			local wnd = g_Login.m_LoginAccounts
			if( wnd.m_btnBack:IsEnable() ) then
				wnd:OnCtrlmsg(wnd.m_btnBack, BUTTON_LCLICK, 0, 0)
			end
		elseif wParam == VK_TAB then
			if g_Login.m_SecretSecurityCardWnd and g_Login.m_SecretSecurityCardWnd:IsShow() then
				if g_Login.m_SecretSecurityCardWnd.m_CryptKey1:IsFocus() then
					g_Login.m_SecretSecurityCardWnd.m_CryptKey2:SetFocus()
				elseif g_Login.m_SecretSecurityCardWnd.m_CryptKey2:IsFocus() then
					g_Login.m_SecretSecurityCardWnd.m_CryptKey3:SetFocus()
				else
					g_Login.m_SecretSecurityCardWnd.m_CryptKey1:SetFocus()
				end
			else	
				if g_Login.m_LoginAccounts.m_editUserName:IsFocus() then
					g_Login.m_LoginAccounts.m_editPassword:SetFocus()
				elseif g_Login.m_LoginAccounts.m_editPassword:IsFocus() then
					g_Login.m_LoginAccounts.m_editUserName:SetFocus()
				else
					g_Login.m_LoginAccounts.m_editUserName:SetFocus()
				end
			end
		elseif wParam == VK_RETURN then
			local wnd = g_Login.m_LoginAccounts
			if( wnd.m_btnOK:IsEnable() ) then
				wnd:OnCtrlmsg( wnd.m_btnOK, BUTTON_LCLICK, 0, 0 )
			end		
		end
	else
		return true
	end
	--returen false 表示hook直接截取了消息
	return false
end

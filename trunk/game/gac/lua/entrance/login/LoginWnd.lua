CLoginAccounts = class( SQRDialog )

function CLoginAccounts.RetLoginConnectStepInfo(msgID, ...)
	if g_Login.m_WndCancelMsgbox and 
		g_Login.m_WndCancelMsgbox:IsShow() then
		local msgInfo =  GetStaticTextClient(msgID, ...)  
		g_Login.m_WndCancelMsgbox.m_wndInfo:SetWndText(msgInfo)
	end
end

local function ShortConnCheckUser()
	g_Login.m_Socket = CConnection:new()
	g_Login.m_DisableInput = true
	
	g_Login.m_WndCancelMsgbox.m_wndInfo:SetWndText(GetStaticTextClient(1053) .. "......")
	g_Login.m_WndCancelMsgbox:ShowWnd(true)
	g_Login.m_WndCancelMsgbox:SetFocus()
	SQRWnd_RegisterHook(g_Login.m_WndCancelMsgbox.m_KeyHook, WM_KEYDOWN)

	--连接到GAS
	local addr=g_Login.m_LoginAccounts.m_addr
	g_Conn = ConnMgrClient:Connect(addr)
	assert(g_Conn)
end

--连接到RCP服务器
local function Connect2Server()
	coroutine.resume(coroutine.create(ShortConnCheckUser))
end

function CLoginAccounts:Ctor(parent)
	self.m_Infos = g_ParseCommandline()
	self.m_strSettingFileName = g_RootPath .. "var/gac/LoginSetting.txt"
	self:CreateFromRes("Login_Accounts", parent)
	self:SetRememberName()
	self.m_KeyHook = CLoginHook:new()
	SQRWnd_RegisterHook(self.m_KeyHook, WM_KEYDOWN)
	SQRWnd_RegisterHook(self.m_KeyHook, WM_KEYUP)	
	SQRWnd_RegisterHook(self.m_KeyHook, WM_SYSKEYDOWN)
	SQRWnd_RegisterHook(self.m_KeyHook, WM_SYSKEYUP)
end

function CLoginAccounts:OnChildCreated()
	self.m_btnBack			= self:GetDlgChild("Btn_Back")
	self.m_BtnYYBack		= self:GetDlgChild("Btn_YYBack")
	self.m_btnOK			= self:GetDlgChild("Btn_OK")
	self.m_wndServerName	= self:GetDlgChild("Wnd_ServerName")
	self.m_editUserName		= self:GetDlgChild("Edit_UserName")
	self.m_editPassword		= self:GetDlgChild("Edit_Password")
	self.m_OpenYYBtn		= self:GetDlgChild("OpenYYBtn")
	self.m_A001				= self:GetDlgChild("A001")
	self.m_Wnd_ID 			= self:GetDlgChild("Wnd_ID")
	self.m_Wnd_Password		= self:GetDlgChild("Wnd_Password")
	self.m_YYicon 			= self:GetDlgChild("YYicon")
--	self.m_OpenWeiBoBtn		= self:GetDlgChild("OpenWeiBoBtn")
--	self.m_WebBrowser		= self:GetDlgChild("WebBrowser")
	
	self.m_editUserName:SetIsSendMsgToParent(true)
	self.m_editPassword:SetIsSendMsgToParent(true)
	self.m_btnOK:EnableWnd(false)
	self.m_wndServerName:ShowWnd(false)
	self.m_OpenYYBtn:ShowWnd(false)
	self.m_YYicon:ShowWnd(false)
	self.m_BtnYYBack:ShowWnd(false)
end

function CLoginAccounts:SetChildWndShow(flag)
	self.m_btnOK:ShowWnd(flag)
	self.m_wndServerName:ShowWnd(flag)
	self.m_editUserName:ShowWnd(flag)
	self.m_editPassword:ShowWnd(flag)
	self.m_OpenYYBtn:ShowWnd(flag)
	self.m_YYicon:ShowWnd(flag)
	self.m_editUserName:ShowWnd(flag)
	self.m_editPassword:ShowWnd(flag)
	self.m_Wnd_Password:ShowWnd(flag)
	self.m_Wnd_ID:ShowWnd(flag)
	self.m_A001:ShowWnd(flag)
	self.m_btnBack:ShowWnd(flag)
end

function CLoginAccounts:YYConnect2Server()
	self:SetAddress()
	Connect2Server()
end

function CLoginAccounts:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if( self.m_btnOK == Child ) then
			if(g_Login.m_IsConnecting) then
				return
			end
			self.m_strUserName = string.lower(self.m_editUserName:GetWndText())
			local flag, errorMsg = CheckUserName(self.m_strUserName)
			if(flag == false and g_Login.m_TrusteeFlag == nil ) then --用户名非法
				MessageBox(g_App:GetRootWnd(),MsgBoxMsg(errorMsg), MB_BtnOK)
				g_Login:SetFocus()
				return
			end
			local strPassword = self.m_editPassword:GetWndText()
			if(string.len(strPassword) == 0) then
				return
			end
			g_Login.m_IsConnecting = true
			self:Encrypt(strPassword)
			self:SetAddress()
			self:SaveUserName()
			Connect2Server()--连接到服务器
		elseif( self.m_btnBack == Child or self.m_BtnYYBack == Child) then
			self.m_btnOK:EnableWnd(false)
			self.m_btnBack:EnableWnd(false)
			g_strSelServerInfo = nil
			if(g_CoreScene) then
				g_CoreSceneMgr:DestroyMainScene()
			end
			coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
		elseif ( self.m_OpenYYBtn == Child ) then
			if g_Login.m_InitYY then
				g_Login.m_YY:ShowWnd(not g_Login.m_YY:IsShow())
			end
		elseif(self.m_OpenWeiBoBtn == Child) then
			g_Login.m_IsConnecting = true
			g_App.m_WeiBo:WeiBoConnect2Server()
		end
	elseif(uMsgID == EDIT_ONCHAR) then
		if(self.m_editUserName == Child or self.m_editPassword == Child) then
			self:PlayScreenFx("dljm/create")
			local strUserName = self.m_editUserName:GetWndText()
			local strPassword = self.m_editPassword:GetWndText()
			local bFlag = string.len(strUserName) > 0 and string.len(strPassword) > 0
			self.m_btnOK:EnableWnd(bFlag)
		end
	elseif(uMsgID == WEBBROWSER_FORWARD) then
		if(Child == self.m_WebBrowser) then
			print(111)
		end
	elseif(uMsgID == WEBBROWSER_LOADCOMPLETE) then
		if(Child == self.m_WebBrowser) then
			print(222)
			local a = self.m_WebBrowser:GetSrcCode()
		end
	elseif(uMsgID == WEBBROWSER_DOCCOMPLETE) then
		if(Child == self.m_WebBrowser) then
			self.m_WebBrowser:ShowWnd(true)
			local sHtml = self.m_WebBrowser:GetSrcCode()
			local _, nCodeBegin = string.find(sHtml, "<SPAN class=fb>")
			local nCodeEnd = string.find(sHtml, "</SPAN>")
			if(nCodeBegin and nCodeEnd) then
				self.m_WebBrowser:ShowWnd(false)
				local sCode = string.sub(sHtml, nCodeBegin+1, nCodeEnd-1)
				g_App.m_WeiBo:WeiBoLoginCheck(sCode)
			end
		end
	end
end

function CLoginAccounts:OnDestroy()
	SQRWnd_UnRegisterHook(self.m_KeyHook)
	if(self.m_LoginWndShowTick) then
		UnRegisterTick(self.m_LoginWndShowTick)
		self.m_LoginWndShowTick = nil
	end
end

function CLoginAccounts:SetRememberName()
	local f = CLuaIO_Open(self.m_strSettingFileName, "r")
	local text = ""
	if(f) then
		text = f:ReadAll()
		f:Close()
	end
	if (text ~= "") then
		local pos = string.find(text, " ")  --照顾老代码(以前在客户端保存了服务器Index，用空格同用户名隔开)
		local sUserName = pos and string.sub(text,1,pos-1) or text
		self.m_editUserName:SetWndText(sUserName)
	end
end

function CLoginAccounts:Encrypt(strPassword)
	local Password_to_MD5 = ""
	if g_Login.m_TrusteeFlag then --账号托管
		Password_to_MD5 = strPassword
	else
		Password_to_MD5 = self.m_strUserName .. " " .. strPassword
	end
	local PasswordMD5 = CreateCBufAcr( CreateCBuf(33) )
	self.m_MD5Digester = CMd5Digester:new()
	self.m_MD5Digester:Begin()
	self.m_MD5Digester:Append(Password_to_MD5, string.len(Password_to_MD5))
	self.m_MD5Digester:GetMD5Base16Str(PasswordMD5)
	self.m_strPasswordMD5 = PasswordMD5:ToString()
end

--设置服务器地址
function CLoginAccounts:SetAddress()
	local pos		= string.find(self.m_Infos[1], ":")
	local strIP		= string.sub(self.m_Infos[1], 1 ,pos-1)
	local strPort	= string.sub(self.m_Infos[1], pos+1, -1)
	self.m_addr		= CAddress:new()
	self.m_addr:SetAddress(strIP)
	self.m_addr:SetPort(strPort)
end

function CLoginAccounts:SaveUserName()
	local f = CLuaIO_Open(self.m_strSettingFileName, "w")
	if(f) then
		f:WriteString(self.m_strUserName)
		f:Close()
	end
end

function CLoginAccounts:Animation(bDelay)
	local function LoginWndShowTick()
		if(self.m_LoginWndShowTick) then
			UnRegisterTick(self.m_LoginWndShowTick)
			self.m_LoginWndShowTick = nil
		end
		self:ShowWnd(true)
		self.m_editUserName:SetFocus()
		self:SetTransparent(0.1)
		self:SetTransparentObj(1000, false)
	end

	if(bDelay) then
		if(self.m_LoginWndShowTick) then
			UnRegisterTick(self.m_LoginWndShowTick)
			self.m_LoginWndShowTick = nil
		end
		self.m_LoginWndShowTick = RegisterTick("LoginWndShowTick", LoginWndShowTick, 3000)
	else
		LoginWndShowTick()
	end
end


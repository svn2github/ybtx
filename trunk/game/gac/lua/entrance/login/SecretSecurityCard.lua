CSecretSecurityCardWnd = class (SQRDialog)
function CSecretSecurityCardWnd:Ctor()
    self:CreateFromRes("SecretSecurityCard", g_Login)
end

function CSecretSecurityCardWnd:OnChildCreated()
    self.m_PublicKey1   =   self:GetDlgChild("PublicKey1")
    self.m_PublicKey2   =   self:GetDlgChild("PublicKey2")
    self.m_PublicKey3   =   self:GetDlgChild("PublicKey3")
    self.m_CryptKey1    =   self:GetDlgChild("CryptKey1")
    self.m_CryptKey2    =   self:GetDlgChild("CryptKey2")
    self.m_CryptKey3    =   self:GetDlgChild("CryptKey3")
    self.m_ClearBtn     =   self:GetDlgChild("ClearBtn")
    self.m_OkBtn        =   self:GetDlgChild("OkBtn")
    self.m_CancelBtn    =   self:GetDlgChild("CancelBtn")
    
    self.m_StayIdleTime =   1
end

function CSecretSecurityCardWnd:CheckStayIdleInSecretSecurityWnd()
    if g_Login.m_SecretSecurityCardWnd.m_StayIdleTime >= 10 then
        UnRegisterTick(self.m_CheckStayIdleTick)
        self.m_CheckStayIdleTick = nil
        if g_Conn and g_Conn.ShutDown then
            g_Conn:ShutDown()
        end
        coroutine.resume( g_App.MainRoutine, EGameState.eToLogin ) 
        MessageBox(g_App:GetRootWnd(),MsgBoxMsg(41), MB_BtnOK)
    else
        g_Login.m_SecretSecurityCardWnd.m_StayIdleTime = g_Login.m_SecretSecurityCardWnd.m_StayIdleTime + 1
    end
end

function CSecretSecurityCardWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
    self.m_StayIdleTime = 1
    if (uMsgID == BUTTON_LCLICK) then
        if Child == self.m_ClearBtn then
            self:ClearSecretKeyInfo()
        
        elseif Child == self.m_OkBtn then
            local publicKeyStr, secretKeyStr = self:GetPublicAndSecretKey()
            g_Login.m_IsConnecting = true
            g_Login.m_LoginAccounts.m_strPasswordMD5 = g_Login.m_SecretSecurityCardWnd.m_strPasswordMD5
            g_Login.m_LoginAccounts.m_strUserName = g_Login.m_SecretSecurityCardWnd.m_strUserName
            g_Login.m_LoginAccounts:SaveUserName()
            g_Login.m_LoginAccounts.publicKeyStr = publicKeyStr
            g_Login.m_LoginAccounts.secretKeyStr = secretKeyStr  
            g_Login.m_LoginAccounts:YYConnect2Server()--连接到服务器       
        elseif Child == self.m_CancelBtn then
            if g_Conn and g_Conn.ShutDown then
		        g_Conn:ShutDown()
	        end
	        coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
        end
    end
end

function CSecretSecurityCardWnd:SetSecretSecurityPublicKey(randomPubKey1,randomPubKey2,randomPubKey3)    
    self.m_PublicKey1:SetWndText(randomPubKey1)
    self.m_PublicKey2:SetWndText(randomPubKey2)
    self.m_PublicKey3:SetWndText(randomPubKey3)
end

function CSecretSecurityCardWnd:ClearSecretKeyInfo()
    self.m_CryptKey1:SetWndText("")    
    self.m_CryptKey2:SetWndText("")
    self.m_CryptKey3:SetWndText("")
end

function CSecretSecurityCardWnd:GetPublicAndSecretKey()
    local publicKey1    = self.m_PublicKey1:GetWndText()
    local publicKey2    = self.m_PublicKey2:GetWndText()
    local publicKey3    = self.m_PublicKey3:GetWndText()
    local publicKeyStr  = publicKey1 .. publicKey2 .. publicKey3
    
    local secretKey1    = self.m_CryptKey1:GetWndText()
    local secretKey2    = self.m_CryptKey2:GetWndText()
    local secretKey3    = self.m_CryptKey3:GetWndText()
    local secretKeyStr  =   secretKey1 .. secretKey2 .. secretKey3
    local Password_to_MD5 = g_Login.m_SecretSecurityCardWnd.m_strUserName .. " " .. secretKeyStr
	local PasswordMD5 = CreateCBufAcr( CreateCBuf(33) )
	self.m_MD5Digester = CMd5Digester:new()
	self.m_MD5Digester:Begin()
	self.m_MD5Digester:Append(Password_to_MD5, string.len(Password_to_MD5))
	self.m_MD5Digester:GetMD5Base16Str(PasswordMD5)
	secretKeyStr = PasswordMD5:ToString()
	return publicKeyStr, secretKeyStr
end

function CSecretSecurityCardWnd:OnDestroy()
    UnRegisterTick(self.m_CheckStayIdleTick)
    self.m_CheckStayIdleTick = nil
end
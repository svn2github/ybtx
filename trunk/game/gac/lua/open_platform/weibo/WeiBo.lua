CWeiBo = class(CWeiBoClient)

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

function CWeiBo:WeiBoConnect2Server()
	g_Login.m_LoginAccounts.m_WebBrowser:ShowWnd(false)
	g_WeiBoConnFlag = true
	g_Login.m_LoginAccounts:SetAddress()
	Connect2Server()
end

function CWeiBo:WeiBoRequestStart(DistStr)
	Gac2Gas:WeiBoRequestStart(g_Conn, g_App.m_iLoginKey, g_Conn:GetLocalPhysicalAddress(), DistStr)
end

function CWeiBo:OpenWebBrowser(token)
	local sUrl = "http://api.t.sina.com.cn/oauth/authorize?oauth_token=" .. token
	g_Login.m_LoginAccounts.m_WebBrowser:SetszSourceName(sUrl)
end

function CWeiBo:WeiBoLoginCheck(sCode)
	Gac2Gas:WeiBoLoginCheck(g_Conn, sCode)
end

function CWeiBo:WeiBoLoginFaild()
print("EGameState.eToLogin")
	if g_Conn and g_Conn.ShutDown then
		g_Conn:ShutDown()
	end
	coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
end
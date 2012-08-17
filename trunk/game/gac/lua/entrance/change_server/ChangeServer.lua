gac_require "framework/rpc/CScriptConnMgrClientHandler"
local TempServerInfo = {}

--=====================客户端的连接情况=========================================
function CScriptConnMgrClientHandler:OnConnected()
	--print("1In the OnConnected")
	Gac2Gas:SendIdToGas(g_Conn, gac_id)
	
	SaveLoginFlow("OnConnected")
	
	if TempServerInfo.m_ConnectOtherServer then
		local Key = TempServerInfo.m_sKey
		local ChangeType = TempServerInfo.m_ChangeType
		if ChangeType then
			SaveLoginFlow("OnConnected  CharEnterOtherServer")
			Gac2Gas:CharEnterOtherServer(g_Conn, ChangeType, Key, TempServerInfo.m_LastMsgStr)
		else
			SaveLoginFlow("OnConnected  CreateAccount")
			Gac2Gas:CreateAccount(g_Conn, Key)--(新创建角色时)先去目标服创建Account,然后回消息,让玩家进入
		end
		TempServerInfo.m_ConnectOtherServer = nil
	elseif g_YYConnFlag then
		g_Login.m_nConnStatus = 2
		if not g_App.m_iLoginKey then
			local sKey = tostring(os.time()+math.random(1,10000))
			g_App.m_iLoginKey = sKey
		end
		local DistStr = g_GetDistStr()
		Gac2Gas:YYRequest(g_Conn,g_App.m_iLoginKey,g_Conn:GetLocalPhysicalAddress(),DistStr)
		CYYCtrl.ReadYYLog("请求YYKey1")
	elseif g_WeiBoConnFlag then
		g_Login.m_nConnStatus = 2
		if not g_App.m_iLoginKey then
			local sKey = tostring(os.time()+math.random(1,10000))
			g_App.m_iLoginKey = sKey
		end
		local DistStr = g_GetDistStr()
		g_App.m_WeiBo:WeiBoRequestStart(DistStr)
	else
		g_Login.m_nConnStatus = 2
		
		if not g_App.m_iLoginKey then
			local sKey = tostring(os.time()+math.random(1,10000))
			g_App.m_iLoginKey = sKey
		end
		--CLoginAccounts.RetLoginConnectStepInfo(1156)
		CLoginAccounts.RetLoginConnectStepInfo(1153)
		local DistStr = g_GetDistStr()
		if g_Login.m_TrusteeFlag then
			Gac2Gas:CheckTrusteeUser(g_Conn,g_Login.m_LoginAccounts.m_strUserName,g_Login.m_LoginAccounts.m_strPasswordMD5, g_App.m_iLoginKey,g_Conn:GetLocalPhysicalAddress(),DistStr)
			g_Login.m_TrusteeFlag = nil
		else
			Gac2Gas:CheckUser( g_Conn,g_Login.m_LoginAccounts.m_strUserName,g_Login.m_LoginAccounts.m_strPasswordMD5, g_App.m_iLoginKey,g_Conn:GetLocalPhysicalAddress(),DistStr,g_Login.m_LoginAccounts.publicKeyStr or "",g_Login.m_LoginAccounts.secretKeyStr or "")
		end
	end
end

function CScriptConnMgrClientHandler:OnConnectFailed(pConn,eReason)
	--print("conn failed.....")
	if not TempServerInfo.m_ConnectOtherServer then
		g_Login.m_IsConnecting = false
	else
  	
  		if IsFunction(g_App.m_Loading.IsShow)
			and g_App.m_Loading:IsShow() then
			g_App.m_Loading:ShowWnd(false)
		end
  	
  		TempServerInfo.m_ConnectOtherServer = nil
  	end
  	
	if g_YYConnFlag then
		if g_Login.m_LoginAccounts then
			CYYCtrl.YYErrLogFun("无法连接到服务器 " .. "eReason" .. eReason)
			CYYCtrl.ReadYYLog("YY无法连接到服务器")
		end		
		CYYCtrl.YYLoginFailure("无法连接到服务器")
	end
	
	MessageBox(g_App:GetRootWnd(),MsgBoxMsg(7), MB_BtnOK)
	
	if g_GameMain then
		g_GameMain:ExitToState( EGameState.eToLogin )
	else
		coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
	end
	
end
	
function CScriptConnMgrClientHandler:OnDisconnect( pConn )
	--print("conn OnDisconnect.....")
	TempServerInfo.m_ConnectOtherServer = nil
  
	if pConn~=nil and pConn:IsShuttingDown() then
		return
	end
	
	StopApexClient()
	
	if CCoreSceneClient_Inst() ~= nil then
		g_CoreSceneMgr:DestroyMainScene()
	end
	
	g_WndMouse:ClearCursorSpecialState()
	
	if g_App.m_Loading ~= nil and g_App.m_Loading:IsShow() then
		g_App.m_Loading:ShowWnd(false)
	end
	
	g_App.m_MsgBox = MessageBox(g_App:GetRootWnd(),MsgBoxMsg(8), MB_BtnOK)
	
	if g_Login then
		CYYCtrl.YYErrLogFun("网络连接断开或者服务器停机")	
		CYYCtrl.YYLoginFailure("网络连接断开或者服务器停机")
	end
	
	if g_GameMain then
		g_GameMain:ExitToState( EGameState.eToLogin )
	else
		coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
	end
end
--===========================================================================

local function ShortConnEnterGame()

	--连接到GAS
	local addr = CAddress:new()
	addr:SetAddress(TempServerInfo.m_Ip)
	addr:SetPort(TempServerInfo.m_Port)
	
	g_Conn = ConnMgrClient:Connect(addr)
	assert(g_Conn)
	local OldShutDown = CConnClient.ShutDown
	function NewShutDown()
		StopApexClient()
		OldShutDown(g_Conn)
	end
	CConnClient.ShutDown = NewShutDown
	
end

--连接到另一台服务器
local function Connect2OtherServer()
	if g_ConnectOtherServerTick then
		UnRegisterTick(g_ConnectOtherServerTick)
		g_ConnectOtherServerTick = nil
	end
	SaveLoginFlow("Connect2OtherServer")
	TempServerInfo.m_ConnectOtherServer = true
	coroutine.resume(coroutine.create(ShortConnEnterGame))
	
end


local function TestConnect2OtherServer()
	SaveLoginFlow("TestConnect2OtherServer")
end

function Gas2Gac:ChangedInSameSceneSuc()
end

function Gas2Gac:RetEnterOtherServer(Conntion, sIp, Port, ChangeType, sKey, LastMsgStr)
	SaveLoginFlow("Gas2Gac:RetEnterOtherServer  " .. ChangeType)
	TempServerInfo = {}
	
	g_GameMain.m_IsTransfer = true
	
	if ChangeType == 1 then
		if g_App.m_Loading == nil then
			g_App.m_Loading = CLoading:new()
		end
		if IsFunction(g_App.m_Loading.IsShow)
			and not g_App.m_Loading:IsShow() then
			g_App.m_Loading:ShowWnd(true)
			g_GameMain:ShowWnd(false)
			g_App.m_Loading:SetPos(1)
		end
		g_App.m_Loading:SetRandomText()
		CRenderSystemClient_Inst():Refresh()
		CRenderSystemClient_Inst():Refresh()
	end
	
	--TempServerInfo.m_Conn = g_Conn
	local ret, msg = apcall(g_Conn.ShutDown, g_Conn)
	SaveLoginFlow("g_Conn:ShutDown()    " .. (msg or " "))
	TempServerInfo.m_Ip = sIp
	TempServerInfo.m_Port = Port
	TempServerInfo.m_ChangeType = ChangeType
	TempServerInfo.m_sKey = sKey
	TempServerInfo.m_LastMsgStr = LastMsgStr
	
	RegisterOnceTick(g_App,"ConnectServer",TestConnect2OtherServer,1000)
	
	g_ConnectOtherServerTick = RegisterTick("ConnectServer", Connect2OtherServer, 1000)
end

function Gas2Gac:ReturnStartApex( Connection )
	StartApexClient()
end

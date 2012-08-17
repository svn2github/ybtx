gac_gas_require "character/Character"
gac_require "framework/main_frame/GacEventTable"
gac_gas_require "event/Event"
engine_require "common/Misc/TypeCheck"
gac_require "world/GameScene"
gac_require "message/message_box/MsgBoxMsg"
gac_gas_require "framework/common/UserNameCheck"
gac_gas_require "framework/common/StringSplit"
gac_require "framework/main_frame/GameState" 

CLogin = class( CGameState )


function CLogin:GotRetCheckUserSuc(gameState)
	
	local function ChangeStateTick()
		if(self.Tick_ChangeState) then
			UnRegisterTick(self.Tick_ChangeState)
			self.Tick_ChangeState = nil
		end
		coroutine.resume( g_App.MainRoutine, gameState)
	end

	self.m_LoginAccounts:SetTransparentObj(1000, true)
	self.Tick_ChangeState = RegisterTick("ChangeStateTick", ChangeStateTick, 1000)
end

function CLogin.ReturnCheckUserByActivationCode( Pipe,randomPubKey1,randomPubKey2,randomPubKey3)
	local user_name 				= g_Login.m_LoginAccounts.m_strUserName
	local password 					= g_Login.m_LoginAccounts.m_strPasswordMD5
	local physical_address 	= g_Conn:GetLocalPhysicalAddress()
	g_Conn:ShutDown()
	coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
	SetEvent( Event.Test.LoginEnded,true, "LoginFailed" )
	g_Login.m_IsConnecting = true
	g_Login.m_SecretSecurityCardWnd = CSecretSecurityCardWnd:new()
	g_Login.m_SecretSecurityCardWnd.m_strUserName = user_name
	g_Login.m_SecretSecurityCardWnd.m_strPasswordMD5 = password
	g_Login.m_SecretSecurityCardWnd.m_PhysicalAddress = physical_address
	g_Login.m_SecretSecurityCardWnd:ShowWnd(true)
	g_Login.m_SecretSecurityCardWnd:SetSecretSecurityPublicKey(randomPubKey1,randomPubKey2,randomPubKey3)
	g_Login.m_SecretSecurityCardWnd.m_StayIdleTime = 1
	g_Login.m_SecretSecurityCardWnd.m_CheckStayIdleTick = RegisterTick("LoginTick",g_Login.m_SecretSecurityCardWnd.CheckStayIdleInSecretSecurityWnd,1000*60)        
end

function CLogin.ReturnCheckUser( Pipe,bFlag,uMsgID,userName)
	g_Login.m_IsConnecting = false 
	if bFlag then
		local GameState = (uMsgID == 6) and EGameState.eToAgreement or EGameState.eToSelectChar
		g_Login:GotRetCheckUserSuc(GameState)
		StartApexClient()
		SetEvent( Event.Test.LoginEnded,true, "LoginSuccess" )
		if IsCppBound(Pipe) then
			Pipe:SetUserName(userName)
		end
		CYYCtrl.YYLoginSucceed()
	else
		--log打印
		if  uMsgID == 3 then
			local ServerName = g_Login.m_LoginAccounts.m_Infos[2]
			local ParamTbl = g_Login.m_LoginAccounts.m_Infos[3]
			if ServerName == nil then
				ServerName = "无"
			end
			local Str = ""
			if ParamTbl and  ParamTbl["yy"] then
				Str = "YYType为:" .. ParamTbl["yy"]
			elseif ParamTbl and ParamTbl["yy"] == nil then
				Str = "YYType为:Nil"
			elseif ParamTbl == nil then
				Str = "ParamTbl为:Nil"
			end
			LogErr("YY专区登陆出错:","服务器名:" .. ServerName .. Str)
		end
		g_Conn:ShutDown()
		MessageBox(g_App:GetRootWnd(),MsgBoxMsg(uMsgID), MB_BtnOK)
		coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
		g_Login:SetFocus()
		CYYCtrl.YYErrLogFun("登陆失败!!" .. "MsgIDRet:" .. uMsgID)
		CYYCtrl.YYLoginFailure("登陆失败!!")
		SetEvent( Event.Test.LoginEnded,true, "LoginFailed" )
	end
end

function CLogin.ReturnCheckRCP( nRet)
	if(nRet == 0) then
		--print("RCP authorization successed")
		--SetEvent( Event.Test.RCPEnded, true, "RCPEndedSuccess" )
		--GAC把user,password发送给GAS		
		local DistStr = g_GetDistStr()
		
		local sMacAddress
		if not IsCppBound(g_Conn) then
			sMacAddress = ""
		else
			sMacAddress = g_Conn:GetLocalPhysicalAddress()
		end
		Gac2Gas:CheckUser( g_Conn,g_Login.m_LoginAccounts.m_strUserName,g_Login.m_LoginAccounts.m_strPasswordMD5,sMacAddress,DistStr,"","" )
	else
		--print("RCP authorization failure")
		--SetEvent( Event.Test.RCPEnded, true, "RCPEndedFailure" )
	end
end

--告诉客户有人在用他的号(玩家帐号在其它服的时候,会提示这个)
function CLogin.RetReSetLogin(IsShowMsg)
	local function SendLogin()
		local sMacAddress
		if not IsCppBound(g_Conn) then
			sMacAddress = ""
		else
			sMacAddress = g_Conn:GetLocalPhysicalAddress()
		end
		Gac2Gas:UserForceTipSigal(g_Conn,sMacAddress)
	end
	--g_Conn:ShutDown()
	local function ContinueLogin(Wnd, uButton)
		local bFlag = (uButton == MB_BtnOK)
		if bFlag then
			SendLogin()
			g_Login.m_LoginTick = RegisterTick("LoginTick",SendLogin,5000)
		else
			g_Conn:ShutDown()
			coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
			g_Login:SetFocus()
			SetEvent( Event.Test.LoginEnded,true, "LoginFailed" )
		end
		return true
	end
	
	if IsShowMsg then
		MessageBox(g_App:GetRootWnd(),MsgBoxMsg(10), BitOr( MB_BtnOK, MB_BtnCancel), ContinueLogin, g_App:GetRootWnd())
	else
		g_Login.m_LoginTick = RegisterTick("LoginTick",SendLogin,4000)
	end
end

function CLogin:OnInit()
	if g_App.m_CurMusicName and g_App.m_CurMusicName ~= "" then
		StopCue(g_App.m_CurMusicName)
	end

	if g_XiuXingTaLastMusic then
		StopCue(g_XiuXingTaLastMusic)
	end
	
	PlayCue("music20")
	g_App.m_CurMusicName = "music20"
	local param = WndCreateParam:new()
	param.szTitle = ""
	param.uStyle = 0x40000000
	param.width = 1024
	param.height = 768
	param:SetParentWnd(g_App:GetRootWnd())
	param.font_size = 12
	self:Create( param )
	self:SetLockFlag( BitOr( LOCK_LEFT, LOCK_RIGHT, LOCK_TOP, LOCK_BOTTOM ) )
	
	local Image		= WND_IMAGE_LIST:new()
	local Flag		= IMAGE_PARAM:new()
	Flag.CtrlMask	= SM_BS_BK
	Flag.StateMask	= IP_ENABLE
	self:SetWndBkImage( Flag, Image )
	self.m_nConnStatus = 0 --连接状态：0-没连接，1-连接RCP，2-连接GAS
	
	g_RootPath = CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/"
	g_App.m_WeiBo = CWeiBo:new()
	self:InitGUI()
	self.m_IsConnecting = false --是否正在连接服务端
	
	local bFlag = not g_CoreScene
	if(bFlag) then
		g_CoreSceneMgr:CreateMainScene("map/dljm")	--载入登陆界面的场景
	end
	self.m_LoginAccounts:Animation(bFlag)
end


function CLogin:InitGUI()
	self.m_LoginAccounts		= CLoginAccounts:new(self)
	self.m_LoginWaitingQueue	= CLoginWaitingQueue:new(self.m_LoginAccounts)
	self.m_WndCancelMsgbox		= CLoginCancelMsgBox:new(self)
	g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
	g_WndMouse:ClearCursorAll()
	CYYCtrl.InitYY(self)
end



function CLogin:OnLoading(fPercent)
end

function CLogin:ShowLoadingWnd()
	if g_App.m_Loading == nil then
		g_App.m_Loading = CLoading:new(g_App:GetRootWnd())
	else
		g_App.m_Loading:ShowWnd(true)
	end
	g_App.m_Loading:SetRandomText()
	g_App.m_Loading:SetPos(1)
	CRenderSystemClient_Inst():Refresh()
	CRenderSystemClient_Inst():Refresh()
end

function CLogin:OnMainSceneCreated(CoreScene)
	g_CoreScene = CoreScene
	g_CoreSceneMgr:RefreshCameraDest(4, 64, 64)
	local RenderScene = g_CoreScene:GetRenderScene()
	RenderScene:PlayCameraPath(0)
	RenderScene:InitSceneRes()
end

function CLogin:OnDestroyMainScene()
end

function CLogin:OnMainSceneDestroyed()
	g_CoreScene = nil
end

function CLogin:LoginUnRegisterTick()
	if self.m_LoginWaitingQueue.m_LoadTime then
		UnRegisterTick(self.m_LoginWaitingQueue.m_LoadTime)
		self.m_LoginWaitingQueue.m_LoadTime = nil
	end
	if(self.Tick_ChangeState) then
		UnRegisterTick(self.Tick_ChangeState)
		self.Tick_ChangeState = nil
	end
end

function CLogin:OnExit()
	self:LoginUnRegisterTick()
	self:DestroyWnd()
	
	CYYManager_DestroyYYMgr()
	if self.m_YY then
		self.m_YY:DestroyWnd()
	end
end

function CLogin:OnLButtonDown(nFlags, x, y)
	if( self.m_LoginAccounts and not self.m_LoginAccounts:IsShow() ) then
		self.m_LoginAccounts:Animation(false)
	end
end

function CLogin:main()
	SetEvent( Event.Test.LoginBegan, true, "Login begin" )
	self:OnInit()
	local re = self:WaitMessage()
	self:OnExit()
	return re
end
--------------------------------------------------------------------------------
function g_GetDistStr()
	local strFileName = g_RootPath .. "etc/gac/dist.txt"
	local f = CLuaIO_Open(strFileName, "r")
	local text = ""
	if(f ~= nil) then
		text = f:ReadAll()
		f:Close()
	end
	return text
end

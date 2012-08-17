gac_require "framework/main_frame/MainFrameInc"
gac_require "framework/main_frame/GameDef"
gac_require "framework/rpc/CScriptConnMgrClientHandler"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_require "information/tooltips/ToolTips"
gac_require "framework/main_frame/ConnClient"

EGameState.eExitGame			= 0
EGameState.eToSelectChar		= 1
EGameState.eToLogin				= 2
EGameState.eToGameMain			= 3
EGameState.eToGetBackRole		= 4
EGameState.eToNewRoleGameMain	= 5
EGameState.eToAgreement			= 6

function CGameState:InitMessageBox()
	RegistMsgBox( 0, g_App:GetRootWnd(), "WndMsg", "WndInfo", "BtnOK", "BtnCancel" );
end

function CGameState:WaitMessage()
	return coroutine.yield()
end

function CGameState:Init()
	--load rpc的协议表
	ConnMgrClient = g_App:GetConnMgr()

	g_ClientHander = CConnMgrClientHandler:new()
	
	
	ScriptConnMgrClientHandler = CScriptConnMgrClientHandler:new()
	ScriptConnMgrClientHandler:RegistCall("gac_gas", "framework/rpc/Gac2GasProList","CConnClient")
	ScriptConnMgrClientHandler:RegistDef("gac_gas", "framework/rpc/Gas2GacProList","CConnClient")
	g_ClientHander:SetScriptHandler(ScriptConnMgrClientHandler)
		
	
	
	ConnMgrClient:SetHandler(g_ClientHander)
	
	--client 的连接
	g_Conn = {}
	
	-- 配置表全局对象
	g_WndMouse = CreateMouseHook()
	SQRWnd_RegisterHook( g_WndMouse, WM_MOUSEMOVE )
	SQRWnd_RegisterHook( g_WndMouse, WM_MOUSELEAVE )
	SQRWnd_RegisterHook( g_WndMouse, WM_LBUTTONUP )
	SQRWnd_RegisterHook( g_WndMouse, WM_RBUTTONUP )
	g_WndMouse:RegisterKey()
	
	--重写ShutDown函数。在断开连接时发消息给Apex
	local OldShutDown = CConnClient.ShutDown
	function NewShutDown(conn)
		StopApexClient()
		OldShutDown(conn)
	end
	CConnClient.ShutDown = NewShutDown
end

function CGameState:End()
	g_WndMouse:UnRegisterKey()
	SQRWnd_UnRegisterHook( g_WndMouse )
	g_ClientHander:SetScriptHandler(nil)
	ConnMgrClient:SetHandler(nil)
end

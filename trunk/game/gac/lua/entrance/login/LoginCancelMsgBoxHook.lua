CLoginCancelMsgBoxHook = class( IHook )

function CLoginCancelMsgBoxHook:OnHook( Wnd, Msg, wParam, lParam )
	if wParam == VK_ESCAPE then
		if(g_Login.m_nConnStatus == 1) then
			g_Login.m_Socket:ShutDown()
			g_Login.m_nConnStatus = 0
		elseif(g_Login.m_nConnStatus == 2) then
			g_Conn:ShutDown()
			g_Login.m_nConnStatus = 0
		else
			if g_Conn and g_Conn.ShutDown then
				g_Conn:ShutDown()
			end
		end
		SQRWnd_UnRegisterHook(g_Login.m_WndCancelMsgbox.m_KeyHook)
		g_Login.m_WndCancelMsgbox:ShowWnd(false)
		g_Login.m_IsConnecting = false
		coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
	else
		return true
	end
	--returen false 表示hook直接截取了消息
	return false
end
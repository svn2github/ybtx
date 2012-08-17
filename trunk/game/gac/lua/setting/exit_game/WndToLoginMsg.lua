WndToLoginMsg 	= class( SQRDialog )

--创建用于返回角色选择界面和登陆界面的通用倒计时对话框
local function CreateToLoginMsgBox(wnd)
	wnd.m_CountDownNum = 15
	wnd:CreateFromRes("WndMsgCancel", g_GameMain)
	wnd:ShowWnd( true )
	wnd.m_WndInfo	= wnd:GetDlgChild("WndInfo")
	wnd.m_BtnCancel	= wnd:GetDlgChild("BtnCancel")
	wnd.m_WndInfo:SetWndText(GetStaticTextClient(g_GameMain.m_ExitGame.m_MsgID))
	wnd.m_BtnCancel:SetWndText(GetStaticTextClient(1025))
	return wnd
end

function WndToLoginMsg:Ctor()
    CreateToLoginMsgBox(self)
end

function WndToLoginMsg:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_BtnCancel) then
			self:Destroy()
		end
	end
end

function WndToLoginMsg:Destroy()
	UnRegisterTick(self.m_CountDownTick)
	self.m_CountDownTick = nil
	self:DestroyWnd()
	if g_GameMain and g_GameMain.m_ToLoginMsgBox then
		g_GameMain.m_ToLoginMsgBox = nil
	end
	Gac2Gas:CancelReadLeftTime(g_Conn)
	
	if(self.CancelAndRunCallBack) then
		self:CancelAndRunCallBack()
		self.CancelAndRunCallBack = nil
	end
end

function WndToLoginMsg:ReturnToCharSelectWndTick()
	
	local distroy_account = true
	local leftTime = g_GameMain.m_ToLoginMsgBox.m_CountDownNum - 1
	g_GameMain.m_ToLoginMsgBox.m_CountDownNum = leftTime

	if leftTime > 0 then
		self.m_WndInfo:SetWndText(GetStaticTextClient(20001,leftTime))
	else
		UnRegisterTick(g_GameMain.m_ToLoginMsgBox.m_CountDownTick)
		g_GameMain.m_ToLoginMsgBox.m_CountDownTick = nil
--		Gac2Gas:Exit2CharList(g_Conn)	
	end
end

function WndToLoginMsg:ReturnToLoginCountDownTick()
	
	local distroy_account = true
	local leftTime = g_GameMain.m_ToLoginMsgBox.m_CountDownNum - 1
	g_GameMain.m_ToLoginMsgBox.m_CountDownNum = leftTime
	
	if leftTime > 0 then
		self.m_WndInfo:SetWndText(GetStaticTextClient(20002,leftTime))
	else
		UnRegisterTick(g_GameMain.m_ToLoginMsgBox.m_CountDownTick)
		g_GameMain.m_ToLoginMsgBox.m_CountDownTick = nil
		if g_Conn and g_Conn.ShutDown then
		    g_Conn:ShutDown()
		end
		if g_GameMain == nil then
			SetEvent(Event.Test.BackToLoginFinished, true)
			coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
		else
			g_GameMain:ExitToState( EGameState.eToLogin )
		end
	end
end

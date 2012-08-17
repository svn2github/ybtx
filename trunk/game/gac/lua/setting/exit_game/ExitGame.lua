gac_gas_require "framework/common/LogoutSetting"

CExitGame		= class( SQRDialog )

function CExitGame.RetCancelLeaveGame()
    if g_GameMain.m_ToLoginMsgBox then
        if  g_GameMain.m_ToLoginMsgBox.m_CountDownTick then
            UnRegisterTick(g_GameMain.m_ToLoginMsgBox.m_CountDownTick)
            g_GameMain.m_ToLoginMsgBox.m_CountDownTick = nil
        end

        g_GameMain.m_ToLoginMsgBox:DestroyWnd()
        g_GameMain.m_ToLoginMsgBox = nil
    end
    if g_GameMain.m_ExitGameMsgBox then
        if g_GameMain.m_ExitGameMsgBox.m_CountDownTick then
            UnRegisterTick(g_GameMain.m_ExitGameMsgBox.m_CountDownTick)
            g_GameMain.m_ExitGameMsgBox.m_CountDownTick = nil
        end
        g_GameMain.m_ExitGameMsgBox:DestroyWnd()
        g_GameMain.m_ExitGameMsgBox = nil
    end
end

function CExitGame.ReturnNotifyClientLeave(uToState)--可以下线
	g_WndMouse:ClearCursorSpecialState()
	if g_GameMain == nil then
		return
	end
	if (g_GameMain.m_CreateChatWnd ~= nil and g_GameMain.m_CreateChatWnd.m_ChatSendArea ~= nil) then
		g_GameMain.m_CreateChatWnd.m_ChatSendArea:ShowWnd(false)
		g_GameMain.m_CreateChatWnd.m_ChatSendArea.bIsShow = false
	end
	if uToState == EGameState.eExitGame then	--退出游戏
		g_GameMain.m_ExitGame:ExitGame()
		
	elseif uToState == EGameState.eToLogin then --回登陆界面
		g_GameMain.m_ExitGame.m_MsgID = 20003
		g_GameMain.m_ExitGame:ToLoginWnd(EGameState.eToLogin)
	
	elseif uToState == EGameState.eToSelectChar then --回角色选择界面
	    g_GameMain.m_ExitGame.m_MsgID = 20004
		g_GameMain.m_ExitGame:ToLoginWnd(EGameState.eToSelectChar)
		
	end
end

function CExitGame.ChangeToStateNow( eToState)
	if g_GameMain == nil then
		return
	end
	if eToState == EGameState.eToSelectChar then --回角色选择界面
		if g_App.m_CurMusicName and g_App.m_CurMusicName ~= "" then
			StopCue(g_App.m_CurMusicName)
		end
		if g_XiuXingTaLastMusic then
			--print("关闭修行塔音乐------回角色选择界面")
			StopBackgroundMusic(nil, g_XiuXingTaLastMusic, nil)
		end
		PlayCue("music24")
		g_App.m_CurMusicName = "music24"
		g_GameMain:ExitToState( EGameState.eToSelectChar )
	end
end

function CExitGame.ReceiveAntiIndulgenceExitGame()
	g_GameMain.m_ExitGame:ExitGameAntiIndu()
end

--Exit窗体--------------------------------------------------------------------------------

function CExitGame:Ctor()
	self:CreateFromRes( "ExitGame", g_GameMain )
end

function CExitGame:OnChildCreated()
	self.m_Exit				= self:GetDlgChild("Exit")
	self.m_ReturnLogin		= self:GetDlgChild("ReturnLogin")
	self.m_Setting			= self:GetDlgChild("Setting")
	self.m_ReturnSelRole	= self:GetDlgChild("ReturnSelRole")
end

function CExitGame:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	
	local countdown = g_CountDownTime["Delay"]
	if g_Conn.m_LogoutSetting then
		countdown = g_CountDownTime[g_Conn.m_LogoutSetting]
	end
	
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Exit) then
			Gac2Gas:NotifyClientLeave(g_Conn, countdown, EGameState.eExitGame)	--我要下线
			-- 别下了，给你弹个窗口吧
			if IsCppBound(g_MainPlayer) and g_GameMain.m_FbActionPanel then
				local PlayerLevel = g_MainPlayer:CppGetLevel()
				if PlayerLevel >= 15 then
					g_GameMain.m_FbActionPanel:PopupActionPanel("Exit")
				end
			end
		elseif(Child == self.m_ReturnLogin) then
			Gac2Gas:NotifyClientLeave(g_Conn, countdown, EGameState.eToLogin)	--我要回登陆界面
		elseif(Child == self.m_Setting) then
			self:ShowWnd( false )
			g_GameMain.m_SysSetting:OpenPanel()
		elseif(Child == self.m_ReturnSelRole) then
			Gac2Gas:NotifyToCharSelect(g_Conn,countdown)						--我要回角色选择界面
		else
			self:ShowWnd( false )
		end
	end
end

function CExitGame:ToLoginWnd(uToState)
	if(g_GameMain.m_ToLoginMsgBox) then return end
	g_GameMain.m_ToLoginMsgBox = WndToLoginMsg:new()
	if uToState == EGameState.eToSelectChar then
		g_GameMain.m_ToLoginMsgBox.m_CountDownTick = RegClassTick("ReturnToCharSelectTick", g_GameMain.m_ToLoginMsgBox.ReturnToCharSelectWndTick, 1000, g_GameMain.m_ToLoginMsgBox)
	else
		g_GameMain.m_ToLoginMsgBox.m_CountDownTick = RegClassTick("ReturnToLoginTick", g_GameMain.m_ToLoginMsgBox.ReturnToLoginCountDownTick, 1000, g_GameMain.m_ToLoginMsgBox)
	end
	self:ShowWnd(false)
end

function CExitGame:ExitGame()
	if g_GameMain ~= nil then
		if( g_GameMain.m_ExitGameMsgBox or g_GameMain.m_ExitGameMsgBoxAntiIndu) then
			return
		end
		g_GameMain.m_ExitGameMsgBox = WndExitMSG:new()
		g_GameMain.m_ExitGameMsgBox.m_CountDownTick = RegClassTick("ExitGameTick", g_GameMain.m_ExitGameMsgBox.ExitGameCountDownTick, 1000, g_GameMain.m_ExitGameMsgBox)
		self:ShowWnd(false)
	end
end

function CExitGame:ExitGameAntiIndu()
	if g_GameMain ~= nil then
		if(g_GameMain.m_ExitGameMsgBoxAntiIndu) then
			return
		end
		g_GameMain.m_ExitGameMsgBoxAntiIndu = CWndExitMSGAntiIndu:new()
		g_GameMain.m_ExitGameMsgBoxAntiIndu.m_CountDownTick = RegClassTick("ExitGameAntiInduTick", g_GameMain.m_ExitGameMsgBoxAntiIndu.ExitGameCountDownTick, 1000, g_GameMain.m_ExitGameMsgBoxAntiIndu)
	end
end
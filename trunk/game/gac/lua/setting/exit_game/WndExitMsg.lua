WndExitMSG		= class( SQRDialog )

--创建退出游戏的倒计时对话框
function WndExitMSG:Ctor()
	self.m_CountDownNum = 15
	self:CreateFromRes("ExitGameMsgBox", g_GameMain)
	self:ShowWnd(true)
end

function WndExitMSG:OnChildCreated()
	self.m_DescriptionRT	= self:GetDlgChild("DescriptionRT")
	self.m_BtnOK			= self:GetDlgChild("Btn_Ok")
	self.m_BtnCancel		= self:GetDlgChild("Btn_Back")
	self.m_BtnOther			= self:GetDlgChild("Btn_Other")
	self.m_DescriptionRT:SetWndText(GetStaticTextClient(1105, self.m_CountDownNum))
	self.m_BtnOK:SetWndText(GetStaticTextClient(1106))
	self.m_BtnCancel:SetWndText(GetStaticTextClient(1107))
	
end


function WndExitMSG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_BtnOK) then 
			--Gac2Gas:ClientLogout(g_Conn, EGameState.eExitGame, g_GameMain.m_ExitGameMsgBox.m_CountDownNum)--那我下线了？
			g_GameMain:ExitToState( EGameState.eExitGame )
		elseif( Child == self.m_BtnCancel) then
			self:EscExitGame()
		elseif(Child == self.m_BtnOther) then
			RunUrl(g_UrlMgr:GetUrl(1))
			g_GameMain:ExitToState( EGameState.eExitGame )
		end
	end
end

function WndExitMSG:EscExitGame()
	UnRegisterTick(self.m_CountDownTick)
	self.m_CountDownTick = nil
	self:DestroyWnd()
	g_GameMain.m_ExitGameMsgBox = nil
	Gac2Gas:CancelReadLeftTime(g_Conn)
end

function WndExitMSG:ExitGameCountDownTick()
	self.m_CountDownNum = self.m_CountDownNum - 1
	
	if self.m_CountDownNum > 0 then
		self.m_DescriptionRT:SetWndText(GetStaticTextClient(1105, self.m_CountDownNum))
	else
		UnRegisterTick(g_GameMain.m_ExitGameMsgBox.m_CountDownTick)
		g_GameMain.m_ExitGameMsgBox.m_CountDownTick = nil
		g_GameMain:ExitToState( EGameState.eExitGame )
	end
end
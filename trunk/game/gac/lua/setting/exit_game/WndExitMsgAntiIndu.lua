CWndExitMSGAntiIndu = class( SQRDialog )

--创建退出游戏的倒计时对话框
function CWndExitMSGAntiIndu:Ctor()
	self.m_CountDownNum = 15
	self:CreateFromRes("ExitGameMsgBoxAntiIndu", g_GameMain)
	self:ShowWnd(true)
	self:SetFocus()
end

function CWndExitMSGAntiIndu:OnChildCreated()
	self.m_DescriptionRT	= self:GetDlgChild("DescriptionRT")
	self.m_BtnOK			= self:GetDlgChild("Btn_Ok")
	self.m_BtnOther			= self:GetDlgChild("Btn_Other")
	self.m_DescriptionRT:SetWndText(GetStaticTextClient(1301, self.m_CountDownNum))
	self.m_BtnOK:SetWndText(GetStaticTextClient(1106))
	self.m_BtnOther:SetWndText(GetStaticTextClient(1302))
end

function CWndExitMSGAntiIndu:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_BtnOK) then 
			g_GameMain:ExitToState( EGameState.eExitGame )
		elseif(Child == self.m_BtnOther) then
			if(1 == g_GameID) then --混服yy号
				RunUrl(g_UrlMgr:GetUrl(3))
			elseif(2 == g_GameID) then --yy专区
				RunUrl(g_UrlMgr:GetUrl(3))
			else
				RunUrl(g_UrlMgr:GetUrl(2))
			end
			if(self.m_CountDownTick) then
				UnRegisterTick(self.m_CountDownTick)
				self.m_CountDownTick = nil
			end
			g_GameMain:ExitToState( EGameState.eExitGame )
		end
	end
end

function CWndExitMSGAntiIndu:ExitGameCountDownTick()
	self.m_CountDownNum = self.m_CountDownNum - 1
	
	if self.m_CountDownNum > 0 then
		self.m_DescriptionRT:SetWndText(GetStaticTextClient(1301, self.m_CountDownNum))
	else
		if(self.m_CountDownTick) then
			UnRegisterTick(self.m_CountDownTick)
			self.m_CountDownTick = nil
		end
		g_GameMain:ExitToState( EGameState.eExitGame )
	end
end

function CWndExitMSGAntiIndu:OnDestroy()
	if(self.m_CountDownTick) then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
end
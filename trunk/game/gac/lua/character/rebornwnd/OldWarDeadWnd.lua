COldWarDeadWnd = class(SQRDialog)

function COldWarDeadWnd:Ctor()
	self:CreateFromRes("OldWarDeadMsgWnd", g_GameMain)
	self.m_WndInfo			 	= self:GetDlgChild("Info")
	self.m_BtnOk					= self:GetDlgChild("Ok")
	self.m_BtnCancel 			= self:GetDlgChild("Cancel")	
end

function COldWarDeadWnd:GetWnd()
	local Wnd = g_GameMain.m_OldWarDeadWnd
	if not Wnd then
		Wnd = COldWarDeadWnd:new(g_GameMain)
		g_GameMain.m_OldWarDeadWnd = Wnd
	end
	return Wnd
end

function COldWarDeadWnd:IsToLogin(LiveNum)
	if ( g_GameMain.m_ToLoginMsgBox and g_GameMain.m_ToLoginMsgBox.m_CountDownTick ) then
		local CallBackToCharDeadWnd = function()
			self:ShowDeadWnd(LiveNum)
		end
		g_GameMain.m_ToLoginMsgBox.CancelAndRunCallBack = CallBackToCharDeadWnd
		return true
	end
	return false
end

function COldWarDeadWnd:ShowDeadWnd(LiveNum)
	--1秒后打开死亡窗口
	if( self:IsToLogin(LiveNum) ) then
		return
	end
	local function CharDeadTick()
		if( self:IsToLogin(LiveNum) ) then
			return
		end
		
		local Times = 2 - LiveNum
		if Times <= 0 then
			self.m_BtnOk:EnableWnd(false)
			self.m_WndInfo:SetWndText(GetStaticTextClient(11018, 0))
		else
			self.m_BtnOk:EnableWnd(true)
			self.m_WndInfo:SetWndText(GetStaticTextClient(11018, Times))
		end
		
		g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
		self:ShowWnd(true)
	end
	RegisterOnceTick(g_App, "OldWarDeadWndTick", CharDeadTick, 1000)
end

function COldWarDeadWnd:HideWnd()
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end

function COldWarDeadWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnCancel == Child then
			Gac2Gas:GoLastMasterSceneReborn(g_Conn)
		elseif self.m_BtnOk == Child then
			Gac2Gas:GoBackOrStay(g_Conn, true)
		end
	end
end

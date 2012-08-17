
CTradeInvitationWnd = class ( SQRDialog ) 

-------------------------------------------------------------------------------------------
--交易邀请窗口
function CTradeInvitationWnd:Ctor()
	self:CreateFromRes( "PlayerDealingRequstWnd", g_GameMain )
    self.m_Close		=	self:GetDlgChild("Close")
	self.m_DenyBtn		=	self:GetDlgChild("DenyBtn")
	self.m_AcceptBtn	=	self:GetDlgChild("AcceptBtn")
end


--交易邀请窗口事件响应函数
function CTradeInvitationWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Close then
			g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
			g_GameMain.m_InviteTradeWnd = nil
			Gac2Gas:DenyedTrade( g_Conn, g_GameMain.m_PlayersTradeWnd.OtherPlayerID ) 

		elseif Child == self.m_DenyBtn then
			g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
			g_GameMain.m_InviteTradeWnd = nil
			Gac2Gas:DenyedTrade( g_Conn, g_GameMain.m_PlayersTradeWnd.OtherPlayerID ) 

		elseif Child == self.m_AcceptBtn then
			g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
			Gac2Gas:AgreeToTrade( g_Conn, g_GameMain.m_PlayersTradeWnd.OtherPlayerID )
		end	
	end	
end
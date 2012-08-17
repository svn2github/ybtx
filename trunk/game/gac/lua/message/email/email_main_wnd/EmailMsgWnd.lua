CWndEmailMSG = class(SQRDialog) --导出类

function CWndEmailMSG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )           --信息提示确认事件
	if (uMsgID == BUTTON_LCLICK) then
		if( Child == self:GetDlgChild( "Btn_Ok" ) ) then
			g_GameMain.m_EmailBox:DelMail()
			self:ShowWnd(false)
		else
			self:ShowWnd(false)
			return
		end
	end	
end

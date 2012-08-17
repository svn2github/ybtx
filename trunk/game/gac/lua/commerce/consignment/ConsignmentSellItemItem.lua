CConsignmentSellItemItemWnd = class ( SQRDialog)

--点击订单item上的X（取消订单）按钮，事件响应函数
function CConsignmentSellItemItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self:GetDlgChild("DelOrderBtn") then
            local wnd = g_GameMain.m_CSMSellCharOrderWnd
            local index = Child.Index
			local orderID =  wnd.m_CSMOrderInfoTbl[index].orderID
			wnd:VirtualExcludeWndClosed()
			local function callback (Context, Button)
				if(Button == MB_BtnOK) then
				    wnd.m_CSMTipsWnd:ShowWnd(true)
                    wnd.m_CSMTipsWnd:SetWndText(GetStaticTextClient(1161))
					Gac2Gas:CSMCancelSellOrder( g_Conn, orderID )
				end
				return true
			end
			self.m_MsgBox = MessageBox(self, MsgBoxMsg(155), BitOr( MB_BtnOK, MB_BtnCancel),callback)
        end
    end
end
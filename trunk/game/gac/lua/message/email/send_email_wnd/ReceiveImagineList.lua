
CReceiveImagineList = class(SQRDialog) 

--用成员变量存储收件人列表窗口中的子控件
function CReceiveImagineList:InitReceiveImagineListChild()
	self.m_friendslist			=	self:GetDlgChild("friendslist")
end


function CReceiveImagineList:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == ITEM_LBUTTONCLICK)then
		local index = self.m_friendslist:GetSelectItem( -1 )
		local wnd = self.m_friendslist:GetSubItem(index,0)
		g_GameMain.m_SendBox.m_ReceiverName:SetWndText(wnd:GetWndText())
		g_GameMain.m_SendBox.m_ImagineList:ShowWnd(false)
		g_GameMain.m_SendBox.m_ImagineList.m_friendslist:DeleteAllItem()
	end
end
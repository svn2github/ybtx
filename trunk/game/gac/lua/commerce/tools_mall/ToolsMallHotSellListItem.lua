CToolsMallHotSellListItemWnd = class (SQRDialog)

--热门看板中信息框itemWnd的事件响应函数
function CToolsMallHotSellListItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if (uMsgID == BUTTON_LCLICK) then
		if (Child == self:GetDlgChild("Buy")) then
		    CToolsMallCommonWnd.CloseToolsMallGiveItemWnd()
        	g_GameMain.m_ToolsMallBuyWnd:ShowWnd(true)
        	local itemType, itemName = g_GameMain.m_ToolsMallWnd:GetChoosedBuyItemInfo(CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InHotLookWnd"], self.Index)  
        	g_GameMain.m_ToolsMallBuyWnd:SetBuyOrGiveItemInfo(itemType, itemName)

        elseif(Child == self:GetDlgChild("Give")) then
            CToolsMallCommonWnd.CloseToolsMallBuyItemWnd()
        	g_GameMain.m_ToolsMallGiveItemWnd:ShowWnd(true)
        	local itemType, itemName = g_GameMain.m_ToolsMallWnd:GetChoosedBuyItemInfo(CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InHotLookWnd"], self.Index)  
        	g_GameMain.m_ToolsMallGiveItemWnd:GiveItemOrRequestOthersFunc(itemType, itemName)

    	elseif(Child == self:GetDlgChild("BigIconWnd"))then
            local itemType, itemName = g_GameMain.m_ToolsMallWnd:GetChoosedBuyItemInfo(CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InHotLookWnd"], self.Index)  
            g_GameMain.m_ToolsMallWnd:Preview(itemType, itemName)
        end
    end
end
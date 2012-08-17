CToolsMallSellListItemWnd = class (SQRDialog)

--陈列物品信息框itemWnd的事件响应函数
function CToolsMallSellListItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if (uMsgID == BUTTON_LCLICK) then
        if (Child == self:GetDlgChild("Buy")) then
            CToolsMallCommonWnd.CloseToolsMallGiveItemWnd()
        	g_GameMain.m_ToolsMallBuyWnd:ShowWnd(true)
        	local itemType, itemName = g_GameMain.m_ToolsMallWnd:GetChoosedBuyItemInfo(CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InMainWnd"], self.Index)  
        	g_GameMain.m_ToolsMallBuyWnd:SetBuyOrGiveItemInfo(itemType, itemName)
        
        elseif(Child == self:GetDlgChild("Give")) then
            CToolsMallCommonWnd.CloseToolsMallBuyItemWnd()
        	g_GameMain.m_ToolsMallGiveItemWnd:ShowWnd(true)
        	local itemType, itemName = g_GameMain.m_ToolsMallWnd:GetChoosedBuyItemInfo(CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InMainWnd"], self.Index)  
        	g_GameMain.m_ToolsMallGiveItemWnd:GiveItemOrRequestOthersFunc(itemType, itemName)
        
        elseif(Child == self:GetDlgChild("BigIconWnd"))then
        	local TabType = g_GameMain.m_ToolsMallWnd.m_CurChoosedTabType
        	if ( 2 == TabType)or(3 == TabType)or(5 == TabType) then
        		local itemType, itemName = g_GameMain.m_ToolsMallWnd:GetChoosedBuyItemInfo(CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InMainWnd"], self.Index)  
        		g_GameMain.m_ToolsMallWnd:Preview(itemType, itemName)
        	end
        end
    end
end

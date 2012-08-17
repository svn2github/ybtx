CConsignmentSellListItemWnd = class ( SQRDialog )

function CConsignmentSellListItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self:GetDlgChild("CSMItemChkBtn") then
            g_GameMain.m_CSMSellOrderWnd.m_CSMOrderItemWndTbl.m_ChoosedItemBtn = Child
            local clickedItemIndex = Child.Index 
            if clickedItemIndex == nil or
                g_GameMain.m_CSMSellOrderWnd.m_PreChoosedItemChkBtn == Child then
               return 
            end
            
            local sellerID = g_GameMain.m_CSMSellOrderWnd.m_CSMOrderInfoTbl[clickedItemIndex].seller
    		if sellerID ~= g_MainPlayer.m_Properties:GetCharName() then
    			g_GameMain.m_CSMSellOrderWnd.m_BuyItemBtn:EnableWnd( true )
    		else
    			g_GameMain.m_CSMSellOrderWnd.m_BuyItemBtn:EnableWnd( false )
    		end
    		if g_GameMain.m_CSMSellOrderWnd.m_PreChoosedItemChkBtn ~= nil then
    		    g_GameMain.m_CSMSellOrderWnd.m_PreChoosedItemChkBtn:SetCheck(false)
    		end
    		g_GameMain.m_CSMSellOrderWnd.m_PreChoosedItemChkBtn = Child
    	end
    end
end
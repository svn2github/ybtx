CBoughtCouponsItemWnd = class(SQRDialog)

function CBoughtCouponsItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then       
        if Child == self:GetDlgChild("ChkBtn") then
            if Child.Index == nil or 
                (Child == g_GameMain.m_BuyCouponsWnd.m_CurChoosedBoughtCouponsChkBtn 
                and g_GameMain.m_BoughtCouponsDetailWnd and 
                g_GameMain.m_BoughtCouponsDetailWnd:IsShow())then
                return      
            end
            g_GameMain.m_BuyCouponsWnd.m_PreChoosedBoughtCouponsChkBtn = g_GameMain.m_BuyCouponsWnd.m_CurChoosedBoughtCouponsChkBtn
            g_GameMain.m_BuyCouponsWnd.m_CurChoosedBoughtCouponsChkBtn = Child
            if g_GameMain.m_BuyCouponsWnd.m_PreChoosedBoughtCouponsChkBtn then
                g_GameMain.m_BuyCouponsWnd.m_PreChoosedBoughtCouponsChkBtn:SetCheck(false)
            end
            g_GameMain.m_BuyCouponsWnd.m_CurChoosedBoughtCouponsChkBtn:SetCheck(true)
            if g_GameMain.m_BoughtCouponsDetailWnd == nil then
                g_GameMain.m_BoughtCouponsDetailWnd  = CBoughtCouponsDetailWnd:new()
            end
            g_GameMain.m_BoughtCouponsDetailWnd:ShowWnd(true)
            g_GameMain.m_BoughtCouponsDetailWnd:SetBoughtCouponsDetail(Child.Index)
        end
    end
end

function CBoughtCouponsItemWnd:SetBoughtCouponsItem(couponsNode)
    local itemIcon = self:GetDlgChild("ItemIcon")
    local smallIcon= couponsNode["SmallIcon"]
    g_LoadIconFromRes(smallIcon, itemIcon, -1, IP_ENABLE, IP_CLICKDOWN)
    
    local couponsID = couponsNode["ID"]

    local itemName =  couponsNode["Name"]
    local itemNameWnd = self:GetDlgChild("ItemName")
    
    itemNameWnd:SetWndText(itemName)
    
end
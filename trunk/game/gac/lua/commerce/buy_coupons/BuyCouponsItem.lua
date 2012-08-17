CBuyCouponsItemWnd  =   class(SQRDialog)

function CBuyCouponsItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self:GetDlgChild("ChkBtn") then
            if Child.Index == nil or 
                (Child == g_GameMain.m_BuyCouponsWnd.m_CurChoosedCouponsChkBtn and
                g_GameMain.m_CouponsDetailWnd and g_GameMain.m_CouponsDetailWnd:IsShow())then
                return      
            end
            g_GameMain.m_BuyCouponsWnd.m_PreChoosedCouponsChkBtn = g_GameMain.m_BuyCouponsWnd.m_CurChoosedCouponsChkBtn
            g_GameMain.m_BuyCouponsWnd.m_CurChoosedCouponsChkBtn = Child
            if g_GameMain.m_BuyCouponsWnd.m_PreChoosedCouponsChkBtn then
                g_GameMain.m_BuyCouponsWnd.m_PreChoosedCouponsChkBtn:SetCheck(false)
            end
            g_GameMain.m_BuyCouponsWnd.m_CurChoosedCouponsChkBtn:SetCheck(true)
            if g_GameMain.m_CouponsDetailWnd == nil then
                g_GameMain.m_CouponsDetailWnd  = CCouponsDetailWnd:new()
            end
            g_GameMain.m_CouponsDetailWnd:SetCouponsDetail(Child.Index)
            g_GameMain.m_CouponsDetailWnd:ShowWnd(true)
        end 
    end
end

function CBuyCouponsItemWnd:SetCouponsInfo(couponsNode)
    local iconBtn = self:GetDlgChild("IconWnd")
    local itemNameBtn = self:GetDlgChild("ItemName")
    local moneyBtn = self:GetDlgChild("MoneyWnd")
    
    local smallIcon = couponsNode["SmallIcon"]
    g_LoadIconFromRes(smallIcon, iconBtn, -1, IP_ENABLE, IP_CLICKDOWN)
    
    local name = couponsNode["Name"]
    itemNameBtn:SetWndText(name)
    
    local price = couponsNode["Price"]
    local moneyMgr = CMoney:new()
    
    local priceStr = moneyMgr:ChangeMoneyToString(price, EGoldType.GoldCoin )
    moneyBtn:SetWndText(priceStr)
end
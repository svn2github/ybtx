CBuyCouponsWnd  =   class (SQRDialog)

function CBuyCouponsWnd.RetCapableOfBuyingCouponsInfoBegin()
    g_GameMain.m_BuyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl = {}
end

function CBuyCouponsWnd.RetCapableOfBuyingCouponsInfo(sequenceID, name, price, smallIcon, desc, url)
    local node = {}
    node["Name"] = name
    node["Price"] = price
    node["Description"] = desc
    node["Url"] = url
    node["ID"] = sequenceID
    node["SmallIcon"] = smallIcon
    
    table.insert(g_GameMain.m_BuyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl, node)
    
end

function CBuyCouponsWnd.RetCapableOfBuyingCouponsInfoEnd()
    g_GameMain.m_BuyCouponsWnd:ShowWnd(true)
    g_GameMain.m_BuyCouponsWnd:ShowCapableOfBuyingCouponsList()
    g_GameMain.m_BuyCouponsWnd.m_BuyTabBtn:SetCheck(true)
end


function CBuyCouponsWnd.RetBoughtCouponsInfoBegin()
    g_GameMain.m_BuyCouponsWnd.m_BoughtCouponsDetailTbl = {}
end

function CBuyCouponsWnd.RetBoughtCouponsInfo(index, ID, sequenceID, name, price, smallIcon, desc, url )
    local node = {}
    node["Index"] = index
    node["ID"] = ID
    node["SequenceID"] = sequenceID
    node["Name"] = name
    node["Price"] = price
    node["Description"] = desc
    node["Url"] = url
    node["SequenceID"] = sequenceID
    node["SmallIcon"] = smallIcon
    
    table.insert(g_GameMain.m_BuyCouponsWnd.m_BoughtCouponsDetailTbl, node)
end

function CBuyCouponsWnd.RetBoughtCouponsInfoEnd()
    g_GameMain.m_BuyCouponsWnd:ShowBoughtCouponsList()
    g_GameMain.m_BuyCouponsWnd.m_GiftTabBtn:SetCheck(true)
end

function CBuyCouponsWnd:Ctor()
    self:CreateFromRes("BuyCouponsWnd", g_GameMain)
    
    g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CBuyCouponsWnd:OnChildCreated()
    self.m_CloseBtn     =   self:GetDlgChild("CloseBtn")
    self.m_BuyTabBtn    =   self:GetDlgChild("BuyTabBtn")
    self.m_GiftTabBtn   =   self:GetDlgChild("GiftTabBtn")
    self.m_List         =   self:GetDlgChild("List")
end

function CBuyCouponsWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then        
            self:ShowWnd(false)
            
        elseif Child == self.m_BuyTabBtn then
            Gac2Gas:GetCapableOfBuyingCouponsList(g_Conn)
            if g_GameMain.m_BoughtCouponsDetailWnd and
                g_GameMain.m_BoughtCouponsDetailWnd:IsShow() then
                g_GameMain.m_BoughtCouponsDetailWnd:ShowWnd(false)     
            end
            
        elseif Child == self.m_GiftTabBtn then
            Gac2Gas:GetBoughtCouponsInfo(g_Conn)
            if g_GameMain.m_CouponsDetailWnd and
                g_GameMain.m_CouponsDetailWnd:IsShow() then
                g_GameMain.m_CouponsDetailWnd:ShowWnd(false)     
            end
        end
    end
end

function CBuyCouponsWnd.OpenBuyCouponsWnd()
    if g_GameMain.m_BuyCouponsWnd:IsShow() then
        g_GameMain.m_BuyCouponsWnd:ShowWnd(false)
    else
        Gac2Gas:GetCapableOfBuyingCouponsList(g_Conn)
    end
end

function CBuyCouponsWnd:ShowCapableOfBuyingCouponsList()
    self.m_List:DeleteAllItem()
    self.m_List:InsertColumn(0, 215)
    self.m_CapableOfBuyingCouponsItemTbl = {}
    self.m_CapableOfBuyingCouponsItemWndTbl = {}
    self.m_CurChoosedCouponsChkBtn = nil
    self.m_PreChoosedCouponsChkBtn = nil
    
    local tblLen = # self.m_CapableOfBuyingCouponsInfoTbl
    for i=1, tblLen do
        self.m_List:InsertItem(i-1, 44)
        local item = self.m_List:GetSubItem(i-1, 0)
        local itemWnd = CBuyCouponsItemWnd:new()
        itemWnd:CreateFromRes("BuyCouponsItem", item)
        itemWnd:ShowWnd(true)
        itemWnd:SetCouponsInfo(self.m_CapableOfBuyingCouponsInfoTbl[i])
        itemWnd.chkBtn = itemWnd:GetDlgChild("ChkBtn")
        itemWnd.chkBtn.Index =  i
        table.insert(self.m_CapableOfBuyingCouponsItemTbl, item)
        table.insert(self.m_CapableOfBuyingCouponsItemWndTbl, itemWnd)
    end
end

function CBuyCouponsWnd:ShowBoughtCouponsList()
    self.m_List:DeleteAllItem()
    self.m_List:InsertColumn(0,215)
    local tblLen = # self.m_BoughtCouponsDetailTbl
    self.m_CurChoosedBoughtCouponsChkBtn = nil
    self.m_PreChoosedBoughtCouponsChkBtn = nil
    
    self.m_BoughtCouponsItemTbl = {}
    self.m_BoughtCouponsItemWndTbl = {}
    for i=1, tblLen do
        self.m_List:InsertItem(i-1, 44)
        local item = self.m_List:GetSubItem(i-1, 0)
        local itemWnd = CBoughtCouponsItemWnd:new()
        itemWnd:CreateFromRes("BoughtCouponsItem", item)
        itemWnd:SetBoughtCouponsItem(self.m_BoughtCouponsDetailTbl[i])
        itemWnd:ShowWnd(true)
        itemWnd.chkBtn = itemWnd:GetDlgChild("ChkBtn")
        itemWnd.chkBtn.Index =  i
        table.insert(self.m_BoughtCouponsItemTbl, item)
        table.insert(self.m_BoughtCouponsItemWndTbl, itemWnd)
    end
end
CBoughtCouponsDetailWnd = class (SQRDialog)

function CBoughtCouponsDetailWnd:Ctor()
    self:CreateFromRes("BoughtCouponsDetail", g_GameMain.m_BuyCouponsWnd)
    
    g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, {g_GameMain.m_BuyCouponsWnd})
end

function CBoughtCouponsDetailWnd:OnChildCreated()
    self.m_CloseBtn     =   self:GetDlgChild("CloseBtn")
    self.m_RichText     =   self:GetDlgChild("RichText")
    self.m_ItemName     =   self:GetDlgChild("ItemName")
    self.m_Code         =   self:GetDlgChild("Code")
    self.m_CopyBtn      =   self:GetDlgChild("CopyBtn")
    self.m_Url        =   self:GetDlgChild("Url")
    self.m_ItemIcon     =   self:GetDlgChild("ItemIcon")
end

function CBoughtCouponsDetailWnd:SetBoughtCouponsDetail(index)
    local node = g_GameMain.m_BuyCouponsWnd.m_BoughtCouponsDetailTbl[index]    
    local ID   = node["ID"]
    local sequenceID = node["SequenceID"]
    self.m_ItemIcon:SetFocus()
    local buyCouponsWnd = g_GameMain.m_BuyCouponsWnd
    local smallIcon = node["SmallIcon"]
    local itemName = node["Name"]
    local description = node["Description"]
    local url       = node["Url"]
    local smallIcon = node["SmallIcon"]
    
    g_LoadIconFromRes(smallIcon, self.m_ItemIcon, -1, IP_ENABLE, IP_CLICKDOWN)
    
    self.m_ItemName:SetWndText(itemName)
    self.m_RichText:SetWndText(description)
    
    self.m_Code:SetWndText(sequenceID)
    self.m_UrlAddress = url
    self.m_Url:SetWndText(GetStaticTextClient(390001, "#c00ff00#l#cffe980#cff8928" .. url .. "#l#c00ff00"))
end


function CBoughtCouponsDetailWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then       
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
            
        elseif Child == self.m_CopyBtn then
            local sequenceID = self.m_Code:GetWndText()
            self.m_Code:CopyToClipboard(sequenceID)
        end   
    elseif uMsgID == RICH_CLICK then
        RunUrl(self.m_UrlAddress)
    end 
end

CCouponsDetailWnd     =   class (SQRDialog)

function CCouponsDetailWnd:Ctor()
    self:CreateFromRes("CouponsDetailWnd", g_GameMain.m_BuyCouponsWnd)

    g_ExcludeWndMgr:InitExcludeWnd(self, 2)
end

function CCouponsDetailWnd:OnChildCreated()
    self.m_CloseBtn     =   self:GetDlgChild("CloseBtn")
    self.m_RichText     =   self:GetDlgChild("RichText")
    self.m_Money        =   self:GetDlgChild("Money")
    self.m_BuyBtn       =   self:GetDlgChild("BuyBtn")
    self.m_Url         =   self:GetDlgChild("Url")
    self.m_Title        =   self:GetDlgChild("Title")
    self.m_Icon         =   self:GetDlgChild("Icon")
end

function CCouponsDetailWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
        
        elseif Child == self.m_BuyBtn then
            if self.m_ID ~= nil and self.m_ItemName ~= nil then
                if g_MainPlayer.m_nMoney < self.m_Price then
                    MessageBox(self, MsgBoxMsg(109), MB_BtnOK, nil)
                    return  
                end
                local function CallBack(Context, Button)
				    if(Button == MB_BtnOK) then
                        Gac2Gas:BuyCouponsUsingJinBi(g_Conn, self.m_ID, self.m_ItemName)
                    end
                    return true
                end
                MessageBox(self, MsgBoxMsg(112, self.m_PriceStr), BitOr( MB_BtnOK, MB_BtnCancel), CallBack)
            end
        end
    elseif uMsgID == RICH_CLICK then
        RunUrl(self.m_UrlAddress)
    end
end


function CCouponsDetailWnd:SetCouponsDetail(index)
    local buyCouponsWnd = g_GameMain.m_BuyCouponsWnd
    local smallIcon = buyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl[index]["SmallIcon"]
    local itemName = buyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl[index]["Name"]
    local price    = buyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl[index]["Price"]
    local description = buyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl[index]["Description"]
    local url       = buyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl[index]["Url"]
    local ID= buyCouponsWnd.m_CapableOfBuyingCouponsInfoTbl[index]["ID"]
    
    self.m_Icon:SetFocus()
    g_LoadIconFromRes(smallIcon, self.m_Icon, -1, IP_ENABLE, IP_CLICKDOWN)
    
    self.m_Title:SetWndText(itemName)
    self.m_RichText:SetWndText(description)
    
    local moneyMgr = CMoney:new()
    local priceStr = moneyMgr:ChangeMoneyToString(price, EGoldType.GoldCoin )
    self.m_Money:SetWndText(priceStr)
    self.m_PriceStr = priceStr
    self.m_Price = price
    self.m_ID = ID
    self.m_ItemName = itemName
    
    self.m_Url:SetWndText(GetStaticTextClient(390001, "#c00ff00#l#cffe980#cff8928" .. url .. "#l"))
    self.m_UrlAddress = url
end

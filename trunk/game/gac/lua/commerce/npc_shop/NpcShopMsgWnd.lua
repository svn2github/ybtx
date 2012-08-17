CNpcShopMsgWnd  =   class (SQRDialog)

local LiuTongShop       = 1     --流通商店
local CommonShop        = 0     --通用商店

local function CreateNpcShopMsgWnd(wnd)
    wnd:CreateFromRes("NpcShopWndMsg", g_GameMain)
    wnd:InitChild()
end

function CNpcShopMsgWnd:Ctor()
    CreateNpcShopMsgWnd(self)
end

function CNpcShopMsgWnd:InitChild()
    self.m_BtnOK    =   self:GetDlgChild("BtnOK")
    self.m_ChkBtn   =   self:GetDlgChild("ChkBtn")
    self.m_CancelBtn=   self:GetDlgChild("CancelBtn")
    self.m_Text     =   self:GetDlgChild("WndInfo")
end

local ShopTipFlagTbl = {}
ShopTipFlagTbl[1] = 0
ShopTipFlagTbl[0] = 1

function CNpcShopMsgWnd:OnOkBtnClick()
    self:ShowWnd(false)
    if g_GameMain.m_NPCShopSell:IsShow() then    
        local wnd = g_GameMain.m_NPCShopSell
        wnd:SendBuyItemRpc(wnd.m_BuyItemInfo)
   
    elseif  (g_GameMain.m_DrinkWnd and g_GameMain.m_DrinkWnd:IsShow() and
            g_GameMain.m_DrinkWnd.m_LiuTongTip ==  1) then
        g_GameMain.m_DrinkWnd:OnDrink()
    elseif (g_GameMain.m_EquipSuperaddWnd:IsShow()) then
            g_GameMain.m_EquipSuperaddWnd:EquipSuperadd()
    end
end

function CNpcShopMsgWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK  then
        if Child == self.m_ChkBtn then
            local tipFlag = 1
            if g_GameMain.m_NPCShopSell.m_LiuTongShopFlag == LiuTongShop  or
                ( g_GameMain.m_DrinkWnd and g_GameMain.m_DrinkWnd:IsShow() and
                g_GameMain.m_DrinkWnd.m_LiuTongTip ==  1 ) or 
                g_GameMain.m_EquipSuperaddWnd:IsShow() then
                tipFlag = ShopTipFlagTbl[g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag]
                Gac2Gas:UpdateShopTipFlag(g_Conn, LiuTongShop, tipFlag)

            elseif g_GameMain.m_NPCShopSell.m_LiuTongShopFlag == CommonShop then
                tipFlag = ShopTipFlagTbl[g_GameMain.m_NPCShopSell.m_CommonShopTipFlag]
                Gac2Gas:UpdateShopTipFlag(g_Conn, CommonShop, tipFlag)   
            end
            if tipFlag ==  0 then
                self.m_ChkBtn:SetCheck(true)
            else
                self.m_ChkBtn:SetCheck(false)
            end

        elseif Child == self.m_BtnOK then
            self:OnOkBtnClick()
            
        elseif Child == self.m_CancelBtn then
            self:ShowWnd(false)
        end 
    end
end
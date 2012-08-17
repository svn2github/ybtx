NpcShopWndMsgSettingLiuTongBtn  =   class (SQRDialog)

local LiuTongShop       = 1     --流通商店
local CommonShop        = 0     --通用商店

local function CreateNpcShopMsgWnd(wnd)
    wnd:CreateFromRes("NpcShopWndMsgSetLiuTongBtn", g_GameMain.m_NPCShopSell)
    wnd:InitChild()
end

function NpcShopWndMsgSettingLiuTongBtn:Ctor()
    CreateNpcShopMsgWnd(self)
end

function NpcShopWndMsgSettingLiuTongBtn:InitChild()
    self.m_BtnOK    =   self:GetDlgChild("BtnOK")
    self.m_ChkBtn   =   self:GetDlgChild("ChkBtn")
    self.m_CancelBtn=   self:GetDlgChild("CancelBtn")
    self.m_Text     =   self:GetDlgChild("WndInfo")
end


local ShopTipFlagTbl = {}
ShopTipFlagTbl[1] = 0
ShopTipFlagTbl[0] = 1

function NpcShopWndMsgSettingLiuTongBtn:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK  then
        if Child == self.m_BtnOK then
            Gac2Gas:SetMoneyType(g_Conn, 1)
            self:ShowWnd(false)
        
        elseif Child == self.m_CancelBtn then
            g_GameMain.m_WndMainBag.m_ChkBindingMoney:SetCheck(true)
            self:ShowWnd(false)
        
        elseif Child ==  self.m_ChkBtn then            
            local tipFlag = ShopTipFlagTbl[g_GameMain.m_NPCShopSell.m_CommonShopTipFlag]
            Gac2Gas:UpdateShopTipFlag(g_Conn, CommonShop, tipFlag)  
            if tipFlag ==  0 then
                self.m_ChkBtn:SetCheck(true)
            else
                self.m_ChkBtn:SetCheck(false)
            end
        end
    end
end
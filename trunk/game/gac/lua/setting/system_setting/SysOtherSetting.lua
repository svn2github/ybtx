SysOtherSettingWnd  =   class (SQRDialog)
local LiuTongShop       = 1     --流通商店
local CommonShop        = 0     --通用商店

function SysOtherSettingWnd:Ctor(parent)
    self:CreateFromRes("SysOtherSetting", parent)
    self:InitChild()
end

function SysOtherSettingWnd:InitChild()
    self.m_CommonShopChkBtn     =   self:GetDlgChild("CommonShopChkBtn")
    self.m_LiuTongShopChkBtn    =   self:GetDlgChild("LiuTongShopChkBtn")
end

local function GetOppositeTipFlagNum(flag)
    if flag == 1 then
        return 0
    elseif flag == 0 then
        return 1
    end 
end

local function GetChkBtnSetFlag(flag)
    if flag == 1 then
        return true
    elseif flag == 0 then
        return false 
    end
end

function SysOtherSettingWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK  then
        if Child == self.m_CommonShopChkBtn then
            local tipFlag = GetOppositeTipFlagNum(g_GameMain.m_NPCShopSell.m_CommonShopTipFlag)
            Gac2Gas:UpdateShopTipFlag(g_Conn, CommonShop, tipFlag)  
            self.m_CommonShopChkBtn:SetCheck(GetChkBtnSetFlag(tipFlag))
            
        elseif Child == self.m_LiuTongShopChkBtn then
            local tipFlag = GetOppositeTipFlagNum(g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag)
            Gac2Gas:UpdateShopTipFlag(g_Conn, LiuTongShop, tipFlag)  
            self.m_LiuTongShopChkBtn:SetCheck(GetChkBtnSetFlag(tipFlag))
        end
    end
end

function SysOtherSettingWnd:UpdateSettingShopTipChkBtn()
    local commonShopTipFlag     =  g_GameMain.m_NPCShopSell.m_CommonShopTipFlag
    local liutongShopTipFlag    =  g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag

    self.m_CommonShopChkBtn:SetCheck(GetChkBtnSetFlag(commonShopTipFlag))
    self.m_LiuTongShopChkBtn:SetCheck(GetChkBtnSetFlag(liutongShopTipFlag))
end

function SysOtherSettingWnd:SetDefault()
    local commonShopTipFlag     =  g_GameMain.m_NPCShopSell.m_CommonShopTipFlag
    local liutongShopTipFlag    =  g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag
    if commonShopTipFlag ~= 1 or liutongShopTipFlag ~= 1 then
        Gac2Gas:SetDefaultShopTipFlag(g_Conn)
    end
end
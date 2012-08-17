CContractManufactureMarketOrderItemWnd = class (CContractManufactureCommon)
local MaxOrderCountPerPage = 10
local LiveSkillDirectionTypeTbl = {}
LiveSkillDirectionTypeTbl["战神"] = 1
LiveSkillDirectionTypeTbl["法魔"] = 2


function CContractManufactureMarketOrderItemWnd:SetMarketCMOrderDetail(index)
    local cmOrderInfoTbl     =   g_GameMain.m_ContractManufactureMarketWnd.ShowOrderInfoTbl
    local cmOrderItemInfoTbl =   g_GameMain.m_ContractManufactureMarketWnd.m_MaterialInfoTbl
    
    local productIconWnd = self:GetDlgChild("ProductIcon")
    
    local productNameWnd = self:GetDlgChild("ProductName")
    --[[
    local materialIconWnd1= self:GetDlgChild("MaterialIcon1")
    local materialIconWnd2= self:GetDlgChild("MaterialIcon2")
    local materialIconWnd3= self:GetDlgChild("MaterialIcon3")
    --]]
    local wndTbl = {prodcutIconWnd, prodcutNameWnd, materialIconWnd1, materialIconWnd2, materialIconWnd3}
    local prescripName = cmOrderInfoTbl[index]["PrescripName"]
    local cmOrderId   = cmOrderInfoTbl[index]["CMOrderID"]
    local leftTime    = cmOrderInfoTbl[index]["EndTime"]
    local money       = cmOrderInfoTbl[index]["Money"]
    local playerName  = cmOrderInfoTbl[index]["PlayerName"]
    local skillName   = cmOrderInfoTbl[index]["SkillName"]
    local direction   = cmOrderInfoTbl[index]["Direction"]
    
    local productType = LiveSkill_Common(skillName, direction, prescripName, "ProductType1")
    local productName = LiveSkill_Common(skillName, direction, prescripName, "ProductName1")
    
    local materialTbl = cmOrderItemInfoTbl[cmOrderId]

    local productIcon = g_ItemInfoMgr:GetItemInfo(productType, productName, "SmallIcon")
    local displayProductName = g_ItemInfoMgr:GetItemLanInfo(productType, productName,"DisplayName")
    g_LoadIconFromRes(productIcon, productIconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
    g_SetWndMultiToolTips(productIconWnd,productType,productName,0 ,1)     
    productNameWnd:SetWndText(displayProductName)
   
    self:GetDlgChild("PlayerName"):SetWndText(playerName)
    
    gac_gas_require "framework/common/CMoney"
    local moneyMgr = CMoney:new()
    local makeProductMoney = money 
    local tblMoneyWnd = {self:GetDlgChild("JinCount"),self:GetDlgChild("Jin"), self:GetDlgChild("YinCount"),self:GetDlgChild("Yin"),self:GetDlgChild("TongCount"),self:GetDlgChild("Tong")}
    moneyMgr:ShowMoneyInfo(makeProductMoney, tblMoneyWnd)
    
    local showLeftTime = math.ceil(leftTime)
	if showLeftTime <= 0 then
	   showLeftTime = GetStaticTextClient(1287)
	else
	    showLeftTime = showLeftTime .. GetStaticTextClient(1121)
	end
    self:GetDlgChild("Time"):SetWndText(showLeftTime)
    
    local productLevel = g_ItemInfoMgr:GetItemInfo(productType, productName, "BaseLevel")
    self:GetDlgChild("Level"):SetWndText(productLevel)
    
    local skillLevel = LiveSkill_Common(skillName, direction, prescripName, "SkillLevel")
    local displaySkillName = Lan_LiveSkillName_Client(MemH64(skillName), "DisplayName")
    self:GetDlgChild("SkillName"):SetWndText(displaySkillName)
    
    local displayDirectionName = ""
    if LiveSkillDirectionTypeTbl[direction] == nil then
        local directioName = Lan_Practiced_Common(MemH64(skillName .. direction), "DisplayName")
        displayDirectionName = directioName .. Lan_DirectionType_Common(MemH64("专精"), "DisplayName")
    else
        displayDirectionName = Lan_DirectionType_Common(MemH64(direction), "DisplayName")
    end
     
    local levelStr = Lan_SmithingExpert_Common(skillLevel, "DisplayName")
    local displaySkill = displayDirectionName .. levelStr
    self:GetDlgChild("SkillLevel"):SetWndText(displaySkill)
end

local function SetCheckChoosedOrder(index)
    local tbl = g_GameMain.m_ContractManufactureMarketWnd.m_MarketOrderChkBtnTbl 
    for i, v in pairs (tbl) do
        local chkBtn = v
        if index == chkBtn.Index then
            chkBtn:SetCheck(true)
        else
            chkBtn:SetCheck(false)
        end
    end
end

function CContractManufactureMarketOrderItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
    if uMsgID == BUTTON_LCLICK then
        if Child == self:GetDlgChild("OrderChkBtn") then
            local playerName = g_GameMain.m_ContractManufactureMarketWnd.m_MarketOrderInfoTbl[self.Index]["PlayerName"]
            if playerName == g_MainPlayer.m_Properties:GetCharName() then
                return  
            end
            local curPageNum = g_GameMain.m_ContractManufactureMarketWnd.m_CurPageNum
            if curPageNum < 1 then
                curPageNum = 1
            end
            g_GameMain.m_ContractManufactureMarketWnd.m_ChoosedCMOrderIndex =  self.Index
            g_GameMain.m_ContractManufactureMarketWnd.m_TakeOrderBtn:EnableWnd(true)
            SetCheckChoosedOrder(self.Index)
        end
    end
end

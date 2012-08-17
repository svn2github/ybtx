CContractManufactureOrderItemWnd = class (CContractManufactureCommon)
lan_load "liveskill/Lan_LiveSkillName_Client"

--显示所有可下代工订单的配置表信息
function CContractManufactureOrderItemWnd:SetContractManuOrderItem(tbl, index)
    local materialNum1 = tbl[index][3][1][3]
    local materialNum2 = tbl[index][3][2][3]
    local materialNum3 = tbl[index][3][3][3]
    local itemLevel =   tbl[index][4]
    local skillName =   tbl[index][5]
    local skillLevel =   tbl[index][6]
    local materialType2= tonumber(tbl[index][3][2][1])
    local materialName2= tbl[index][3][2][2]
    local materialType3= tonumber(tbl[index][3][3][1])
    local materialName3= tbl[index][3][3][2]
    
    
    local productIconWnd = self:GetDlgChild("ProductIcon")
    local productNameWnd = self:GetDlgChild("ProductName")
    local materialIconWnd1 = self:GetDlgChild("MaterialIcon1")
    local materialIconWnd2 = self:GetDlgChild("MaterialIcon2")
    local materialIconWnd3 = self:GetDlgChild("MaterialIcon3")
    local productAndMaterialWnd = {productIconWnd, productNameWnd, materialIconWnd1, materialIconWnd2, materialIconWnd3}
    

    self:SetContractManuOrderDetail(tbl, index, productAndMaterialWnd)
    self:GetDlgChild("Level"):SetWndText(itemLevel)
    
    local displaySkillName = Lan_LiveSkillName_Client(MemH64(skillName), "DisplayName")
    self:GetDlgChild("SkillName"):SetWndText(displaySkillName)
    
    self:GetDlgChild("SkillLevel"):SetWndText(skillLevel)
    
    self:GetDlgChild("MaterialNum1"):SetWndText(materialNum1)
    if materialType2 ~= nil and materialName2 ~= "" then
        self:GetDlgChild("MaterialNum2"):SetWndText(materialNum2)
    else
        self:GetDlgChild("MaterialNum2"):ShowWnd(false)
    end
    if materialType3 ~= nil and materialName3 ~= "" then
        self:GetDlgChild("MaterialNum3"):SetWndText(materialNum3)  
    else
        self:GetDlgChild("MaterialNum3"):ShowWnd(false)
    end
    productIconWnd:SetFocus()
    materialIconWnd1:SetFocus()
    materialIconWnd2:SetFocus()
    materialIconWnd3:SetFocus()
end

function CContractManufactureOrderItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
    if uMsgID == BUTTON_LCLICK then
        if Child == self:GetDlgChild("CancelOrder") then
            g_GameMain.m_ContractManufactureOrderWnd:SetNeedContractManuOrderDetail(self.Index)
            g_GameMain.m_ContractManufactureOrderWnd:CheckInput()
        end
    end
end
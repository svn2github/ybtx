CContractManufactureCharOrderItemWnd = class (CContractManufactureCommon)

function CContractManufactureCharOrderItemWnd:SetCharCMOrderDetail(index)

    local cmOrderInfoTbl     =   g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderTbl
    local cmOrderItemInfoTbl =   g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderMaterialTbl
    
    local prodcutIconWnd = self:GetDlgChild("ProductIcon")
    local prodcutNameWnd = self:GetDlgChild("ProductName")
    local materialIconWnd1= self:GetDlgChild("MaterialIcon1")
    local materialIconWnd2= self:GetDlgChild("MaterialIcon2")
    local materialIconWnd3= self:GetDlgChild("MaterialIcon3")
    
    local wndTbl = {prodcutIconWnd, prodcutNameWnd, materialIconWnd1, materialIconWnd2, materialIconWnd3}
    
    local prescripName = cmOrderInfoTbl[index]["PrescripName"]
    local cmOrderId   = cmOrderInfoTbl[index]["CMOrderID"]
    local leftTime    = cmOrderInfoTbl[index]["EndTime"]
    local money       = cmOrderInfoTbl[index]["Money"]
    local skillName   = cmOrderInfoTbl[index]["SkillName"]
    local direction   = cmOrderInfoTbl[index]["Direction"]
    local productType = LiveSkill_Common(skillName, direction, prescripName, "ProductType1")
    local productName = LiveSkill_Common(skillName, direction, prescripName, "ProductName1")
    
    local materialTbl = cmOrderItemInfoTbl[cmOrderId]
    local tbl= {}
    tbl[1] = {productType, productName, materialTbl}

    self:SetContractManuOrderDetail(tbl, 1, wndTbl)
    
    local productLevel = g_ItemInfoMgr:GetItemInfo(productType, productName, "BaseLevel")
    self:GetDlgChild("ProductLevel"):SetWndText(productLevel)
    
    local showLeftTime = math.ceil(leftTime)
	if showLeftTime <= 0 then
	   showLeftTime = GetStaticTextClient(1287)
	else
	    showLeftTime = showLeftTime .. GetStaticTextClient(1121)
	end
    self:GetDlgChild("TimeLimit"):SetWndText(showLeftTime)
    
    gac_gas_require "framework/common/CMoney"
    local moneyMgr = CMoney:new()
    local tblMoneyWnd = {self:GetDlgChild("JinCount"),self:GetDlgChild("Jin"), self:GetDlgChild("YinCount"),self:GetDlgChild("Yin"),self:GetDlgChild("TongCount"),self:GetDlgChild("Tong")}
    local makeProductMoney = money
    moneyMgr:ShowMoneyInfo(makeProductMoney,tblMoneyWnd)

	
	
	local materialNumWnd = {self:GetDlgChild("MaterialNum1"),self:GetDlgChild("MaterialNum2"),self:GetDlgChild("MaterialNum3")}
	for i=1, 3 do
        local materialNode = materialTbl[i]
        if materialNode ~= nil then
            local materialNum = materialNode[3]
            materialNumWnd[i]:SetWndText(materialNum)            
        else
            materialNumWnd[i]:ShowWnd(false)
        end
	end
end

function CContractManufactureCharOrderItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
    if uMsgID == BUTTON_LCLICK then
        if Child == self:GetDlgChild("CancelOrder") then    
            local function CallBack(Context, Button)
				if(Button == MB_BtnOK) then
                    local cmOrderID = g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderTbl[self.Index]["CMOrderID"]
                    Gac2Gas:CancelContractManufactureOrder(g_Conn, cmOrderID)
                end
                return true
            end
            MessageBox(self, MsgBoxMsg(155), BitOr( MB_BtnOK, MB_BtnCancel), CallBack)
        end
    end
end
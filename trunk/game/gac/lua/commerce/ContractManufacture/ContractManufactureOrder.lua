CContractManufactureOrderWnd = class (CContractManufactureCommon)
cfg_load "liveskill/LiveSkill_Common"
local TotalNumColor = g_ColorMgr:GetColor("生活技能需要的材料总数目")
local NotEnoughNumColor =  g_ColorMgr:GetColor("生活技能物品数目不够")

local function SendCharCMOrderInfo()
    local wnd = g_GameMain.m_ContractManufactureOrderWnd
    if wnd.m_CurPageNum == 0 then
        wnd.m_CurPageNum  = 1
    end
    Gac2Gas:SearchCharContractManufactureOrder(g_Conn,  wnd.m_CurPageNum )
    g_GameMain.m_ContractManufactureOrderWnd:SetNeedContractManuOrderDetail()
    g_GameMain.m_ContractManufactureOrderWnd:CheckInput()
end


function CContractManufactureOrderWnd.RetCharCMOrderBegin()
    g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderTbl = {}
    g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderMaterialTbl = {}
end

function CContractManufactureOrderWnd.RetCharContractManufactureOrder(cmOrderID, skillName, direction, prescripName, endTime, money)
    local node = {}
    node["CMOrderID"] = cmOrderID
    node["SkillName"] = skillName
    node["Direction"] = direction
    node["PrescripName"] = prescripName
    node["EndTime"] = endTime
    node["Money"] = money
    
    table.insert(g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderTbl, node)
end

function CContractManufactureOrderWnd.RetCharCMOrderMaterialInfo(cmOrderID, index, materialType, materialName, materialCount)
    if g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderMaterialTbl[cmOrderID] == nil then
        g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderMaterialTbl[cmOrderID] = {}
    end
    g_GameMain.m_ContractManufactureOrderWnd.m_CharCMOrderMaterialTbl[cmOrderID][index] = {materialType, materialName, materialCount}
end

function CContractManufactureOrderWnd.RetCharCMOrderEnd(totalPage)
    g_GameMain.m_ContractManufactureOrderWnd.m_MaxPageNum = totalPage
    if g_GameMain.m_ContractManufactureOrderWnd.m_CurPageNum and
        g_GameMain.m_ContractManufactureOrderWnd.m_CurPageNum > totalPage then
        g_GameMain.m_ContractManufactureOrderWnd.m_CurPageNum = totalPage
    end 
    g_GameMain.m_ContractManufactureOrderWnd:DrawCharCMOrderList()
end

function CContractManufactureOrderWnd.RetAddCMOrderSuc() 
    SendCharCMOrderInfo()
end

function CContractManufactureOrderWnd.RetCancelContractManufactureOrder()
    SendCharCMOrderInfo()
end

local function CreateContractManufactureOrder(wnd)
    wnd:CreateFromRes("ContractManufacture", g_GameMain)
    wnd:InitChild()
    
    wnd.m_CurPageNum = 1
    wnd.m_MaxPageNum = 0
    wnd.m_TimeLimit  = 24
    
    wnd.m_TempOrderPageNum = 1

    wnd.m_MaterialNumFlagTbl = {false, false, false}
    wnd.m_TimeChkBtn1:SetCheck(true)
    wnd:InitCanBeContractedManufactureOrderInfo()
    wnd:SetCanBeContractedManufactureOrderInfo()
    g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
	wnd:SetStyle(0x00040000)
end


function CContractManufactureOrderWnd:Ctor()
    CreateContractManufactureOrder(self)
end

function CContractManufactureOrderWnd:InitChild()
    self.m_CloseBtn         =   self:GetDlgChild("CloseBtn")
    self.m_ProductIcon      =   self:GetDlgChild("ProductIcon")
    self.m_ProductNameWnd      =   self:GetDlgChild("ProductName")
    self.m_MaterialIcon1    =   self:GetDlgChild("MaterialIcon1")
    self.m_MaterialName1    =   self:GetDlgChild("MaterialName1")
    self.m_MaterialNum1     =   self:GetDlgChild("MaterialNum1")
    self.m_MaterialIcon2    =   self:GetDlgChild("MaterialIcon2")
    self.m_MaterialName2    =   self:GetDlgChild("MaterialName2")
    self.m_MaterialNum2     =   self:GetDlgChild("MaterialNum2")
    self.m_MaterialIcon3    =   self:GetDlgChild("MaterialIcon3")
    self.m_MaterialName3    =   self:GetDlgChild("MaterialName3")
    self.m_MaterialNum3     =   self:GetDlgChild("MaterialNum3")
    self.m_TimeChkBtn1      =   self:GetDlgChild("TimeChkBtn1")
    self.m_TimeChkBtn2      =   self:GetDlgChild("TimeChkBtn2")
    
    self.m_MaterialWndTbl  = {{self.m_MaterialIcon1, self.m_MaterialName1, self.m_MaterialNum1},
    {self.m_MaterialIcon2, self.m_MaterialName2, self.m_MaterialNum2},
    {self.m_MaterialIcon3, self.m_MaterialName3, self.m_MaterialNum3}}
    
    self.m_JinCount         =   self:GetDlgChild("JinCount")
    self.m_YinCount         =   self:GetDlgChild("YinCount")
    self.m_TongCount        =   self:GetDlgChild("TongCount")
    self.m_FeeJin           =   self:GetDlgChild("FeeJin")
    self.m_FeeYin           =   self:GetDlgChild("FeeYin")
    self.m_FeeTong          =   self:GetDlgChild("FeeTong")
    self.m_NeedContractManufacture = self:GetDlgChild("NeedCM")
    
    self.m_CMListCtrl       =   self:GetDlgChild("CMListCtrl")
    self.m_OrderListCtrl    =   self:GetDlgChild("OrderListCtrl")
    self.m_PrePageBtn       =   self:GetDlgChild("PrePageBtn")
    self.m_NextPageBtn      =   self:GetDlgChild("NextPageBtn")
    self.m_CurPageText      =   self:GetDlgChild("CurPageText")
    
    self.m_ProductIcon:SetFocus()      
    self.m_MaterialIcon1:SetFocus()         
    self.m_MaterialIcon2:SetFocus()
    self.m_MaterialIcon3:SetFocus()
    
    self.m_TempOrderPageUpBtn = self:GetDlgChild("TempPageUpBtn")
    self.m_TempOrderPageDownBtn = self:GetDlgChild("TempPageDownBtn")
    self.m_TempPageNumBtn = self:GetDlgChild("TempPageNum")
    self.m_NeedContractManufacture:EnableWnd(false)
end


function CContractManufactureOrderWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
            
        elseif Child ==  self.m_PrePageBtn then
            if self.m_CurPageNum > 1 then
                self.m_CurPageNum = self.m_CurPageNum - 1
                Gac2Gas:SearchCharContractManufactureOrder(g_Conn, self.m_CurPageNum)
            end
        
        elseif Child == self.m_NextPageBtn then
            if self.m_CurPageNum < self.m_MaxPageNum then
                self.m_CurPageNum = self.m_CurPageNum + 1 
                Gac2Gas:SearchCharContractManufactureOrder(g_Conn, self.m_CurPageNum)
            end
               
        elseif Child == self.m_NeedContractManufacture then  
            if g_MainPlayer.m_ItemBagLock then
                MsgClient(160043)
                return
            end        
            local money  = self:GetInputPrice()
            local totalFee = self:GetSysFee()
            local suc, msgID = g_MoneyMgr:CheckPayingSysFeeEnough(totalFee)
            if totalFee == 0 then
                suc = false 
            end
			if suc == false then
                self.m_MsgBox = MessageBox(self, MsgBoxMsg(msgID), MB_BtnOK)
            else
                if msgID ~= nil then
                    local function callBack(Context,Button) --如果选择确定按钮，则将支付方式改为另一种类型
		                if(Button == MB_BtnOK) then 
                            Gac2Gas:AddNeedContractManufactureOrder(g_Conn, self.m_SkillType, self.m_Direction, self.m_TimeLimit, money, self.m_PrescripName)                
                        end 
                        return true 
                    end   
                    self.m_MsgBox = MessageBox(self, MsgBoxMsg(msgID), BitOr(MB_BtnOK, MB_BtnCancel), callBack)                    
                else
                    Gac2Gas:AddNeedContractManufactureOrder(g_Conn, self.m_SkillType, self.m_Direction, self.m_TimeLimit, money, self.m_PrescripName)                
                end
            end
            
        elseif Child == self.m_TimeChkBtn1 then
            self.m_TimeLimit = 24
            self.m_TimeChkBtn1:SetCheck(true)
            self.m_TimeChkBtn2:SetCheck(false)
        
        elseif Child == self.m_TimeChkBtn2 then
            self.m_TimeLimit = 48
            self.m_TimeChkBtn2:SetCheck(true)
            self.m_TimeChkBtn1:SetCheck(false)
            
        elseif Child == self.m_TempOrderPageUpBtn then
            if self.m_TempOrderPageNum > 1 then
                self.m_TempOrderPageNum =  self.m_TempOrderPageNum - 1
                self:SetCanBeContractedManufactureOrderInfo()
            end
            
        elseif Child == self.m_TempOrderPageDownBtn then
            local maxNum = math.ceil(# self.m_CanBeContractedManuOrderTbl /4)
            if self.m_TempOrderPageNum <  maxNum then
                self.m_TempOrderPageNum =  self.m_TempOrderPageNum + 1
                self:SetCanBeContractedManufactureOrderInfo()
            end
        end
        
    elseif uMsgID == WND_NOTIFY  then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
            self:CheckInput( Child )
        end      
    end
end

function CContractManufactureOrderWnd:InitCanBeContractedManufactureOrderInfo()
    self.m_CanBeContractedManuOrderTbl = {}
    local keys = LiveSkill_Common:GetKeys()
    
    for i, v in pairs(keys) do
        local SkillType = v
        local DirectionTypeKeys = LiveSkill_Common:GetKeys(SkillType)
        for j, w in pairs(DirectionTypeKeys) do
            local directionType = w
            local prescripNamekeys = LiveSkill_Common:GetKeys(SkillType, directionType)
            for q, r in pairs(prescripNamekeys) do
                local prescripName = r
                if LiveSkill_Common(SkillType, directionType, prescripName, "CanBeContractedManu") == 1 then
                    local productType = LiveSkill_Common(SkillType, directionType, prescripName, "ProductType1")
                    local productionName = LiveSkill_Common(SkillType, directionType, prescripName, "ProductName1")
                    local materialType1 = LiveSkill_Common(SkillType, directionType, prescripName, "MaterialType1")
                    local materialName1 = LiveSkill_Common(SkillType, directionType, prescripName, "Material1")
                    local materialNum1  = LiveSkill_Common(SkillType, directionType, prescripName, "NumberNeed1")
                    local materialType2 = LiveSkill_Common(SkillType, directionType, prescripName, "MaterialType2")
                    local materialName2 = LiveSkill_Common(SkillType, directionType, prescripName, "Material2")
                    local materialNum2  = LiveSkill_Common(SkillType, directionType, prescripName, "NumberNeed2")
                    local materialType3 = LiveSkill_Common(SkillType, directionType, prescripName, "MaterialType3")          
                    local materialName3 =  LiveSkill_Common(SkillType, directionType, prescripName, "Material3")
                    local materialNum3  = LiveSkill_Common(SkillType, directionType, prescripName, "NumberNeed3")
                    local materialTbl=  {{tonumber(materialType1), materialName1, tonumber(materialNum1)},
                    {tonumber(materialType2), materialName2, tonumber(materialNum2)},
                    {tonumber(materialType3), materialName3, tonumber(materialNum3)}}
                    local itemLevel = g_ItemInfoMgr:GetItemInfo(productType, productionName, "BaseLevel")
                    local skillLevel = LiveSkill_Common(SkillType, directionType, prescripName, "SkillLevel")
                    local node = {productType, productionName, materialTbl, itemLevel, SkillType, skillLevel, directionType, prescripName}
                    table.insert(self.m_CanBeContractedManuOrderTbl, node)
                end
            end
        end
    end
end


function CContractManufactureOrderWnd:SetCanBeContractedManufactureOrderInfo()
    local tbl = self.m_CanBeContractedManuOrderTbl
    local orderTblLen = # self.m_CanBeContractedManuOrderTbl
    self.m_CanBeContractedManuOrderItemTbl = {}
    self.m_CanBeContractedManuOrderItemWndTbl = {}
    self.m_CMListCtrl:DeleteAllItem()
    self.m_CMListCtrl:InsertColumn(0,600)
    if self.m_TempOrderPageNum < 1 then
        self.m_TempOrderPageNum  = 1
    end
    
    local totalPageNum = math.ceil(orderTblLen/4)
    if self.m_TempOrderPageNum > totalPageNum  then
        self.m_TempOrderPageNum = totalPageNum
    end
    local beginIndex = (self.m_TempOrderPageNum-1) * 4 + 1
    local endIndex = self.m_TempOrderPageNum * 4
    if endIndex > orderTblLen then
        endIndex = orderTblLen
    end
    local index = 0
    for i= beginIndex, endIndex do
        self.m_CMListCtrl:InsertItem(i-1, 50)
        local item = self.m_CMListCtrl:GetSubItem(index, 0)
        local orderItemWnd = CContractManufactureOrderItemWnd:new() 
        orderItemWnd:CreateFromRes("ContracManfactureItem", item)
        orderItemWnd:ShowWnd(true)
        orderItemWnd:SetMouseWheel(true)
        orderItemWnd:SetContractManuOrderItem(tbl, i)
                
        orderItemWnd.Index = i
        table.insert(self.m_CanBeContractedManuOrderItemTbl, item)
        table.insert(self.m_CanBeContractedManuOrderItemWndTbl, orderItemWnd)
        index = index + 1
    end
    self.m_TempPageNumBtn:SetWndText(self.m_TempOrderPageNum .. "/" .. totalPageNum)
end

--下代工订单的面板，设置详细的订单信息
function CContractManufactureOrderWnd:SetNeedContractManuOrderDetail(index)
    
    if index == nil then
        index = self.m_CurChoosedIndex 
    else
        self.m_CurChoosedIndex = index
    end
    if index == nil then
        return  
    end
    local tbl = self.m_CanBeContractedManuOrderTbl
    local wndTbl = {self.m_ProductIcon, self.m_ProductNameWnd, self.m_MaterialIcon1, self.m_MaterialIcon2, self.m_MaterialIcon3}
    
    self:SetContractManuOrderDetail(tbl, index, wndTbl)
    
    local needMaterialType1 = tbl[index][3][1][1]
    local needMaterialType2 = tbl[index][3][2][1]
    local needMaterialType3 = tbl[index][3][3][1]
    local needMaterialName1 = tbl[index][3][1][2]
    local needMaterialName2 = tbl[index][3][2][2]
    local needMaterialName3 = tbl[index][3][3][2]
    local needMaterialNum1 = tbl[index][3][1][3]
    local needMaterialNum2 = tbl[index][3][2][3]
    local needMaterialNum3 = tbl[index][3][3][3]
        
    self.m_ProductType      = tbl[index][1]
    self.m_ProductName      = tbl[index][2]
    self.m_MaterialTbl = {{needMaterialType1, needMaterialName1, needMaterialNum1},
    {needMaterialType2, needMaterialName2, needMaterialNum2},
    {needMaterialType3, needMaterialName3, needMaterialNum3},}
    self.m_SkillType    =   tbl[index][5]
    self.m_Direction    =   tbl[index][7]
    self.m_PrescripName =   tbl[index][8]
    
    for i, v in pairs(self.m_MaterialTbl) do
        local needMaterialType = v[1]
        local needMaterialName = v[2]
        local needMaterialNum  = v[3]
        if needMaterialName ~= "" then
            local lanMaterialName = g_ItemInfoMgr:GetItemLanInfo(needMaterialType, needMaterialName, "DisplayName")
            self.m_MaterialWndTbl[i][2]:SetWndText(lanMaterialName)
            local countInBagMaterial = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, needMaterialType, needMaterialName)
             
            if countInBagMaterial >= needMaterialNum then
                self.m_MaterialNumFlagTbl[i] =true 
                self.m_MaterialWndTbl[i][3]:SetWndText(TotalNumColor   .. countInBagMaterial .. "/" .. needMaterialNum)
            else
                self.m_MaterialNumFlagTbl[i] = false
                self.m_MaterialWndTbl[i][3]:SetWndText(NotEnoughNumColor .. countInBagMaterial .. TotalNumColor  .. "/" .. needMaterialNum)
            end
        else
            self.m_MaterialNumFlagTbl[i] =true 
            self.m_MaterialWndTbl[i][2]:SetWndText("")
            self.m_MaterialWndTbl[i][3]:SetWndText("")
        end 
    end
end

 
function CContractManufactureOrderWnd:CheckInput( clickedWnd )
	local inputValidate = false
	local money = CMoney:new()
	--如果要输入数字性的东西，则检测输入的是否合法
	if clickedWnd == self.m_YinCount or  
		clickedWnd == self.m_TongCount then
		money:CheckInputMoneyValidate(clickedWnd, 2)
		
	elseif clickedWnd == self.m_JinCount then
		money:CheckInputMoneyValidate(clickedWnd, 1)
	end
	if self.m_JinCount:GetWndText() ~= "" or 
		self.m_YinCount:GetWndText() ~= "" or 
		self.m_TongCount:GetWndText() ~= "" then
		inputValidate = true
	end
	
	local itemNameWnd = self.m_ItemName
	if (self.m_JinCount:GetWndText() ~= "" or self.m_YinCount:GetWndText() ~= "" or self.m_TongCount:GetWndText() ~= "")
		and inputValidate and (self.m_MaterialNumFlagTbl[1] and self.m_MaterialNumFlagTbl[2] and self.m_MaterialNumFlagTbl[3])
  		and self.m_ProductNameWnd:GetWndText() ~= ""  then
		self.m_NeedContractManufacture:EnableWnd( true )
	else 
		self.m_NeedContractManufacture:EnableWnd( false )
	end  	 
	
	self:SetSysFee()
end

function CContractManufactureOrderWnd:GetInputPrice()
	local goldCount = tonumber( self.m_JinCount:GetWndText() )
	local silverCount = tonumber( self.m_YinCount:GetWndText() )
	local tongCount = tonumber( self.m_TongCount:GetWndText() )
	
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local moneyCount = money:ChangeGoldAndArgentToMoney(goldCount, silverCount, tongCount)

	return moneyCount
end

function CContractManufactureOrderWnd:SetSysFee()
    local inputPrice =self:GetInputPrice()
    local money = CMoney:new()
    local totalFee = money:CalculateTotalFee(inputPrice, 1, 0.01, self.m_TimeLimit/24)
    local wndTbl = {self.m_FeeJin, self.m_FeeYin, self.m_FeeTong}
    money:ShowMoneyInfoWithoutShowingWndFalse(totalFee,wndTbl)
end

function CContractManufactureOrderWnd:GetSysFee()
    local money = CMoney:new()
    local feeJinCount =  self.m_FeeJin:GetWndText()
    local feeYinCount =  self.m_FeeYin:GetWndText()
    local feeTongCount = self.m_FeeTong:GetWndText()
    local totalFee = money:ChangeGoldAndArgentToMoney(feeJinCount, feeYinCount, feeTongCount)
    return totalFee
end

function CContractManufactureOrderWnd:DrawCharCMOrderList()
    local cmOrderInfoTbl     =  self.m_CharCMOrderTbl
    local cmOrderItemInfoTbl =  self.m_CharCMOrderMaterialTbl
    
    self.m_OrderListCtrl:DeleteAllItem()
    self.m_OrderListCtrl:InsertColumn(0, 600)
    self.m_CharOrderItemTbl = {}
    self.m_CharOrderItemWndTbl = {}
    
    if self.m_MaxPageNum == 0 then
        self.m_CurPageText:SetWndText(0 .. "/" .. 0) 
    else
        self.m_CurPageText:SetWndText(self.m_CurPageNum .. "/" .. self.m_MaxPageNum)    
    end
    
    local orderCount =  # self.m_CharCMOrderTbl
    for i= 1, orderCount do
        self.m_OrderListCtrl:InsertItem(i-1, 50)
        local item = self.m_OrderListCtrl:GetSubItem(i-1, 0)
        local itemWnd = CContractManufactureCharOrderItemWnd:new()
        itemWnd:CreateFromRes("ContracManfactureCharItem", item)
        itemWnd:ShowWnd(true)
        itemWnd.Index = i
        itemWnd:SetCharCMOrderDetail(i)
        
        table.insert(self.m_CharOrderItemTbl, item)
        table.insert(self.m_CharOrderItemWndTbl, itemWnd)
    end
end
lan_load "liveskill/Lan_LiveSkillName_Client"
lan_load "liveskill/Lan_SmithingExpert_Common"
cfg_load "liveskill/LiveSkill_Common"
cfg_load "liveskill/SmithingExpert_Common"

CContractManufactureMarketWnd = class (SQRDialog)

function CContractManufactureMarketWnd.RetCommonCMOrderBegin()
    g_GameMain.m_ContractManufactureMarketWnd.m_MaterialInfoTbl = {}	
    g_GameMain.m_ContractManufactureMarketWnd.m_MarketOrderInfoTbl = {}
    
end

function CContractManufactureMarketWnd.RetCommonContractManufactureOrder(cmOrderID, playerName, skillName, direction, prescripName, leftTime, money)
    local node = {}
    node["CMOrderID"]   = cmOrderID
    node["SkillName"]   = skillName
    node["Direction"]   = direction
    node["PrescripName"] = prescripName
    node["EndTime"]     = leftTime
    node["Money"]       = money
    node["PlayerName"]  = playerName 
    table.insert(g_GameMain.m_ContractManufactureMarketWnd.m_MarketOrderInfoTbl, node)
end

function CContractManufactureMarketWnd.RetCommonCMOrderMaterialInfo(cmOrderID, index, materialType, materialName, materialCount)
       if g_GameMain.m_ContractManufactureMarketWnd.m_MaterialInfoTbl[cmOrderID] == nil then
        g_GameMain.m_ContractManufactureMarketWnd.m_MaterialInfoTbl[cmOrderID] = {}
    end
    g_GameMain.m_ContractManufactureMarketWnd.m_MaterialInfoTbl[cmOrderID][index] = {materialType, materialName, materialCount}
end

function CContractManufactureMarketWnd.RetCommonCMOrderEnd()

	totalPage = math.ceil(# g_GameMain.m_ContractManufactureMarketWnd.m_MarketOrderInfoTbl /10)
	g_GameMain.m_ContractManufactureMarketWnd.m_MaxPageNum = totalPage
	
	g_GameMain.m_ContractManufactureMarketWnd:getsortInfoTbl()
	g_GameMain.m_ContractManufactureMarketWnd:setPage()
	g_GameMain.m_ContractManufactureMarketWnd:DrawCommonCMOrderList()

    
    if table.getn(g_GameMain.m_ContractManufactureMarketWnd.m_MarketOrderInfoTbl) == 0 then
        g_GameMain.m_ContractManufactureMarketWnd.m_Text:ShowWnd(true)
    else
        g_GameMain.m_ContractManufactureMarketWnd.m_Text:ShowWnd(false) 
    end
end

function CContractManufactureMarketWnd:getsortInfoTbl()

	local LiveSkillDirectionTypeTbl = {}
	LiveSkillDirectionTypeTbl["战神"] = 1
	LiveSkillDirectionTypeTbl["法魔"] = 2
    for i = 1,# self.m_MarketOrderInfoTbl do
		local prescripName = self.m_MarketOrderInfoTbl[i]["PrescripName"]
		local cmOrderId   = self.m_MarketOrderInfoTbl[i]["CMOrderID"]
		local leftTime    = self.m_MarketOrderInfoTbl[i]["EndTime"]
    	local money       = self.m_MarketOrderInfoTbl[i]["Money"]
		local playerName  = self.m_MarketOrderInfoTbl[i]["PlayerName"]
		local skillName   = self.m_MarketOrderInfoTbl[i]["SkillName"]
    	local direction   = self.m_MarketOrderInfoTbl[i]["Direction"]
    	
		local productType = LiveSkill_Common(skillName, direction, prescripName, "ProductType1")
    	local productName = LiveSkill_Common(skillName, direction, prescripName, "ProductName1")
    	
    	local skillLevel = LiveSkill_Common(skillName, direction, prescripName, "SkillLevel")
		local displayProductName = g_ItemInfoMgr:GetItemLanInfo(productType, productName,"DisplayName")	--物品名称
		local productLevel = g_ItemInfoMgr:GetItemInfo(productType, productName, "BaseLevel")    --需要等级
		local displaySkillName = Lan_LiveSkillName_Client(MemH64(skillName), "DisplayName")
		self.m_MarketOrderInfoTbl[i]["Level"] = productLevel
		self.m_MarketOrderInfoTbl[i]["displayProductName"] = displayProductName
		
		local displayDirectionName = ""
		if LiveSkillDirectionTypeTbl[direction] == nil then
        	local directioName = Lan_Practiced_Common(MemH64(skillName .. direction), "DisplayName")
        	displayDirectionName = directioName .. Lan_DirectionType_Common(MemH64("专精"), "DisplayName")
    	else
        	displayDirectionName = Lan_DirectionType_Common(MemH64(direction), "DisplayName")
    	end
    	local levelStr = Lan_SmithingExpert_Common(skillLevel, "DisplayName")
    	local displaySkill = displayDirectionName .. levelStr
    	self.m_MarketOrderInfoTbl[i]["SortSkillName"] = displaySkillName..displaySkill	--技能名称加等级
    	
    end

end

function CContractManufactureMarketWnd:setPage()	--设置分页，显示分页内容

	self.ShowOrderInfoTbl={}		--清空显示内容
	local startNumber = (g_GameMain.m_ContractManufactureMarketWnd.m_CurPageNum - 1)*10 + 1
--	local endNumber	= startNumber+(9 and (# self.m_MarketOrderInfoTbl - startNumber))
	if((# self.m_MarketOrderInfoTbl - startNumber)>9) then
		endNumber = startNumber+9
	elseif((# self.m_MarketOrderInfoTbl - startNumber)<=9) then
		endNumber = # self.m_MarketOrderInfoTbl 
	end
	for i = startNumber , endNumber do
		table.insert(self.ShowOrderInfoTbl ,self.m_MarketOrderInfoTbl[i])
	end

end

function CContractManufactureMarketWnd:sortInfoTbl(p,bFlag)	--按一定的顺序排序

	g_GameMain.m_ContractManufactureMarketWnd:getsortInfoTbl()

	if(bFlag) then
		table.sort(self.m_MarketOrderInfoTbl, function (a, b) return a[p] < b[p] end)
	else
		table.sort(self.m_MarketOrderInfoTbl, function (a, b) return a[p] > b[p] end)
	end
	g_GameMain.m_ContractManufactureMarketWnd:setPage()
	g_GameMain.m_ContractManufactureMarketWnd:DrawCommonCMOrderList()
end

function CContractManufactureMarketWnd.RetTakeContractManufactureOrder() 
    g_GameMain.m_ContractManufactureMarketWnd:SendCMCommonSearchInfo()
end


local function CreateContractManufactureMarket(wnd)
    wnd:CreateFromRes("ContractManufatureMarket", g_GameMain)
    wnd:InitChild()
    wnd.m_CurPageNum = 1
    wnd.m_MaxPageNum = 0
    wnd.m_ChoosedCMOrderIndex = 0
    g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:SetStyle(0x00040000)
end

function CContractManufactureMarketWnd:Ctor()
    CreateContractManufactureMarket(self)
end

function CContractManufactureMarketWnd.OpenContractManufMarketWnd()
    g_GameMain.m_ContractManufactureMarketWnd:ShowWnd(true)
    g_GameMain.m_ContractManufactureMarketWnd:SetCurMoneyStr()
    g_GameMain.m_ContractManufactureMarketWnd.m_Text:ShowWnd(false) 
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_ContractManufactureMarketWnd)
end

local LiveSkillNameTbl  = {}	--存放技能名字，锻造，铸甲，工艺，烹饪，制药，花卉，采矿
local LiveSkillLevelTbl = {}
local LiveSkillLevelOrderTbl = {}
local function ReadLiveSkillInfo()
    local keys = LiveSkill_Common:GetKeys()
    for i, v in pairs (keys) do
        local skillName = v
        local displaySkillName = Lan_LiveSkillName_Client(MemH64(skillName), "DisplayName")
        LiveSkillNameTbl[displaySkillName] = skillName
    end
    
    local levelKeys = Lan_SmithingExpert_Common:GetKeys()
    for i,v in pairs (levelKeys) do
        local skillLevelNum =  SmithingExpert_Common(v, "ExpertLevel")
        local displaySkillLevel = Lan_SmithingExpert_Common(v, "DisplayName")
        LiveSkillLevelTbl[displaySkillLevel] = skillLevelNum
        if skillLevelNum > 0 then
            LiveSkillLevelOrderTbl[skillLevelNum] = displaySkillLevel
        end
    end
end
ReadLiveSkillInfo()


function CContractManufactureMarketWnd:InitChild()
    self.m_CloseBtn         =   self:GetDlgChild("CloseBtn")
    self.m_OrderListCtrl    =   self:GetDlgChild("OrderList")
    self.m_PrePageBtn       =   self:GetDlgChild("PrePageBtn")
    self.m_NextPageBtn      =   self:GetDlgChild("NextPageBtn")
    self.m_CurPageText      =   self:GetDlgChild("CurPageText")
    self.m_OrderKeyNameEditor    =   self:GetDlgChild("OrderKeyNameEditor")
    self.m_OrderLowLevelEditor   =   self:GetDlgChild("OrderLowLevelEditor")
    self.m_OrderHighLevelEditor  =   self:GetDlgChild("OrderHighLevelEditor")
    self.m_SkillTypeComBox  =   self:GetDlgChild("SkillTypeComBox")
    self.m_SkillLevelCombox =   self:GetDlgChild("SkillLevelCombox")
    self.m_SearchBtn        =   self:GetDlgChild("SearchBtn")
    self.m_ResetBtn         =   self:GetDlgChild("ResetBtn")
    self.m_MakeOrderBtn     =   self:GetDlgChild("MakeOrder")
    self.m_TakeOrderBtn     =   self:GetDlgChild("TakeOrder")
    
    self.m_JinCount         =   self:GetDlgChild("JinCount")
    self.m_YinCount         =   self:GetDlgChild("YinCount")
    self.m_TongCount        =   self:GetDlgChild("TongCount")
    self.m_Jin              =   self:GetDlgChild("Jin")
    self.m_Yin              =   self:GetDlgChild("Yin")
    self.m_Tong             =   self:GetDlgChild("Tong")

	self.m_tblSortBtn = {}
	self.m_tblSortBtn["displayProductName"]	= self:GetDlgChild("ProductName")
	self.m_tblSortBtn["SortSkillName"]		= self:GetDlgChild("SkillName")
	self.m_tblSortBtn["Money"]			= self:GetDlgChild("Fee")
	self.m_tblSortBtn["Level"]			= self:GetDlgChild("Level")
	self.m_tblSortBtn["EndTime"]			= self:GetDlgChild("Time")
	self.m_tblSortBtn["PlayerName"]			= self:GetDlgChild("OrderPlayer")
	for p, v in pairs(self.m_tblSortBtn) do
		v.bFlag = true
	end
	self.m_MarketOrderInfoTbl = {}
	self.ShowOrderInfoTbl	={}			--用于显示当前页的信息表
    self.m_Text             =   self:GetDlgChild("Text")
    self.m_Text:ShowWnd(false)
    self:InitSkillLevelAndNameComBox()
    self.m_JinCount:SetFocus()
end

local NoneChoiseOfSkillName = GetStaticTextClient(1288)

function CContractManufactureMarketWnd:InitSkillLevelAndNameComBox()
    local skillNameCount =  # LiveSkillNameTbl
   	local comBoxWidth = self.m_SkillTypeComBox:GetWndWidth()
	local comBoxItemHeight = 20  
	self.m_SkillTypeComBox:InsertColumn(0, comBoxWidth)
	self.m_SkillTypeComBox:InsertItem(NoneChoiseOfSkillName, 0, comBoxItemHeight)
	local index = 1
    for i, v in pairs (LiveSkillNameTbl) do
        self.m_SkillTypeComBox:InsertItem(i, index ,comBoxItemHeight)
        index = index + 1
    end
    self.m_SkillTypeComBox:SetWndText(NoneChoiseOfSkillName)

    local skillNameCount =  # LiveSkillLevelTbl
   	local comBoxWidth2 = self.m_SkillLevelCombox:GetWndWidth()
	self.m_SkillLevelCombox:InsertColumn(0, comBoxWidth2)
	self.m_SkillLevelCombox:InsertItem(NoneChoiseOfSkillName, 0, comBoxItemHeight)

    for i=1, # LiveSkillLevelOrderTbl do
        self.m_SkillLevelCombox:InsertItem(LiveSkillLevelOrderTbl[i], i ,comBoxItemHeight)
    end 
    self.m_SkillLevelCombox:SetWndText(NoneChoiseOfSkillName)
end

function CContractManufactureMarketWnd:ResetContractManufactureInfo()
    self.m_OrderKeyNameEditor:SetWndText("")
    self.m_OrderLowLevelEditor:SetWndText("")
    self.m_OrderHighLevelEditor:SetWndText("")
    self.m_SkillTypeComBox:SetWndText("")
    self.m_SkillLevelCombox:SetWndText("")
    self.m_SkillTypeComBox:SetWndText(NoneChoiseOfSkillName)
    self.m_SkillLevelCombox:SetWndText(NoneChoiseOfSkillName)
end

function CContractManufactureMarketWnd:CheckCMCommonSearchInfo()
    local lowLevel = tonumber(self.m_OrderLowLevelEditor:GetWndText()) or 1
    local highLevel= tonumber(self.m_OrderHighLevelEditor:GetWndText()) or 80
    if  lowLevel < 1 or highLevel > 80 then
       MessageBox( self, MsgBoxMsg(154), MB_BtnOK )
       return false 
    end
    if lowLevel > highLevel then
        MessageBox( self, MsgBoxMsg(154), MB_BtnOK )
        return false  
    end
    return true
end

function CContractManufactureMarketWnd:GetCMMarketSearchInfo()
    local productName       = self.m_OrderKeyNameEditor:GetWndText()
    local skillName         = self.m_SkillTypeComBox:GetWndText()
    local skillLevel        = self.m_SkillLevelCombox:GetWndText()
    local productLowLevel   = tonumber(self.m_OrderLowLevelEditor:GetWndText()) or 0
    local productHighLevel  = tonumber(self.m_OrderHighLevelEditor:GetWndText()) or 0

    if skillName == NoneChoiseOfSkillName then
        skillName = ""
    else
        skillName = LiveSkillNameTbl[skillName]
    end
    if skillLevel == NoneChoiseOfSkillName then
        skillLevel = 0
    else
        skillLevel = LiveSkillLevelTbl[skillLevel]
    end
    return productName, skillName, skillLevel, productLowLevel, productHighLevel
end


function CContractManufactureMarketWnd:SendCMCommonSearchInfo()
    local ret = self:CheckCMCommonSearchInfo()
    if ret then
        local productName, skillName, skillLevel, productLowLevel, productHighLevel =self:GetCMMarketSearchInfo()
        Gac2Gas:SearchCommonContractManuOrder(g_Conn, productName, skillName, skillLevel, productLowLevel, productHighLevel, self.m_CurPageNum)
    end
end


function CContractManufactureMarketWnd:GetTakeCMOrderTipsInfo()
    local index = self.m_ChoosedCMOrderIndex
    local skillName = self.m_MarketOrderInfoTbl[index]["SkillName"]
    local direction = self.m_MarketOrderInfoTbl[index]["Direction"]
    local prescripName = self.m_MarketOrderInfoTbl[index]["PrescripName"]
    local productType = LiveSkill_Common(skillName, direction, prescripName,"ProductType1")
    local productName = LiveSkill_Common(skillName, direction, prescripName,"ProductName1")
    local displayProductName = g_ItemInfoMgr:GetItemLanInfo(productType, productName,"DisplayName")
    
    local money = self.m_MarketOrderInfoTbl[index]["Money"]
    gac_gas_require "framework/common/CMoney"
    local moneyMgr = CMoney:new()
    
    local moneyStr = moneyMgr:ChangeMoneyToString(money, EGoldType.GoldCoin)
    local makeProductMoney = math.ceil( money * 0.95)
    local sysFee = money - makeProductMoney
    local makeProductMoneyStr = moneyMgr:ChangeMoneyToString(makeProductMoney, EGoldType.GoldCoin)
    local sysFeeStr = moneyMgr:ChangeMoneyToString(sysFee)
    
    local skillName = self.m_MarketOrderInfoTbl[index]["SkillName"]
    local direction = self.m_MarketOrderInfoTbl[index]["Direction"]
    
    local coolTime = LiveSkill_Common(skillName, direction, prescripName, "CoolTime")
 
    local hour = tonumber(string.format("%.0f", coolTime/3600))
    local minute =  tonumber(string.format("%.0f",  (coolTime - hour*3600)/60))
    local second = tonumber(string.format("%.0f",  coolTime - hour * 3600 - minute *60))
  
    local coolTimeStr = ""
    if hour > 0 then
       coolTimeStr = hour .. GetStaticTextClient(1121)
    end
    if minute > 0 then
        coolTimeStr =coolTimeStr .. minute .. GetStaticTextClient(1120)
    end
    if second > 0 then
        coolTimeStr = coolTimeStr .. second .. GetStaticTextClient(1119)
    end
    return displayProductName, moneyStr, makeProductMoneyStr, coolTimeStr
end

function CContractManufactureMarketWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
            
        elseif Child ==  self.m_PrePageBtn then
            if self.m_CurPageNum > 1 then
                self.m_CurPageNum = self.m_CurPageNum - 1
				self:RetCommonCMOrderEnd()
            end
        elseif Child == self.m_NextPageBtn then
            if self.m_CurPageNum < self.m_MaxPageNum then
                self.m_CurPageNum = self.m_CurPageNum + 1 
              --  self:SendCMCommonSearchInfo()
                self:RetCommonCMOrderEnd()
            end
            
        elseif Child == self.m_SearchBtn then
            self.m_CurPageNum = 1
            self:SendCMCommonSearchInfo()
            
        elseif Child == self.m_ResetBtn then
            self:ResetContractManufactureInfo()
        
        elseif Child == self.m_MakeOrderBtn then
            self:ShowWnd(false)
            _,g_GameMain.m_ContractManufactureOrderWnd  = apcall(CContractManufactureOrderWnd.new, CContractManufactureOrderWnd)
            g_GameMain.m_ContractManufactureOrderWnd.m_CurPageNum = 1
            Gac2Gas:SearchCharContractManufactureOrder(g_Conn, g_GameMain.m_ContractManufactureOrderWnd.m_CurPageNum)
            g_GameMain.m_ContractManufactureOrderWnd:ShowWnd(true)
            
        elseif Child == self.m_TakeOrderBtn then
            if g_MainPlayer.m_ItemBagLock then
                MsgClient(160044)
                return
            end
            if self.m_ChoosedCMOrderIndex == 0 then
                return  
            end
            local cmOrderID = self.m_MarketOrderInfoTbl[self.m_ChoosedCMOrderIndex]["CMOrderID"]
            local productName, money, makeProductMoney, coolTime= self:GetTakeCMOrderTipsInfo()
            makeProductMoney = "" --目前屏蔽了关于交易税的显示
            local function CallBack(Context, Button)
				if(Button == MB_BtnOK) then
                    Gac2Gas:TakeContractManufactureOrder(g_Conn, cmOrderID)
                end
                return true
            end
            MessageBox(self, MsgBoxMsg(20002, productName, money, coolTime), BitOr( MB_BtnOK, MB_BtnCancel), CallBack)
		else
			for p, v in pairs(self.m_tblSortBtn) do
				if(Child == v) then
					self:sortInfoTbl(p, v.bFlag)
					v.bFlag = not v.bFlag
					break
				end
			end
		end

    elseif uMsgID == WND_NOTIFY  then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
		    if Child == self.m_OrderLowLevelEditor then
                local commerceCommon = CCommerceCommon:new()
                commerceCommon:CheckInputLevel(Child, 1)
            end
		    if Child == self.m_OrderHighLevelEditor then
                local commerceCommon = CCommerceCommon:new()
                commerceCommon:CheckInputLevel(Child, 150)
		    end
        end            
    end
end


function CContractManufactureMarketWnd:SetCurMoneyStr()
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()

	local tblWnd = {self.m_JinCount, self.m_Jin, self.m_YinCount, self.m_Yin, self.m_TongCount, self.m_Tong}
	money:ShowMoneyInfo(g_MainPlayer.m_nMoney,tblWnd)
end

function CContractManufactureMarketWnd:DrawCommonCMOrderList()
    self.m_ChoosedCMOrderIndex = 0
    self.m_TakeOrderBtn:EnableWnd(false)
    local cmOrderInfoTbl     =  self.m_MarketOrderInfoTbl
    local cmOrderItemInfoTbl =  self.m_MaterialInfoTbl
    
    self.m_OrderListCtrl:DeleteAllItem()
    self.m_OrderListCtrl:InsertColumn(0, 660)
    self.m_MarketOrderItemTbl = {}
    self.m_MarketOrderItemWndTbl = {}
    self.m_MarketOrderChkBtnTbl = {}
    
    if self.m_MaxPageNum == 0 then
        self.m_CurPageText:SetWndText(0 .. "/" .. 0) 
    else
        self.m_CurPageText:SetWndText(self.m_CurPageNum .. "/" .. self.m_MaxPageNum)    
    end
    
    local orderCount =  # self.ShowOrderInfoTbl
    for i= 1, orderCount do
        self.m_OrderListCtrl:InsertItem(i-1, 50)
        local item = self.m_OrderListCtrl:GetSubItem(i-1, 0)
        local itemWnd = CContractManufactureMarketOrderItemWnd:new()
        itemWnd:CreateFromRes("ContractManufatureMarketItem", item)
        itemWnd:ShowWnd(true)
        itemWnd.Index = i +(self.m_CurPageNum-1)*10
        
        itemWnd:SetMarketCMOrderDetail(i)
        
        table.insert(self.m_MarketOrderItemTbl, item)
        table.insert(self.m_MarketOrderItemWndTbl, itemWnd)
        local chkBtn = itemWnd:GetDlgChild("OrderChkBtn")
        chkBtn.Index = i +(self.m_CurPageNum-1)*10
        table.insert(self.m_MarketOrderChkBtnTbl, chkBtn)
    end    
end



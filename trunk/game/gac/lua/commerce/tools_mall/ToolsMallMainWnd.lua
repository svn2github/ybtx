CToolsMallMainWnd = class (SQRDialog)

cfg_load "commerce/ToolsMall_Common"
cfg_load "commerce/ToolsMallItemPrice_Common"

local function CloseToolsMallBuyOrGiveWnd()
    if g_GameMain.m_ToolsMallGiveItemWnd:IsShow() then
        g_GameMain.m_ToolsMallGiveItemWnd:ShowWnd(false)
    end
    if g_GameMain.m_ToolsMallBuyWnd:IsShow() then
         g_GameMain.m_ToolsMallBuyWnd:ShowWnd(false)
    end
end


function CToolsMallMainWnd.RetTakeYongBingBi(yongBingBiCount)
    if yongBingBiCount == 0 then
        if g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd == nil then
            g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd  = CToolsMallShowYongBingBiWnd:new()
        end
        g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:ShowWnd(true)
        g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:SetFocus()
    else
        g_GameMain.m_ToolsMallWnd.m_MaxYongBingBi = yongBingBiCount
        if g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd == nil then
           g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd = CToolsMallTakeMoneyWnd:new()
        end
        g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:ShowWnd(true)
        g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:SetFocus()
        g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd.m_MoneyCountBtn:SetWndText(1)
        g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd.m_Text:SetWndText(GetStaticTextClient(310000,yongBingBiCount))
    end
    CloseToolsMallBuyOrGiveWnd()
end


function CToolsMallMainWnd.RetHotSaleItemEnd(type)
    g_GameMain.m_ToolsMallWnd.m_HotLookHotSaleItemTbl = {}
    g_GameMain.m_ToolsMallWnd.m_HotSellListWnd:DeleteAllItem()
    local toolsMallWnd = g_GameMain.m_ToolsMallWnd
    local tblFromDB = toolsMallWnd.m_HotSaleItemInfoTblFromDB
    local tblFromCfg = toolsMallWnd.m_HotLookHotSaleItemTblFromCfg
    for i,v in pairs (tblFromDB) do
        local itemName = i
        if tblFromCfg[itemName] ~= nil then
            local nBigID, itemCount = tblFromCfg[2], tblFromCfg[3]
            if nBigID == itemType then
                local soldCount = nCount
                if nCount > itemCount then
                    tblFromCfg[itemName] = {nBigID, soldCount}
                end
                tblFromDB[itemName] = 0 
            end
        end
        if tblFromDB[itemName] ~= 0 then
            table.insert(toolsMallWnd.m_HotLookHotSaleItemTb, {itemType,itemName, soldCount})
        end   
    end
    for i, v in pairs (tblFromCfg) do
        local itemName = i
        table.insert(toolsMallWnd.m_HotLookHotSaleItemTbl, v)
    end
    
    
    table.sort(toolsMallWnd.m_HotLookHotSaleItemTbl, function(a,b) return (a[3]>b[3])end)
	toolsMallWnd:DrawHotWndList(type)
end

local TabMaxNum = 8     --标签栏共有数目：礼包、时装、坐骑、药品、装备、宝石、书画、其他
local SortMaxNum = 4    --筛选显示按钮数目：新品、热卖、折价、全部

local function CreateToolsMallMainWnd(wnd)
    wnd:CreateFromRes("ToolsMall", g_GameMain)
    wnd:InitToolsMallMainWndChild()
    wnd.m_HotLookTmpDB = {}       --存数据空中热卖信息（供热门看板用）
    wnd.m_CurChoosedInHotWnd = 1    --热门看板当前选择的类型
    wnd.m_CurPageNo = 1
    wnd:InitToolsMallSellItemInfo() --从配置表取数据
    wnd:ResetToolsMallInfo()
    wnd:DrawSellListWnd()
    OnCreateRoleModel(wnd)
    g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
end

function CToolsMallMainWnd:Ctor()
    CreateToolsMallMainWnd(self)
end

--初始化道具商城面板中的子控件
function CToolsMallMainWnd:InitToolsMallMainWndChild()
    self.m_CloseBtn         =   self:GetDlgChild("CloseBtn")
    self.m_MinBtn           =   self:GetDlgChild("MinBtn")
    self.m_TabCtrlTbl       =   {}
    for i=1, TabMaxNum do
        local tabStr = "Tab" .. i
        table.insert(self.m_TabCtrlTbl, self:GetDlgChild(tabStr))
    end
    self.m_SortCtrlTbl      =   {}
    for i=1, SortMaxNum do
        local sortTabStr = "Sort" .. i
        table.insert(self.m_SortCtrlTbl, self:GetDlgChild(sortTabStr))
    end
    self.m_NewItemBtn       =   self:GetDlgChild("New")
    self.m_HotSaleBtn       =   self:GetDlgChild("HotSale")
    self.m_OffPriceBtn      =   self:GetDlgChild("OffPrice")	--现在不能用  特价
    self.m_AllItemBtn       =   self:GetDlgChild("All")			--现在不能用  许愿
    
    self.m_OffPriceBtn:EnableWnd(false)    
    self.m_AllItemBtn:EnableWnd(false) 
    
    self.m_YongBingBiBtn    =   self:GetDlgChild("BiBtn")
    self.m_YongBingQuanBtn  =   self:GetDlgChild("QuanBtn")
    self.m_JiFenBtn         =   self:GetDlgChild("JiFenBtn")
    self.m_FirstPageBtn     =   self:GetDlgChild("FirstPage")
    self.m_NextPageBtn      =   self:GetDlgChild("NextPage")
    self.m_PrePageBtn       =   self:GetDlgChild("PrePage")
    self.m_LastPageBtn      =   self:GetDlgChild("lastPage")
    self.m_BuyHistoryBtn    =   self:GetDlgChild("BuyHistory")	 --现在不能用
 
    self.m_BuyHistoryBtn:EnableWnd(false) 
    
    self.m_SellListWnd      =   self:GetDlgChild("SellList")
    self.m_HotSellListWnd   =   self:GetDlgChild("HotSellList")
    self.m_HotSellListWnd:SetMouseWheel(true)
    self.m_PlayerLevel      =   self:GetDlgChild("LevelText")
    self.m_JiFenCount       =   self:GetDlgChild("JiFenCount")
    self.m_WndBody			=   self:GetDlgChild("ModleWnd")
    self.m_LeftBtn			=	self:GetDlgChild("Left")
    self.m_RightBtn			=	self:GetDlgChild("Right")
    self.m_RenewBtn			=	self:GetDlgChild("Renew")
    self.m_TakeYongBingBi   =   self:GetDlgChild("TakeYongBingBi")
    
    --self.m_TakeYongBingBi:SetMouseOverDescAfter(ToolTipsText(1100))
    self.m_TakeYongBingBi:EnableWnd(false)

    self.m_nDir				= 128   
    self:SetMouseWheel(true)
    
    self.m_BindingJinCount  =   self:GetDlgChild("BindingJinCount")
    self.m_BindingYinCount  =   self:GetDlgChild("BindingYinCount")
    self.m_BindingTongCount =   self:GetDlgChild("BindingTongCount")
    self.m_BindingJinImage  =   self:GetDlgChild("BindingJinImage")
    self.m_BindingYinImage  =   self:GetDlgChild("BindingYinImage")
    self.m_BindingTongImage =   self:GetDlgChild("BindingTongImage")
    
    self.m_JinCount         =   self:GetDlgChild("JinCount")
    self.m_YinCount         =   self:GetDlgChild("YinCount")
    self.m_TongCount        =   self:GetDlgChild("TongCount")
    self.m_JinImage         =   self:GetDlgChild("JinImage")
    self.m_YinImage        =   self:GetDlgChild("YinImage")
    self.m_TongImage        =   self:GetDlgChild("TongImage")
end

--读道具商城配置表，将新品、折价、所有物品的信息分类存储在不同的表中											
function CToolsMallMainWnd:InitToolsMallSellItemInfo()															
    self.m_AllSellItemInfoTbl   = {}        --所有物品信息表
    self.m_OffPriceItemInfoTbl  = {}        --折价物品信息表
    self.m_NewItemInfoTbl   = {}            --新品信息表
    self.m_HotLookHotSaleItemTblFromCfg = {}   --配置表中配置的在热门看板显示的热卖商品
    self.m_HotSaleItemInfoTbl = {}          --用于在主界面显示的热卖商品
    self.m_HotLookNewSaleItemInfoTbl = {}
   
    local Tabs = ToolsMall_Common:GetKeys()
	for j =1,#Tabs do
		local Tabj = Tabs[j]
		local DefaultKeys = ToolsMall_Common(Tabj):GetKeys()
		for i =1,#DefaultKeys do
			local v = ToolsMall_Common(Tabj,DefaultKeys[i])
        	local saleTabType = tonumber(Tabj)
	        if self.m_AllSellItemInfoTbl[saleTabType] == nil then
	            self.m_AllSellItemInfoTbl[saleTabType] = {}
	            self.m_OffPriceItemInfoTbl[saleTabType]= {}
	            self.m_NewItemInfoTbl[saleTabType] = {}
	            self.m_HotSaleItemInfoTbl[saleTabType]= {}
	        end
	        local itemType = v("ItemType")
	        local itemName = v("ItemName")
	        local offPrice = v("OffPrice")
	        local isNewItem= v("IsNewItem")
	        local isHotSale = v("HotSale")

	        local node1 = {itemType, itemName}
	        table.insert(self.m_AllSellItemInfoTbl[saleTabType], node1)
	        if offPrice ~= 0 then
	            local node2 = {itemType, itemName, offPrice}
	            table.insert(self.m_OffPriceItemInfoTbl[saleTabType], node2)
	        end
	        if isNewItem == 1 then
	            local newOrderNum = v("NewOrderNum")
	            local node3 = {itemType, itemName, newOrderNum}
	            table.insert(self.m_NewItemInfoTbl[saleTabType], node3)
	            table.insert(self.m_HotLookNewSaleItemInfoTbl, node3)
	        end
	        
	        if isHotSale == 1 then
	        	local sellCount = v("SellCount")
                local node4 = {itemType, itemName, sellCount}
                self.m_HotLookHotSaleItemTblFromCfg[itemName] = node4
               
                local node5 = {itemType, itemName}
                table.insert(self.m_HotSaleItemInfoTbl[saleTabType], node5)
       		end
       	end
    end

    table.sort(self.m_HotLookNewSaleItemInfoTbl, function(a,b) return (a[3]>b[3])end)    
end

function CToolsMallMainWnd.OpenToolsMallMainWnd()
    local toolsMallShowFlag = not g_GameMain.m_ToolsMallWnd:IsShow()
    g_GameMain.m_ToolsMallWnd:ShowWnd(toolsMallShowFlag)
    g_GameMain.m_ToolsMallWnd:UpdatePlayerLevel()
    g_GameMain.m_ToolsMallWnd:UpdateBindingMoneyCount()
    g_GameMain.m_ToolsMallWnd:UpdateMoneyCount()
    g_GameMain.m_ToolsMallWnd.m_JiFenCount:SetWndText(0)
    g_GameMain.m_ToolsMallWnd:InitModel(false)
    g_GameMain.m_ToolsMallWnd:DrawSellListWnd()
    g_GameMain.m_ToolsMallWnd:DrawHotWndList( g_GameMain.m_ToolsMallWnd.m_CurChoosedInHotWnd)
    g_GameMain.m_ToolsMallWnd.m_ArmorItemNameTbl  = {}
end

function CToolsMallMainWnd:UpdatePlayerLevel()
    local level = g_MainPlayer:CppGetLevel()
    g_GameMain.m_ToolsMallWnd.m_PlayerLevel:SetWndText(level)
end

function CToolsMallMainWnd:UpdateMoneyCount()
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()

	--local tblWnd = {self.m_JinCount, self.m_JinImage, self.m_YinCount, self.m_YinImage, self.m_TongCount, self.m_TongImage}
	--money:ShowMoneyInfo(g_MainPlayer.m_nMoney,tblWnd)
    self.m_JinCount:SetWndText(tostring(g_MainPlayer:GetGold())) 
	self.m_YinCount:SetWndText(tostring(g_MainPlayer:GetArgent())) 
	self.m_TongCount:SetWndText(tostring(g_MainPlayer:GetCopper())) 
end

function CToolsMallMainWnd:UpdateBindingMoneyCount()
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()

	--local tblWnd = {self.m_BindingJinCount, self.m_BindingJinImage, self.m_BindingYinCount, self.m_BindingYinImage, self.m_BindingTongCount, self.m_BindingTongImage}
	--money:ShowMoneyInfo(g_MainPlayer.m_nBindMoney,tblWnd)
    self.m_BindingJinCount:SetWndText(tostring(g_MainPlayer:GetBindingGold())) 
	self.m_BindingYinCount:SetWndText(tostring(g_MainPlayer:GetBindingArgent())) 
	self.m_BindingTongCount:SetWndText(tostring(g_MainPlayer:GetBindingCopper())) 
end

--模型的左旋转
local function DrawRoleLeftTick(Tick, RoleStatus )
	local dir = RoleStatus.m_nDir - 5
	RoleStatus.m_nDir = (dir > 0 and 255 or 0) + dir
	RoleStatus:OnDrawRole(RoleStatus.m_LeftBtn)
end

--模型的右旋转
local function DrawRoleRightTick(Tick, RoleStatus )
	local dir = RoleStatus.m_nDir + 5
	RoleStatus.m_nDir = (dir > 255 and -255 or 0) + dir
	RoleStatus:OnDrawRole(RoleStatus.m_RightBtn)
end

function CToolsMallMainWnd:VirtualExcludeWndClosed()
    g_GameMain.m_ToolsMallBuyWnd:ShowWnd(false)
    g_GameMain.m_ToolsMallGiveItemWnd:ShowWnd(false)
    if g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd then
       g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:ShowWnd(false) 
    end
    if g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd then
       g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:ShowWnd(false) 
    end
end


function CToolsMallMainWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_CLICKDOWN then
		if Child == self.m_LeftBtn and self.m_DrawTick == nil then
			self.m_nHasDraw = false
			self.m_DrawTick = RegisterTick("DrawRoleLeftTick", DrawRoleLeftTick,33,self)
		elseif Child == self.m_RightBtn and self.m_DrawTick == nil then
			self.m_nHasDraw = false
			self.m_DrawTick = RegisterTick("DrawRoleRightTick", DrawRoleRightTick,33,self)
		end	
    elseif (uMsgID == BUTTON_LCLICK) then
        if (Child == self.m_CloseBtn) then
        	if self.m_DrawTick ~= nil then
				UnRegisterTick(self.m_DrawTick)
				self.m_DrawTick = nil
			end
            g_GameMain.m_ToolsMallWnd:ShowWnd(false)
        
        elseif (Child == self.m_FirstPageBtn) then
            self.m_CurPageNo = 1
            self:DrawSellListWnd()
         
        elseif (Child == self.m_LastPageBtn) then
            self.m_CurPageNo = self.m_MaxPageNo
            self:DrawSellListWnd()
            
        elseif (Child == self.m_NextPageBtn) then
        	self.m_CurPageNo = self.m_CurPageNo + 1
            self:DrawSellListWnd()
                   
        elseif (Child == self.m_PrePageBtn) then
            self.m_CurPageNo = self.m_CurPageNo - 1
            self:DrawSellListWnd()
            
        elseif (Child == self.m_NewItemBtn) then
            self.m_CurChoosedInHotWnd = 1   --热门看板中的“新品”标签页
            self:DrawHotWndList(self.m_CurChoosedInHotWnd)
            
        elseif (Child == self.m_HotSaleBtn) then
            self.m_CurChoosedInHotWnd = 2	--热门看板中的“热卖”标签页
            Gac2Gas:GetHotSaleItemInfo(g_Conn, self.m_CurChoosedInHotWnd)	
            
        elseif (Child == self.m_OffPriceBtn) then
            self.m_CurChoosedInHotWnd = 3	--热门看板中的“折扣”标签页
            local type = self.m_CurChoosedInHotWnd
            self:DrawHotWndList()
            
        elseif (Child == self.m_AllItemBtn) then
            self.m_CurChoosedInHotWnd = 4	--热门看板中的“许愿”标签页
            local type = self.m_CurChoosedInHotWnd
            self:DrawHotWndList()
            
        elseif (Child == self.m_LeftBtn) then
        	if self.m_DrawTick ~= nil then
				if self.m_nHasDraw == false then
					DrawRoleLeftTick(nil, self )
				end
			end
        elseif (Child == self.m_RightBtn) then
      		if self.m_DrawTick ~= nil then
				if self.m_nHasDraw == false then
					DrawRoleRightTick(nil, self )
				end
			end
        elseif (Child == self.m_RenewBtn) then
            self.m_nDir = 128
            self:InitModel(false)
        elseif (Child == self.m_TakeYongBingBi) then
            Gac2Gas:TakeYongBingBi(g_Conn)
        else
            self:CheckWhichOrderTypeChoosed(Child)
        end 
    end
end


function CToolsMallMainWnd:ResetToolsMallInfo()
    self.m_CurChoosedTabType    = 1         --商城上方的标签栏，当前选中的tab,
    self.m_CurChoosedSortType   = 1         --每个tab中，新品、特价、热卖、全部中的当前筛选项
    self.m_CurPageNo            = 1         --当前page
end

function CToolsMallMainWnd:SellItemInfoTblList(index)
    local SellItemInfoTblList = {}      --通过点击新品、热卖、折价、全部按钮，索引到不同的物品表
    SellItemInfoTblList[1] = self.m_NewItemInfoTbl     --保存新品信息表
    SellItemInfoTblList[2] = self.m_HotSaleItemInfoTbl --保存热卖物品信息表
    SellItemInfoTblList[3] = self.m_OffPriceItemInfoTbl--保存折价物品信息表
    SellItemInfoTblList[4] = self.m_AllSellItemInfoTbl --保存所有物品信息表
    return SellItemInfoTblList[index]
end

function CToolsMallMainWnd:DrawListItem(itemWnd, item, itemTbl, index, showOffPrice)
    itemWnd:CreateFromRes("ToolsMallItem", item)
    itemWnd:ShowWnd( true )
    itemWnd:SetMouseWheel(true)
    itemWnd:GetDlgChild("Buy"):SetMouseWheel(true)
    itemWnd:GetDlgChild("Give"):SetMouseWheel(true)
    itemWnd:GetDlgChild("Give"):EnableWnd(false)
    itemWnd:GetDlgChild("BigIconWnd"):SetFocus()
    itemWnd:GetDlgChild("BigIconWnd"):SetMouseWheel(true)
    local itemType = itemTbl[index][1]
    local itemName = itemTbl[index][2]
	
    local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"SmallIcon")
    
    local bindingPrice = ToolsMallItemPrice_Common(tostring(itemType), itemName, "BindingPrice")
    local payType 
    if bindingPrice == 0 then
    	itemWnd:GetDlgChild("YongBingBi"):ShowWnd(false)
    else
        payType = EGoldType.GoldBar
    end
    local Price =g_ItemInfoMgr:GetItemInfo(itemType, itemName, "Price")
    if Price == 0 then
    	itemWnd:GetDlgChild("YongBingQuan"):ShowWnd(false)
    else
        payType = EGoldType.GoldCoin
    end
    g_SetWndMultiToolTips(itemWnd:GetDlgChild("BigIconWnd"),itemType,itemName,0,nil,true,payType)
    local c_money = CMoney:new()
    local bigIconWnd = itemWnd:GetDlgChild("BigIconWnd")
    g_LoadIconFromRes(SmallIcon, bigIconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
    local DisplayName = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayName")
    itemWnd:GetDlgChild("ItemName"):SetWndText(DisplayName)
    itemWnd:GetDlgChild("YongBingBi"):SetWndText(c_money:ChangeMoneyToString(bindingPrice,EGoldType.GoldBar))
    itemWnd:GetDlgChild("YongBingQuan"):SetWndText(c_money:ChangeMoneyToString(Price,EGoldType.GoldCoin))
    local jiFenPriceWnd = itemWnd:GetDlgChild("JiFen")
    if showOffPrice then 
        local offPrice = itemTbl[index][3]
        jiFenPriceWnd:SetWndText(GetStaticTextClient(8321,offPrice))
        jiFenPriceWnd:ShowWnd(false)
        jiFenPriceWnd:EnableWnd(false)
    else
        jiFenPriceWnd:ShowWnd(false)
        jiFenPriceWnd:EnableWnd(false)
    end
end

--选中标签栏中某个选项后，显示物品信息
function CToolsMallMainWnd:DrawSellListWnd()
    self.m_SellListWndItemTbl = {}
    self.m_SellListWndItemWndTbl = {}
    local nowChoosedTbl = self:SellItemInfoTblList(self.m_CurChoosedSortType)
    local curDrawItemInfoTbl = nowChoosedTbl[self.m_CurChoosedTabType]
    if curDrawItemInfoTbl == nil then
        curDrawItemInfoTbl = {}
    end 
    local totalTblLen = # curDrawItemInfoTbl
    
    self.m_MaxPageNo = math.ceil(totalTblLen / 12)
    if not self.m_CurPageNo or self.m_CurPageNo > self.m_MaxPageNo then
    	self.m_CurPageNo = 1
    end
    local curPageStartIndex = (self.m_CurPageNo - 1)*12 + 1
    
    local tblLen = 12 
    if totalTblLen - curPageStartIndex < 12 then
        tblLen = totalTblLen - curPageStartIndex + 1
    end
    if self.m_CurPageNo >= self.m_MaxPageNo then
       self.m_LastPageBtn:EnableWnd(false) 
       self.m_NextPageBtn:EnableWnd(false)
    else
       self.m_LastPageBtn:EnableWnd(true) 
       self.m_NextPageBtn:EnableWnd(true)
    end
    if self.m_CurPageNo ==  1 then
        self.m_FirstPageBtn:EnableWnd(false)
        self.m_PrePageBtn:EnableWnd(false)
    else
        self.m_FirstPageBtn:EnableWnd(true)
        self.m_PrePageBtn:EnableWnd(true) 
    end
    self.m_ItemWndTbl = {}
    local showOffPriceItem = false
    if self.m_CurChoosedSortType == 3 then --是折价物品
        showOffPriceItem = true
    end
    
    self.m_SellListWnd:DeleteAllItem()
    local itemWidth = 170		--self.m_SellListWnd:GetWndWidth()/3
    local colCount = 3
    for i=1, colCount do
        self.m_SellListWnd:InsertColumn(i-1, itemWidth)
    end
    
    local itemHeight = 108		--self.m_SellListWnd:GetWndHeight()/4
    local rowCount = math.ceil(tblLen / 4) + 1
    for i=1, rowCount do
       self.m_SellListWnd:InsertItem(i-1, itemHeight)
    end
    local index  = curPageStartIndex
    local itemIndexInOnePage = 1
    for i=1, rowCount do
        for j=1, colCount do
            local itemWnd
            local item = self.m_SellListWnd:GetSubItem(i-1, j-1)
            if itemIndexInOnePage <= tblLen then
                itemWnd = CToolsMallSellListItemWnd:new()
                self:DrawListItem(itemWnd, item, curDrawItemInfoTbl, index, showOffPriceItem)
                itemWnd.Index = index
                item:ShowWnd(true)
                item:EnableWnd(true)
             else
                item:ShowWnd(false)
                item:EnableWnd(false)
             end
             table.insert(self.m_SellListWndItemTbl, item)
             table.insert(self.m_SellListWndItemWndTbl, itemWnd)
             index = index + 1
             itemIndexInOnePage = itemIndexInOnePage + 1
        end
    end
end

--大类别是否选中
function CToolsMallMainWnd:SetTabTypeChoosed(Child)
    local choosedFlag = false
    for i =1, TabMaxNum do	
        if self.m_TabCtrlTbl[i] == Child then
            if self.m_CurChoosedTabType ~= i then
                self.m_CurChoosedTabType = i		--得到大分类
                self:DrawSellListWnd()
                choosedFlag = true
            end 
            Child:SetCheck(true)
            break
        end
    end  
    if choosedFlag == true then
        for i =1, TabMaxNum do	
            if i  ~= self.m_CurChoosedTabType then
                self.m_TabCtrlTbl[i]:SetCheck(false)
            end
        end
    end
end

--热卖、新品、折价、全部
function CToolsMallMainWnd:SetSortTypeChoosed(Child)
    local choosedFlag = false
    for i=1, SortMaxNum do
        if self.m_SortCtrlTbl[i] == Child then
            if self.m_CurChoosedSortType ~= i then
                self.m_CurChoosedSortType = i        ---得到小分类
                self:DrawSellListWnd()
                choosedFlag = true
            end
            Child:SetCheck(true)
            break
        end
    end  
    if choosedFlag then
        for i=1, SortMaxNum do
            if i ~= self.m_CurChoosedSortType then
                self.m_SortCtrlTbl[i]:SetCheck(false)
            end
        end
    end
end

--检查标签栏中哪个要显示的物品类型被选中了
function CToolsMallMainWnd:CheckWhichOrderTypeChoosed(Child)
    self.m_CurPageNo = 1
    self:SetTabTypeChoosed(Child)
    
    self:SetSortTypeChoosed(Child)
end

CToolsMallMainWnd.ChoosdeBuyItemTypeTbl =  {}
CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InMainWnd"]  = 1
CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InHotLookWnd"]  = 2

--得到选择的物品的type和name
--参数:infoType(1是主界面，2是热门看板)
function CToolsMallMainWnd:GetChoosedBuyItemInfo(infoType, index)
	local curDrawItemInfoTbl = {}
	if infoType == CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InMainWnd"] then
        local nowChoosedTbl = self:SellItemInfoTblList(self.m_CurChoosedSortType) 	--得到表
        curDrawItemInfoTbl = nowChoosedTbl[self.m_CurChoosedTabType]	
    elseif infoType == CToolsMallMainWnd.ChoosdeBuyItemTypeTbl["InHotLookWnd"] then
        curDrawItemInfoTbl = self:GetHotLookWndItemInfoByType(self.m_CurChoosedInHotWnd)
    end
    
    self.m_ChoosedItemType = curDrawItemInfoTbl[index][1]
	self.m_ChoosedItemName = curDrawItemInfoTbl[index][2]
    
    local itemType = curDrawItemInfoTbl[index][1]
    local itemName = curDrawItemInfoTbl[index][2]

    return itemType, itemName
end

function  CToolsMallMainWnd:CheckAllowToDoActing()
    if  g_GameMain.m_ToolsMallBuyWnd:IsShow() or
        g_GameMain.m_ToolsMallGiveItemWnd:IsShow() or
        (g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd and
            g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:IsShow()) or
         (g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd and
            g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:IsShow() )then
       return false 
    end
    return true
end

function CToolsMallMainWnd:OnActive(bActive)
    if bActive and g_GameMain.m_ToolsMallBuyWnd then
        if g_GameMain.m_ToolsMallBuyWnd:IsShow() then
            g_GameMain.m_ToolsMallBuyWnd:SetFocus()
        end            
        if g_GameMain.m_ToolsMallGiveItemWnd:IsShow() then
            g_GameMain.m_ToolsMallGiveItemWnd:SetFocus()
        end
        if g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd and
            g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:IsShow() then
            g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:SetFocus()
        end
        if g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd and
            g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:IsShow() then
            g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:SetFocus() 
        end
    end
end

--参数：1为新品；2为热卖 
function CToolsMallMainWnd:GetHotLookWndItemInfoByType(type)
    if type == 1 then
        return self.m_HotLookNewSaleItemInfoTbl or {} 
        
    elseif type == 2 then
        return  self.m_HotLookHotSaleItemTbl or {} 
    end 
end

--陈列热门看板中的内容(右边小窗口)
function CToolsMallMainWnd:DrawHotWndList(type)
    self.m_HotSellListWnd:DeleteAllItem()
    self.m_HotSellListWnd:InsertColumn(0, 170)
    local tbl = self:GetHotLookWndItemInfoByType(type) or {}
    local tblLen =  # tbl
    self.m_HotSellListWndItemTbl = {}
    self.m_HotSellListWndItemWndTbl = {}
    local itemHeight = 108		--self.m_HotSellListWnd:GetWndHeight()/4
    self.m_HotSellListWnd:InsertColumn(0,self.m_HotSellListWnd:GetWndOrgWidth())

    local index = 1

    for i, v in pairs (tbl) do
        if index <= 10 then
            local itemWnd = CToolsMallHotSellListItemWnd:new()
            itemWnd.Index = i
            self.m_HotSellListWnd:InsertItem(index-1, itemHeight)
            local item = self.m_HotSellListWnd:GetSubItem(index-1, 0)
            self:DrawListItem(itemWnd, item, tbl, i, false)
            index = index + 1
            table.insert(self.m_HotSellListWndItemTbl, item)
            table.insert(self.m_HotSellListWndItemWndTbl, itemWnd)
        end
    end
end


------------------------用于角色购买物品预览---------------

function CToolsMallMainWnd:OnDestroy()
	OnDestroy(self)
end

--参数：表明是角色本身的装备还是试穿了商城里的装备
function CToolsMallMainWnd:InitModel(showType) 
	if showType == false then --显示角色本身的模型
		self.MainHandWeapon = nil
		self.AssociateWeapon = nil
		self.m_ArmorItemNameTbl = {}
	end
	self.m_bLoadPiece = nil
	self:DrawRole(showType)
	SetAni(self.m_WndBody.RenderObj)
end                                                                   


local function GetWeaponTypeNum(HandType)
	if  HandType == "双" then
		return 99
	elseif HandType == "单" or  HandType == "主" then
		return EEquipPart.eWeapon
	elseif HandType == "副" then 
		return EEquipPart.eAssociateWeapon
	end
end

function CToolsMallMainWnd:SetWeaponPreview(nBigId, itemName, PlayerInfo, UseLevel)
	local WeaponInfo = {nBigId,itemName,nil}
	local Reslut = g_ItemInfoMgr:WeaponCanUse(PlayerInfo,nBigId,itemName,UseLevel,WeaponInfo,AssociateWeaponInfo,eAssociateWeapon)
	local EquipTypeStr = g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"EquipType")
	local SEquipType = GetEquipHandType(EquipTypeStr)
	local EquipType  = GetWeaponTypeNum(SEquipType)
	if Reslut then
		if EquipType == EEquipPart.eWeapon then
			self.MainHandWeapon = itemName
			self.DoubltHand = false
			
		elseif EquipType == EEquipPart.eAssociateWeapon  then
			self.AssociateWeapon = itemName
			self.DoubltHand = false
		
		elseif 99 == EquipType then
			self.DoubltHand = true
			self.MainHandWeapon = itemName
			self.AssociateWeapon = nil
		end
		self:InitModel(true)
	else
		MsgClient(6300)
	end		
end

function CToolsMallMainWnd:SetShieldPreview(nBigId, itemName, PlayerInfo, UseLevel)
	local AssociateWeaponInfo = {nBigId,itemName,nil}
	local Reslut = g_ItemInfoMgr:WeaponCanUse(PlayerInfo,nBigId,itemName,UseLevel,WeaponInfo,AssociateWeaponInfo,eAssociateWeapon)
	if Reslut then
		if self.DoubltHand then
			self.AssociateWeapon = itemName
			self.MainHandWeapon = g_MainPlayer.m_Properties:GetWeaponIndexID()
			self.DoubltHand = false
		else
			self.AssociateWeapon = itemName
		end
	    self:InitModel(true)
	else
		MsgClient(6300)
	end
end


function CToolsMallMainWnd:SetJewelryPreview(nBigId, itemName, PlayerInfo, UseLevel)
	local EquipPartStr = g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"EquipPart")
	local EquipPart = EquipPartStrToNum(EquipPartStr)
	local result = g_ItemInfoMgr:JewelryCanUse(PlayerInfo, nBigId, itemName, UseLevel)
    if result then
    	if EquipPart == EEquipPart.eNecklace then
    	
    	elseif EquipPart == EEquipPart.eNecklace then
    		
    	elseif EquipPart == EEquipPart.eAccouterment then

    	end
    	self.DoubltHand = false
    else
        MsgClient(6300)
    end
end


--{headIndexID, wearIndexID, handIndexID, shoeIndexID, shoulderIndexID, backIndexID}
function CToolsMallMainWnd:SetArmorPreview(nBigId, itemName, PlayerInfo, UseLevel)
	local Reslut = g_ItemInfoMgr:ArmorCanUse(PlayerInfo,nBigId,itemName,UseLevel)
	local EquipPartStr = g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"EquipPart")
	local EquipPart = EquipPartStrToNum(EquipPartStr)
	if Reslut then
		if EquipPart == EEquipPart.eShoe then
			self.m_ArmorItemNameTbl[4] = itemName
		
		elseif EquipPart == EEquipPart.eHand then
			self.m_ArmorItemNameTbl[3] = itemName
			
		elseif EquipPart == EEquipPart.eShoulder then
            self.m_ArmorItemNameTbl[5] = itemName	            
		
		elseif EquipPart == EEquipPart.eWear then
			self.m_ArmorItemNameTbl[2] = itemName
		
		elseif EquipPart == EEquipPart.eHead then
			self.m_ArmorItemNameTbl[1] = itemName
			
		elseif EquipPart == EEquipPart.eBack then
			self.m_ArmorItemNameTbl[6] = itemName
		end
		self.DoubltHand = false
		self:InitModel(true)
	else
		MsgClient(6300)
	end
end

--试穿信息                                                                  
function CToolsMallMainWnd:Preview(nBigId, itemName)	
	local UseLevel = g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"BaseLevel") --装备使用等级
	local PlayerInfo = {["Id"] = g_MainPlayer.m_uID
							,["Class"] = g_MainPlayer:CppGetClass()
							,["Level"] = 999   -- 为了方便接口调用判断。直接给一个绝对大的值。方便用于试穿
							,["Sex"] = g_MainPlayer.m_Properties:GetSex()
							,["Camp"] = g_MainPlayer:CppGetBirthCamp()
							,["IsInBattleState"] = g_MainPlayer:IsInBattleState()
							,["IsInForbitUseWeaponState"] = g_MainPlayer:IsInForbitUseWeaponState()}
    
    
	if 5 == nBigId then --若是武器
        self:SetWeaponPreview(nBigId, itemName, PlayerInfo, UseLevel)
	
	elseif 7 == nBigId then --若是副手（盾牌，法器）
        self:SetShieldPreview(nBigId, itemName, PlayerInfo, UseLevel)
        
	elseif	(8 == nBigId) or (9 == nBigId) then --若是饰品
        self:SetJewelryPreview(nBigId, itemName, PlayerInfo, UseLevel)
	
	elseif 6 == nBigId then
        self:SetArmorPreview(nBigId, itemName, PlayerInfo, UseLevel)
	end
end


--绘制角色
function CToolsMallMainWnd:DrawRole(change)   
	if not self:IsShow() then
		return
	end
	local CustomResID = {
				g_MainPlayer.m_Properties:GetFaceResID(),
				g_MainPlayer.m_Properties:GetHairResID(),}
	if self.m_bLoadPiece == nil then
	    local tblResID = self:GetArmorResID()
        self.m_WndBody.RenderObj:ClearAddSke()
        self.m_WndBody.RenderObj:ClearAllEfx()
        self.m_WndBody.RenderObj:RemoveAllPiece()
	    InitPlayerCommonAni(self.m_WndBody.RenderObj,g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex(), nil)
	    InitPlayerCustomRes(self.m_WndBody.RenderObj,CustomResID,g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
		InitPlayerModel(self.m_WndBody.RenderObj,tblResID,g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())		
		
		if change == nil or change == false then 
			PrepareWeapon(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eWeapon,g_MainPlayer))
			PrepareOffWeapon(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,g_MainPlayer))
			InitWeaponRender(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eWeapon,g_MainPlayer),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
			InitWeaponRender(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,g_MainPlayer),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
		else    --试穿装备后的模型显示			
		    PrepareWeapon(self.m_WndBody.RenderObj, self:GetResID(EEquipPart.eWeapon,g_MainPlayer,self.MainHandWeapon))
            PrepareOffWeapon(self.m_WndBody.RenderObj, self:GetResID(EEquipPart.eAssociateWeapon,g_MainPlayer,self.AssociateWeapon))
            InitWeaponRender(self.m_WndBody.RenderObj, self:GetResID(EEquipPart.eWeapon,g_MainPlayer,self.MainHandWeapon),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
			
			if	(self.MainHandWeapon ~= nil) and (self.AssociateWeapon == nil)then
				InitWeaponRender(self.m_WndBody.RenderObj, nil, g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
			else
				InitWeaponRender(self.m_WndBody.RenderObj, self:GetResID(EEquipPart.eAssociateWeapon,g_MainPlayer,self.AssociateWeapon),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
			end
		end
		self.m_bLoadPiece = true
	end
	self.m_nHasDraw = true
	self.m_WndBody.RenderObj:SetDirection(CDir:new(self.m_nDir))
	if g_MainPlayer:CppGetClass() == 9 then
		self.m_WndBody.RenderObj:SetScale(1.8)
	else
		self.m_WndBody.RenderObj:SetScale(2.2)
	end
	self.m_WndBody.RenderObj:SetPosition( CVector3f:new( 0, -150, 0 ) )
end


function CToolsMallMainWnd:OnDrawRole(btnChange)
	if(btnChange:IsHeld()) then
		self:DrawRole()
	else
		if(self.m_nHasDraw == false) then
			self:DrawRole()
		end
		UnRegisterTick(self.m_DrawTick)
		self.m_DrawTick = nil
	end
end

--得到防具的ResID
function CToolsMallMainWnd:GetArmorResID()
    local uBodyResID, uArmResID, uShoeResID = g_GetDefaultResID(g_MainPlayer:CppGetClass())

    local headIndexID =  g_MainPlayer.m_Properties:GetHeadIndexID()
    local wearIndexID =  g_MainPlayer.m_Properties:GetBodyIndexID()
    local handIndexID     =  g_MainPlayer.m_Properties:GetArmIndexID()
    local shoeIndexID =  g_MainPlayer.m_Properties:GetShoeIndexID()
    local shoulderIndexID =  g_MainPlayer.m_Properties:GetShoulderIndexID()
    local backIndexID =  g_MainPlayer.m_Properties:GetBackIndexID()
    local tbl = {headIndexID, wearIndexID, handIndexID, shoeIndexID, shoulderIndexID, backIndexID}
    
    local resIDTbl = {}
    
    for i, v in pairs (tbl) do
        local ResID = nil 
        local ItemName = self.m_ArmorItemNameTbl[i]
        if not ItemName then
            local IndexID = v
            if IndexID ~= 0 then
                ResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
            end
        else
            ResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),ItemName,"ResID")
        end
        if i ==  2 then--身体
           ResID = ResID or uBodyResID
           
        elseif i == 3 then --护腕部分
            ResID = ResID or uArmResID
        elseif i ==  4 then --脚部分
            ResID = ResID or uShoeResID
        end
        table.insert(resIDTbl, ResID)
    end
    return resIDTbl
end

----获得装备resid
function CToolsMallMainWnd:GetResID(EquipPart,Player,ItemName)
	local uBodyResID, uArmResID, uShoeResID = g_GetDefaultResID(Player:CppGetClass())
	local ResID = 0
	if EquipPart == EEquipPart.eWeapon then --主手武器
		local WeaponResID = nil
		if not ItemName then
			local IndexID =  Player.m_Properties:GetWeaponIndexID()
			if IndexID ~= 0 then
				WeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[IndexID],"ResID")]
			end
		else
            WeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticWeaponBigID(),ItemName,"ResID")]
		end
		return (WeaponResID and WeaponResID or ResID)
	
	elseif EquipPart == EEquipPart.eAssociateWeapon then --副手武器	
		local OffWeaponResID  = nil
		local nBigID =  Player.m_Properties:GetOffWeaponBigID()
		if nBigID ~= 0 then
			if not ItemName then
				local IndexID =  Player.m_Properties:GetOffWeaponIndexID()
				if g_ItemInfoMgr:IsStaticWeapon(nBigID) then
					if CheckWeaponEquipType(g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseWeaponTbl[IndexID],"EquipType")) then 
						OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseWeaponTbl[IndexID],"AssociateResID")]
					else
						OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseWeaponTbl[IndexID],"ResID")]
					end
				elseif g_ItemInfoMgr:IsStaticShield(nBigID) then
					OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseShieldTbl[IndexID],"ResID")]
				end
			else
				nBigID =  7 
				OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,ItemName,"ResID")]
			end
		end
		return (OffWeaponResID and OffWeaponResID or ResID)			
	end
end


lan_load "commerce/Lan_CSMTreeList_Client"
cfg_load "commerce/CSMTreeList_Client"

CConsignmentSellListWnd = class ( CConsignmentCommon )

local Panel = {}
Panel.SellOrder = 1
Panel.CharSellOrder = 3

local FuncTbl ={
	GetSearchInfoByItemType 		= 1,			--[查询某类物品]对应的Gac2Gas:GetSearchInfoByItemType,参数：nBigID（物品大类）
	GetSearchInfoNeedParent 		= 2,			--[查询某类物品中的某个属性列]对应的Gac2Gas:GetSearchInfoByItemAttr，参数:nBigID， attrColName(属性列名)
	GetSearchInfoBySomeItemType 	= 3,			--[查询某几类物品]对应的Gac2Gas:GetSearchInfoBySomeItemType,参数：type(数字代表某几个配置表)
	GetSearchInfoByExactItemAttr	= 4,			--[查询某类物品中的某个属性列表中的一个具体的属性]对应的Gac2Gas:GetSearchInfoByExactItemAttr,参数：nBigID, attrColName(属性列名)，exactAttrName（具体的某个属性：如布甲）
    GetSearchInfoByItemTypeOrAttr     = 5,         --[查询某几类物品中某一列符合要求的好几种物品]对应的Gac2Gas:CSMGetOrderBySeveralSortItem,参数：index(物品type所在的tbl的索引值)，attrIndex(物品在某几个配置表中的某几列符合要求的tbl索引值)
}

local IcludingSeveralItemTypeTbl = {
	Sundries = 1
}

--创建寄售交易所出售物品列表窗口函数
local function CreateConsignmentSellListWnd( Wnd )
	Wnd:CreateFromRes( "CSMSellOrderListWnd", g_GameMain )
	Wnd:InitCSMSellListWndChild()

	Wnd.m_IsItemNameOrSuitName = 1 --1是根据物品名称搜索，2为根据套装名称搜索
	Wnd.m_AdvanceSoulRoot    = 0
	Wnd.m_MaxAdvancePhase = -1
    Wnd.m_IntenSoulRoot = 0
    Wnd.m_MaxIntenPhase = -1
    Wnd.m_Quality = -1
    Wnd.m_CampFlag = true --查询本阵营可用物品
    
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:SetStyle(0x00040000)

	return Wnd
end

function CConsignmentSellListWnd:Ctor()
    CreateConsignmentSellListWnd(self)
end

function CConsignmentSellListWnd:InitCSMSellListWndChild()
    self.m_CloseBtn			=	self:GetDlgChild("CloseBtn")
    self.m_SellItemBtn      =   self:GetDlgChild("SellItemBtn")
    self.m_BuyItemBtn       =   self:GetDlgChild("BuyItemBtn")
	self.m_ConsignmentList	=	self:GetDlgChild("CSMListCtrlWnd")
	self.m_PrePageBtn		=	self:GetDlgChild("PageUpBtn")		
	self.m_NextPageBtn		=	self:GetDlgChild("PageDownBtn")
	self.m_ResetOrderListBtnTbl = {}
	for i=1, 5 do
	    local orderNameStr = "ListName" .. i
	    local orderTypeBtn = self:GetDlgChild(orderNameStr)
	    table.insert(self.m_ResetOrderListBtnTbl, orderTypeBtn)
	end
	self.m_PageNo			=	self:GetDlgChild("CurPageNum")
	self.m_CSMListTree		=	self:GetDlgChild("CSMTreeList")
	
	--search部分
	self.m_Reset				=	self:GetDlgChild("Reset")
	self.m_Search				=	self:GetDlgChild("Search")	
	self.m_LowLevel				=	self:GetDlgChild("LowLevel")
	self.m_HighLevel			=	self:GetDlgChild("HighLevel")
	self.m_InputName			=	self:GetDlgChild("InputSearchName")
	self.m_GoldCount            =   self:GetDlgChild("GoldCount")
	self.m_YinCount             =   self:GetDlgChild("YinCount")
	self.m_TongCount            =   self:GetDlgChild("TongCount")
	self.m_GoldImage            =   self:GetDlgChild("GoldImage")
	self.m_YinImage             =   self:GetDlgChild("YinImage")
	self.m_TongImage            =   self:GetDlgChild("TongImage")
	self.m_SuitNameChkBtn       =   self:GetDlgChild("ActiveSuitNameSearch")
	self.m_Text                 =   self:GetDlgChild("Text") -- 未查询到结果的提示信息窗口
	self.m_TipsText             =   self:GetDlgChild("TipsText")
	self:GetCSMTreeListTbl()
	self.m_CSMListTree:SetMouseWheel(true)
	self.CSMListTree = {}
	self.ChoosedNode = 0
	local Node = CTreeCtrlNode.g_AddTreeCtrlNode(self.m_CSMListTree, self.tree_config_data)
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_UNCHECK_ENABLE)
	self.m_CSMListTree:SetWndTextColor(Flag, 0xffc4b26a)
	self.m_CSMListTree:SetWndTextBcColor(Flag,0xffc4b26a)
	if Node ~= nil and #Node ~=0 then
		table.insert(self.CSMListTree, Node)
	end
	
    self.m_IntenSoulComBox = self:GetDlgChild("IntenSoulComBox")
	self.m_AdvanceSoulComBox = self:GetDlgChild("AdvanceSoulComBox")
	self.m_QualityComBox = self:GetDlgChild("QualityComBox")
	self.m_MaxAdvanceComBox = self:GetDlgChild("MaxAdvanceComBox")
	self.m_MaxIntenComBox = self:GetDlgChild("MaxIntenComBox")
	self.m_IntenSoulText = self:GetDlgChild("IntenSoulText")
	self.m_MaxIntenPhaseText = self:GetDlgChild("MaxIntenText")
	self.CurPage = 0
	self:InitSellOrderWndInfo()
	--清空搜索名称、套装、等级窗口中的text
	self.m_InputName:SetWndText("")
	self.m_LowLevel:SetWndText("")
	self.m_HighLevel:SetWndText("")
    self.m_CampChkBtn = self:GetDlgChild("CampChkBtn")	
    self.m_CampChkBtn:SetCheck(true)
    
	self:CreateSearchInfoComBox()
	self:InitSearchInfoComBox()
	self.m_Search:EnableWnd(false)
	self.m_ListWidth = self.m_ConsignmentList:GetWndWidth()
	--self.m_ListHeight = self.m_ConsignmentList:GetWndHeight()
end

local NoneChoiseOfSoulRoot = GetStaticTextClient(1288)
local NoneChoiseOfPhase= GetStaticTextClient(1289)
local NoneChoiseOfInitPhase = GetStaticTextClient(1290)


--初始化进阶和强化魂跟属性ComBox
function CConsignmentSellListWnd:CreateSearchInfoComBox()
	local comBoxWidth = self.m_IntenSoulComBox:GetWndWidth()
	self.m_IntenSoulComBox:InsertColumn(0, comBoxWidth)
	local comBoxItemHeight = 20       
	local intenSoulText = CConsignment.GetEquipIntenSoulTbl()
	self.m_IntenSoulComBox:InsertItem(NoneChoiseOfSoulRoot, 0, comBoxItemHeight)
	local intenSoulTextTbl = {}
	for i, v in pairs(intenSoulText) do
	    intenSoulTextTbl[v] = i
	end
    for i, v in pairs (intenSoulTextTbl) do
        local displaySoul = v 
        self.m_IntenSoulComBox:InsertItem(" " .. displaySoul, i ,comBoxItemHeight)
    end
	
    local advanceComBoxWidth = self.m_AdvanceSoulComBox:GetWndWidth()
	self.m_AdvanceSoulComBox:InsertColumn(0, advanceComBoxWidth)
	local advanceComBoxHeight = 20       
	local advanceSoulText = CConsignment.GetEquipAdvanceSoulTbl()

	self.m_AdvanceSoulComBox:InsertItem(NoneChoiseOfSoulRoot, 0, advanceComBoxHeight)
	local advanceSoulTextTbl = {} 
	for i, v in pairs (advanceSoulText) do 
	    advanceSoulTextTbl[v] = i
	end
	for i, v in pairs (advanceSoulTextTbl) do
	    local displaySoul = v
	    self.m_AdvanceSoulComBox:InsertItem(" " .. displaySoul, i , advanceComBoxHeight) 
	end
	
	local maxAdvanceComBoxWidth = self.m_MaxAdvanceComBox:GetWndWidth()
	self.m_MaxAdvanceComBox:InsertColumn(0, maxAdvanceComBoxWidth)
	local maxAdvancePhaseInfo = {NoneChoiseOfPhase, 0, 1, 2, 3, 4, 5, 6, 7 }
	for i=1, # maxAdvancePhaseInfo do
	    self.m_MaxAdvanceComBox:InsertItem(" " .. maxAdvancePhaseInfo[i], i-1, comBoxItemHeight)
	end
	
    local maxIntenComBoxWidth = self.m_MaxIntenComBox:GetWndWidth()
	self.m_MaxIntenComBox:InsertColumn(0, maxIntenComBoxWidth)
	local maxIntenPhaseInfo = {NoneChoiseOfPhase, 0, 1, 2, 3, 4, 5}
	for i=1, # maxIntenPhaseInfo do
	    self.m_MaxIntenComBox:InsertItem(" " .. maxIntenPhaseInfo[i], i-1, comBoxItemHeight)
	end
	
	
	self.m_colorTbl = {g_ColorMgr:GetColor("稀1"),g_ColorMgr:GetColor("稀0"), g_ColorMgr:GetColor("稀1"),
                    	g_ColorMgr:GetColor("稀2"), g_ColorMgr:GetColor("蓝色装备"),
                    	g_ColorMgr:GetColor("紫色装备"), g_ColorMgr:GetColor("橙色装备"),
                    	g_ColorMgr:GetColor("稀6"), g_ColorMgr:GetColor("稀7")}
	
    local qualityComBoxWidth = self.m_QualityComBox:GetWndWidth()
	self.m_QualityComBox:InsertColumn(0, qualityComBoxWidth)
	local qualityInfo = {NoneChoiseOfPhase,  GetStaticTextClient(1276), GetStaticTextClient(1277),
	                            GetStaticTextClient(1278), GetStaticTextClient(1279),
	                            GetStaticTextClient(1280), GetStaticTextClient(1281),
	                            GetStaticTextClient(1282), GetStaticTextClient(1282)}
    local Flag = IMAGE_PARAM:new(SM_CB_BT, IP_ENABLE)
	local Flag1 = IMAGE_PARAM:new(SM_CB_BT, IP_MOUSEOVER)
	local Flag2 = IMAGE_PARAM:new(SM_CB_BT, IP_CLICKDOWN)
    self.m_itemTbl = {}
	for i=1, # qualityInfo do
	    self.m_QualityComBox:InsertItem(" " .. qualityInfo[i], i-1, comBoxItemHeight)
	    local item = self.m_QualityComBox:GetItem(i-1)
	    local pos = string.find(self.m_colorTbl[i], "c")
	    local colorStr = "0xff" .. string.sub(self.m_colorTbl[i], pos + 1) 
	    item:SetWndTextColor(Flag, colorStr)
	    item:SetWndTextColor(Flag1, colorStr)
	    item:SetWndTextColor(Flag2, colorStr)
	    table.insert(self.m_itemTbl, item)
	end
end

function CConsignmentSellListWnd:InitSearchInfoComBox()
    self.m_IntenSoulComBox:SetWndText(NoneChoiseOfSoulRoot)
    self.m_AdvanceSoulComBox:SetWndText(NoneChoiseOfSoulRoot)
    self.m_MaxIntenComBox:SetWndText(NoneChoiseOfInitPhase)
    self.m_MaxAdvanceComBox:SetWndText(NoneChoiseOfInitPhase)
    self.m_QualityComBox:SetWndText(NoneChoiseOfInitPhase)
    self.m_IntenSoulComBox:EnableWnd(false)
    self.m_MaxIntenComBox:EnableWnd(false)
end

--检测寄售交易所树形控件中填写的text，在TreeListMap.lua中TreeListMap表是否有对应的信息
function CConsignmentSellListWnd:CheckCSMTreeListInfoValidate(treeListCfgTbl)   
    for i=1, # treeListCfgTbl do
        local treeText = treeListCfgTbl[i].text 
        if treeText ~= nil then
            if self.TreeListMap[treeText] == nil then
                print(treeText .. "在TreeListMap中没有该text")
            end
        end
        local treeSub = treeListCfgTbl[i].sub
        if treeSub ~= nil then
            self:CheckCSMTreeListInfoValidate(treeSub) 
        end
    end
end

--创建拍卖行在查询订单时，客户端的提示信息窗口
local function CreateCSMTipsWnd(parentWnd)
    local wnd = SQRDialog:new()
    wnd:CreateFromRes("CSMTipsWnd", parentWnd)
    wnd:SetStyle(0xc5400000)
    return wnd
end

function CConsignmentSellListWnd:InitSCMTipsInfo()
    if # self.m_CSMOrderInfoTbl< 1 then
        g_GameMain.m_CSMSellOrderWnd.m_TipsText:ShowWnd(true)
        g_GameMain.m_CSMSellOrderWnd.m_TipsText:SetWndText(GetStaticTextClient(1291))
        g_GameMain.m_CSMSellOrderWnd.m_Text:ShowWnd(false)
    end
end

function CConsignmentSellListWnd:OpenCSMSellOrderWnd()
	self:ShowWnd(true)
	self:SetCurMoneyStr()
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_WndMainBag)
	self:InitSellOrderWndInfo()
	Gac2Gas:OpenCSMWnd(g_Conn)
	self.m_InputName:SetFocus()
	self:InitSCMTipsInfo()
    if self.m_CSMTipsWnd ~= nil then
        if self.m_CSMTipsWnd:IsShow() then
            self.m_CSMTipsWnd:ShowWnd(false)
        end
    end
end

function CConsignmentSellListWnd:VirtualExcludeWndClosed()
    if self.m_CSMTipsWnd then
        self.m_CSMTipsWnd:ShowWnd(false)
    end
end

function CConsignmentSellListWnd:InitSellOrderWndInfo()
    self.m_CSMOrderInfoTbl = {}     --保存订单信息的table
    self.m_CSMOrderItemWndTbl = {}  --保存订单itemWnd的table
    self.m_PageNo:SetWndText(string.format("%d/%d", 0, 0 ))
	self.m_PrePageBtn:EnableWnd(false)
	self.m_NextPageBtn:EnableWnd(false)
	self.m_BuyItemBtn:EnableWnd(false)
	self.m_ConsignmentList:DeleteAllItem()
	self.m_PreChoosedItemChkBtn = nil
    self:SetSearchBtnState()
    self.m_CheckSuitFlag = false --初始搜索套装名为false
end

--显示物品列表信息,购买了物品列表中的某个物品后，需要重新显示物品列表
--参数：wndObject窗口面板，当前页面首位置物品的ID
function CConsignmentSellListWnd:ShowSellList()
	g_GameMain.m_CSMSellOrderWnd.m_CSMTipsWnd:ShowWnd(false)
	local ItemListWnd = self.m_ConsignmentList
	local tbl = self.m_CSMOrderInfoTbl
	local len = # tbl
	if len > 0 then
	    self.m_Text:ShowWnd(false)
	    self.m_TipsText:ShowWnd(false)
	else
	    self.m_Text:ShowWnd(true)
	    self.m_TipsText:ShowWnd(false)
	end

	for i=1, table.maxn( tbl) do
		if ( i == 1 ) then
			ItemListWnd:InsertColumn( 0, 555 )
		end
		ItemListWnd:InsertItem( i-1, 40 )
	    local itemWnd = CConsignmentSellListItemWnd:new()
		local item = ItemListWnd:GetSubItem( i-1, 0)
		itemWnd:CreateFromRes( "CSMListCtrlItem", item )
		itemWnd:ShowWnd( true )
		local iconWnd = itemWnd:GetDlgChild( "ItemIcon" )
		itemWnd.ItemBtn = itemWnd:GetDlgChild( "CSMItemChkBtn" )
		itemWnd.ItemBtn:SetMouseWheel(true)
		local index = string.find(tbl[i].itemName, "*", 1, true)
		local itemName = tbl[i].itemName
		if index ~= nil then
			itemName = string.sub(tbl[i].itemName, index + 1)
		end
		local itemType = tonumber(tbl[i].itemType)
		local SmallIcon = g_ItemInfoMgr:GetItemInfo( itemType, itemName, "SmallIcon")
		g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
		if tbl[i].itemCount > 1 then
			iconWnd:SetWndText( "   " .. tbl[i].itemCount )
		end
		local dynInfo =  g_DynItemInfoMgr:GetDynItemInfo(tbl[i].itemID)
		local displayName = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayName")
	    local nameShowColor = self:GetItemShowColorInCSM(itemType, itemName, tbl[i].itemID) 
	    local BaseLevel = g_ItemInfoMgr:GetItemInfo( itemType, itemName, "BaseLevel")
	    if g_ItemInfoMgr:IsBoxitem(itemType) and dynInfo.OpenedFlag then
	        displayName =  displayName .. g_ColorMgr:GetColor("不可用颜色") .. "(" .. GetStaticTextClient(1230) .. ")"
	    end
		local ItemData = {nameShowColor .. displayName,tbl[i].sellPrice,BaseLevel,tbl[i].leftTime .. GetStaticTextClient(1121),tbl[i].seller}
		self:SetStrInItem(itemWnd, ItemData)
		itemWnd.ItemBtn.Index = i
		itemWnd.IconWnd = iconWnd
		g_SetWndMultiToolTips(iconWnd,itemType,itemName,tbl[i].itemID ,tbl[i].itemCount)     
		--g_SetWndMultiToolTips(itemWnd.ItemBtn,itemType,itemName,tbl[i].itemID ,tbl[i].itemCount)
		table.insert( self.m_CSMOrderItemWndTbl, itemWnd )
	end
end


function CConsignmentSellListWnd:SetSearchBtnState()
	if   self.m_InputName:GetWndText() == "" and
	     self.m_LowLevel:GetWndText() == "" and
	     self.m_HighLevel:GetWndText() == "" and 
	     self.m_AdvanceSoulRoot == 0 and
	     self.m_MaxAdvancePhase == -1 and
	     self.m_Quality  == -1 and 
	     self.ChoosedNode == 0 then
        self.m_Search:EnableWnd(false) 
	else 
        self.m_Search:EnableWnd(true)      
	end
end



function CConsignmentSellListWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
    if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
			g_GameMain.m_WndMainBag:ShowWnd(false) 

		elseif Child == self.m_SellItemBtn then	
			if g_GameMain.m_CSMSellCharOrderWnd == nil then
				g_GameMain.m_CSMSellCharOrderWnd = CConsignmentSellItemWnd:new()
				g_GameMain.m_CSMSellCharOrderWnd.m_SellPriceState = g_GameMain.m_CSMSellCharOrderWnd:GetCSMRememberPriceState() 
			end
			local wnd = g_GameMain.m_CSMSellCharOrderWnd
			wnd.OrderType = 0
			wnd.CurPage = 0
			--第二个参数为第一页（一）
			wnd:ShowWnd(true)
			local AutoCloseWnd = CAutoCloseWnd:new()
			AutoCloseWnd:AutoCloseWnd(wnd)
			wnd:OpenMainBagWnd()
			wnd.m_Jin:SetFocus()
			wnd.m_ChoosedTimeLimit = 24
			wnd.m_TimeChkBtn1:SetCheck(true)
			Gac2Gas:CSMSearchCharSellOrder( g_Conn, wnd.OrderType, 1)
			if wnd.m_CSMTipsWnd == nil then 
			    wnd.m_CSMTipsWnd = CreateCSMTipsWnd(wnd)
			else
			    wnd.m_CSMTipsWnd:ShowWnd(true)
			end
	        wnd.m_CSMTipsWnd:SetWndText(GetStaticTextClient(1161))

		elseif Child == self.m_PrePageBtn then 
			self.CurPage = self.CurPage - 1
			self:SearchAddTreeListInfo(1161)
		
		elseif Child == self.m_NextPageBtn then
			self.CurPage = self.CurPage + 1
			self:SearchAddTreeListInfo(1161)
		
		elseif Child ==  self.m_Reset then
			self:ResetConsignmentSearchInfo()

		elseif Child == self.m_Search  then
			self:SearchAddTreeListInfo(1160)
		
		elseif Child == self.m_SuitNameChkBtn then			
            if self.m_IsItemNameOrSuitName == 1 then
                self.m_IsItemNameOrSuitName = 2
                self.m_SuitNameChkBtn:SetCheck(true)
            elseif self.m_IsItemNameOrSuitName == 2 then
                self.m_IsItemNameOrSuitName = 1 
                self.m_SuitNameChkBtn:SetCheck(false)
            end
            
	        
		elseif Child == self.m_BuyItemBtn then	
            if g_MainPlayer.m_ItemBagLock then
                MsgClient(160047)
                return
            end
            local index = self.m_CSMOrderItemWndTbl.m_ChoosedItemBtn.Index
			local orderID = self.m_CSMOrderInfoTbl[index].orderID
			local needMoney = self.m_CSMOrderInfoTbl[index].sellPrice
			if needMoney > g_MainPlayer.m_nMoney then
				self.m_MsgBox  = MessageBox( self, MsgBoxMsg(150), MB_BtnOK )
			else	
				Gac2Gas:CSMBuyOrder( g_Conn, orderID )
			end
		elseif Child == self.m_CampChkBtn then
		    self.m_CampFlag = not self.m_CampFlag
		    self.m_CampChkBtn:SetCheck(self.m_CampFlag)
		    self:SearchAddTreeListInfo(1160)
		else
		    self:SearhOrderInfoByClickedOrderType(Child)     
		end
	
	elseif uMsgID == WND_NOTIFY  then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			if Child == self.m_LowLevel then 
				if not IsNumber( tonumber(self.m_LowLevel:GetWndText()) ) then
					self.m_LowLevel:SetWndText("")
				else
				    self.CurPage = 1
				end
				local commerceCommon = CCommerceCommon:new()
				commerceCommon:CheckInputLevel(Child, 1)
				self:SetSearchBtnState()
	
			elseif Child == self.m_HighLevel then
				if not IsNumber( tonumber(self.m_HighLevel:GetWndText()) ) then
					self.m_HighLevel:SetWndText("")
			    else
			        self.CurPage = 1
				end	    
				local commerceCommon = CCommerceCommon:new()
				commerceCommon:CheckInputLevel(Child, 150)
				self:SetSearchBtnState()
				
		  elseif Child == self.m_InputName then
		        self.CurPage = 1
                self:SetSearchBtnState()
			end	
		end
	elseif( uMsgID == BUTTON_CLICKDOWN) then
		if self.m_CSMListTree == Child then
			self.ChoosedNode = self.m_CSMListTree:GetCurrentNode()
			if self.ChoosedNode == 0 then
				return 
			end
			self:SetSearchBtnState()
			--self:SearchAddTreeListInfo(1160)
	    end
	elseif (uMsgID == ITEM_LBUTTONCLICK) then
	    if self.m_AdvanceSoulComBox == Child then
	        local choosedItem = self.m_AdvanceSoulComBox:GetCurrentSel()
	        if(not choosedItem or 0 > choosedItem) then return end
	        self.m_AdvanceSoulRoot = choosedItem 
            self.CurPage = 1
			self:SetSearchBtnState()
	    elseif self.m_MaxAdvanceComBox == Child then
	        local choosedItem = self.m_MaxAdvanceComBox:GetCurrentSel()
	        if (not choosedItem or 0 > choosedItem) then return end
            self.m_MaxAdvancePhase = choosedItem -1
            self.CurPage = 1
			self:SetSearchBtnState()
			if self.m_MaxAdvancePhase == 7 then
                self.m_IntenSoulComBox:EnableWnd(true)
                self.m_MaxIntenComBox:EnableWnd(true)
            else
                self.m_IntenSoulComBox:EnableWnd(false)
                self.m_MaxIntenComBox:EnableWnd(false)
                self.m_MaxIntenComBox:SetWndText(NoneChoiseOfInitPhase)
                self.m_IntenSoulComBox:SetWndText(NoneChoiseOfSoulRoot)
                self.m_MaxIntenPhase = - 1
                self.m_IntenSoulRoot = 0
		    end
		elseif self.m_IntenSoulComBox == Child then
		    local choosedItem = self.m_IntenSoulComBox:GetCurrentSel() 
		    if(not choosedItem or 0 > choosedItem) then return end 
		    self.m_IntenSoulRoot = choosedItem 
		    self.CurPage = 1
			self:SetSearchBtnState()
	    elseif self.m_MaxIntenComBox == Child then
            local choosedItem = self.m_MaxIntenComBox:GetCurrentSel() 
		    if(not choosedItem or 0 > choosedItem) then return end 
		    self.m_MaxIntenPhase = choosedItem -1
		    self.CurPage = 1
			self:SetSearchBtnState() 
	    elseif self.m_QualityComBox == Child then
            local choosedItem = self.m_QualityComBox:GetCurrentSel() 
		    if(not choosedItem or 0 > choosedItem) then return end 
		    local Flag = IMAGE_PARAM:new(SM_CB_BK, IP_ENABLE)  
            local pos = string.find(self.m_colorTbl[choosedItem +1], "c")
	        local colorStr = "0xff" .. string.sub(self.m_colorTbl[choosedItem+1], pos + 1) 
	        self.m_QualityComBox:SetWndTextColor(Flag, colorStr)
	        --self.m_QualityComBox:SetWndTextBcColor(Flag, colorStr)
		    self.m_Quality = choosedItem -1
		    self.CurPage = 1
			self:SetSearchBtnState() 
		end
	end
end

-----------------------------------SendSearchInfo相关----------------------------------
function CConsignmentSellListWnd:SearhOrderInfoByClickedOrderType(clickedChild)
    for i=1, 5 do
        if self.m_ResetOrderListBtnTbl[i] == clickedChild then
			self.OrderType = i
			self:SearchAddTreeListInfo(1160)    
			break
		end 
    end
end

--重置搜索子窗口所需信息
function CConsignmentSellListWnd:ResetConsignmentSearchInfo()
	self.m_InputName:SetWndText( "" )
	self.m_LowLevel:SetWndText( "" )
	self.m_HighLevel:SetWndText( "" )
	self.m_AdvanceSoulRoot = 0
	self.m_MaxAdvancePhase = -1
	self.m_IntenSoulRoot = 0
    self.m_MaxIntenPhase = -1
    self.m_Quality = -1
    self.m_IsItemNameOrSuitName = 1
    self.m_IntenSoulComBox:SetWndText(NoneChoiseOfSoulRoot)
    self.m_MaxIntenComBox:SetWndText(NoneChoiseOfInitPhase)
    self.m_AdvanceSoulComBox:SetWndText(NoneChoiseOfSoulRoot)
    self.m_MaxAdvanceComBox:SetWndText(NoneChoiseOfInitPhase)
    self.m_QualityComBox:SetWndText(NoneChoiseOfInitPhase)
    local qualityColorStr = g_ColorMgr:GetColor("稀1")
    local Flag = IMAGE_PARAM:new(SM_CB_BK, IP_ENABLE)  
    local pos = string.find(qualityColorStr, "c")
    local colorStr = "0xff" .. string.sub(qualityColorStr, pos + 1) 
    self.m_QualityComBox:SetWndTextColor(Flag, colorStr)
	self.m_Search:EnableWnd(false)
    if self.ChoosedNode ~= 0 then
	    self.m_CSMListTree:SelectNode(self.ChoosedNode, false)
	end
	self.ChoosedNode = 0
    self.m_IntenSoulComBox:EnableWnd(false)
    self.m_MaxIntenComBox:EnableWnd(false)
    self.m_CampFlag = true
    self.m_CampChkBtn:SetCheck(true)
    self.m_SuitNameChkBtn:SetCheck(false)
    self:InitSCMTipsInfo()
end

--获取搜索信息
function CConsignmentSellListWnd:GetConsignmentSearchInfo()
	local itemName = self.m_InputName:GetWndText()
	local lowLevel  = tonumber( self.m_LowLevel:GetWndText() )
	local highLevel = tonumber( self.m_HighLevel:GetWndText() )
    if lowLevel == nil then
        lowLevel = 0 
    end
    if highLevel == nil then
        highLevel = 0
    end
	if self.OrderType == 0 or self.OrderType == nil then
		self.OrderType = 1
	end
	if self.CurPage == 0 or self.CurPage == nil then
		self.CurPage = 1
	end
	return itemName, lowLevel, highLevel
end

--加上树形控件上的node信息搜索相关order
function CConsignmentSellListWnd:SearchAddTreeListInfo(msgID)
	self:InitSellOrderWndInfo()
	local name, sLevel, eLevel = self:GetConsignmentSearchInfo()
	if string.len(name) > 60 then
        MessageBox( self, MsgBoxMsg(162), MB_BtnOK )
        return     
	end
	
	if sLevel > eLevel then         
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(154), MB_BtnOK )
		return
	end 

	local curClickNode = self.ChoosedNode
	if curClickNode ~= nil and curClickNode~= 0  then
		local	NodeText = self.m_CSMListTree:GetNodeText(curClickNode)
		local nodeInProPos = self.m_TreeDataInLan[NodeText]
		local nodeInPro = self.m_TreeDataInPro[nodeInProPos[1]][nodeInProPos[2]]
		local sendInfoTbl = self.TreeListMap[nodeInPro]
		if  sendInfoTbl == nil then
--			print("没有node的查询信息~~")
			return
		end
		
		if sendInfoTbl[1] == FuncTbl.GetSearchInfoByItemType then				--[查询某类物品]
			Gac2Gas:CSMGetOrderByItemType(g_Conn, Panel.SellOrder, sendInfoTbl[2], name , sLevel, eLevel, self.OrderType, self.CurPage, self.m_IsItemNameOrSuitName, self.m_AdvanceSoulRoot, self.m_MaxAdvancePhase, self.m_IntenSoulRoot, self.m_MaxIntenPhase, self.m_Quality, self.m_CampFlag)
	
		elseif 	sendInfoTbl[1] == FuncTbl.GetSearchInfoNeedParent then			--[查询某类物品中的某两个属性列]需要其parentNode
			local parentNode = self.m_CSMListTree:GetNodeParent(curClickNode)
			local parentNodeText = self.m_CSMListTree:GetNodeText(parentNode) 
			Gac2Gas:CSMGetOrderByItemAttr(g_Conn, Panel.SellOrder, sendInfoTbl[2], sendInfoTbl[3], parentNodeText, sendInfoTbl[4], sendInfoTbl[5], name , sLevel, eLevel, self.OrderType, self.CurPage, self.m_IsItemNameOrSuitName, self.m_AdvanceSoulRoot, self.m_MaxAdvancePhase, self.m_IntenSoulRoot, self.m_MaxIntenPhase, self.m_Quality, self.m_CampFlag)
		
		elseif 	sendInfoTbl[1] == FuncTbl.GetSearchInfoBySomeItemType then		--[查询某几类物品]
			Gac2Gas:CSMGetOrderBySomeItemType(g_Conn, Panel.SellOrder, sendInfoTbl[2] , name , sLevel, eLevel, self.OrderType, self.CurPage, self.m_IsItemNameOrSuitName, self.m_AdvanceSoulRoot, self.m_MaxAdvancePhase, self.m_IntenSoulRoot, self.m_MaxIntenPhase, self.m_Quality,self.m_CampFlag)
		
		elseif sendInfoTbl[1] == FuncTbl.GetSearchInfoByExactItemAttr then		--[查询某类物品中的某个属性列表中的一个具体的属性]
			Gac2Gas:CSMGetOrderByExactItemAttr(g_Conn, Panel.SellOrder, sendInfoTbl[2], sendInfoTbl[3], name , sLevel, eLevel, self.OrderType,self.CurPage, self.m_IsItemNameOrSuitName, self.m_AdvanceSoulRoot, self.m_MaxAdvancePhase, self.m_IntenSoulRoot, self.m_MaxIntenPhase, self.m_Quality, self.m_CampFlag)
		
		elseif sendInfoTbl[1] == FuncTbl.GetSearchInfoByItemTypeOrAttr then --根据物品type查询几类物品或者某些类中的符合要求的属性
		    Gac2Gas:CSMGetOrderBySeveralSortItem(g_Conn, Panel.SellOrder,sendInfoTbl[2], sendInfoTbl[3], name , sLevel, eLevel, self.OrderType,self.CurPage, self.m_IsItemNameOrSuitName, self.m_AdvanceSoulRoot, self.m_MaxAdvancePhase, self.m_IntenSoulRoot, self.m_MaxIntenPhase, self.m_Quality, self.m_CampFlag)
		end
	else
		Gac2Gas:CSMSearchSellOrder( g_Conn, name , sLevel, eLevel, self.OrderType, self.CurPage, self.m_IsItemNameOrSuitName, self.m_AdvanceSoulRoot, self.m_MaxAdvancePhase, self.m_IntenSoulRoot, self.m_MaxIntenPhase, self.m_Quality, self.m_CampFlag)		
	end
	if self.m_CSMTipsWnd == nil then 
	    self.m_CSMTipsWnd = CreateCSMTipsWnd(self)	    
	else
	    self.m_CSMTipsWnd:ShowWnd(true)
	end
    self.m_Text:ShowWnd(false)
    self.m_TipsText:ShowWnd(false)
    self.m_CSMTipsWnd:SetWndText(GetStaticTextClient(msgID))
end
-------------------------寄售交易所显示剩余金钱---------------------------------------
function CConsignmentSellListWnd:SetCurMoneyStr()
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()

	local tblWnd = {self.m_GoldCount, self.m_GoldImage, self.m_YinCount, self.m_YinImage, self.m_TongCount, self.m_TongImage}
	money:ShowMoneyInfo(g_MainPlayer.m_nMoney,tblWnd)
end

--------------------------选中item后，激活购买按钮------------------------------------------------


lan_load "commerce/Lan_CSMTreeList_Client"
cfg_load "commerce/CSMTreeList_Client"
cfg_load "commerce/PurchasingAgent_Common"

CPurchasingAgentMainWnd = class ( CConsignmentCommon )
CPurchasingAgentItemWnd = class ( SQRDialog )

local FuncTbl ={
	GetSearchInfoByItemType 			= 1,			--[查询某类物品]对应的Gac2Gas:GetSearchInfoByItemType,参数：nBigID（物品大类）
	GetSearchInfoNeedParent 			= 2,			--[查询某类物品中的某个属性列]对应的Gac2Gas:GetSearchInfoByItemAttr，参数:nBigID， attrColName(属性列名)
	GetSearchInfoBySomeItemType 	= 3,			--[查询某几类物品]对应的Gac2Gas:GetSearchInfoBySomeItemType,参数：type(数字代表某几个配置表)
	GetSearchInfoByExactItemAttr	= 4,			--[查询某类物品中的某个属性列表中的一个具体的属性]对应的Gac2Gas:GetSearchInfoByExactItemAttr,参数：nBigID, attrColName(属性列名)，exactAttrName（具体的某个属性：如布甲）
  GetSearchInfoByItemTypeOrAttr = 5,      --[查询某几类物品中某一列符合要求的好几种物品]对应的Gac2Gas:CSMGetOrderBySeveralSortItem,参数：index(物品type所在的tbl的索引值)，attrIndex(物品在某几个配置表中的某几列符合要求的tbl索引值)
}

local qualityInfo = {GetStaticTextClient(1289),  GetStaticTextClient(1276), GetStaticTextClient(1277),
										GetStaticTextClient(1278), GetStaticTextClient(1279),
										GetStaticTextClient(1280), GetStaticTextClient(1281),
										GetStaticTextClient(1282), GetStaticTextClient(1282)}

function CPurchasingAgentMainWnd.GetWnd()
	local Wnd = g_GameMain.m_PurchasingAgentMainWnd
	if not Wnd then
		Wnd = CPurchasingAgentMainWnd:new(g_GameMain)
		g_GameMain.m_PurchasingAgentMainWnd = Wnd
		_,g_GameMain.m_PurchasingAgentMainWnd.TreeListMap = apcall(CConsignmentTreeListMap.CreateCSMTreeListMap)
	end
	return Wnd
end

local orderCountOnePage = 50
function CPurchasingAgentMainWnd:Ctor(parent)
	self:CreateFromRes("PurchasingAgentMainWnd", parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1, false)
end

function CPurchasingAgentMainWnd:OnChildCreated()
	self.m_Close 					= self:GetDlgChild("Close")
	self.m_QuickSellBtn 	= self:GetDlgChild("quickSell")
	self.m_PurchasingBtn 	= self:GetDlgChild("PurchasingBtn")
	self.m_SellBtn 				= self:GetDlgChild("Sell")
	self.m_SearchBtn 			= self:GetDlgChild("SearchBtn")
	self.m_ResetBtn 			= self:GetDlgChild("ResetBtn")
	self.m_PrePage			 	= self:GetDlgChild("prePage")
	self.m_NextPage			 	= self:GetDlgChild("nextPage")
	self.m_CSMBuyItemList	=	self:GetDlgChild("ItemList")
	self.m_CSMBuyTreeList	=	self:GetDlgChild("TreeList")
	self.m_Text						= self:GetDlgChild("text")
	self.m_CSMBuyTreeList:SetMouseWheel(true)
	
	self:GetCSMTreeListTbl()
	self.m_Node = CTreeCtrlNode.g_AddTreeCtrlNode(self.m_CSMBuyTreeList, self.tree_config_data)
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_UNCHECK_ENABLE)
	self.m_CSMBuyTreeList:SetWndTextColor(Flag, 0xffc4b26a)
	self.m_CSMBuyTreeList:SetWndTextBcColor(Flag,0xffc4b26a)
	
	self.m_CurrentMoney 	= self:GetDlgChild("currentMoney")
	self.m_QualityComBox	= self:GetDlgChild("QualityComBox")
	self.m_ItemNameEdit		=	self:GetDlgChild("ItemNameEdit")
	self.m_LowLevelEdit 	=	self:GetDlgChild("LowLevelEdit")
	self.m_HighLevelEdit	=	self:GetDlgChild("HighLevelEdit")
	self.m_CurPageNumEdit	= self:GetDlgChild("curPageNum")
	
	self.m_tblSorBtn = {}
	self.m_tblSorBtn["ItemName"]			= self:GetDlgChild("ItemName")
	self.m_tblSorBtn["Level"]		= self:GetDlgChild("level")
	self.m_tblSorBtn["Count"]			= self:GetDlgChild("count")
	self.m_tblSorBtn["SingleMoney"]			= self:GetDlgChild("singleMoney")
	self.m_tblSorBtn["TotallMoney"]	= self:GetDlgChild("totallMoney")
	self.m_tblSorBtn["LeftTime"]			= self:GetDlgChild("leftTime")
	self.m_tblSorBtn["BuyPlayerName"]			= self:GetDlgChild("playerName")
	self.m_tblSorBtn["ItemName"].SortID = 1
	self.m_tblSorBtn["Level"].SortID = 2
	self.m_tblSorBtn["Count"].SortID = 3
	self.m_tblSorBtn["SingleMoney"].SortID = 4
	self.m_tblSorBtn["TotallMoney"].SortID = 5
	self.m_tblSorBtn["LeftTime"].SortID = 6
	self.m_tblSorBtn["BuyPlayerName"].SortID = 7
	for p, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
	
	self:CreateQualityComBox()
	self:SetStyle(0x00040000)
	
	self.m_tblBuyItemInfo = {}
	self.m_tblCanSellInfo = {}
	self.m_ChoosedTreeNode = 0
	self.m_ChoosedQualityNode = -1
	self.m_CurrentPage = 0
	self.m_SortId = 4
	self.m_MaxPage = 1
	
	self.m_SellBtn:EnableWnd(false)
	self.m_Text:ShowWnd(false)
end

function CPurchasingAgentMainWnd:CreateQualityComBox()
	self.m_colorTbl = {g_ColorMgr:GetColor("稀1"),g_ColorMgr:GetColor("稀0"), g_ColorMgr:GetColor("稀1"),
											g_ColorMgr:GetColor("稀2"), g_ColorMgr:GetColor("蓝色装备"),
											g_ColorMgr:GetColor("紫色装备"), g_ColorMgr:GetColor("橙色装备"),
											g_ColorMgr:GetColor("稀6"), g_ColorMgr:GetColor("稀7")}
											
	local qualityComBoxWidth = self.m_QualityComBox:GetWndWidth()
	self.m_QualityComBox:InsertColumn(0, qualityComBoxWidth)
	local Flag = IMAGE_PARAM:new(SM_CB_BT, IP_ENABLE)
	local Flag1 = IMAGE_PARAM:new(SM_CB_BT, IP_MOUSEOVER)
	local Flag2 = IMAGE_PARAM:new(SM_CB_BT, IP_CLICKDOWN)
	self.m_QualityComBoxTbl = {}
	local comBoxItemHeight = 20
	for i=1, # qualityInfo do
		self.m_QualityComBox:InsertItem(" " .. qualityInfo[i], i-1, comBoxItemHeight)
		local item = self.m_QualityComBox:GetItem(i-1)
		local pos = string.find(self.m_colorTbl[i], "c")
		local colorStr = "0xff" .. string.sub(self.m_colorTbl[i], pos + 1) 
		item:SetWndTextColor(Flag, colorStr)
		item:SetWndTextColor(Flag1, colorStr)
		item:SetWndTextColor(Flag2, colorStr)
		table.insert(self.m_QualityComBoxTbl, item)
	end
end

function CPurchasingAgentMainWnd:OpenPanel(bShow)
	self:ShowWnd(bShow)
	if bShow then
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
		self:SetCurMoneyStr()
		self.m_tblBuyItemInfo = {}
		self:ResetSearch()
		self:DrawBuyItemInfoList()
	end
end

function CPurchasingAgentMainWnd:SetCurMoneyStr()
	self.m_CurrentMoney:SetWndText(g_MoneyMgr:ChangeMoneyToString(g_MainPlayer:GetMoney(),EGoldType.GoldCoin))
end

function CPurchasingAgentMainWnd:VirtualExcludeWndClosed()
	CPurchasingAgentSellWnd.GetWnd():OpenPanel(false)
	CPurchasingAgentQuickSellWnd.GetWnd():OpenPanel(false)
end

function CPurchasingAgentMainWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_PurchasingBtn then
			self:OpenPanel(false)
			CPurchasingWnd.GetWnd():OpenPanel(true)
		elseif Child == self.m_QuickSellBtn then
			self:OnQuickSellBtn()
		elseif Child == self.m_SellBtn then
			self:OnSellBtn()
		elseif Child == self.m_SearchBtn then
			self:OnSearchBtn()
		elseif Child == self.m_ResetBtn then
			self:ResetSearch()
		elseif (Child == self.m_PrePage) then
			if self.m_CurrentPage > 1 then
				self.m_CurrentPage = self.m_CurrentPage - 1
				self:SearchByLastSearchCondition()
			end
		elseif (Child == self.m_NextPage) then
			if self.m_MaxPage > self.m_CurrentPage then
				self.m_CurrentPage = self.m_CurrentPage + 1
				self:SearchByLastSearchCondition()
			end
		elseif Child == self.m_Close then
			self:OpenPanel(false)
		else
			for p, v in pairs(self.m_tblSorBtn) do
				if(Child == v) then
					self.m_SortId = Child.SortID
					--if self.m_SortId ~= 5 then
						--Gac2Gas:SortByType(self.m_SortId)
					--end
					self:SortMember(p, v.bFlag)
					v.bFlag = not v.bFlag
					break
				end
			end
		end
	elseif( uMsgID == BUTTON_CLICKDOWN) then
		if self.m_CSMBuyTreeList == Child then
			self.m_ChoosedTreeNode = self.m_CSMBuyTreeList:GetCurrentNode()
		end
	elseif uMsgID == WND_NOTIFY then
		if Child == self.m_LowLevelEdit or Child == self.m_HighLevelEdit then
			if not IsNumber( tonumber(Child:GetWndText()) ) then
				Child:SetWndText("")
			end
			local commerceCommon = CCommerceCommon:new()
			commerceCommon:CheckInputLevel(Child, 1)
		end
	elseif (uMsgID == ITEM_LBUTTONCLICK) then
		if self.m_QualityComBox == Child then
	   	local choosedItem = self.m_QualityComBox:GetCurrentSel()
	   	if(not choosedItem or 0 > choosedItem) then return end
	   	self.m_ChoosedQualityNode = choosedItem -1
	  elseif self.m_CSMBuyItemList == Child then
			self.m_SellBtn:EnableWnd(true)
		end
	elseif (uMsgID == ITEM_LBUTTONDBLCLICK) then
--		if self.m_CSMBuyItemList == Child then
--			self:OnSellBtn()
--		end
	end
end

function CPurchasingAgentMainWnd:OnSearchBtn()
	local ItemLanName = self.m_ItemNameEdit:GetWndText()
	--ItemLanName物品显示名,在PurchasingAgent_Common中找到对应的真实名
--	local ItemRealName = nil
--	if ItemLanName ~= "" then
--		local keys = PurchasingAgent_Common:GetKeys()
--		for i,v in pairs( keys ) do
--			local ItemName = PurchasingAgent_Common(v,"ItemName")
--			local ItemBigID = PurchasingAgent_Common(v,"BigID")
--			local DisPlayName = g_ItemInfoMgr:GetItemLanInfo(ItemBigID,ItemName,"DisplayName")
--			if DisPlayName == ItemLanName then
--				ItemRealName = ItemName
--				break
--			end
--		end
--	else
--		ItemRealName = ""
--	end
--	if not ItemRealName then
--		MsgClient(8303)
--		return
--	end
	local Search_LowLevel = tonumber(self.m_LowLevelEdit:GetWndText())
	local Search_HighLevel = tonumber(self.m_HighLevelEdit:GetWndText())
	Search_LowLevel = Search_LowLevel and Search_LowLevel or 0
	Search_HighLevel = Search_HighLevel and Search_HighLevel or 0
	if Search_LowLevel > Search_HighLevel then
		MsgClient(8302)
		return
	end
	self.m_CurrentPage = 1
	self:SearchByArg(self.m_ChoosedTreeNode,ItemLanName,Search_LowLevel,Search_HighLevel,self.m_ChoosedQualityNode,self.m_SortId,self.m_CurrentPage)
end

function CPurchasingAgentMainWnd:SearchByArg(curClickNode,ItemLanName,Search_LowLevel,Search_HighLevel,ChoosedQualityNode,SortId,CurrentPage)
	if curClickNode ~= nil and curClickNode~= 0  then
		local	NodeText = self.m_CSMBuyTreeList:GetNodeText(curClickNode)
		local nodeInProPos = self.m_TreeDataInLan[NodeText]
		local nodeInPro = self.m_TreeDataInPro[nodeInProPos[1]][nodeInProPos[2]]
		local sendInfoTbl = self.TreeListMap[nodeInPro]
		if sendInfoTbl == nil then
			return
		end
		if sendInfoTbl[1] == FuncTbl.GetSearchInfoByItemType then				--[查询某类物品]
			Gac2Gas:CSMSearchBuyOrder(g_Conn,1,sendInfoTbl[2],0,ItemLanName,Search_LowLevel,Search_HighLevel,
																	ChoosedQualityNode,SortId,CurrentPage)
		elseif 	sendInfoTbl[1] == FuncTbl.GetSearchInfoNeedParent then			--[查询某类物品中的某两个属性列]需要其parentNode
			Gac2Gas:CSMSearchBuyOrder(g_Conn,sendInfoTbl[1],sendInfoTbl[2],sendInfoTbl[3],ItemLanName,Search_LowLevel,Search_HighLevel,
																	ChoosedQualityNode,SortId,CurrentPage)
		elseif 	sendInfoTbl[1] == FuncTbl.GetSearchInfoBySomeItemType then		--[查询某几类物品]
			Gac2Gas:CSMSearchBuyOrder(g_Conn,2,sendInfoTbl[2],0,ItemLanName,Search_LowLevel,Search_HighLevel,
																	ChoosedQualityNode,SortId,CurrentPage)
		elseif sendInfoTbl[1] == FuncTbl.GetSearchInfoByExactItemAttr then		--[查询某类物品中的某个属性列表中的一个具体的属性]
			Gac2Gas:CSMSearchBuyOrder(g_Conn,3,sendInfoTbl[2],sendInfoTbl[3],ItemLanName,Search_LowLevel,Search_HighLevel,
																	ChoosedQualityNode,SortId,CurrentPage)
		elseif sendInfoTbl[1] == FuncTbl.GetSearchInfoByItemTypeOrAttr then --根据物品type查询几类物品或者某些类中的符合要求的属性
		  Gac2Gas:CSMSearchBuyOrder(g_Conn,4,sendInfoTbl[2],sendInfoTbl[3],ItemLanName,Search_LowLevel,Search_HighLevel,
																	ChoosedQualityNode,SortId,CurrentPage)
		end
	else
		Gac2Gas:CSMSearchBuyOrder( g_Conn,0,0,0,ItemLanName,Search_LowLevel,Search_HighLevel,ChoosedQualityNode,SortId,CurrentPage)
	end
	
	self.m_LastSearch_CurClickNode 				= curClickNode
	self.m_LastSearch_ItemLanName 				= ItemLanName
	self.m_LastSearch_Search_LowLevel 		= Search_LowLevel
	self.m_LastSearch_Search_HighLevel 		= Search_HighLevel
	self.m_LastSearch_ChoosedQualityNode 	= ChoosedQualityNode
	self.m_LastSearch_SortId 							= SortId
	--self.m_LastSearch_CurrentPage 				= CurrentPage
end

function CPurchasingAgentMainWnd:SearchByLastSearchCondition()
	if self.m_LastSearch_CurClickNode and self.m_LastSearch_ItemLanName and self.m_LastSearch_Search_LowLevel and
		 self.m_LastSearch_Search_HighLevel and self.m_LastSearch_ChoosedQualityNode and self.m_LastSearch_SortId and self.m_CurrentPage then
			self:SearchByArg(self.m_LastSearch_CurClickNode,self.m_LastSearch_ItemLanName,self.m_LastSearch_Search_LowLevel,
									self.m_LastSearch_Search_HighLevel,self.m_LastSearch_ChoosedQualityNode,self.m_LastSearch_SortId,self.m_CurrentPage)
	end
end

function CPurchasingAgentMainWnd:ResetSearch()
	self.m_ItemNameEdit:SetWndText("")
	self.m_LowLevelEdit:SetWndText("")
	self.m_HighLevelEdit:SetWndText("")
	self.m_Text:ShowWnd(false)
	local qualityColorStr = g_ColorMgr:GetColor("稀1")
	local Flag = IMAGE_PARAM:new(SM_CB_BK, IP_ENABLE)  
	local pos = string.find(qualityColorStr, "c")
	local colorStr = "0xff" .. string.sub(qualityColorStr, pos + 1) 
	self.m_QualityComBox:SetWndTextColor(Flag, colorStr)
	self.m_QualityComBox:SetWndText(" " .. qualityInfo[1])
	if self.m_ChoosedTreeNode ~= 0 then
		self.m_CSMBuyTreeList:SelectNode(self.m_ChoosedTreeNode, false)
	end
	self.m_ChoosedTreeNode = 0
	self.m_ChoosedQualityNode = -1
	self.m_PrePage:EnableWnd(false)
	self.m_NextPage:EnableWnd(false)
	self.m_CurrentPage = 0
	self.m_MaxPage = 0
	self.m_CurPageNumEdit:SetWndText(self.m_CurrentPage.."/"..self.m_MaxPage)
end

function CPurchasingAgentMainWnd:OnSellBtn()
--	local Index = self.m_CSMBuyItemList:GetSelectItem(-1) + 1
	local Index = g_GameMain.m_PurchasingAgentMainWnd.m_CurrentChoosedItem.m_Index
	if(not Index or Index < 1 or Index > # self.m_tblBuyItemInfo) then return end
	local info = self.m_tblBuyItemInfo[Index]
	CPurchasingAgentSellWnd.GetWnd():OpenPanel(true,info)
	CPurchasingAgentSellWnd.GetWnd():SetFocus()
end

function CPurchasingAgentMainWnd:OnQuickSellBtn()	
	CPurchasingAgentQuickSellWnd.GetWnd():OpenPanel(true)
	CPurchasingAgentQuickSellWnd.GetWnd():SetFocus()
	Gac2Gas:CSMFastSearchBuyOrderItem(g_Conn)
end

function CPurchasingAgentMainWnd:SortMember(index, bFlag)
	if(bFlag) then
		table.sort(self.m_tblBuyItemInfo, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_tblBuyItemInfo, function (a, b) return a[index] > b[index] end)
	end
	--self.m_CurrentPage = 1
	self:DrawBuyItemInfoList()
end

function CPurchasingAgentMainWnd:DrawBuyItemInfoList()
	self.m_CurrentChoosedItem = nil --sort,search 要清空
	self.m_CSMBuyItemList:DeleteAllItem()
	self.m_tblCSMBuyItemWnd = {}
	self.m_SellBtn:EnableWnd(false)
	self.m_CurPageNumEdit:SetWndText(self.m_CurrentPage.."/"..self.m_MaxPage)
	self.m_CSMBuyItemList:InsertColumn( 0, 650 )
	
	if self.m_CurrentPage == 0 and self.m_MaxPage == 0 then
		self.m_PrePage:EnableWnd(false)
		self.m_NextPage:EnableWnd(false)
	else
		self.m_PrePage:EnableWnd(true)
		self.m_NextPage:EnableWnd(true)
	end
	
	local tbl_temp = {}
	for i = 1, # self.m_tblBuyItemInfo do
		if self.m_tblBuyItemInfo[i] == nil then
			break
		end
		table.insert(tbl_temp,self.m_tblBuyItemInfo[i])
	end
	for i = 1, # tbl_temp do
		local info = tbl_temp[i]
		self.m_CSMBuyItemList:InsertItem(i-1, 40)
		local item = self.m_CSMBuyItemList:GetSubItem( i-1, 0 )
		local embedvalue = CPurchasingAgentItemWnd.CreateWnd(item)
		embedvalue:ShowWnd( true )
		embedvalue.m_Index = i
		embedvalue:ShowCSMBuyItemWndInfo(info)
		table.insert(self.m_tblCSMBuyItemWnd, embedvalue)
	end
end

local function FlushItemFlashInfo(tbl_CanSellInfo)
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	local space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local BagSize = space:Size()
	for i = 1, BagSize do
		local fromRoom,srcPos 
		if(i > g_PlayerMainBag.size) then  
			fromRoom,srcPos = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetSlotIndex(i)
		else
			fromRoom = g_StoreRoomIndex.PlayerBag
			srcPos = i
		end
		local CurGrid = SpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
		local type, name = CurGrid:GetType()
		if tbl_CanSellInfo[name] then
			g_GameMain.m_WndMainBag.m_ctItemRoom:DelItemFlashInfo(fromRoom, srcPos)
			g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(fromRoom, srcPos)
		end
	end
end

function CPurchasingAgentMainWnd:DelStoreRoomPurchasingFlashInfo()
	local tbl_CanSellInfo = self.m_tblCanSellInfo
	self.m_tblCanSellInfo = {}
	FlushItemFlashInfo(tbl_CanSellInfo)
end

function CPurchasingAgentMainWnd:DrawStoreRoomPurchasingFlashInfo()
	FlushItemFlashInfo(self.m_tblCanSellInfo)
end

function CPurchasingAgentMainWnd:IsCanSellPurchasingItem(ItemName, bindingType)
	if bindingType == 2 or bindingType == 3 then --物品绑定类型为：绑定
		return false
	end
	return self.m_tblCanSellInfo[ItemName] ~= nil
end

function CPurchasingAgentMainWnd:ReturnPurchasingInfoBegin()
	self.m_tblBuyItemInfo = {}
	self.m_CurrentChoosedItem = nil
end
function CPurchasingAgentMainWnd:ReturnPurchasingInfoEnd(ItemCount)
	if ItemCount == 0 then
		self.m_Text:ShowWnd(true)
	else
		self.m_Text:ShowWnd(false)
	end
	self.m_MaxPage = math.floor(ItemCount / 50)
	local over = math.floor(ItemCount % 50)
	if over > 0 then
		self.m_MaxPage = self.m_MaxPage + 1
	end
	if ItemCount == 0 then
		self.m_CurrentPage = 0
		self.m_MaxPage = 0
	elseif self.m_MaxPage < 1 then
		self.m_MaxPage = 1
	end
	self:DrawBuyItemInfoList()
end
function CPurchasingAgentMainWnd:ReturnPurchasingInfo(OrderId,ItemName,Level,Count,SingleMoney,LeftTime,BuyPlayerName)
	local info = {}
	info.OrderId			= OrderId
	info.ItemName 		= ItemName
	info.Level 				= Level
	info.Count 				= Count
	info.SingleMoney 	= SingleMoney
	info.TotallMoney 	= Count*SingleMoney
	info.LeftTime 		= LeftTime > 0 and LeftTime or 1
	info.BuyPlayerName = BuyPlayerName
	table.insert(self.m_tblBuyItemInfo,info)
end

function CPurchasingAgentMainWnd:RetCanSellPurchasingInfoBegin()
	self.m_tblCanSellInfo = {}
end
function CPurchasingAgentMainWnd:RetCanSellPurchasingInfoEnd()
	self:DrawStoreRoomPurchasingFlashInfo()
end
function CPurchasingAgentMainWnd:RetCanSellPurchasingInfo(ItemName)
	self.m_tblCanSellInfo[ItemName] = true
end

----------------   item wnd   -------------------
function CPurchasingAgentItemWnd.CreateWnd(parent)
	local wnd = CPurchasingAgentItemWnd:new()
	wnd:CreateFromRes("PurchasingAgentItemWnd", parent)
	return wnd
end

function CPurchasingAgentItemWnd:OnChildCreated()
	self.m_ItemIcon		 	= self:GetDlgChild("icon")
	self.m_ItemName 		= self:GetDlgChild("itemName")
	self.m_Level 				= self:GetDlgChild("level")
	self.m_Count 				= self:GetDlgChild("count")
	self.m_SingleMoney 	= self:GetDlgChild("singleMoney")
	self.m_TotalMoney 	= self:GetDlgChild("totalMoney")
	self.m_LeftTime 		= self:GetDlgChild("leftTime")
	self.m_PlayerName 	= self:GetDlgChild("playerName")
	self.m_ItemChkBtn 	= self:GetDlgChild("ItemChkBtn")
	self.m_ItemChkBtn:SetMouseWheel(true)
end

function CPurchasingAgentItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_ItemChkBtn then
			if g_GameMain.m_PurchasingAgentMainWnd.m_CurrentChoosedItem then
				g_GameMain.m_PurchasingAgentMainWnd.m_CurrentChoosedItem.m_ItemChkBtn:SetCheck(false)
			end
			g_GameMain.m_PurchasingAgentMainWnd.m_CurrentChoosedItem = self
			g_GameMain.m_PurchasingAgentMainWnd.m_CurrentChoosedItem.m_ItemChkBtn:SetCheck(true)
			g_GameMain.m_PurchasingAgentMainWnd.m_SellBtn:EnableWnd(true)
		end
	end
end

function CPurchasingAgentItemWnd:ShowCSMBuyItemWndInfo(info)
	local ItemBigID = PurchasingAgent_Common(info.ItemName,"BigID")
	if not ItemBigID then
		LogErr("搜索求购的订单物品现在不在PurchasingAgent_Common表中,可能已经删除",info.ItemName)
		return
	end
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(ItemBigID,info.ItemName,"SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_ItemIcon, -1, IP_ENABLE, IP_ENABLE)
	g_SetItemRichToolTips(self.m_ItemIcon, ItemBigID, info.ItemName, 0)
	local DisPlayName = g_ItemInfoMgr:GetItemLanInfo(ItemBigID,info.ItemName,"DisplayName")
	self.m_ItemName:SetWndText(DisPlayName)
	self.m_Level:SetWndText(info.Level)
	self.m_Count:SetWndText(info.Count)
	self.m_SingleMoney:SetWndText(g_MoneyMgr:ChangeMoneyToString(info.SingleMoney,EGoldType.GoldCoin))
	self.m_TotalMoney:SetWndText(g_MoneyMgr:ChangeMoneyToString(info.TotallMoney,EGoldType.GoldCoin))
	self.m_LeftTime:SetWndText(info.LeftTime..GetStaticTextClient(1121))
	self.m_PlayerName:SetWndText(info.BuyPlayerName)
end

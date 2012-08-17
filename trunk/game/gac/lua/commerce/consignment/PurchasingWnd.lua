cfg_load "commerce/PurchasingAgent_Common"
gac_gas_require "framework/common/CMoney"
gac_require "commerce/consignment/PurchasingInc"

CPurchasingWnd = class ( SQRDialog )
CPurchasingItemWnd = class ( SQRDialog )

local comBoxItemHeight = 20
local qualityInfo = {GetStaticTextClient(1289),  GetStaticTextClient(1276), GetStaticTextClient(1277),
											GetStaticTextClient(1278), GetStaticTextClient(1279),
											GetStaticTextClient(1280), GetStaticTextClient(1281),
											GetStaticTextClient(1282), GetStaticTextClient(1282)}


function CPurchasingWnd.GetWnd()
	local Wnd = g_GameMain.m_PurchasingWnd
	if not Wnd then
		Wnd = CPurchasingWnd:new(g_GameMain)
		g_GameMain.m_PurchasingWnd = Wnd
	end
	return Wnd
end

function CPurchasingWnd:Ctor(parent)
	self:CreateFromRes("PurchasingWnd", parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1, true)
end

function CPurchasingWnd:OnChildCreated()
	self.m_CloseBtn 	= self:GetDlgChild("Close")
	self.m_BuyBtn 		= self:GetDlgChild("buyBtn")
	self.m_StopbuyBtn = self:GetDlgChild("stopbuyBtn")
	self.m_ItemNumAddBtn = self:GetDlgChild("ItemNumAdd")
	self.m_ItemNumDecBtn = self:GetDlgChild("ItemNumDec")
	self.m_ItemWndBtn	= self:GetDlgChild("ItemWndBtn")
	
	self.m_ItemTypeComBox1 	= self:GetDlgChild("ItemTypeComBox1")
	self.m_ItemTypeComBox2 	= self:GetDlgChild("ItemTypeComBox2")
	self.m_ItemComBox 			= self:GetDlgChild("ItemComBox")
	self.m_QualityComBox 		= self:GetDlgChild("QualityComBox")
	self.m_TimeComBox 			= self:GetDlgChild("TimeComBox")
	self.m_MyBuyItemList		= self:GetDlgChild("buyItemList")
	
	self.m_MoneyEdit1 = self:GetDlgChild("money1")
	self.m_MoneyEdit2 = self:GetDlgChild("money2")
	self.m_MoneyEdit3 = self:GetDlgChild("money3")
	self.m_ItemNumText = self:GetDlgChild("ItemNum")
	self.m_MoneyInfo1 = self:GetDlgChild("moneyInfo1")
	self.m_MoneyInfo2 = self:GetDlgChild("moneyInfo2")
	self.m_MoneyInfo3 = self:GetDlgChild("moneyInfo3")
	self.m_AveragePriceText = self:GetDlgChild("averagePriceText")
	self:SetStyle(0x00040000)
	
	self.m_TimeComBoxTbl = {}
	self.m_BigTypeComBoxTbl = {}
	self.m_SmallTypeComBoxTbl = {}
	self.m_ItemComBoxTbl = {}
	self.m_TimeTbl= {24,48,72}
	
	self:InitPurchasingItemNode()
	self:InitData()	
	self:CreateTimeComBox()
	self:CreateQualityComBox()
	self:CreateBuyItemFilterComBox()
	self.m_StopbuyBtn:EnableWnd(false)
end

function CPurchasingWnd:InitData()
	self.m_BigTypeChoosedItem = nil
	self.m_SmallTypeChoosedItem = nil
	self.m_QualityChoosedItem = nil
	self.m_TimeChoosedItem = 0
	self.m_ItemChoosedItem = nil
	self.m_uAveragePrice = 0
end

function CPurchasingWnd:ReInitData()
	self:InitData()
	self.m_ItemTypeComBox2:RemoveAllItem()
	self.m_ItemComBox:RemoveAllItem()
	self.m_ItemTypeComBox1:SetWndText("")
	self.m_ItemTypeComBox2:SetWndText("")
	self.m_ItemComBox:SetWndText("")
	self.m_QualityComBox:SetWndText("")
	self.m_TimeComBox:SetWndText(24)
	
	self.m_MoneyEdit1:SetWndText("")
	self.m_MoneyEdit2:SetWndText("")
	self.m_MoneyEdit3:SetWndText("")
	self.m_ItemNumText:SetWndText(1)
	self.m_MoneyInfo1:SetWndText("")
	self.m_MoneyInfo2:SetWndText("")
	self.m_MoneyInfo3:SetWndText("")
	self.m_AveragePriceText:SetWndText("")
	self.m_ItemWndBtn:SetMouseOverDescAfter("")
	g_DelWndImage(self.m_ItemWndBtn, 1, IP_ENABLE, IP_CLICKDOWN)
	self:OnBuyItemCountChanged()
	self.m_StopbuyBtn:EnableWnd(false)
	self.m_BuyBtn:EnableWnd(true)
end

function CPurchasingWnd:CreateTimeComBox()
	local ComBoxWidth = self.m_TimeComBox:GetWndWidth()
	self.m_TimeComBox:InsertColumn(0, ComBoxWidth)
	local TimeInfo = {24,48,72}
	for i=1, # TimeInfo do
		self.m_TimeComBox:InsertItem(" " .. TimeInfo[i], i-1, comBoxItemHeight)
		local item = self.m_TimeComBox:GetItem(i-1)
		table.insert(self.m_TimeComBoxTbl, item)
	end
end

function CPurchasingWnd:CreateQualityComBox()
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

function CPurchasingWnd:CreateBuyItemFilterComBox()
	local ComBoxWidth = self.m_ItemTypeComBox1:GetWndWidth()
	self.m_ItemTypeComBox1:InsertColumn(0, ComBoxWidth)
	local keys = Lan_CSMTreeList_Client:GetKeys()
	for i,v in pairs( keys ) do
		local ParentNodeName = CSMTreeList_Client(i,"ParentTreeNode")
		local uCount = 0
		for i,v in pairs( self.m_tblPurchasingItemNode ) do
			if ParentNodeName ~= nil and v.ParentNodeName == ParentNodeName then
				uCount = uCount + 1
			end
		end
		local ShowStr = Lan_CSMTreeList_Client(i,"DisParentTreeNode").."("..uCount..")"
		self.m_ItemTypeComBox1:InsertItem(" " .. ShowStr, i-1, comBoxItemHeight)
		local item = self.m_ItemTypeComBox1:GetItem(i-1)
		item.Index = i-1
		item.ShowName = ShowStr
		self.m_BigTypeComBoxTbl[ParentNodeName] = item
	end
end

function CPurchasingWnd:OpenPanel(bShow)
	if bShow then
		self.m_tblMyPurchasingItemInfo = {}
		self:ReInitData()
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
		g_GameMain.m_WndMainBag:ShowWnd(true)
		g_GameMain.m_WndMainBag:SetFocus()
		Gac2Gas:CSMSearchCharBuyOrder(g_Conn)
	end
	self:ShowWnd(bShow)
end

function CPurchasingWnd:VirtualExcludeWndClosed()
	g_GameMain.m_WndMainBag:ShowWnd(false)
end

function CPurchasingWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_ItemNumAddBtn) then
			self:AddBuyItemCount()
		elseif (Child == self.m_ItemNumDecBtn) then
			self:DelBuyItemCount()
		elseif (Child == self.m_BuyBtn) then
			self:Buy()
		elseif (Child == self.m_StopbuyBtn) then
			self:CancelBuy()
		elseif (Child == self.m_ItemWndBtn) then
			self:OnClickItemBtn()
		elseif Child == self.m_CloseBtn then
			self:OpenPanel(false)
			CPurchasingAgentMainWnd.GetWnd():OpenPanel(true)
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
		if (Child == self.m_ItemWndBtn) then
			self:ReInitData()
		end
	elseif uMsgID == WND_NOTIFY then
		if (Child == self.m_MoneyEdit1 or Child == self.m_MoneyEdit2 or Child == self.m_MoneyEdit3) then
			if not IsNumber( tonumber(Child:GetWndText()) ) then
				Child:SetWndText("")
			elseif Child == self.m_MoneyEdit1 then
				local money = CMoney:new()
				money:CheckInputMoneyValidate(Child, 1)
			elseif Child == self.m_MoneyEdit2 or Child == self.m_MoneyEdit3 then
				local money = CMoney:new()
				money:CheckInputMoneyValidate(Child, 2)
			end
			self:OnBuyItemCountChanged()
		elseif (Child == self.m_ItemNumText) then
			if not IsNumber( tonumber(Child:GetWndText()) ) or (tonumber(Child:GetWndText()) < 1 or tonumber(Child:GetWndText()) > 10000) then
				Child:SetWndText(1)
			end
			self:OnBuyItemCountChanged()
		end
	elseif (uMsgID == ITEM_LBUTTONCLICK) then
		if self.m_ItemTypeComBox1 == Child then
			local choosedItem = self.m_ItemTypeComBox1:GetCurrentSel()
			if(not choosedItem or 0 > choosedItem) then return end
			self.m_BigTypeChoosedItem = choosedItem
			self:OnChooseBigTypeChanged()
			self:ShowItemCanBuy()
		elseif self.m_ItemTypeComBox2 == Child then
			local choosedItem = self.m_ItemTypeComBox2:GetCurrentSel()
			if(not choosedItem or 0 > choosedItem) then return end
			self.m_SmallTypeChoosedItem = choosedItem
			self:ShowItemCanBuy()
		elseif self.m_QualityComBox == Child then
			local choosedItem = self.m_QualityComBox:GetCurrentSel()
			self.m_QualityChoosedItem = nil
			if(choosedItem and choosedItem > 0) then
				self.m_QualityChoosedItem = choosedItem - 1
			end
			self:ShowItemCanBuy()
		elseif self.m_ItemComBox == Child then
			local choosedItem = self.m_ItemComBox:GetCurrentSel()
			if(not choosedItem or 0 > choosedItem) then return end
			self.m_ItemChoosedItem = choosedItem
			self:OnChosseItem()
		elseif self.m_TimeComBox == Child then
			local choosedItem = self.m_TimeComBox:GetCurrentSel()
			if(not choosedItem or 0 > choosedItem) then return end
			self.m_TimeChoosedItem = choosedItem
			self:OnBuyItemCountChanged()
	end
	end
end

function CPurchasingWnd:OnChooseBigTypeChanged()
	self.m_ItemTypeComBox2:RemoveAllItem()
	self.m_ItemTypeComBox2:SetWndText("")
	self.m_SmallTypeComBoxTbl = {}
	self.m_ItemComBox:RemoveAllItem()
	self.m_ItemComBox:SetWndText("")
	self.m_ItemComBoxTbl = {}
	local ComBoxWidth = self.m_ItemTypeComBox2:GetWndWidth()
	self.m_ItemTypeComBox2:InsertColumn(0, ComBoxWidth)
	local uIndex = self.m_BigTypeChoosedItem + 1
	local ItemWndIndex = 0
	
	local bExistChild = false
	for k=2, 12 do
		local lanNodeStr
		lanNodeStr = string.format("%s%d", "DisChildNode", k-1)
		local nodeInLan =   Lan_CSMTreeList_Client(uIndex,lanNodeStr)
		if IsNumber(nodeInLan) ~= true then
			bExistChild = true
			if nodeInLan == "" or nodeInLan == nil  then           
				break
			else
				local ChildNodeStr = string.format("%s%d", "ChildNode", k-1)
				local ChildNodeName = CSMTreeList_Client(uIndex,ChildNodeStr)
				if ChildNodeName ~= "" then
					local ParentNodeName = CSMTreeList_Client(uIndex,"ParentTreeNode")
					local uCount = 0
					for i,v in pairs( self.m_tblPurchasingItemNode ) do
						if (ParentNodeName ~= nil and v.ParentNodeName == ParentNodeName) and
							(ChildNodeName == nil or ChildNodeName == "" or v.ChildNodeName == ChildNodeName) then
							uCount = uCount + 1
						end
					end
					nodeInLan = nodeInLan .. "("..uCount..")"
					self.m_ItemTypeComBox2:InsertItem(" " .. nodeInLan, ItemWndIndex, comBoxItemHeight)
					local item = self.m_ItemTypeComBox2:GetItem(ItemWndIndex)
					item.Index = ItemWndIndex
					item.ShowName = nodeInLan
					self.m_SmallTypeComBoxTbl[ChildNodeName] = item
					ItemWndIndex = ItemWndIndex + 1
				end
			end
		end
	end
	if not bExistChild then
		self:ShowItemCanBuy()
	end
end

function CPurchasingWnd:ShowItemCanBuy()
	if self.m_BigTypeChoosedItem == nil then
		return
	end
	self.m_ItemComBox:RemoveAllItem()
	self.m_ItemComBox:SetWndText("")
	self.m_ItemComBoxTbl = {}
	self.m_ItemWndBtn:SetMouseOverDescAfter("")
	g_DelWndImage(self.m_ItemWndBtn, 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_ItemChoosedItem = nil
	local ComBoxWidth = self.m_ItemComBox:GetWndWidth()
	self.m_ItemComBox:InsertColumn(0, ComBoxWidth)
	local uIndex = self.m_BigTypeChoosedItem + 1
	local ParentNodeName = CSMTreeList_Client(uIndex,"ParentTreeNode")
	if ParentNodeName == nil then
		return
	end
	local nodeStr
	if self.m_SmallTypeChoosedItem == 0 or self.m_SmallTypeChoosedItem == nil then
		nodeStr = string.format("%s", "ChildNode1")
	else
		nodeStr = string.format("%s%d", "ChildNode", self.m_SmallTypeChoosedItem+1)
	end
	local ChildNodeName =   CSMTreeList_Client(uIndex,nodeStr)
	if ChildNodeName == nil then
		return
	end

	local ItemWndIndex = 0
	for i,v in pairs( self.m_tblPurchasingItemNode ) do
		if ParentNodeName ~= nil and v.ParentNodeName == ParentNodeName and
				ChildNodeName ~= nil and v.ChildNodeName == ChildNodeName then
			local ItemName = i
			local ItemBigID = PurchasingAgent_Common(ItemName,"BigID")
			local DisPlayName = g_ItemInfoMgr:GetItemLanInfo(ItemBigID,ItemName,"DisplayName")
			local Quality = g_ItemInfoMgr:GetItemInfo(ItemBigID, ItemName,"Quality")--稀有度
			if Quality == self.m_QualityChoosedItem or self.m_QualityChoosedItem == nil then
				self.m_ItemComBox:InsertItem(" " .. DisPlayName, ItemWndIndex, comBoxItemHeight)
				local item = self.m_ItemComBox:GetItem(ItemWndIndex)
				item.ItemType = ItemBigID
				item.ItemName = ItemName
				table.insert(self.m_ItemComBoxTbl, item)
				ItemWndIndex = ItemWndIndex + 1
			end
		end
	end
end

function CPurchasingWnd:OnChosseItem()
	if self.m_ItemChoosedItem then
		local ItemInfo = self.m_ItemComBoxTbl[self.m_ItemChoosedItem+1]
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(ItemInfo.ItemType,ItemInfo.ItemName,"SmallIcon")
		g_DelWndImage(self.m_ItemWndBtn, 1, IP_ENABLE, IP_CLICKDOWN)
		g_LoadIconFromRes(SmallIcon, self.m_ItemWndBtn, -1, IP_ENABLE, IP_CLICKDOWN)
		g_SetItemRichToolTips(self.m_ItemWndBtn, ItemInfo.ItemType, ItemInfo.ItemName, 0)
		self.m_AveragePriceText:SetWndText("")
		Gac2Gas:CSMGetAveragePriceByItemName(g_Conn,ItemInfo.ItemName)
	end
end

function CPurchasingWnd:OnBuyItemCountChanged()
	local BuyItemCount = tonumber(self.m_ItemNumText:GetWndText())
	if not BuyItemCount then
		return
	end
	local uGold = tonumber(self.m_MoneyEdit1:GetWndText())
	local uSilve = tonumber(self.m_MoneyEdit2:GetWndText())
	local uCopper = tonumber(self.m_MoneyEdit3:GetWndText())
	uGold = uGold and uGold or 0
	uSilve = uSilve and uSilve or 0
	uCopper = uCopper and uCopper or 0
	
	local itemPrice = uGold*10000 + uSilve*100 + uCopper
	local curOderTime = self.m_TimeTbl[self.m_TimeChoosedItem+1]
	local totalFee = g_MoneyMgr:CalculateTotalFee(itemPrice, BuyItemCount, 0.01, curOderTime/24)
	local totalCostMoney = itemPrice*BuyItemCount + totalFee

	uGold = math.floor(totalCostMoney / 10000)
	local uLeftCopper = totalCostMoney - uGold*10000
	uSilve = math.floor(uLeftCopper / 100)
	uCopper = uLeftCopper - uSilve*100
	
	self.m_MoneyInfo1:SetWndText(uGold)
	self.m_MoneyInfo2:SetWndText(uSilve)
	self.m_MoneyInfo3:SetWndText(uCopper)
end

function CPurchasingWnd:OnClickItemBtn()
	local state, context = g_WndMouse:GetCursorState()
	if state == ECursorState.eCS_PickupItem then --手上有物品
		self:OnChooseItem(context)
		g_WndMouse:ClearCursorAll()
	end
end

function CPurchasingWnd:OnChooseItem(context)
	if not context then
		return
	end
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )	
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	
	if grid then
		local bCanPurcasing = false
		local BigId, Index = grid:GetType()
		for i,v in pairs( self.m_tblPurchasingItemNode ) do
			if i == Index then
				bCanPurcasing = true
				local ParentNodeName = v.ParentNodeName
				local ChildNodeName = v.ChildNodeName
				self.m_BigTypeChoosedItem 	= self.m_BigTypeComBoxTbl[ParentNodeName].Index
				self.m_ItemTypeComBox1:SetWndText(self.m_BigTypeComBoxTbl[ParentNodeName].ShowName)
				self:OnChooseBigTypeChanged()
				if ChildNodeName ~= "" then
					if self.m_SmallTypeComBoxTbl[ChildNodeName] then
						self.m_SmallTypeChoosedItem = self.m_SmallTypeComBoxTbl[ChildNodeName].Index
						self.m_ItemTypeComBox2:SetWndText(self.m_SmallTypeComBoxTbl[ChildNodeName].ShowName)
					else
						LogErr("求购物品没有匹配的分类",ChildNodeName)
					end
				end
				self.m_QualityChoosedItem = g_ItemInfoMgr:GetItemInfo(BigId, Index, "Quality")--稀有度
				self.m_QualityComBox:SetWndText(qualityInfo[self.m_QualityChoosedItem+2])
				self:ShowItemCanBuy()
				for j=1, # self.m_ItemComBoxTbl do
					if self.m_ItemComBoxTbl[j].ItemName == Index then
						local DisPlayName = g_ItemInfoMgr:GetItemLanInfo(self.m_ItemComBoxTbl[j].ItemType,Index,"DisplayName")
						self.m_ItemComBox:SetWndText(DisPlayName)
						self.m_ItemChoosedItem = j-1
						self:OnChosseItem()
						break
					end
				end
				break
			end
		end
		if not bCanPurcasing then
			MsgClient(8307)
		end
	end
end

function CPurchasingWnd:GetItemSingleMoney()
	local uGold = tonumber(self.m_MoneyEdit1:GetWndText())
	local uSilve = tonumber(self.m_MoneyEdit2:GetWndText())
	local uCopper = tonumber(self.m_MoneyEdit3:GetWndText())
	local uSingleMoney = 0
	if uGold then
		uSingleMoney = uGold * 100 * 100
	end
	if uSilve then
		uSingleMoney = uSingleMoney + uSilve * 100
	end
	if uCopper then
		uSingleMoney = uSingleMoney + uCopper
	end
	return uSingleMoney
end

function CPurchasingWnd:AddBuyItemCount()
	local BuyItemCount = tonumber(self.m_ItemNumText:GetWndText())
	if not BuyItemCount then
		BuyItemCount = 0
	end
	BuyItemCount = BuyItemCount + 1
	if BuyItemCount <= 10000 then
		self.m_ItemNumText:SetWndText(BuyItemCount)
		self:OnBuyItemCountChanged()
	end
end

function CPurchasingWnd:DelBuyItemCount()
	local BuyItemCount = tonumber(self.m_ItemNumText:GetWndText())
	if not BuyItemCount then
		BuyItemCount = 1
	end
	if BuyItemCount > 1 then
		BuyItemCount = BuyItemCount - 1
	end
	self.m_ItemNumText:SetWndText(BuyItemCount)
	self:OnBuyItemCountChanged()
end

function CPurchasingWnd:Buy()
	local BuyItemCount = tonumber(self.m_ItemNumText:GetWndText())
	if not BuyItemCount then
		MsgClient(8301)
		return
	end
	self.LastPurchasingSingleMoney 	= self:GetItemSingleMoney()
	if self.m_BigTypeChoosedItem == nil or self.m_TimeChoosedItem == nil or self.m_ItemChoosedItem == nil then
		MsgClient(8301)
		return
	elseif self.LastPurchasingSingleMoney == 0 then
		MsgClient(8308)
		return
	elseif self.m_uAveragePrice and self.m_uAveragePrice > 0 and 
		(self.LastPurchasingSingleMoney < self.m_uAveragePrice/10 or self.LastPurchasingSingleMoney > self.m_uAveragePrice*2 )then
		MsgClient(8311)
		return
	end
	local ItemInfo = self.m_ItemComBoxTbl[self.m_ItemChoosedItem+1]
	self.LastPurchasingItemName 		= ItemInfo.ItemName
	self.LastPurchasingLeftTime 		= self.m_TimeTbl[self.m_TimeChoosedItem+1]
	self.LastBuyItemCount 					= BuyItemCount
	self.m_BuyBtn:EnableWnd(false)
	Gac2Gas:CSMAddBuyOrder(g_Conn, ItemInfo.ItemName, BuyItemCount, self:GetItemSingleMoney(), self.m_TimeTbl[self.m_TimeChoosedItem+1])
end

function CPurchasingWnd:CancelBuy()
	if not self.m_CurrentChoosedItem then
		return
	end
--	local Index = self.m_MyBuyItemList:GetSelectItem(-1) + 1
--	local tbl = self.m_tblMyPurchasingItemInfo
--	if(not Index or Index < 1 or Index > #tbl) then return end
--	local info = tbl[Index]
	Gac2Gas:CSMCancelBuyOrder(g_Conn,self.m_CurrentChoosedItem.OrderId)
end

function CPurchasingWnd:ReturnMyPurchasingInfoBegin()
	self.m_tblMyPurchasingItemInfo = {}
end

function CPurchasingWnd:ReturnMyPurchasingInfo(OrderId,ItemName,SingleMoney,LeftTime,LeftBuyCount)
	local OrderInfo = {}
	OrderInfo.OrderId = OrderId
	OrderInfo.ItemName = ItemName
	OrderInfo.SingleMoney = SingleMoney
	OrderInfo.LeftTime = LeftTime
	OrderInfo.LeftBuyCount = LeftBuyCount
	table.insert(self.m_tblMyPurchasingItemInfo,OrderInfo)
end

function CPurchasingWnd:ReturnMyPurchasingInfoEnd()
	self.m_MyBuyItemList:DeleteAllItem()
	self.m_tblCSMMyBuyItemWnd = {}
	self.m_MyBuyItemList:InsertColumn( 0, 550 )
	for i = 1, # self.m_tblMyPurchasingItemInfo do
		self:AddMyPurchasingItemInfo(i)
	end
	self.m_CurrentChoosedItem = nil
	self.m_StopbuyBtn:EnableWnd(false)
end

function CPurchasingWnd:AddMyPurchasingItemInfo(i)
	local OrderInfo = self.m_tblMyPurchasingItemInfo[i]
	if not OrderInfo then
		return
	end
	local ItemBigID = PurchasingAgent_Common(OrderInfo.ItemName,"BigID")
	if not ItemBigID then
		LogErr("之前求购的订单物品现在不在PurchasingAgent_Common表中,可能已经删除",OrderInfo.ItemName)
		return
	end
	self.m_MyBuyItemList:InsertItem(i-1, 40)
	local item = self.m_MyBuyItemList:GetSubItem( i-1, 0 )
	local embedvalue = CPurchasingItemWnd.CreateWnd(item)
	embedvalue:ShowWnd( true )
	embedvalue:ShowPurchasingItemWndInfo(OrderInfo)
	embedvalue.OrderId = OrderInfo.OrderId
	table.insert(self.m_tblCSMMyBuyItemWnd, embedvalue)
end

function CPurchasingWnd:RetAddMyPurchasing(Success,OrderId)
	self.m_BuyBtn:EnableWnd(true)
	if Success and self:IsShow() then
		self:ReturnMyPurchasingInfo(OrderId,self.LastPurchasingItemName,self.LastPurchasingSingleMoney,self.LastPurchasingLeftTime,self.LastBuyItemCount)
		self:AddMyPurchasingItemInfo(# self.m_tblMyPurchasingItemInfo)
		self:ReInitData()
	end
end

function CPurchasingWnd:RetAveragePriceByItemName(AveragePrice,Count)
	if Count == 0 then
		self.m_uAveragePrice = 0
		self.m_AveragePriceText:SetWndText("")
	else
		self.m_uAveragePrice = AveragePrice
		self.m_AveragePriceText:SetWndText(GetStaticTextClient(1157)..g_MoneyMgr:ChangeMoneyToString(AveragePrice,EGoldType.GoldCoin))
	end
end

function CPurchasingWnd:RetCancelBuy(Success,OrderId)
	if Success and self:IsShow() then
		for i=1, # self.m_tblMyPurchasingItemInfo do
			if self.m_tblMyPurchasingItemInfo[i].OrderId == OrderId then
				table.remove(self.m_tblMyPurchasingItemInfo,i)
				break
			end
		end
		self:ReturnMyPurchasingInfoEnd()
	end
end


function CPurchasingWnd:InitPurchasingItemNode()
	--把PurchasingAgent_Common表内物品的ParentNode,ChildNode初始化出来
	self.m_tblPurchasingItemNode = {}
	local PurchasingKeys = PurchasingAgent_Common:GetKeys()
	for i,v in pairs( PurchasingKeys ) do
		local ItemName = PurchasingAgent_Common(v,"ItemName")
		local ItemBigID = PurchasingAgent_Common(v,"BigID")
		local ParentNode = GetChildNodeByItemType(ItemBigID,ItemName)
		if ParentNode then
			local ParentNodeName = CSMTreeList_Client(ParentNode,"ParentTreeNode")
			local ChildNodeName = GetChildNode(ItemBigID,ItemName)
			self.m_tblPurchasingItemNode[ItemName] = {}
			self.m_tblPurchasingItemNode[ItemName].ParentNodeName = ParentNodeName
			self.m_tblPurchasingItemNode[ItemName].ChildNodeName = ChildNodeName
		else
			LogErr("InitPurchasingItemNode ParentNode is nil",ItemBigID.." , "..ItemName)
		end
	end
end

----------------   item wnd   -------------------
function CPurchasingItemWnd.CreateWnd(parent)
	local wnd = CPurchasingItemWnd:new()
	wnd:CreateFromRes("PurchasingItemWnd", parent)
	return wnd
end

function CPurchasingItemWnd:OnChildCreated()
	self.m_Icon 			= self:GetDlgChild("Icon")
	self.m_Name 			= self:GetDlgChild("Name")
	self.m_Money 			= self:GetDlgChild("Money")
	self.m_Time 			= self:GetDlgChild("Time")
	self.m_CancelBtn 	= self:GetDlgChild("Cancel")
	self.m_CheckBtn 	= self:GetDlgChild("CheckBtn")
end

function CPurchasingItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_CancelBtn) and (self.OrderId) then
			Gac2Gas:CSMCancelBuyOrder(g_Conn,self.OrderId)
		elseif (Child == self.m_CheckBtn) then
			if g_GameMain.m_PurchasingWnd.m_CurrentChoosedItem then
				g_GameMain.m_PurchasingWnd.m_CurrentChoosedItem.m_CheckBtn:SetCheck(false)
			end
			g_GameMain.m_PurchasingWnd.m_CurrentChoosedItem = self
			g_GameMain.m_PurchasingWnd.m_CurrentChoosedItem.m_CheckBtn:SetCheck(true)
			g_GameMain.m_PurchasingWnd.m_StopbuyBtn:EnableWnd(true)
		end
	end
end

function CPurchasingItemWnd:ShowPurchasingItemWndInfo(OrderInfo)
	local ItemBigID = PurchasingAgent_Common(OrderInfo.ItemName,"BigID")
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(ItemBigID,OrderInfo.ItemName,"SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	g_SetItemRichToolTips(self.m_Icon, ItemBigID, OrderInfo.ItemName, 0)
	
	local DisPlayName = g_ItemInfoMgr:GetItemLanInfo(ItemBigID,OrderInfo.ItemName,"DisplayName")
	self.m_Name:SetWndText(DisPlayName)
	self.m_Icon:SetWndText(OrderInfo.LeftBuyCount)
	self.m_Money:SetWndText(g_MoneyMgr:ChangeMoneyToString(OrderInfo.SingleMoney,EGoldType.GoldCoin))
	self.m_Time:SetWndText(OrderInfo.LeftTime..GetStaticTextClient(1121))
end


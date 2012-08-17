
CPurchasingAgentQuickSellWnd = class ( SQRDialog )

function CPurchasingAgentQuickSellWnd.GetWnd()
	local Wnd = g_GameMain.m_PurchasingAgentQuickSellWnd
	if not Wnd then
		Wnd = CPurchasingAgentQuickSellWnd:new(g_GameMain)
		g_GameMain.m_PurchasingAgentQuickSellWnd = Wnd
	end
	return Wnd
end

function CPurchasingAgentQuickSellWnd:Ctor(parent)
	self:CreateFromRes("PurchasingAgentQuickSellWnd", parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, true)
end

function CPurchasingAgentQuickSellWnd:OnChildCreated()
	self.m_ItemBtn = self:GetDlgChild("ItemBtn")
	self.m_OkBtn = self:GetDlgChild("okBtn")
	self.m_CloseBtn = self:GetDlgChild("close")
	self.m_SingleMoneyText = self:GetDlgChild("singleMoneyText")
	self.m_TotallMoneyText = self:GetDlgChild("totallMoneyText")
	self.m_StaticText = self:GetDlgChild("staticText")
end

function CPurchasingAgentQuickSellWnd:ReSetData()
	g_DelWndImage(self.m_ItemBtn, 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_SingleMoneyText:SetWndText("")
	self.m_TotallMoneyText:SetWndText("")
	self.m_ItemBtn:SetWndText("")
	self.m_RoomIndex = nil
	self.m_srcPos = nil
	self.m_OrderId = nil
	self:ClearLashSellInfo()
	self.m_StaticText:SetWndText(GetStaticTextClient(1158))
end

function CPurchasingAgentQuickSellWnd:VirtualExcludeWndClosed()
	CPurchasingAgentMainWnd.GetWnd():DelStoreRoomPurchasingFlashInfo()
	g_GameMain.m_WndMainBag:ShowWnd(false)
	self:ClearLashSellInfo()
end

function CPurchasingAgentQuickSellWnd:OpenPanel(bShow)
	self:ShowWnd(bShow)
	if bShow then
		self:ReSetData()
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
		CPurchasingAgentSellWnd.GetWnd():OpenPanel(false)
		g_GameMain.m_WndMainBag:ShowWnd(true)
		g_GameMain.m_WndMainBag:SetFocus()
	end
end

function CPurchasingAgentQuickSellWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_ItemBtn) then
			self:OnClickItemBtn()
		elseif (Child == self.m_OkBtn) then
			self:OnSellBtn()
		elseif (Child == self.m_CloseBtn) then
			self:OpenPanel(false)
		end
	elseif (uMsgID == BUTTON_RIGHTCLICKDOWN) then
		if (Child == self.m_ItemBtn) then
			self:ReSetData()
		end
	end
end

function CPurchasingAgentQuickSellWnd:OnShowPurchasingAgentSellInfo(Grid, SingleMoney, Count)
	if Grid == nil then
		self:ClearLashSellInfo()
		return
	end
	local BigId, Index = Grid:GetType()
	local OpenedFlag = false
	local ItemIDTbl = Grid:GetItemID(Grid:Size())
	if #(ItemIDTbl) > 0 then
		local dynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemIDTbl[1])
		OpenedFlag = dynInfo.OpenedFlag
	end
	
	local tbl_PurchasingItemTbl = g_GameMain.m_PurchasingAgentMainWnd.m_tblCanSellInfo
	if tbl_PurchasingItemTbl[Index] and (not OpenedFlag) then
		local ItemCount = Grid:Size()
		local SellCount = (Count > ItemCount) and ItemCount or Count
		local Money = SingleMoney * SellCount
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(BigId,Index,"SmallIcon")
		g_DelWndImage(self.m_ItemBtn, 1, IP_ENABLE, IP_CLICKDOWN)
		g_LoadIconFromRes(SmallIcon, self.m_ItemBtn, -1, IP_ENABLE, IP_CLICKDOWN)
		self.m_ItemBtn:SetWndText(SellCount)
		self.m_SingleMoneyText:SetWndText(g_MoneyMgr:ChangeMoneyToString(SingleMoney,EGoldType.GoldCoin))
		self.m_TotallMoneyText:SetWndText(g_MoneyMgr:ChangeMoneyToString(Money,EGoldType.GoldCoin))
		self.m_StaticText:SetWndText(GetStaticTextClient(1159))
	else
		self:ClearLashSellInfo()
		MsgClient(8305)
		g_DelWndImage(self.m_ItemBtn, 1, IP_ENABLE, IP_CLICKDOWN)
		self.m_SingleMoneyText:SetWndText("")
		self.m_TotallMoneyText:SetWndText("")
		self.m_ItemBtn:SetWndText("")
	end
end

function CPurchasingAgentQuickSellWnd:OnClickItemBtn()
	local state, context = g_WndMouse:GetCursorState()
	self:PurchasingAgentSell(context)
	g_WndMouse:ClearCursorAll()
end

function CPurchasingAgentQuickSellWnd:PurchasingAgentSell(context)
	if not context then
		return
	end
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )	
	self.m_RoomIndex = fromRoom
	self.m_srcPos = srcPos
	self:ClearLashSellInfo()
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local BigId, Index = grid:GetType()
	Gac2Gas:CSMGetTopPriceBuyOrderByItemName(g_Conn,Index)
	local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
	table.insert(self.m_SelectGridTable, {x, y, fromRoom})
	SrcCt:SetClickedWndState(x, y, fromRoom, false)
end

function CPurchasingAgentQuickSellWnd:ClearLashSellInfo()
	if self.m_SelectGridTable == nil then
		self.m_SelectGridTable = {}
		return
	end
	for i=1, #( self.m_SelectGridTable ) do
		if self.m_SelectGridTable[i] ~= nil then
			local x, y, roomIndex = unpack(self.m_SelectGridTable[i],1,3)
			local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
			SrcCt:SetClickedWndState(x, y, roomIndex, true)
		end
	end
	self.m_SelectGridTable = {}
end

function CPurchasingAgentQuickSellWnd:RetQuickSellItemOrder(OrderId,SingleMoney,Count)
	if self.m_RoomIndex == nil or self.m_srcPos == nil then
		self:ReSetData()
		return
	end
	self.m_OrderId = OrderId
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(self.m_RoomIndex, self.m_srcPos)
	self:OnShowPurchasingAgentSellInfo(grid, SingleMoney, Count)
end

function CPurchasingAgentQuickSellWnd:RetQuickSellItemOrderFail()
	self:ReSetData()
end

function CPurchasingAgentQuickSellWnd:OnSellBtn()
	if self.m_OrderId and self.m_RoomIndex and self.m_srcPos then
		Gac2Gas:CSMSellGoods2Order(g_Conn,self.m_OrderId,self.m_RoomIndex,self.m_srcPos)
	end
end

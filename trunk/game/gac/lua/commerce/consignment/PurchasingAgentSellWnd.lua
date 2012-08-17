
CPurchasingAgentSellWnd = class ( SQRDialog )

function CPurchasingAgentSellWnd.GetWnd()
	local Wnd = g_GameMain.m_PurchasingAgentSellWnd
	if not Wnd then
		Wnd = CPurchasingAgentSellWnd:new(g_GameMain)
		g_GameMain.m_PurchasingAgentSellWnd = Wnd
	end
	return Wnd
end

function CPurchasingAgentSellWnd:Ctor(parent)
	self:CreateFromRes("PurchasingAgentSellWnd", parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, true)
end

function CPurchasingAgentSellWnd:OnChildCreated()
	self.m_ItemBtn = self:GetDlgChild("ItemBtn")
	self.m_OkBtn = self:GetDlgChild("okBtn")
	self.m_CloseBtn = self:GetDlgChild("close")
	self.m_SingleMoneyText = self:GetDlgChild("singleMoneyText")
	self.m_TotallMoneyText = self:GetDlgChild("totallMoneyText")
end

function CPurchasingAgentSellWnd:ReSetData()
	g_DelWndImage(self.m_ItemBtn, 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_SingleMoneyText:SetWndText("")
	self.m_TotallMoneyText:SetWndText("")
	self.m_ItemBtn:SetWndText("")
	self.m_RoomIndex = nil
	self.m_srcPos = nil
	self:ClearLashSellInfo()
end

function CPurchasingAgentSellWnd:VirtualExcludeWndClosed()
	g_GameMain.m_WndMainBag:ShowWnd(false)
	self:ClearLashSellInfo()
end

function CPurchasingAgentSellWnd:OpenPanel(bShow,PurchasingInfo)
	--g_ExcludeWndMgr:ShowWnd(self, bShow)
	self:ShowWnd(bShow)
	if bShow then
		CPurchasingAgentQuickSellWnd.GetWnd():OpenPanel(false)
		self:ReSetData()
		g_GameMain.m_WndMainBag:ShowWnd( true)
	end
	self.m_PurchasingInfo = PurchasingInfo
	local ItemRealName = nil
	if PurchasingInfo ~= nil then
		local keys = PurchasingAgent_Common:GetKeys()
		for i,v in pairs( keys ) do
			local ItemName = PurchasingAgent_Common(v,"ItemName")
			if ItemName == PurchasingInfo.ItemName then
				self.m_PurchasingInfo.ItemBigID = PurchasingAgent_Common(v,"BigID")
				break
			end
		end
		if self.m_PurchasingInfo.ItemBigID == nil then
			LogErr("出现PurchasingAgent_Common表不存在的的物品求购订单", PurchasingInfo.ItemName) 
		end
	end
end

function CPurchasingAgentSellWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
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

function CPurchasingAgentSellWnd:OnShowPurchasingAgentSellInfo(Grid)
	if Grid == nil then
		return false
	end
	local BigId, Index = Grid:GetType()
	local OpenedFlag = false
	local ItemIDTbl = Grid:GetItemID(Grid:Size())
	if #(ItemIDTbl) > 0 then
		local dynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemIDTbl[1])
		OpenedFlag = dynInfo.OpenedFlag
	end
	
	if Index == self.m_PurchasingInfo.ItemName and (not OpenedFlag) then
		local ItemCount = Grid:Size()
		local SellCount = (self.m_PurchasingInfo.Count > ItemCount) and ItemCount or self.m_PurchasingInfo.Count
		local Money = self.m_PurchasingInfo.SingleMoney * SellCount
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(BigId,Index,"SmallIcon")
		g_DelWndImage(self.m_ItemBtn, 1, IP_ENABLE, IP_CLICKDOWN)
		g_LoadIconFromRes(SmallIcon, self.m_ItemBtn, -1, IP_ENABLE, IP_CLICKDOWN)
		self.m_ItemBtn:SetWndText(SellCount)
		self.m_SingleMoneyText:SetWndText(g_MoneyMgr:ChangeMoneyToString(self.m_PurchasingInfo.SingleMoney,EGoldType.GoldCoin))
		self.m_TotallMoneyText:SetWndText(g_MoneyMgr:ChangeMoneyToString(Money,EGoldType.GoldCoin))
		return true
	else
		local DisPlayName = g_ItemInfoMgr:GetItemLanInfo(self.m_PurchasingInfo.ItemBigID,self.m_PurchasingInfo.ItemName,"DisplayName")
		MsgClient(8304, DisPlayName)
		g_DelWndImage(self.m_ItemBtn, 1, IP_ENABLE, IP_CLICKDOWN)
		self.m_SingleMoneyText:SetWndText("")
		self.m_TotallMoneyText:SetWndText("")
		self.m_ItemBtn:SetWndText("")
		return false
	end
end

function CPurchasingAgentSellWnd:OnClickItemBtn()
	if not self.m_PurchasingInfo then
		return
	end
	local state, context = g_WndMouse:GetCursorState()
	self:PurchasingAgentSell(context)
	g_WndMouse:ClearCursorAll()
end

function CPurchasingAgentSellWnd:PurchasingAgentSell(context)
	if not self.m_PurchasingInfo or not context then
		return
	end
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )	
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local bRet = self:OnShowPurchasingAgentSellInfo(grid)
	self.m_RoomIndex = fromRoom
	self.m_srcPos = srcPos
	self:ClearLashSellInfo()
	if bRet then
		local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
		table.insert(self.m_SelectGridTable, {x, y, fromRoom})
		SrcCt:SetClickedWndState(x, y, fromRoom, false)
	end
end

function CPurchasingAgentSellWnd:ClearLashSellInfo()
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

function CPurchasingAgentSellWnd:OnSellBtn()
	if self.m_PurchasingInfo and self.m_RoomIndex and self.m_srcPos then
		Gac2Gas:CSMSellGoods2Order(g_Conn,self.m_PurchasingInfo.OrderId,self.m_RoomIndex,self.m_srcPos)
	end
end

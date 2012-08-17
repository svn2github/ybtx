gac_gas_require "relation/tong/TongProdItemMgr"
gac_gas_require "relation/tong/TongMantleMgr"

CTongProductionCenterWnd		= class(SQRDialog, CTongProductionBase)	--导出类
CTongProductionCenterListItem	= class(SQRDialog, CTongProductionListItemBase)

function CTongProductionCenterWnd:Ctor()
	self:InitInfo()
	self:CreateFromRes("TongBuilldingItemCreate", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongProductionCenterWnd:InitInfo()
	self.m_TongProdItemInfo	= CTongProdItemInfoMgr:new()
	self.m_TongMantleInfo	= CTongMantleInfoMgr:new()
	
	self.m_tblTruckItemInfo	= self.m_TongProdItemInfo:GetTableByType(2)
	self.m_tblBombItemInfo	= self.m_TongProdItemInfo:GetTableByType(3)
	self.m_tblMantleInfo	= self.m_TongMantleInfo:GetTableOrderByLevel()
end

function CTongProductionCenterWnd:OnGetCreatingItemInfo()
	Gac2Gas:GetCreatingProdItemInfo(g_Conn)
end

function CTongProductionCenterWnd:OnCancelCreateItem(Child)
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160040)
		return 
	end
	Gac2Gas:CancelCreateProdItem(g_Conn, Child.itemId)
end

function CTongProductionCenterWnd:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
	if(bFlag) then
		Gac2Gas:GetTongRelativeLine(g_Conn)
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
	end
end

function CTongProductionCenterWnd:GetTongProdItemInfo(creatingItemName)
	local itemType		= 10
	local NeedTime	= nil
	local tblMantleInfo = self.m_TongMantleInfo:GetTable()
	if tblMantleInfo(creatingItemName) then
		itemType = 6
		NeedTime = tblMantleInfo(creatingItemName,"NeedTime")
	end
	local wholeTime	= g_ItemInfoMgr:GetItemInfo(itemType, creatingItemName,"NeedTime") or NeedTime
	return itemType, wholeTime
end

function CTongProductionCenterWnd:SetWaitingIcon(tblWaitingItem, i)
	local itemType = self:GetTongProdItemInfo(tblWaitingItem[2])
	g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo(itemType, tblWaitingItem[2],"SmallIcon"), self.m_tblWaitingIcon[i], -1, IP_ENABLE, IP_ENABLE)
	g_SetItemRichToolTips(self.m_tblWaitingIconBtn[i], itemType, tblWaitingItem[2], 0)
end

---------------------------------------------------------------
function CTongProductionCenterWnd:DrawItemCreateList(relativeLine)
	self.m_ItemList:DeleteAllItem()
	self.m_ItemList:InsertColumn(0, self.m_ItemList:GetWndOrgWidth())
	local tblMantleInfo = {}
	for i = 1, #(self.m_tblMantleInfo) do
		if(self.m_tblMantleInfo[i]("MantleLevel") <= relativeLine + 1) then
			table.insert(tblMantleInfo, self.m_tblMantleInfo[i])
		end
	end
	local tbl = {self.m_tblTruckItemInfo, self.m_tblBombItemInfo, tblMantleInfo}
	self.m_tblCreateItem = {}
	local index = 0
	for i = 1, #tbl do
		for j = 1, #(tbl[i]) do
			self.m_ItemList:InsertItem(index, 50)
			local item = self.m_ItemList:GetSubItem(index, 0)
			local itemWnd = self:CreateListItemWnd(item)
			itemWnd:SetStoreRoom(5)
			table.insert(self.m_tblCreateItem, itemWnd)
			itemWnd:SetInfo(tbl[i][j], (i == 3) and 6 or 10)
			index = index + 1
		end
	end
end

------创建建筑列表的行窗体
function CTongProductionCenterWnd:CreateListItemWnd(parent)
	local wnd = CTongProductionCenterListItem:new()
	wnd:Init(parent)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongProductionCenterListItem:OnCreate()
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160040)
		return 
	end
	Gac2Gas:CreateTongProdItem(g_Conn, self.m_tblInfo("Name"), self.m_itemType, self.m_StoreRoom)
end

function CTongProductionCenterListItem:GetSmallIcon(itemType, info)
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, info("Name"),"SmallIcon")
	return SmallIcon
end
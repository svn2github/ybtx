gac_gas_require "relation/tong/TongProdItemMgr"

CTongBuildingItemCreateWnd			= class(SQRDialog, CTongProductionBase)	--导出类
CTongTongBuildingItemCreateListItem	= class(SQRDialog, CTongProductionListItemBase)

function CTongBuildingItemCreateWnd:Ctor()
	self:InitInfo()
	self:CreateFromRes("TongBuilldingCreate", g_GameMain)
	self:DrawItemCreateList(1,false)
	self:OnLabelGetCheck(self.labelTbl[1])
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongBuildingItemCreateWnd:InitInfo()
	self.m_BuildingCommon   = CTongProdItemInfoMgr:new()
	self.m_CreateItemCommon = self.m_BuildingCommon:GetCanBeCreateBuilding()
	Gac2Gas:GetTongLevelToFilterAvailableBuildingItems(g_Conn)
end

function CTongBuildingItemCreateWnd:OnGetCreatingItemInfo()
	Gac2Gas:GetCreatingBuildingItemInfo(g_Conn)
end

function CTongBuildingItemCreateWnd:OnCancelCreateItem(Child)
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160039)
		return 
	end
	Gac2Gas:CancelCreateBuildingItem(g_Conn, Child.itemId)
end

function CTongBuildingItemCreateWnd:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
	if(bFlag) then
		self:OnGetCreatingItemInfo()
		Gac2Gas:GetTongLevelToFilterAvailableBuildingItems(g_Conn)
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
	end
	
	for k, v in ipairs(self.m_tblBuildingItem) do
		v:SetStoreRoom(5)
	end
end

function CTongBuildingItemCreateWnd:GetTongProdItemInfo(creatingItemName)
	local itemType = 10
	local NeedTime = g_ItemInfoMgr:GetItemInfo(itemType, creatingItemName,"NeedTime")
	return itemType,NeedTime
end

function CTongBuildingItemCreateWnd:SetWaitingIcon(tblWaitingItem, i)
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(10, tblWaitingItem[2],"SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_tblWaitingIcon[i], -1, IP_ENABLE, IP_ENABLE)
	g_SetItemRichToolTips(self.m_tblWaitingIconBtn[i], 10, tblWaitingItem[2], 0)
end

---------------------------------------------------------------
--根据不同的标签显示相应类的建筑项目
function CTongBuildingItemCreateWnd:DrawItemCreateList(index,showAvailableItems)
	self.m_ItemList:DeleteAllItem()
	self.m_tblBuildingItem = {}
	self.m_ItemList:InsertColumn(0, self.m_ItemList:GetWndOrgWidth())
	local tbl = showAvailableItems and self.m_AvailableCreateItemCommon[index] or self.m_CreateItemCommon[index]
	if not tbl then return end	
	for i = 1, #tbl do
		self.m_ItemList:InsertItem(i-1, 50)
		local item = self.m_ItemList:GetSubItem(i-1, 0)
		local itemWnd = self:CreateListItemWnd(item)
		table.insert(self.m_tblBuildingItem, itemWnd)
		itemWnd:SetInfo(tbl[i], 10)
	end
end

--筛选当前小队等级所能建造的项目
function CTongBuildingItemCreateWnd:FilterAvailableBuildingItems(tongLevel)
	local tblBuildingItem = self.m_CreateItemCommon
	local tblAvailableBuildingItem = {}
	for i = 1, #tblBuildingItem do
		tblAvailableBuildingItem[i] = {}
		for k,v in ipairs(tblBuildingItem[i]) do
			if v("NeedTongLevel") <= tongLevel then
				table.insert(tblAvailableBuildingItem[i],v)
			end
		end
	end
	self.m_AvailableCreateItemCommon = tblAvailableBuildingItem
end

------创建建筑列表的行窗体
function CTongBuildingItemCreateWnd:CreateListItemWnd(parent)
	local wnd = CTongTongBuildingItemCreateListItem:new()
	wnd:Init(parent)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongTongBuildingItemCreateListItem:OnCreate()
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160039)
		return 
	end
	Gac2Gas:CreateBuildingItem(g_Conn, self.m_tblInfo("Name"), self.m_StoreRoom)
end

function CTongTongBuildingItemCreateListItem:GetSmallIcon(itemType, info)
	return info("SmallIcon")
end
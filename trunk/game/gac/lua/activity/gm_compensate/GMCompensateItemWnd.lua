lan_load "gm/Lan_GMActivity_Common"

CGMCompensateWnd = class(SQRDialog) 
CGMCompensateItemWnd = class(SQRDialog)

function CGMCompensateWnd:Ctor()
	self:CreateFromRes("GMcmpenateItemWnd",g_GameMain)
	self.m_CompensateItemWnd = self:GetDlgChild("ItemList")
	self.m_CloseBtn = self:GetDlgChild("Close")
	self.m_GetItemBtn = self:GetDlgChild("BtnGetItem")
	self.m_ActivityDis = self:GetDlgChild("ActivityDis")
	self.m_ItemWndList = {}
	self.m_ItemBtnWndList = {}
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end


function CGMCompensateWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_CloseBtn)then
			self:ShowWnd(false)
		elseif(Child == self.m_GetItemBtn)then
			local itemIndex = self.m_CompensateItemWnd:GetSelectItem(-1)
			if(-1 == itemIndex) then return end
			local subItemIndex	= self.m_CompensateItemWnd:GetSelectSubItem(itemIndex, -1)
			local nowItemWnd = self.m_ItemBtnWndList[itemIndex*2 + subItemIndex + 1]
			if nowItemWnd then
				local DisplayName = nowItemWnd.DisplayName
				local left_num = nowItemWnd.left_num
				if nowItemWnd.ActivityID == 1000000000 and tonumber(DisplayName) then
					Gac2Gas:TakeActivationCodeCompensate(g_Conn,tonumber(DisplayName))
				else
					Gac2Gas:TakeCompensateItem(g_Conn, nowItemWnd.ActivityID, DisplayName, left_num)
				end
			end
		end
	end
end

function CGMCompensateWnd:OpenWnd(ActivityID)
	self:SetItemInfo(ActivityID)
	self.m_ActivityDis:SetWndText(Lan_GMActivity_Common(ActivityID,"DisplayName"))
end

function CGMCompensateWnd:SetItemInfo(ActivityID)
	local tblItemInfo = g_GameMain.m_GmActivity.m_tblCompenateItemInfo[ActivityID]
	g_GameMain.m_GmActivity:ShowWnd(false)
	if not tblItemInfo or #tblItemInfo == 0 then
		MessageBox( g_GameMain, MsgBoxMsg(9500), MB_BtnOK)
		self:ShowWnd(false)
		return
	end
		
	self.m_CompensateItemWnd:DeleteAllItem()
	local column = 2
	local row = math.ceil(#tblItemInfo/column)
	local itemWidth = self.m_CompensateItemWnd:GetWorkAreaOrgWidth()
	
	for i=1, column do
		self.m_CompensateItemWnd:InsertColumn( i-1, itemWidth/2)
	end
	local ItemIndex = 1
	self.m_ItemWndList = {}
	self.m_ItemBtnWndList = {}
	for i=1, row do
		self.m_CompensateItemWnd:InsertItem(i-1, 52)
		for j=1, column do
			if ItemIndex <=  #tblItemInfo then
				local itemWnd = CGMCompensateItemWnd:new()
				local item	  = self.m_CompensateItemWnd:GetSubItem(i-1, j-1)
				itemWnd:CreateFromRes("GMcompenateItem", item)
				itemWnd:ShowWnd( true )
				local iconWnd = itemWnd:GetDlgChild("ItemBtn")
				local disWnd  = itemWnd:GetDlgChild("ItemDis")
				local itemType = tblItemInfo[ItemIndex][1]
				local itemName = tblItemInfo[ItemIndex][2]
				local left_num = tblItemInfo[ItemIndex][4]
				if itemType == 0 then
					g_LoadIconFromRes(2413, iconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
					local money = tonumber(string.sub(itemName,string.find(itemName,":")+1,string.len(itemName)))
					disWnd:SetWndText(GetStaticTextClient(1701,g_MoneyMgr:ChangeMoneyToString(money*100,EGoldType.GoldCoin)))
				else
					local ItemInfo = g_ItemInfoMgr:GetItemFunInfo( itemType, itemName)
					local DisplayName = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName, "DisplayName" )
					g_LoadIconFromRes(ItemInfo( "SmallIcon" ), iconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
					disWnd:SetWndText(GetStaticTextClient(1700,DisplayName,left_num))  
					local BindingStyle = tblItemInfo[ItemIndex][7] 
					g_SetWndMultiToolTips(item, itemType, itemName, 0,nil,nil,nil,BindingStyle)
				end
				iconWnd.DisplayName = tblItemInfo[ItemIndex][3]
				iconWnd.ActivityID = tblItemInfo[ItemIndex][8]
				iconWnd.left_num = left_num
				table.insert(self.m_ItemBtnWndList, iconWnd)
				table.insert(self.m_ItemWndList, itemWnd)
				ItemIndex = ItemIndex + 1
			else
				local itemWnd = SQRDialog:new()
				local item	  = self.m_CompensateItemWnd:GetSubItem(i-1, j-1)
				item:EnableWnd(false)
			end
		end
	end
	self:ShowWnd(true)
end
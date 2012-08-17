gac_require "item/wnd_item_bag/LcStoreRoomPublic"

CTongDepotWnd = class(SQRDialog)

function CTongDepotWnd:Ctor()
	self.m_Page = 1
	self:CreateFromRes("TongDepot", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongDepotWnd:OnChildCreated()
	self.m_TongItemRoom	= ClassCast(LcStoreRoomPublic, self:GetDlgChild("ItemList"))
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_tblPageBtn = {}
	for i = 1, 5 do
		self.m_tblPageBtn[i] = self:GetDlgChild("PageCB" .. i)
	end
	self.m_tblPageBtn[1]:SetCheck(true)
end

--消息
function CTongDepotWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(self:IsShow()) then
		if(uMsgID == BUTTON_LCLICK) then
			if g_MainPlayer.m_ItemBagLock and Child ~= self.m_XBtn then
				MsgClient(160033)
				return 
			end
			if(Child == self.m_XBtn) then
				self:ShowWnd(false)
				--Gac2Gas:SortCollectiveBag(g_Conn, type, page) --临时放在这里以后也许会用
			else
				for i = 1, #self.m_tblPageBtn do
					if(Child == self.m_tblPageBtn[i]) then
						self.m_Page = i
						self:GetTongDepotOnePageItemList(i)
						break
					end
				end
			end
		elseif(uMsgID == ITEM_LBUTTONUP or uMsgID == ITEM_LBUTTONCLICK) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160033)
				return 
			end
			if(Child == self.m_TongItemRoom) then
				self.m_TongItemRoom:DoByClickRoom(g_StoreRoomIndex.TongDepot, uParam1, uParam2, self)--g_StoreRoomIndex.TongDepot用于查找删除这个常量
			end
		elseif(uMsgID == ITEM_RBUTTONCLICK) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160033)
				return 
			end
			if(Child == self.m_TongItemRoom) then
				self.m_TongItemRoom:DoByRClick(g_StoreRoomIndex.TongDepot, uParam1, uParam2, self)--g_StoreRoomIndex.TongDepot用于查找删除这个常量
			end
		elseif(uMsgID == BUTTON_DRAG) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160033)
				return 
			end
			if(Child == self.m_TongItemRoom) then
				self.m_TongItemRoom:DoByDragRoom(g_StoreRoomIndex.TongDepot, uParam1, uParam2, self)
			end
		end
	end
end

------绘制格子及物品------
function CTongDepotWnd:Draw()
	self.m_TongItemRoom:SetSize(g_StoreRoomIndex.TongDepot, g_TongDepotMain.size, g_TongDepotMain.columns)
	g_GameMain.m_BagSpaceMgr:SetSpaceRelateLc(g_StoreRoomIndex.TongDepot, self.m_TongItemRoom)
end

function CTongDepotWnd:OpenPanel()
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
	self:GetTongDepotOnePageItemList(self.m_Page)
end

------向服务器索取帮会物品列表------
function CTongDepotWnd:GetTongDepotOnePageItemList(page)
	Gac2Gas:GetCollectiveBagOnePageItemList(g_Conn, g_StoreRoomIndex.TongDepot, page)
end
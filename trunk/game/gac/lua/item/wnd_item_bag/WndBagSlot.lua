CWndBagSlots = class(SQRDialog)

function CWndBagSlots:Ctor(parent)
	self:CreateFromRes("WndMasterListSubGrid", parent)
	self:ShowWnd(true)
end

--包裹栏窗口
function CWndBagSlots:OnChildCreated()
	self.m_rcLastBegin = CFRect:new()
	self:GetWndRect(self.m_rcLastBegin)
	
	self.m_lcAddBagSlots = ClassCast(LcStoreRoomAppend, self:GetDlgChild("WndMasterListSubGrid"))
	self.m_lcAddBagSlots.m_BagAddBackpacks={}
	self.m_lcAddBagSlots.m_Bags ={}
	self.m_lcAddBagSlots.m_WndParent = self
	self.m_lcAddBagSlots:SetSize(g_AddRoomIndexClient.PlayerExpandBag, g_MainBagExpand.size, g_MainBagExpand.columns, g_SlotIndex.FirstBagSlot)
	
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	SpaceMgr:SetSpaceRelateLc(g_AddRoomIndexClient.PlayerExpandBag, self.m_lcAddBagSlots)
end

--包裹栏窗口事件响应
function CWndBagSlots:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if( uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_LBUTTONUP ) then
		if( Child == self.m_lcAddBagSlots ) then
			self.m_lcAddBagSlots:ClickAddBackpack(uParam1,uParam2)
		end
	elseif( uMsgID == BUTTON_DRAG ) then
		if( Child == self.m_lcAddBagSlots ) then
			self.m_lcAddBagSlots:DoByDragRoom(uParam1,uParam2)
		end
	end
end

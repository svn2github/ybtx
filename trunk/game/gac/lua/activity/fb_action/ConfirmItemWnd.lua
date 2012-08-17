CConfirmWnd = class(SQRDialog)
CConfirmItem = class(SQRDialog)

function CConfirmWnd:Ctor(parent)
	self:CreateFromRes("MatchGameItemWnd",parent) 
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_ItemInfo = self:GetDlgChild("ItemList")
	self.m_Text = self:GetDlgChild("Text")
	--g_ExcludeWndMgr:InitExcludeWnd(self, 2)
end


function CConfirmWnd:GetWnd(actionName, activityName)
	if not g_GameMain.m_ConfirmWnd then
		g_GameMain.m_ConfirmWnd = CConfirmWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_ConfirmWnd
	wnd.m_ActionName = actionName
	wnd.m_activityName = activityName
	wnd.m_CountDownTime = 60
	local lanName = g_GetFbActionNameLanStr(actionName)
	wnd.m_Text:SetWndText(GetStaticTextClient(340001, lanName))
	wnd.m_OkBtn:SetWndText(GetStaticTextClient(400001, wnd.m_CountDownTime))
	wnd:OpenPanel()
	local len = table.getn(g_GameMain.m_Createtbl)
	if len <= 0 then
		wnd:ShowWnd(false)
		return 
	end
	if wnd.m_ShowTimeTick then
		UnRegisterTick(wnd.m_ShowTimeTick)
		wnd.m_ShowTimeTick = nil
	end
	wnd.m_ShowTimeTick = RegClassTick("MatchGameItemTick", wnd.ShowTimeTick, 1000, wnd)
	wnd:ShowWnd(true)
	return wnd
end

function CConfirmWnd.CreateTbl()
	g_GameMain.m_Createtbl = {}
end

function CConfirmWnd.InsertTbl(activityName,ActionName, ItemName, nHaveCount)
	table.insert(g_GameMain.m_Createtbl, {activityName,ActionName, ItemName, nHaveCount})
end

function CConfirmWnd:OpenPanel()
	self:DrawMemberList()
end


function CConfirmWnd:DrawMemberList()
	self.m_ItemInfo:DeleteAllItem()
	local tblLen = # g_GameMain.m_Createtbl
	local listWidth = self.m_ItemInfo:GetWndOrgWidth()
	self.m_ItemInfo:InsertColumn(0, listWidth)
	for i=1, tblLen  do
		self.m_ItemInfo:InsertItem(i - 1, 50)
		local itemWnd = self.m_ItemInfo:GetSubItem(i - 1, 0)
		local itemName = g_GameMain.m_Createtbl[i][3]
		local item = self:CreateItemListWnd(itemWnd, itemName)
		item:AddInfo(itemName)
		itemWnd.m_Wnd = item
		self:ShowWnd(true)
	end
end


function CConfirmWnd:ShowTimeTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime < 0 then
		self:Destroy()
		return
	end
	self.m_OkBtn:SetWndText(GetStaticTextClient(400001, self.m_CountDownTime))
	self:ShowWnd(true)
end

function CConfirmWnd:CreateItemListWnd(parent, itemName)
	local wnd = CConfirmItem:new()
	wnd:CreateFromRes("ItemWnd", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CConfirmItem:OnChildCreated()
	self.m_ItemIcon = self:GetDlgChild("ItemIcon")
	self.m_DisplayName = self:GetDlgChild("DisplayName")
end

function CConfirmItem:AddInfo(itemName)
	local itemType = 46
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, itemName, "SmallIcon")
	local nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag, itemType, itemName)
	self.m_ItemIcon:SetWndText(nItemCount)
	g_LoadIconFromRes(SmallIcon, self.m_ItemIcon, -1, IP_ENABLE, IP_CLICKDOWN)
	g_SetWndMultiToolTips(self.m_ItemIcon,itemType,itemName, 0 ,1) 
	local itemLanInfo = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayName")  
	self.m_DisplayName:SetWndText(itemLanInfo)
end

function CConfirmWnd:Destroy()
	if self.m_ShowTimeTick then
		UnRegisterTick(self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self:ShowWnd(false)
end
 

function CConfirmWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CancelBtn) then
			self:Destroy()
		elseif (Child == self.m_OkBtn) then
			local index = self.m_ItemInfo:GetSelectItem(-1) + 1
			if index == 0 then 
				return 
			end
			if not g_MainPlayer:CppIsAlive() then
				MsgClient(844, g_GameMain.m_Createtbl[index][3])
				return
			end
			if g_MainPlayer:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_InDoingSkill) then
				MsgClient(838)
				return
			end
			Gac2Gas:UseFbItem(g_Conn, g_GameMain.m_Createtbl[index][1], g_GameMain.m_Createtbl[index][2], g_GameMain.m_Createtbl[index][3])
			self:Destroy()
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		
	end
end

function CConfirmWnd.UnRegisterUseItemTick()
	if g_GameMain.m_ConfirmWnd then
		if g_GameMain.m_ConfirmWnd.m_ShowTimeTick then
			UnRegisterTick(g_GameMain.m_ConfirmWnd.m_ShowTimeTick)
			g_GameMain.m_ConfirmWnd.m_ShowTimeTick = nil
		end
	end
end

gac_require "relation/association/AssociationGroupListInc"

local function SetPos(wnd, x, y)
	local Rect = CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x+uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y+uHeight
	end
	wnd:SetLogicRect(Rect)
end

--***************************************************************************************
--好友列表窗体
--***************************************************************************************
function CreateAssociationGroupListWnd(parent)
	local wnd = CAssociationGroupListWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationGroupListWnd:Init(parent)
	self.m_tblItemWnd = {} --itemWnd表
	self:CreateFromRes("AssociationFriendList", parent)
	self:ShowWnd(true)
end

function CAssociationGroupListWnd:OnChildCreated()
	self.m_List = self:GetDlgChild("List")
end

--消息
function CAssociationGroupListWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == LISTCTRL_RIGHTCLICK) then
		if(Child == self.m_List) then
			self:ShowMenu(uParam1, uParam2)
		end
	elseif(uMsgID == ITEM_RBUTTONCLICK) then
		if(Child == self.m_List) then
			local tblFriendGroup	= g_GameMain.m_AssociationBase.m_tblFriendGroup
			local groupId			= tblFriendGroup[uParam1+1][1][2]
			local groupName			= tblFriendGroup[uParam1+1][1][1]
			self:ItemShowMenu(groupId, groupName)
		end
	elseif(uMsgID == ITEM_LBUTTONDBLCLICK) then
		if(Child == self.m_List) then
			local tblFriendGroup	= g_GameMain.m_AssociationBase.m_tblFriendGroup
			local groupId			= tblFriendGroup[uParam1+1][1][2]
			local groupName			= tblFriendGroup[uParam1+1][1][1]
			local wnd = g_GameMain.m_AssociationWnd:CreateAssociationPubChatWnd(groupId, groupName) --双击群聊
		end
	end
end

--设置好友群列表
function CAssociationGroupListWnd:SetFriendGroupList()
	local base = g_GameMain.m_AssociationBase
	local tbl = base.m_tblFriendGroup --好友群表
	self.m_tblItemWnd = {}
	self.m_List:DeleteAllItem()
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	for i = 0, #tbl-1 do
		self.m_List:InsertItem(i, 20)
		local item = self.m_List:GetSubItem(i, 0)
		local itemWnd = self:CreateAssoFriendGroupListItem(item, i)
		table.insert(self.m_tblItemWnd, itemWnd)
	end
end

--准备查找好友群
function CAssociationGroupListWnd:PreFindGroup()
	local wnd = g_GameMain.m_AssociationFindWnd
	wnd.m_FindGroupRadio:SetCheck(true)
	wnd:ChangeTab()
	wnd:Back()
	wnd:ShowWnd(true)
end

--准备创建好友群(显示创建群窗体)
function CAssociationGroupListWnd:CreateFriendGroup()
	CreateAssociationFriendGroupCreateWnd(g_GameMain)
end

--群聊
function CAssociationGroupListWnd:ShowPubChatWnd(context)
	local groupId, groupName = unpack(context)
	g_GameMain.m_AssociationWnd:CreateAssociationPubChatWnd(groupId, groupName)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

--离开群
function CAssociationGroupListWnd:LeaveGroup(groupId)
	Gac2Gas:LeaveGroup(g_Conn, groupId)
end

--屏蔽群消息
function CAssociationGroupListWnd:RefuseGroupMsgOrNot(groupId)
	local wnd = g_GameMain.m_AssociationBase
	local uRefuseOrNot = wnd.m_RefuseGroupMsgGroupId[groupId]	
	if uRefuseOrNot == 0 then
		uRefuseOrNot = 1
	else
		uRefuseOrNot = 0
	end
	Gac2Gas:RefuseGroupMsgOrNot(g_Conn,groupId,uRefuseOrNot)
end

--解散群
function CAssociationGroupListWnd:DisbandGroup(groupId)
	Gac2Gas:DisbandGroup(g_Conn, groupId)
end

--显示解散群的确定框
function CAssociationGroupListWnd:ShowDisbandGroupMsgBox(context)
	local groupId, groupName = unpack(context)
	local string = MsgBoxMsg(15005, groupName)
	MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), AssoDisbandGroupMBoxClick, groupId)
end

--解散群对话框的回调
function AssoDisbandGroupMBoxClick(groupId, index)
	if(1 == index) then
		g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[2]:DisbandGroup(groupId)
	end
	return true
end

--右键菜单----------------------------------------------
function CAssociationGroupListWnd:OnMenuMsg(func)
	if(func) then func(self) end
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

function CAssociationGroupListWnd:ShowMenu(uParam1, uParam2)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	
	local Menu	= CMenu:new("TargetMenu", g_GameMain)
	g_GameMain.m_Menu	= Menu
	--创建群
	Menu:InsertItem(GacMenuText(403), self.OnMenuMsg, nil, false, false, nil, self, self.CreateFriendGroup)
	--查找群
	Menu:InsertItem(GacMenuText(404), self.OnMenuMsg, nil, false, false, nil, self, self.PreFindGroup)
	SetPos(Menu, uParam1, uParam2)
end

function CAssociationGroupListWnd:ItemShowMenu(groupId, groupName)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local tblGroupIndex		= g_GameMain.m_AssociationBase.m_tblFriendGroupIndex
	local groupIndex		= tblGroupIndex[groupId]
	local tblFriendGroup	= g_GameMain.m_AssociationBase.m_tblFriendGroup
	local ctrlType			= tblFriendGroup[groupIndex][1][6]
	local Menu	= CMenu:new("TargetMenu", g_GameMain)
	g_GameMain.m_Menu	= Menu
	--发送群组消息
	Menu:InsertItem(GacMenuText(501), self.ShowPubChatWnd, nil, false, false, nil, self, {groupId, groupName})
	--退出该群
	Menu:InsertItem(GacMenuText(502), self.LeaveGroup, nil, false, false, nil, self, groupId)
	if g_GameMain.m_AssociationBase.m_RefuseGroupMsgGroupId[groupId] == 1 then
		--开启群消息
		Menu:InsertItem(GacMenuText(505), self.RefuseGroupMsgOrNot, nil, false, false, nil, self, groupId)
	else
		--屏蔽群消息
		Menu:InsertItem(GacMenuText(504), self.RefuseGroupMsgOrNot, nil, false, false, nil, self, groupId)
	end
	if(1 == ctrlType) then
		--解散该群
		Menu:InsertItem(GacMenuText(503), self.ShowDisbandGroupMsgBox, nil, false, false, nil, self, {groupId, groupName})
	end
	--创建群
	Menu:InsertItem(GacMenuText(403), self.OnMenuMsg, nil, false, false, nil, self, self.CreateFriendGroup)
	--查找群
	Menu:InsertItem(GacMenuText(404), self.OnMenuMsg, nil, false, false, nil, self, self.PreFindGroup)
	local posCursor = CFPos:new()
	g_App:GetRootWnd():GetLogicCursorPos(posCursor)
	SetPos(Menu, posCursor.x, posCursor.y)

end

------无敌分割线----------------------------------------------------------------------------------------------------
function CAssociationGroupListWnd:CreateAssoFriendGroupListItem(parent, index) --index是从0开始的
	local wnd = CAssociationGroupListItem:new()
	wnd:Init(parent, index)
	return wnd
end

function CAssociationGroupListItem:Init(parent, index)
	self.m_itemIndex = index --自己在list中的序列
	self:CreateFromRes("AssociationFriendGroupListItem", parent)
	self:ShowWnd( true )
end

function CAssociationGroupListItem:OnChildCreated()
	local base	= g_GameMain.m_AssociationBase
	local tbl	= base.m_tblFriendGroup --好友群表
	self.m_Icon			= self:GetDlgChild("Icon")
	self.m_GroupName	= self:GetDlgChild("GroupName")
	self.m_GroupName:SetWndText(tbl[self.m_itemIndex+1][1][1])
end
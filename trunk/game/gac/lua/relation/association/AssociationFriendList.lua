gac_require "relation/association/AssociationFriendListInc"
gac_require "relation/association/AssociationChat"
gac_require "relation/association/AssociationFriendGroup"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gac_require "framework/texture_mgr/DynImageMgr"

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
function CreateAssociationFriendListWnd(parent)
	local wnd = CAssociationFriendListWnd:new()
	wnd:CreateFromRes("AssociationFriendList", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CAssociationFriendListWnd:OnChildCreated()
	self.m_List = self:GetDlgChild("List")
	self.m_addClassTitleEdit = self:CreateAddClassEdit(self)
end

--消息
function CAssociationFriendListWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == LISTCTRL_RIGHTCLICK) then
		if(Child == self.m_List) then
			self:ShowMenu(uParam1, uParam2)
		end
	end
end

--设置列表listctrl控件
function CAssociationFriendListWnd:SetFriendList()
	local base	= g_GameMain.m_AssociationBase
	local tbl	= base.m_tblFriendListClass
	self.m_List:DeleteAllItem()
	self.m_tblItemWnd = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	for i = 0, #tbl do
		self.m_List:InsertItem(i, 20)
		local item = self.m_List:GetSubItem(i, 0)
		local itemWnd = self:CreateAssociationFriendListItem(item, i, self)
		self.m_tblItemWnd[i+1] = itemWnd
	end
end

--关闭所有组的列表
function CAssociationFriendListWnd:CloseAllClass()
	for i = 1, #(self.m_tblItemWnd) do
		self.m_tblItemWnd[i]:ShowChildList(false)
	end
end

function CAssociationFriendListWnd:ListShift(bFlag)
	if( self.m_addClassTitleEdit:IsShow() == bFlag ) then
		return
	end
	local rect = CFRect:new()
	self.m_List:GetLogicRect(rect)
	rect.top = rect.top + (bFlag and 26 or -26)
	self.m_List:SetLogicRect(rect)
	self.m_addClassTitleEdit:ShowWnd(bFlag)
	
	if(bFlag) then
		self.m_addClassTitleEdit.m_TitleEdit:SetWndText("")
		self.m_addClassTitleEdit.m_TitleEdit:SetFocus()
	end
end

--------------------------------------------------------------------
--准备添加新组别
function CAssociationFriendListWnd:PreAddClass()
	local base = g_GameMain.m_AssociationBase
	local tbl = base.m_tblFriendListClass
	self:CloseAllClass()
	self:ListShift(true)
end

--准备查找好友
function CAssociationFriendListWnd:PreFindPlayer()
	local wnd = g_GameMain.m_AssociationFindWnd
	wnd.m_FindPlayerRadio:SetCheck(true)
	wnd:ChangeTab()
	wnd:Back()
	wnd:ShowWnd(true)
end

--准备查找好友群
function CAssociationFriendListWnd:PreFindGroup()
	local wnd = g_GameMain.m_AssociationFindWnd
	wnd.m_FindGroupRadio:SetCheck(true)
	wnd:ChangeTab()
	wnd:Back()
	wnd:ShowWnd(true)
end

--右键菜单----------------------------------------------
function CAssociationFriendListWnd:OnMenuMsg(func)
	if(func) then func(self) end
	self:DestroyMenu()
end

function CAssociationFriendListWnd:DestroyMenu()
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

function CAssociationFriendListWnd:ShowMenu(uParam1, uParam2)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	
	local Menu	= CMenu:new("TargetMenu", g_GameMain)
	g_GameMain.m_Menu	= Menu
	--添加好友分组
	Menu:InsertItem(GacMenuText(401), self.OnMenuMsg, nil, false, false, nil, self, self.PreAddClass)
	--添加好友
	Menu:InsertItem(GacMenuText(402), self.OnMenuMsg, nil, false, false, nil, self, self.PreFindPlayer)
	SetPos(Menu, uParam1, uParam2)
end
--------------------------------------------------------

--***************************************************************************************
--好友列表listctrl中的itemWnd
--***************************************************************************************
function CAssociationFriendListWnd:CreateAssociationFriendListItem(parent, index, fatherWnd)
	local wnd = CAssociationFriendListItem:new()
	wnd:Init(parent, index, fatherWnd) 
	return wnd
end

function CAssociationFriendListItem:Init(parent, index, fatherWnd)
	local base		= g_GameMain.m_AssociationBase
	local tbl		= base.m_tblFriendListClass
	
	self.m_isBlacklist		= (index == #tbl)
	if(self.m_isBlacklist) then
		self.m_tblClassList = base.m_tblBlackList
	else
		self.m_tblClassList = tbl[index+1] --本组的玩家列表
	end
	
	self.m_fatherWnd = fatherWnd
	self.m_itemIndex = index --自己在list中的序列
	self:CreateFromRes("AssociationListItem", parent)
	self:ShowWnd( true )
	self:ResetItem()
end

function CAssociationFriendListItem:OnChildCreated()
	self.m_TitleCheck			= self:GetDlgChild("TitleCheck")
	self.m_TitleEdit			= self:GetDlgChild("TitleEdit")
	self.m_ChildList			= self:GetDlgChild("ChildList")
	self.m_TitleCheck.m_Icon	= self:CreateClassTitleIcon(self.m_TitleCheck)
	
	self.m_TitleCheck:SetCheck(false)
	self.m_TitleCheck.m_Icon.m_IconCB:SetCheck(false)
	
	self.m_TitleEdit:ShowWnd(false)
	self.m_TitleCheck:SetTextOffsetX(25)
	self.m_TitleEdit:SetMaxTextLenght(8)
end

function CAssociationFriendListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_TitleCheck) then
			local bFlag = self.m_ChildList:IsShow()
			self:ShowChildList(not bFlag)
		end
	elseif(uMsgID == EDIT_LOSEFOCUS or uMsgID == EDIT_RETURN) then
		if(Child == self.m_TitleEdit) then
			if(self.RenameClass) then
				self:RenameClass()
			end
		end
	elseif(uMsgID == BUTTON_RCLICK) then
		if(Child == self.m_TitleCheck) then
			self:ShowClassTitleMenu(uParam1, uParam2)
		end
	elseif(uMsgID == ITEM_RBUTTONCLICK) then
		if(Child == self.m_ChildList) then
			local posCursor = CFPos:new()
			g_App:GetRootWnd():GetLogicCursorPos(posCursor)
			self:ShowFriendMenu(posCursor.x, posCursor.y, uParam1)
		end
	elseif(uMsgID == ITEM_LBUTTONDBLCLICK) then
		if(Child == self.m_ChildList) then
			local chatWithId	= self.m_tblClassList[uParam1+1].playerId
			local chatWithName	= self.m_tblClassList[uParam1+1].playerName
			g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(chatWithId, chatWithName) --双击私聊
		end
	end
end

--开关子list
function CAssociationFriendListItem:ShowChildList(bFlag)
	local height = bFlag and #(self.m_tblChildItemWnd)*40 or 0
	g_GameMain.m_AssociationBase.m_tblOpenRecord[self.m_itemIndex] = bFlag
	self.m_TitleCheck:SetCheck(bFlag)
	self.m_TitleCheck.m_Icon.m_IconCB:SetCheck(bFlag)
	self.m_ChildList:ShowWnd(bFlag)
	self.m_fatherWnd.m_List:SetItemHeight(height+20, self.m_itemIndex)
end

--设定子list
function CAssociationFriendListItem:SetChildList()
	local tbl = self.m_tblClassList
	self.m_ChildList:DeleteAllItem()
	self.m_tblChildItemWnd = {}
	self.m_ChildList:InsertColumn(0, self.m_ChildList:GetWndOrgWidth())
	
	local bFriend		= not self.m_isBlacklist
	local bCheck		= g_GameMain.m_AssociationWnd.m_ShowOnlineCK:GetCheck()
	local nOnlineCount	= tbl.nOnlineCount or 0
	local num = (bFriend and bCheck) and nOnlineCount or #(tbl)
	
	for i = 0, num - 1 do
		self.m_ChildList:InsertItem(i, 40)
		local item		= self.m_ChildList:GetSubItem(i, 0)
		local itemWnd	= self:CreateAssociationListItemChild(item, self, i, self.m_isBlacklist)
		self.m_tblChildItemWnd[i+1] = itemWnd
	end
	
	local height = #(self.m_tblChildItemWnd)*40
	self:SetWndHeight(self.m_ChildList, height)
	self:SetWndHeight(self, height+20)
end

function CAssociationFriendListItem:SetWndHeight(wnd, height)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	Rect.bottom = Rect.top + height
	wnd:SetLogicRect(Rect)
end

--重设当前一级item
function CAssociationFriendListItem:ResetItem()
	self:ResetTitle()
	self:SetChildList()
	local tbl = g_GameMain.m_AssociationBase.m_tblOpenRecord
	self:ShowChildList(tbl[self.m_itemIndex] or false)
end

function CAssociationFriendListItem:ResetItemInfoById(playerId)
	local wnd = g_GameMain.m_AssociationWnd.m_AssociationListWnd
	for i, v in ipairs(self.m_tblChildItemWnd) do
		if( v.m_tblMemInfo.playerId == playerId ) then
			v:SetInfoText(self.m_isBlacklist)
			break
		end
	end
end

function CAssociationFriendListItem:ResetItemInfoOnline()
	local wnd = g_GameMain.m_AssociationWnd.m_AssociationListWnd
	for i, v in ipairs(self.m_tblChildItemWnd) do
		if( 1 == v.m_tblMemInfo.nOnline ) then
			v:SetInfoText(self.m_isBlacklist)
		end
	end
end

function CAssociationFriendListItem:ResetTitle()
	local bFriend		= not self.m_isBlacklist
	local nOnlineCount	= self.m_tblClassList.nOnlineCount or 0
	local sOnlineCount	= string.format( "(%d/%d)", nOnlineCount, #(self.m_tblClassList) )
	self.m_TitleCheck:SetWndText(self.m_tblClassList.className .. sOnlineCount)
end

--准备重命名好友组
function CAssociationFriendListItem:PreRenameClass()
	if(self.m_isBlacklist or 0 == self.m_itemIndex) then return end
	local oldName = self.m_TitleCheck:GetWndText()
	self.m_TitleCheck:SetWndText("")
	self.m_TitleEdit:ShowWnd(true)
	self.m_TitleEdit:SetFocus()
	
	self.RenameClass = function ()
		if(self.m_TitleEdit and self.m_TitleEdit:IsEnable()) then
			local newName = self.m_TitleEdit:GetWndText()
			if("" == newName) then
				self.m_TitleCheck:SetWndText(oldName)
				self.m_TitleEdit:ShowWnd(false)
				return
			end
			local classId	= self.m_tblClassList.classId
			self.m_TitleEdit:SetWndText("")
			self.m_TitleEdit:ShowWnd(false)
			
			Gac2Gas:RenameFriendClass(g_Conn, classId, newName, oldName)
		end
	end
end

--打开成员信息面板并显示成员信息
function CAssociationFriendListItem:ShowPersonalInfo(context)
	g_GameMain.m_AssociationBase:ShowPersonalInfo(context)
end

--删除好友
local function DeleteMyFriend(friendID, index)
	if(1 == index) then
		Gac2Gas:DeleteMyFriend(g_Conn, friendID)
	end
	return true
end

function CAssociationFriendListItem:PreDeleteMyFriend(tblClassIdAndIndex)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local base			= g_GameMain.m_AssociationBase
	local classIndex	= base.m_tblFriendClassIndex[tblClassIdAndIndex[1]]
	local tbl			= base.m_tblFriendListClass[classIndex]
	local friendID		= tbl[tblClassIdAndIndex[2]].playerId
	MessageBox(g_GameMain, MsgBoxMsg(15007, tbl[tblClassIdAndIndex[2]].playerName), BitOr(MB_BtnOK,MB_BtnCancel), DeleteMyFriend, friendID)
end

--删除黑名单成员
function CAssociationFriendListItem:DeleteBlackListMember(index)
	local base			= g_GameMain.m_AssociationBase
	local tbl			= base.m_tblBlackList
	local blackMemID	= tbl[index].playerId
	Gac2Gas:DeleteBlackListMember(g_Conn, blackMemID)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

local function MoveToBlackList(friendID, index)
	if(1 == index) then
		Gac2Gas:MoveToBlackList(g_Conn, friendID)
	else
		g_GameMain.m_AssociationBase.RetMoveToBlackList = nil
	end
	return true
end

--移动到黑名单
function CAssociationFriendListItem:PreMoveToBlackList(tblClassIdAndIndex)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local base			= g_GameMain.m_AssociationBase
	local classIndex	= base.m_tblFriendClassIndex[tblClassIdAndIndex[1]]
	local tbl			= base.m_tblFriendListClass[classIndex]
	local friendID		= tbl[tblClassIdAndIndex[2]].playerId
	local friendName	= tbl[tblClassIdAndIndex[2]].playerName
	g_GameMain.m_AssociationBase.RetMoveToBlackList = function ()
		local tblItem	= g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd
		table.insert(base.m_tblBlackList, tbl[tblClassIdAndIndex[2]])
		table.remove(tbl, tblClassIdAndIndex[2])
		base:SortFriendListByBeOnline(tblClassIdAndIndex[1])
		self:ResetItem()
		tblItem[#tblItem]:ResetItem()
		base:DeletePriChatCallInfo(friendID)
		g_GameMain.m_AssociationBase.RetMoveToBlackList = nil
	end
	MessageBox(g_GameMain, MsgBoxMsg(15008, friendName), BitOr(MB_BtnOK,MB_BtnCancel), MoveToBlackList, friendID)
end

--移动到组
function CAssociationFriendListItem:ChangeToClass(context)
	local base			= g_GameMain.m_AssociationBase
	local classIndex	= base.m_tblFriendClassIndex[context[1]]
	local tbl			= base.m_tblFriendListClass
	local newClassIndex	= context[3]
	local friendID		= tbl[classIndex][context[2]].playerId
	local newClassID	= tbl[newClassIndex].classId
	
	g_GameMain.m_AssociationBase.RetChangeToClass = function ()
		local tblItem	= g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd
		table.insert(tbl[newClassIndex], tbl[classIndex][context[2]])
		table.remove(tbl[classIndex], context[2])
		base:SortFriendListByBeOnline(context[1])
		base:SortFriendListByBeOnline(newClassID)
		self:ResetItem()
		tblItem[newClassIndex]:ResetItem()
	end

	Gac2Gas:ChangeToClass(g_Conn, friendID, newClassID)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

--显示准备添加好友
function CAssociationFriendListItem:ShowChooseClass(index)
	local base		= g_GameMain.m_AssociationBase
	local tbl		= base.m_tblBlackList
	local tblInfo	= {}
	tblInfo.id		= tbl[index].playerId
	tblInfo.name	= tbl[index].playerName
	g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, false)
end

--准备添加好友组
function CAssociationFriendListItem:PreAddClass()
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]:PreAddClass()
end

--删除好友组
function CAssociationFriendListItem:DeleteFriendClass()
	local base			= g_GameMain.m_AssociationBase
	local tblFriend		= base.m_tblFriendListClass[self.m_itemIndex+1]
	if(next(tblFriend)) then
		MessageBox(g_GameMain, MsgBoxMsg(15006), BitOr(MB_BtnOK,MB_BtnCancel), AssociationDelClass, tblFriend.classId)
	else
		Gac2Gas:DeleteFriendClass(g_Conn, tblFriend.classId)
	end
end

--删除好友组的MessageBox回调函数
function AssociationDelClass(classId, index)
	if(1 == index) then
		Gac2Gas:DeleteFriendClass(g_Conn, classId)
	end
	return true
end

--邀请组队
function CAssociationFriendListItem:InviteMakeTeam(id)
	Gac2Gas:InviteMakeTeam(g_Conn, id)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

--私聊
function CAssociationFriendListItem:CreatePrivateChatWnd(context)
	local chatWithId, chatWithName = unpack(context)
	g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(chatWithId, chatWithName)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

--交易
function CAssociationFriendListItem:SendTradeInvitation(id)
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160015)
		return 
	end
	Gac2Gas:SendTradeInvitation(g_Conn, id)
end

--邀请加入佣兵团
function CAssociationFriendListItem:TongInvite(id)
	Gac2Gas:InviteJoinTong(g_Conn, id)
end

--申请加入佣兵团
function CAssociationFriendListItem:TongApp(id)
	Gac2Gas:RequestJoinTong(g_Conn, id)
end

function CAssociationFriendListItem:InviteArmyCorps(id)
	Gac2Gas:InviteJoinArmyCorps(g_Conn, id)
end

--邀请加入好友群
function CAssociationFriendListItem:InviteToGroup(context)
	local base			= g_GameMain.m_AssociationBase
	local classIndex	= base.m_tblFriendClassIndex[context[1]]
	local tbl			= base.m_tblFriendListClass[classIndex]
	local inviteeId		= tbl[context[2]].playerId
	local groupId		= base.m_tblFriendGroup[context[3]][1][2]
	Gac2Gas:InviteToGroup(g_Conn, inviteeId, groupId)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

function CAssociationFriendListItem:UpdateFriendInfo(nFriendId)
	Gac2Gas:UpdateFriendInfo(g_Conn, nFriendId)
end

--右键菜单----------------------------------------------
function CAssociationFriendListItem:OnMenuMsg(func)
	if(func) then func(self) end
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
end

function CAssociationFriendListItem:ShowFriendMenu(x, y, uParam1)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local base			= g_GameMain.m_AssociationBase
	local tblMemInfo 	= self.m_tblClassList[uParam1+1]
	local classId		= self.m_tblClassList.classId
	local Menu	= CMenu:new("TargetMenu", g_GameMain)
	Menu.m_List	= Menu:GetDlgChild("ListCtrl")
	g_GameMain.m_Menu	= Menu
	
	if(2 == self.m_tblClassList.classId) then --如果是黑名单
		--添加为好友
		Menu:InsertItem(GacMenuText(601), self.ShowChooseClass, nil, false, true, self.DestroyChildMenu, self, uParam1+1)
		--删除
		Menu:InsertItem(GacMenuText(602), self.DeleteBlackListMember, nil, false, true, self.DestroyChildMenu, self, uParam1+1)
	else
		--私聊
		Menu:InsertItem(GacMenuText(604), self.CreatePrivateChatWnd, nil, false, true, self.DestroyChildMenu, self, {tblMemInfo.playerId, tblMemInfo.playerName})
		--移动到组 >>
		Menu:InsertItem(GacMenuText(607), self.DestroyMenu, nil, false, true, self.ShowMoveToClassMenu, self, {classId, uParam1+1})
		if(next(base.m_tblCreateGroupIndex) or next(base.m_tblManageGroupIndex)) then
			--邀请加入好友群 >>
			Menu:InsertItem(GacMenuText(610), self.DestroyMenu, nil, false, true, self.ShowInviteToGroupMenu, self, {classId, uParam1+1})
		end
		--转到黑名单
		Menu:InsertItem(GacMenuText(605), self.PreMoveToBlackList, nil, false, true, self.DestroyChildMenu, self, {classId, uParam1+1})
		--删除好友
		Menu:InsertItem(GacMenuText(609), self.PreDeleteMyFriend, nil, false, true, self.DestroyChildMenu, self, {classId, uParam1+1})
		--查看信息
		Menu:InsertItem(GacMenuText(608), self.ShowPersonalInfo, nil, false, true, self.DestroyChildMenu, self, {tblMemInfo.playerId, tblMemInfo.playerName})
		
		if(1 == tblMemInfo.nOnline) then
			--邀请组队
			Menu:InsertItem(GacMenuText(603), self.InviteMakeTeam, nil, false, true, self.DestroyChildMenu, self, tblMemInfo.playerId)
			--交易
			Menu:InsertItem(GacMenuText(606), self.SendTradeInvitation, nil, false, true, self.DestroyChildMenu, self, tblMemInfo.playerId)
			--邀请加入佣兵团/申请加入佣兵团
			if( 0 ~= g_MainPlayer.m_Properties:GetTongID() and
				0 == tblMemInfo.nTongId and
				0 ~= g_TongPurviewMgr:GetInviteMemberValue(g_GameMain.m_TongBase.m_TongPos) ) then
				Menu:InsertItem(GacMenuText(612), self.TongInvite, nil, false, true, self.DestroyChildMenu, self, tblMemInfo.playerId)
			end
			if CArmyCorpsPanel.GetWnd():CanInviteArmyCorps() then
				Menu:InsertItem(GacMenuText(614), self.InviteArmyCorps, nil, false, true, self.DestroyChildMenu, self, tblMemInfo.playerId)
			end
		end
		if( 0 == g_MainPlayer.m_Properties:GetTongID() and 0 ~= tblMemInfo.nTongId ) then
			Menu:InsertItem(GacMenuText(613), self.TongApp, nil, false, true, self.DestroyChildMenu, self, tblMemInfo.nTongId)
		end
		
		--刷新
		Menu:InsertItem(GacMenuText(611), self.UpdateFriendInfo, nil, false, true, self.DestroyChildMenu, self, tblMemInfo.playerId)
	end
	SetPos(Menu, x, y)
end

function CAssociationFriendListItem:ShowClassTitleMenu(x, y)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local tbl = {	{GacMenuText(701),	self.PreRenameClass},		--重命名
					{GacMenuText(702),	self.PreAddClass},			--添加好友分组
					{GacMenuText(703),	self.DeleteFriendClass}	}	--删除好友分组
	local Menu	= CMenu:new("TargetMenu", g_GameMain)
	Menu.m_List	= Menu:GetDlgChild("ListCtrl")
	g_GameMain.m_Menu	= Menu
	if(self.m_isBlacklist or 0 == self.m_itemIndex) then
		Menu:InsertItem(tbl[2][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[2][2])
	else
		for i = 1, #tbl do
			Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
		end
	end
	SetPos(Menu, x, y)
end

--右键子菜单----------------------------------------------
function CAssociationFriendListItem:DestroyChildMenu()
	if(self.m_ChildMenu) then
		self.m_ChildMenu:Destroy()
		self.m_ChildMenu = nil
	end
end

--移动到组的子菜单
function CAssociationFriendListItem:ShowMoveToClassMenu(tblClassIdAndIndex)
	if(self.m_ChildMenu) then
		self.m_ChildMenu:Destroy()
		self.m_ChildMenu = nil
	end
	local base 	= g_GameMain.m_AssociationBase
	local tbl	= base.m_tblFriendListClass
	if(#tbl < 2) then return end
	local Menu			= CMenu:new("TargetMenu", g_GameMain.m_Menu)
	Menu.m_List			= Menu:GetDlgChild("ListCtrl")
	self.m_ChildMenu	= Menu
	for i = 1, #tbl do
		local classIndex = base.m_tblFriendClassIndex[tblClassIdAndIndex[1]]
		local context = {unpack(tblClassIdAndIndex)}
		table.insert(context, i)
		if(i ~= classIndex) then
			Menu:InsertItem(tbl[i].className, self.ChangeToClass, nil, false, false, nil, self, context)
		end
	end
	local rect = self:GetItemRect(g_GameMain.m_Menu.m_List, 1)
	SetPos(Menu, 2*rect.left-rect.right, rect.top)
end

--邀请入群的子菜单
function CAssociationFriendListItem:ShowInviteToGroupMenu(tblClassIdAndIndex)
	if(self.m_ChildMenu) then
		self.m_ChildMenu:Destroy()
		self.m_ChildMenu = nil
	end
	local base					= g_GameMain.m_AssociationBase
	local tblFriendGroup		= base.m_tblFriendGroup
	local tblCreateGroupIndex	= base.m_tblCreateGroupIndex
	local tblManageGroupIndex	= base.m_tblManageGroupIndex
	local Menu					= CMenu:new("TargetMenu", g_GameMain.m_Menu)
	Menu.m_List					= Menu:GetDlgChild("ListCtrl")
	self.m_ChildMenu			= Menu
	for i = 1, #tblCreateGroupIndex do
		local context = {unpack(tblClassIdAndIndex)}
		table.insert(context, tblCreateGroupIndex[i])
		Menu:InsertItem(tblFriendGroup[tblCreateGroupIndex[i]][1][1], self.InviteToGroup, nil, false, false, nil, self, context)
	end
	for i = 1, #tblManageGroupIndex do
		local context = {unpack(tblClassIdAndIndex)}
		table.insert(context, tblManageGroupIndex[i])
		Menu:InsertItem(tblFriendGroup[tblManageGroupIndex[i]][1][1], self.InviteToGroup, nil, false, false, nil, self, context)
	end
	local rect = self:GetItemRect(g_GameMain.m_Menu.m_List, 2)
	SetPos(Menu, 2*rect.left-rect.right, rect.top)
end
--------------------------------------------------------
function CAssociationFriendListItem:GetItemRect(listWnd, itemIndex)
	local rect = CFRect:new()
	listWnd:GetSubItem(itemIndex, 0):GetLogicRect(rect)
	return rect
end

------------------------------------------------------------------------------------------------------
function CAssociationFriendListItem:CreateClassTitleIcon(parent)
	local wnd = CAssociationClassTitleIcon:new()
	wnd:CreateFromRes("AssociationClassTitleIcon", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CAssociationClassTitleIcon:OnChildCreated()
	self.m_IconCB = self:GetDlgChild("IconCB")
end

--***************************************************************************************
--好友列表listctrl中的/itemWnd中的/listctrl中的/itemWnd
--***************************************************************************************
function CAssociationFriendListItem:CreateAssociationListItemChild(parent, belongToWnd, index, isBlackList)
	local wnd = CAssociationFriendListItemChild:new()
	wnd:Init(parent, belongToWnd, index, isBlackList)
	return wnd
end

function CAssociationFriendListItemChild:Init(parent, belongToWnd, index, isBlackList)
	self.m_tblMemInfo		= belongToWnd.m_tblClassList[index+1]
	self.m_childItemIndex	= index
	self:CreateFromRes("AssociationListItemChild", parent)
	self:ShowWnd( true )
	
	self:SetInfoText(isBlackList)
end

function CAssociationFriendListItemChild:OnChildCreated()
	self.m_Class	= self:GetDlgChild("Class")
	self.m_Name		= self:GetDlgChild("Name")
	self.m_Level	= self:GetDlgChild("Level")
	self.m_Location	= self:GetDlgChild("Location")
	self.m_tblTeam	= {}
	for i = 1, 4 do
		self.m_tblTeam[i] = self:GetDlgChild("Team" .. i)
	end
end

function CAssociationFriendListItemChild:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)

end

function CAssociationFriendListItemChild:SetInfoText(isBlackList)
	local tblInfo = self.m_tblMemInfo
	if(isBlackList) then
		self:EnableWnd(false)
		self.m_Location:SetWndText("---")
		self.m_Level:SetWndText("---")
	else
		self:EnableWnd(1 == tblInfo.nOnline)
		self.m_Location:SetWndText( GetSceneDispalyName(tblInfo.sScene) )
		self.m_Level:SetWndText( GetStaticTextClient(8322, tblInfo.playerLevel) )
	end
	for i, v in ipairs(self.m_tblTeam) do
		v:ShowWnd(tblInfo.nTeamSize > i)
	end
	if( tblInfo.nPlayerCamp and tblInfo.nPlayerClass ) then
		self:SetClassPic(tblInfo.nPlayerCamp, tblInfo.nPlayerClass)
	end
	self.m_Name:SetWndText(tblInfo.playerName)
end

function CAssociationFriendListItemChild:SetClassPic(nCamp, nClass)
	local sEnableImg, sDisableImg = g_DynImageMgr:GetClassPic(nCamp, nClass)
	g_DynImageMgr:AddImageByIP(self.m_Class, sEnableImg, IP_ENABLE)
	g_DynImageMgr:AddImageByIP(self.m_Class, sDisableImg, IP_DISABLE)
end

------------------------------------------------------------------------------------------------------
function CAssociationFriendListWnd:CreateAddClassEdit(parent)
	local wnd = CAssociationAddClassEdit:new()
	wnd:CreateFromRes("AssociAddClassEdit", parent)
	return wnd
end

function CAssociationAddClassEdit:OnChildCreated()
	self.m_TitleEdit = self:GetDlgChild("TitleEdit")
	self.m_TitleEdit:SetMaxTextLenght(8)
end

function CAssociationAddClassEdit:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == EDIT_LOSEFOCUS or uMsgID == EDIT_RETURN) then
		if(Child == self.m_TitleEdit) then
			local className		= self.m_TitleEdit:GetWndText()
			local textFltMgr	= CTextFilterMgr:new()
			g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]:ListShift(false)
			if( not textFltMgr:IsValidName(className) ) then
				MsgClient(130007)
				return
			end
			Gac2Gas:AddFriendClass(g_Conn, className)
		end
	end
end

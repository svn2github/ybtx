gac_require "relation/association/AssociationChatBase" --要放在前面
gac_require "relation/association/AssociationChatInc"

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

local canSendMaxLength = 1024

--*****************************************************************************
--私聊窗口
--*****************************************************************************
function CreateAssociationPrivateChatWnd(parent, chatWithId, chatWithName)
	local wnd = CAssociationPrivateChatWnd:new()
	wnd:Init(parent, chatWithId, chatWithName)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	table.insert(g_GameMain.m_tblAssociationPriChat, wnd)
	return wnd
end

function CAssociationPrivateChatWnd:Init(parent, chatWithId, chatWithName)
	self.m_ChatWithId	= chatWithId
	self.m_ChatWithName	= chatWithName
	self.m_TextFilter	= CTextFilterMgr:new()
	self:InitEx()
	self.m_RecordPath	= string.format("%s%s%d%d%s", g_RootPath, "var/gac/cm/", g_MainPlayer.m_Properties:GetCharID(), self.m_ChatWithId, ".cm")
	self:CreateFromRes("AssociationPrivateChat", parent)
	self:ShowWnd( true )
end

function CAssociationPrivateChatWnd:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_MinimumBtn	= self:GetDlgChild("MinimumBtn")
	self.m_Send			= self:GetDlgChild("Send")
	self.m_InputText	= self:GetDlgChild("InputText")
	self.m_Title		= self:GetDlgChild("Title")
	self.m_Team			= self:GetDlgChild("Team")
	self.m_Trade		= self:GetDlgChild("Trade")
	self.m_MsgRecordRt	= self:GetDlgChild("MsgRecordRt")
	self.m_MsgRecordBtn	= self:GetDlgChild("MsgRecordBtn")
	self.m_Title:SetWndText(self.m_ChatWithName)
	self.m_MsgRecordRt:ShowWnd(false)
	self.m_MsgRecordBtn:ShowWnd(false)
	
	self.m_Static		= self:GetDlgChild("Static")
	self.m_OutputWnd	= self:CreateOutPutWnd(self.m_Static)
	self.m_OutPutText	= self.m_OutputWnd.m_Output
	self.m_InputText:SetMaxTextLenght(200)
end

---------------------------------------------------------------------------
function CAssociationPrivateChatWnd:CreateOutPutWnd(parent)
	local wnd = CAssociationPrivateChatOutput:new()
	wnd:CreateFromRes("AssociationPrivateChatOutput", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CAssociationPrivateChatOutput:OnChildCreated()
	--self.m_XBtn	= self:GetDlgChild("XBtn")
	self.m_Warning	= self:GetDlgChild("Warning")
	self.m_Output	= self:GetDlgChild("RichText")
--	local width		= self:GetWndOrgWidth()
--	self.m_Warning:SetAutoWidth(width, width)
	self.m_Warning:SetWndText("#cff0000"..GetStaticTextClient(200000))
--	local rtWarning = CFRect:new()
--	local rtOutput = CFRect:new()
--	self.m_Warning:GetLogicRect(rtWarning)
--	self.m_Output:GetLogicRect(rtOutput)
--	rtOutput.top = rtWarning.bottom
--	self.m_Output:SetLogicRect(rtOutput)
end

function CAssociationPrivateChatOutput:CloseWarning()
	self.m_Warning:ShowWnd(false)
	local rt = CFRect:new()
	self:GetLogicRect(rt)
	self.m_Output:SetLogicRect(rt)
end
---------------------------------------------------------------------------

--消息
function CAssociationPrivateChatWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_MinimumBtn) then
			g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreatePrivateChatMinimum(self.m_ChatWithId, self.m_ChatWithName, nil, self)
			self:ShowWnd(false, true)
		elseif(Child == self.m_Team) then
			Gac2Gas:InviteMakeTeam(g_Conn, self.m_ChatWithId)
			MsgClient(142)
		elseif(Child == self.m_Trade) then
			if (not g_GameMain.m_PlayersTradeWnd:IsShow() ) then
				if g_MainPlayer.m_ItemBagLock then
					MsgClient(160015)
					return 
				end
				Gac2Gas:SendTradeInvitation(g_Conn, self.m_ChatWithId)
				MsgClient(6032)
			end
		elseif(Child == self.m_MsgRecordBtn) then
			if(self.m_MsgRecordRt:IsShow()) then
				self.m_MsgRecordRt:ShowWnd(false)
			else
				self.m_MsgRecordRt:ShowWnd(true)
				self:ReadRecord()
			end
		elseif(Child == self.m_Send) then
			self:SendMsg()
			self.m_InputText:SetFocus()
		end
	elseif(uMsgID == EDIT_CTRLRETURN) then
		if( Child == self.m_InputText and not self.m_InputText:GetSendType() ) then
			self:SendMsg()
		end
	elseif(uMsgID == EDIT_RETURN) then
		if( Child == self.m_InputText and self.m_InputText:GetSendType() ) then
			self:SendMsg()
		end
	end
end

function CAssociationPrivateChatWnd:ReadRecord()
	local f = CLuaIO_Open(self.m_RecordPath, "rb")
	if f then
		self.m_tblMsgRecord = {}
		while(true) do
			local line = f:ReadLine()
			if("" ~= line) then
				table.insert(self.m_tblMsgRecord, line)
			else
				f:Close()
				break
			end
		end
		self.m_MsgRecordRt:SetWndText( table.concat(self.m_tblMsgRecord) )
	end
end

function CAssociationPrivateChatWnd:SendMsg(str)
	local text = self:SendMsgCommon(str)
	if(not text) then return end
	Gac2Gas:PrivateChatSendMsg(g_Conn, self.m_ChatWithId, text)
end

function CAssociationPrivateChatWnd:CloseWnd()
	local tbl = g_GameMain.m_tblAssociationPriChat
	for i = 1, #tbl do
		if(tbl[i].m_ChatWithId == self.m_ChatWithId) then
			table.remove(tbl, i)
			break
		end
	end
end

--*****************************************************************************
--群聊窗口
--*****************************************************************************
function CreateAssociationPublicChatWnd(parent, groupId, groupName)
	local wnd = CAssociationPublicChatWnd:new()
	wnd:Init(parent, groupId, groupName)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	table.insert(g_GameMain.m_tblAssociationPubChat, wnd)
	return wnd
end

function CAssociationPublicChatWnd:Init(parent, groupId, groupName)
	self.tblMemItemWnd	= {}
	self.m_GroupId		= groupId
	self.m_GroupName	= groupName
	self:InitEx()
	self.m_TextFilter	= CTextFilterMgr:new()
	self:CreateFromRes("AssociationPublicChat", parent)
	self:ShowWnd( true )
	self:DrawMemListItem()
	self:ShowDeclareText()
end

function CAssociationPublicChatWnd:OnChildCreated()
	self.m_XBtn					= self:GetDlgChild("XBtn")
	self.m_MinimumBtn			= self:GetDlgChild("MinimumBtn")
	self.m_Send					= self:GetDlgChild("Send")
	self.m_InputText			= self:GetDlgChild("InputText")
	self.m_MemberList			= self:GetDlgChild("MemberList")
	self.m_Title				= self:GetDlgChild("Title")
	self.m_DeclareRichText		= self:GetDlgChild("DeclareRichText")
	self.m_DeclareEdit			= self:GetDlgChild("DeclareEdit")
	self.m_OnChangeDeclareBtn	= self:GetDlgChild("OnChangeDeclareBtn")
	self.m_ChangeDeclareBtn		= self:GetDlgChild("ChangeDeclareBtn")
	self.m_GroupIdWnd			= self:GetDlgChild("GroupId")
	self.m_Title:SetWndText(self.m_GroupName)
	self.m_ChangeDeclareBtn:ShowWnd(false)
	self.m_DeclareEdit:ShowWnd(false)
	self.m_GroupIdWnd:SetWndText(GetStaticTextClient(2011,self.m_GroupId))
	
	self.m_Static		= self:GetDlgChild("Static")
	self.m_OutputWnd	= self:CreateOutPutWnd(self.m_Static)
	self.m_OutPutText	= self.m_OutputWnd.m_Output
	self.m_InputText:SetMaxTextLenght(200)
end

---------------------------------------------------------------------------
function CAssociationPublicChatWnd:CreateOutPutWnd(parent)
	local wnd = CAssociationPublicChatOutput:new()
	wnd:CreateFromRes("AssociationPublicChatOutput", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CAssociationPublicChatOutput:OnChildCreated()
	--self.m_XBtn	= self:GetDlgChild("XBtn")
	self.m_Warning	= self:GetDlgChild("Warning")
	self.m_Output	= self:GetDlgChild("RichText")
--	local width		= self:GetWndOrgWidth()
--	self.m_Warning:SetAutoWidth(width, width)
	self.m_Warning:SetWndText("#cff0000"..GetStaticTextClient(200000))
--	local rtWarning	= CFRect:new()
--	local rtOutput	= CFRect:new()
--	self.m_Warning:GetLogicRect(rtWarning)
--	self.m_Output:GetLogicRect(rtOutput)
--	rtOutput.top = rtWarning.bottom
--	self.m_Output:SetLogicRect(rtOutput)
end

function CAssociationPublicChatOutput:CloseWarning()
	self.m_Warning:ShowWnd(false)
	local rt = CFRect:new()
	self:GetLogicRect(rt)
	self.m_Output:SetLogicRect(rt)
end
---------------------------------------------------------------------------

--消息
function CAssociationPublicChatWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_MinimumBtn) then
			g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreatePublicChatMinimum(self.m_GroupId, nil, self)
			self:ShowWnd(false, true)
		elseif(Child == self.m_Send) then
			self:SendMsg()
			self.m_InputText:SetFocus()
		elseif(Child == self.m_OnChangeDeclareBtn) then
			self:OnChangeDeclare()
		elseif(Child == self.m_ChangeDeclareBtn) then
			self:ChangeDeclare()
		end
	elseif(uMsgID == ITEM_RBUTTONCLICK) then
		if(Child == self.m_MemberList) then
			self:ShowMemListMenu(uParam1+1)
		end
	elseif(uMsgID == EDIT_CTRLRETURN) then
		if( Child == self.m_InputText and not self.m_InputText:GetSendType() ) then
			self:SendMsg()
		end
	elseif(uMsgID == EDIT_RETURN) then
		if( Child == self.m_InputText and self.m_InputText:GetSendType() ) then
			self:SendMsg()
		end
	end
end

function CAssociationPublicChatWnd:SendMsg()
	local text = self:SendMsgCommon()
	if(not text) then return end
	Gac2Gas:PublicChatSendMsg(g_Conn, self.m_GroupId, text)
end

function CAssociationPublicChatWnd:CloseWnd()
	local tbl = g_GameMain.m_tblAssociationPubChat
	for i = 1, #tbl do
		if(tbl[i].m_GroupId == self.m_GroupId) then
			table.remove(tbl, i)
			break
		end
	end
	
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	
end

function CAssociationPublicChatWnd:OnChangeDeclare()
	self:AboutChangeDeclare(true)
	self.m_DeclareEdit:SetWndText(self.m_DeclareRichText:GetWndText())
end

function CAssociationPublicChatWnd:ChangeDeclare()
	local sDeclare = self.m_TextFilter:RemoveTab1( self.m_DeclareEdit:GetWndText() )
	if sDeclare == "" then
		MsgClient(5008)
		return
	end
	if( not self.m_TextFilter:IsValidMsg(sDeclare) ) then
		MsgClient(130058)
		return
	end
	
	self:AboutChangeDeclare(false)
	Gac2Gas:ChangeGroupDeclare(g_Conn, self.m_GroupId, sDeclare)
end

function CAssociationPublicChatWnd:AboutChangeDeclare(bFlag) --true准备修改, false确认修改
	self.m_OnChangeDeclareBtn:ShowWnd(not bFlag)
	self.m_ChangeDeclareBtn:ShowWnd(bFlag)
	self.m_DeclareRichText:ShowWnd(not bFlag)
	self.m_DeclareEdit:ShowWnd(bFlag)
end

function CAssociationPublicChatWnd:ShowDeclareText()
	local base               = g_GameMain.m_AssociationBase
	local groupIndex         = base.m_tblFriendGroupIndex[self.m_GroupId]
	local tblThisFriendGroup = base.m_tblFriendGroup[groupIndex]
	self.m_DeclareRichText:SetWndText(tblThisFriendGroup[1][5])
end

function CAssociationPublicChatWnd:DrawMemListItem()
	self.m_MemberList:DeleteAllItem()
	self.tblMemItemWnd = {}
	local base					= g_GameMain.m_AssociationBase
	local groupIndex			= base.m_tblFriendGroupIndex[self.m_GroupId]
	local tblThisFriendGroup	= base.m_tblFriendGroup[groupIndex]
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndWidth())
	for i = 1, #(tblThisFriendGroup[2]) do
		self.m_MemberList:InsertItem(i-1, 20)
		local item		= self.m_MemberList:GetSubItem(i-1, 0)
		local itemWnd	= self:CreateMemListItem(item)
		table.insert(self.tblMemItemWnd, itemWnd)
		itemWnd:ShowMemInfo(tblThisFriendGroup[2][i][1], tblThisFriendGroup[2][i][3], tblThisFriendGroup[2][i][4])
	end
end

--右键菜单----------------------------------------------

function CAssociationPublicChatWnd:ShowMemListMenu(index)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local base					= g_GameMain.m_AssociationBase
	local groupIndex			= base.m_tblFriendGroupIndex[self.m_GroupId]
	local tblThisFriendGroup	= base.m_tblFriendGroup[groupIndex]
	
	local Menu	= CMenu:new("TargetMenu", g_GameMain)
	Menu.m_List	= Menu:GetDlgChild("ListCtrl")
	g_GameMain.m_Menu	= Menu
	if(tblThisFriendGroup[2][index][2] == g_MainPlayer.m_uID) then --选择的是自己
		
	else
		--加为好友
		Menu:InsertItem(GacMenuText(301), self.OnAddFriend, nil, false, false, nil, self, {tblThisFriendGroup[2][index][2], tblThisFriendGroup[2][index][1]})
		if(1 == tblThisFriendGroup[1][6]) then
			if(2 == tblThisFriendGroup[2][index][3]) then
				--取消管理员
				Menu:InsertItem(GacMenuText(302), self.SetManager, nil, false, false, nil, self, {tblThisFriendGroup[2][index][2], self.m_GroupId, 3})
			elseif(3 == tblThisFriendGroup[2][index][3]) then
				--设置为管理员
				Menu:InsertItem(GacMenuText(303), self.SetManager, nil, false, false, nil, self, {tblThisFriendGroup[2][index][2], self.m_GroupId, 2})
			end
			--转让群主
			Menu:InsertItem(GacMenuText(304), self.SetManager, nil, false, false, nil, self, {tblThisFriendGroup[2][index][2], self.m_GroupId, 1})
			--请离该群
			Menu:InsertItem(GacMenuText(305), self.ShowKickOutOfGroupMsgBox, nil, false, false, nil, self, {tblThisFriendGroup[2][index][1], tblThisFriendGroup[2][index][2], self.m_GroupName, self.m_GroupId})
		elseif(2 == tblThisFriendGroup[1][6]) then
			if(3 == tblThisFriendGroup[2][index][3]) then
				--请离该群
				Menu:InsertItem(GacMenuText(306), self.ShowKickOutOfGroupMsgBox, nil, false, false, nil, self, {tblThisFriendGroup[2][index][1], tblThisFriendGroup[2][index][2], self.m_GroupName, self.m_GroupId})
			end
		end
	end
	--查看资料
	Menu:InsertItem(GacMenuText(307), self.ShowPersonalInfo, nil, false, false, nil, self, {tblThisFriendGroup[2][index][2], tblThisFriendGroup[2][index][1]})
	local posCursor = CFPos:new()
	g_App:GetRootWnd():GetLogicCursorPos(posCursor)
	SetPos(Menu, posCursor.x, posCursor.y)
end
--------------------------------------------------------
--加为好友
function CAssociationPublicChatWnd:OnAddFriend(context)
	local tblInfo	= {}
	tblInfo.id		= context[1]
	tblInfo.name	= context[2]
	g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, false)
end

--设置权限
function CAssociationPublicChatWnd:SetManager(context)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local memberId, groupId, ctrlType = unpack(context)
	Gac2Gas:SetCtrlType(g_Conn, memberId, groupId, ctrlType)
end

--打开成员信息面板并显示成员信息
function CAssociationPublicChatWnd:ShowPersonalInfo(context)
	g_GameMain.m_AssociationBase:ShowPersonalInfo(context)
end

--显示解散群的确定框
function CAssociationPublicChatWnd:ShowKickOutOfGroupMsgBox(context)
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:Destroy()
		g_GameMain.m_Menu = nil
	end
	local memberName, memberId, groupName, groupId = unpack(context)
	local string = MsgBoxMsg(15004, memberName, groupName)
	MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), AssoKickOutOfGroupMBoxClick, {memberId, groupId})
end

function AssoKickOutOfGroupMBoxClick(context, index)
	local memberId, groupId = unpack(context)
	if(1 == index) then
		Gac2Gas:KickOutOfGroup(g_Conn, memberId, groupId)
	end
	return true
end

------------------------------------------------------------------
function CAssociationPublicChatWnd:CreateMemListItem(parent)
	local wnd = CAssociationPubChatMemListItemWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationPubChatMemListItemWnd:ShowMemInfo(name, ctrlType, beOnline)
	local str = (1 == ctrlType) and CTextureFontMgr_Inst():GetEmtStrByIdx("#1004") or (2 == ctrlType and CTextureFontMgr_Inst():GetEmtStrByIdx("#1024") or "")
	self.m_Icon:SetWndText(str)
	self:SetName(name, beOnline)
end
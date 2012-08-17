gac_require "relation/association/AssociationInc"
gac_require "relation/association/AssociationList"
gac_require "relation/association/AssociationPersonalInfo"

function CreateAssociationWnd(parent)
	local wnd = CAssociationWnd:new()
	wnd:Init(parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	return wnd
end

function CAssociationWnd:Init(parent)
	self.m_bNotInitBefore	= true
	self.m_strShowSen		= ""
	self:CreateFromRes("Association", parent)
	self.m_AssociationListWnd = CreateAssociationListWnd(self.m_Static)
end

function CAssociationWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_Static			= self:GetDlgChild("Static")
	self.m_SearchBtn		= self:GetDlgChild("SearchBtn")
	self.m_PersonalInfoBtn	= self:GetDlgChild("PersonalInfoBtn")
	self.m_SearchEdit		= self:GetDlgChild("SearchEdit")
	self.m_ShowSenBtn		= self:GetDlgChild("ShowSenBtn")
	self.m_ShowSenEdit		= self:GetDlgChild("ShowSenEdit")
	self.m_ShortOpenBtn		= self:GetDlgChild("ShortOpenBtn")
	self.m_ShowOnlineCK		= self:GetDlgChild("ShowOnlineCK")
	self.m_ShowSenEdit:ShowWnd(false)
	self.m_ShowSenBtn:SetTextOffsetX(8)
	self.m_ShowSenBtn:SetTextOffsetX(5)
end

--消息
function CAssociationWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_SearchBtn) then
			local wnd = g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]
			if( wnd ) then
				wnd:PreFindPlayer()
			end
		elseif(Child == self.m_PersonalInfoBtn) then
			self:OpenPersonalInfoWnd()
		elseif(Child == self.m_ShowSenBtn) then
			self:PreChangeShowSen()
		elseif(Child == self.m_ShortOpenBtn) then
			self:OpenChatWnd()
			self.m_SearchEdit:SetWndText("")
			self.m_SearchEdit.tblSelect = nil
		elseif(Child == self.m_ShowOnlineCK) then
			for i, v in ipairs(g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd) do
				v:ResetItem()
			end
		end
	elseif(uMsgID == EDIT_ONCHAR or uMsgID == EDIT_DELETE) then
		if(Child == self.m_SearchEdit) then
			local str = self.m_SearchEdit:GetWndText()
			if str ~= "" then
				self:ShowPlayer(str)
				self.m_SearchEdit:SetFocus()
			end
		end
	elseif(uMsgID == EDIT_LOSEFOCUS or uMsgID == EDIT_RETURN) then
		if(Child == self.m_ShowSenEdit) then
			self:ChangeShowSen()
		end
	end
end

function CAssociationWnd:VirtualExcludeWndOnClose()
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]:ListShift(false)
end

function CAssociationWnd:VirtualExcludeWndClosed()
	self:DestroyMenu()
end

function CAssociationWnd:AboutChangeShowSen(bFlag)
	self.m_ShowSenBtn:ShowWnd(not bFlag)
	self.m_ShowSenEdit:ShowWnd(bFlag)
	if( bFlag ) then
		self.m_ShowSenEdit:SetFocus()
	end
end

function CAssociationWnd:PreChangeShowSen()
	self:AboutChangeShowSen(true)
	self.m_strShowSen = self.m_ShowSenBtn:GetWndText()
	self.m_ShowSenBtn:SetWndText("")
	self.m_ShowSenEdit:SetWndText(self.m_strShowSen)
end

function CAssociationWnd:ChangeShowSen()
	self:AboutChangeShowSen(false)
	local showSen = self.m_ShowSenEdit:GetWndText()
	Gac2Gas:ChangeShowSentence(g_Conn, showSen)
end

function CAssociationWnd:RetChangeShowSen(bFlag, newStr)
	local str = bFlag and newStr or self.m_strShowSen
	self.m_ShowSenBtn:SetWndText(str)
end

function CAssociationWnd:ChangePersonalInfo(showSen)
	self.m_ShowSenBtn:SetWndText(showSen)
end

--打开个人信息编辑面板
function CAssociationWnd:OpenPersonalInfoWnd()
	if(not g_GameMain.m_AssociationPersonalInfoWnd) then
		g_GameMain.m_AssociationPersonalInfoWnd = CreateAssociationPersonalityInfoEditWnd(g_GameMain)
	end
	g_GameMain.m_AssociationPersonalInfoWnd:OpenPanel()
end

--选择添加到某组窗体**************************************************************************************

--添加到组的comboBox窗体
function CAssociationWnd:CreateAddToClassWnd(tblInfo, bIninvited) --false:不是被邀请,而是主动邀请时
	local tblWnd	= g_GameMain.m_tblAddToClassCB
	local index		= tblInfo.name
	if( tblWnd[index] ) then return end

	local wnd = CAssociationAddToClassWnd:new()
	wnd:Init(g_GameMain, bIninvited, tblInfo)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	wnd:OpenWnd()
	
	tblWnd[index] = wnd
	return wnd
end

function CAssociationAddToClassWnd:Init(parent, bIninvited, tblInfo)
	self.m_Id			= tblInfo.id
	self.m_Name			= tblInfo.name
	self.m_Class		= tblInfo.class
	self.m_bIninvited	= bIninvited
	self:CreateFromRes("AssociationAddToClass", parent)
	self.m_NameLable:SetWndText( GetStaticTextClient(2004, self.m_Name) )
	self:ShowWnd( true )
end

function CAssociationAddToClassWnd:OnChildCreated()
	self.m_ChooseClassCB	= self:GetDlgChild("ChooseClassCB")
	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_NameLable		= self:GetDlgChild("NameLable")
	self.m_XBtn				= self:GetDlgChild("XBtn")
end

function CAssociationAddToClassWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then --发送添加好友请求
			local classIndex	= self.m_ChooseClassCB:GetCurrentSel() + 1
			local classId		= g_GameMain.m_AssociationBase.m_tblFriendListClass[classIndex].classId
			if(self.m_bIninvited) then
				Gac2Gas:RespondAddFriend(g_Conn, self.m_Id, self.m_Class, classId)
			else
				if(self.m_Id) then
					Gac2Gas:AddFriendToClass(g_Conn, self.m_Id, classId)
				elseif(self.m_Name) then
					Gac2Gas:AddFriendToClassByName(g_Conn, self.m_Name, classId)
				end
			end
			self:ShowWnd(false)
		elseif(Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CAssociationAddToClassWnd:VirtualExcludeWndClosed()
	self.m_ChooseClassCB:RemoveAllItem()
	self:DestroyWnd()
	g_GameMain.m_tblAddToClassCB[self.m_Name] = nil
end

--设定选择组别的下拉列表
function CAssociationAddToClassWnd:SetAllGroup()
	local base = g_GameMain.m_AssociationBase
	local tbl = base.m_tblFriendListClass
	for i = 1, #tbl do
		self.m_ChooseClassCB:InsertItem(tbl[i].className, i-1, -1)
	end
	self.m_ChooseClassCB:SetSel(0)
end

--打开选择组别对话框(内部函数)
function CAssociationAddToClassWnd:OpenWnd()
	self:SetAllGroup()
	self:ShowWnd(true)
end

--*********************************************************************************************
--创建私聊窗体
function CAssociationWnd:CreateAssociationPriChatWnd(chatWithId, chatWithName)
	if(g_GameMain.m_PlayerInfoTbl.m_PlayerCharID == chatWithId) then --双击了自己
		return
	end
	
	if(g_GameMain.m_AssociationBase:IsInBlackListById(chatWithId)) then--检查是否在黑名单中
		MsgClient(5009)
		return
	end

	local tblPriChat = g_GameMain.m_tblAssociationPriChat
	for i = 1, #tblPriChat do
		if(tblPriChat[i].m_ChatWithId == chatWithId) then --如果和此人聊天的窗体已经打开
			tblPriChat[i]:ShowWnd(true)
			tblPriChat[i].m_InputText:SetFocus()
			g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("私聊", chatWithId)
			return tblPriChat[i]
		end
	end
	
	local wnd		= CreateAssociationPrivateChatWnd(g_GameMain, chatWithId, chatWithName)
	local tblText	= g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("私聊", chatWithId)
	if( tblText and next(tblText) ) then
		for i, v in ipairs(tblText) do
			wnd:AddNewLineInfo(v)
		end
	end
	wnd.m_InputText:SetFocus()
	return wnd
end

--创建群聊窗体
function CAssociationWnd:CreateAssociationPubChatWnd(groupId, groupName)
	local chatWnd = self:FindAssociationPubChatWndByGroupId(groupId)
	if(chatWnd) then
		chatWnd:ShowWnd(true)
		chatWnd.m_InputText:SetFocus()
		g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("群聊", groupId)
		return chatWnd
	end
	
	local wnd		= CreateAssociationPublicChatWnd(g_GameMain, groupId, groupName)
	local tblText	= g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("群聊", groupId)
	if( tblText and next(tblText) ) then
		for i, v in ipairs(tblText) do
			wnd:AddNewLineInfo(v)
		end
	end
	wnd.m_InputText:SetFocus()
	return wnd
end

--在打开的群聊窗口中根据群ID查找所要窗口
function CAssociationWnd:FindAssociationPubChatWndByGroupId(groupId)
	local tbl = g_GameMain.m_tblAssociationPubChat
	for i = 1, #tbl do
		if(tbl[i].m_GroupId == groupId) then --如果和此人聊天的窗体已经打开
			return tbl[i]
		end
	end
end

--只通过对方名字开始私聊
function CAssociationWnd:StartChatByName(memberName, str)
	Gac2Gas:GetMemberIdbyNameForPrivateChat(g_Conn, memberName, str)
end

-----------------------------------------------------------------------------------------------------
--根据输入的字符串模糊查找玩家
function CAssociationWnd:SearchPlayerByName(str)
	local tblFriendListClass = g_GameMain.m_AssociationBase.m_tblFriendListClass
	local playerTbl = {}
	if next(tblFriendListClass) then
		for i = 1, #tblFriendListClass do
			local playerInfo = tblFriendListClass[i]
			for j = 1, #playerInfo do
				if playerInfo[j].playerName then
					if string.find( string.lower(playerInfo[j].playerName), string.lower(str) ) then
						local tbl = {}
						tbl.playerId	= playerInfo[j].playerId
						tbl.playerName	= playerInfo[j].playerName
						table.insert(playerTbl, tbl)
					end
				end
			end
		end
	end
	return playerTbl
end
-----------------------------------------------------------------------------------------
--根据输入的字符串显示查找到的玩家
function CAssociationWnd:ShowPlayer(str)
	self:DestroyMenu()
	local playerTbl = self:SearchPlayerByName(str)
	if playerTbl and #playerTbl == 0 then
		return 
	end
	local rect = CFRect:new()
	self.m_SearchEdit:GetLogicRect(rect)
	local menu	= CMenuEx:new("TargetMenu", self)
	g_GameMain.m_Menu	= menu
	menu:SetPos(rect.left, rect.bottom)
	if next(playerTbl) then
		self.m_SearchEdit.tblSelect = playerTbl[1]
		for i = 1, #playerTbl do
			menu:InsertItem(playerTbl[i].playerName, self.SelectPlayer, nil, false, false, nil, self, playerTbl[i])
		end
	end
end

function CAssociationWnd:OnMenuMsg(func)
	if(func) then func(self) end
	self:DestroyMenu()
end

function CAssociationWnd:SelectPlayer(tbl)
	self:DestroyMenu()
	self.m_SearchEdit.tblSelect = tbl
	self.m_SearchEdit:SetWndText(tbl.playerName)
end

function CAssociationWnd:OpenChatWnd()
	self:DestroyMenu()
	local tbl = self.m_SearchEdit.tblSelect
	if( not ( tbl and next(tbl) ) ) then return end
	self:CreateAssociationPriChatWnd(tbl.playerId, tbl.playerName)
end

function CAssociationWnd:OnActive()
	self:DestroyMenu()
end

function CAssociationWnd:DestroyMenu()
	if(g_GameMain.m_Menu) then
		g_GameMain.m_Menu:DestroyWnd()
		g_GameMain.m_Menu = nil
	end
end

function CMenuEx:OnActive(bActive)
	--Do Nothing
end

function Gas2Gac:ReturnInitFriendGroupInfoEnd(Conn)
	g_GameMain.m_AssociationWnd.m_bNotInitBefore = false
end

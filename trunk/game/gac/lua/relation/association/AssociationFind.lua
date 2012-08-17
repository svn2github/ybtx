gac_require "relation/association/AssociationFindInc"
gac_gas_require "relation/association/AssociationMgr"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
lan_load "association/Lan_Asso_GroupType_Common"

--背景界面**********************************************************************************************
function CreateAssociationFindWnd(parent)
	local wnd = CAssociationFindWnd:new()
	wnd:Init(parent)
	wnd:ShowWnd(false)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	return wnd
end

function CAssociationFindWnd:Init(parent)
	self:CreateFromRes("AssociationFind", parent)
	self:ShowWnd( true )
	self.m_AssociationFindPlayerWnd		= self:CreateAssociationFindPlayerWnd(self.m_PlayerStatic) 		--查找好友面板
	self.m_AssociationRetFindPlayerWnd	= self:CreateAssociationRetFindPlayerWnd(self.m_PlayerStatic)	--返回查找好友面板
	self.m_AssociationFindGroupWnd		= self:CreateAssociationFindGroupWnd(self.m_GroupStatic)		--查找好友群面板
	self.m_AssociationRetFindGroupWnd	= self:CreateAssociationRetFindGroupWnd(self.m_GroupStatic)		--返回查找好友面板
	
	self.m_AssociationRetFindPlayerWnd:ShowWnd(false)
	self.m_AssociationRetFindGroupWnd:ShowWnd(false)
end

function CAssociationFindWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_PlayerStatic		= self:GetDlgChild("PlayerStatic")
	self.m_GroupStatic		= self:GetDlgChild("GroupStatic")
	self.m_FindPlayerRadio	= self:GetDlgChild("FindPlayerRadio")
	self.m_FindGroupRadio	= self:GetDlgChild("FindGroupRadio")
	self.m_FindPlayerRadio:SetCheck(true)
	self.m_GroupStatic:ShowWnd(false)
end

function CAssociationFindWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_FindPlayerRadio or Child == self.m_FindGroupRadio) then
			self:ChangeTab()
		end
	end
end

function CAssociationFindWnd:Back()
	self.m_AssociationFindPlayerWnd:ShowWnd(true)
	self.m_AssociationFindGroupWnd:ShowWnd(true)
	self.m_AssociationRetFindPlayerWnd:ShowWnd(false)
	self.m_AssociationRetFindGroupWnd:ShowWnd(false)
	
	self.m_AssociationFindPlayerWnd.m_AccuratelyFindRadio:SetCheck(true)
	self.m_AssociationFindGroupWnd.m_AccuratelyFindRadio:SetCheck(true)
	self.m_AssociationFindPlayerWnd:ChangeFindTypeShow()
	self.m_AssociationFindGroupWnd:ChangeFindTypeShow()
end

function CAssociationFindWnd:ChangeTab()
	self.m_PlayerStatic:ShowWnd( self.m_FindPlayerRadio:GetCheck() )
	self.m_GroupStatic:ShowWnd( self.m_FindGroupRadio:GetCheck() )
end

--查找玩家界面**********************************************************************************************
function CAssociationFindWnd:CreateAssociationFindPlayerWnd(parent)
	local wnd = CAssociationFindPlayerWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationFindPlayerWnd:Init(parent)

	self.m_tblClass = g_GameMain.m_DisplayCommonObj:GetPlayerClassCanUseTable()
	local tbl = {}
	tbl.classID = 0
	tbl.classDisplayName = "---"
	self.m_tblClass[0] = tbl
	
	self.m_tblScene = g_AssociationMgr:GetCanSearchSceneDisplayNameTable()
	self:CreateFromRes("AssociationFindPlayer", parent)
	self:ShowWnd( true )
end

function CAssociationFindPlayerWnd:OnChildCreated()
	self.m_SearchBtn		= self:GetDlgChild("SearchBtn")
	self.m_BaseOnNameEdit	= self:GetDlgChild("BaseOnNameEdit")
	self.m_BaseOnIdEdit		= self:GetDlgChild("BaseOnIDEdit")
	
	self.m_AccuratelyFindRadio	= self:GetDlgChild("AccuratelyFindRadio")
	self.m_ConditionFindRadio	= self:GetDlgChild("ConditionFindRadio")
	
	self.m_AccuratelyFindRadio:SetCheck(true)
	self.m_PlaceComBo			= self:GetDlgChild("PlaceComBo")
	self.m_VocationComBo		= self:GetDlgChild("VocationComBo")
	self.m_SexComBo				= self:GetDlgChild("SexComBo")
	self.m_MinLevelEdit			= self:GetDlgChild("MinLevelEdit")
	self.m_MaxLevelEdit			= self:GetDlgChild("MaxLevelEdit")
	
	----------------------------------------------------
	self.m_PlaceLable = self:GetDlgChild("PlaceLable")
--	self.m_PlaceComBo:ShowWnd(false)
--	self.m_PlaceLable:ShowWnd(false)
	----------------------------------------------------
	
	self.m_MinLevelEdit:SetWndText(1)
	self.m_MaxLevelEdit:SetWndText(80)
	self.m_MinLevelEdit:SetMaxTextLenght(3)
	self.m_MaxLevelEdit:SetMaxTextLenght(3)
	
	self.m_tblScene[0] = ""
	for i = 0, #(self.m_tblScene) do
		local sDisplayName = self.m_tblScene[i] == "" and "---" or GetSceneDispalyName(self.m_tblScene[i])
		self.m_PlaceComBo:InsertItem(sDisplayName, i, -1)
	end
	
	for i = 0, #(self.m_tblClass) do
		self.m_VocationComBo:InsertItem(self.m_tblClass[i].classDisplayName, i, -1)
	end
	
	local tbl = {[0] = "---", GetStaticTextClient(3501), GetStaticTextClient(3502)} --男, 女
	for i = 0, #tbl do
		self.m_SexComBo:InsertItem(tbl[i], i, -1)
	end
	
	self:ChangeFindTypeShow()
end

--消息
function CAssociationFindPlayerWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_SearchBtn) then
			self:SearchPlayer()
		elseif(Child == self.m_AccuratelyFindRadio or Child == self.m_ConditionFindRadio) then
			self:ChangeFindTypeShow()
		end
	end
end

function CAssociationFindPlayerWnd:ChangeFindTypeShow()
	local bFlag = self.m_AccuratelyFindRadio:GetCheck()
	self.m_BaseOnNameEdit:EnableWnd(bFlag)
	self.m_BaseOnIdEdit:EnableWnd(bFlag)
	self.m_PlaceComBo:EnableWnd(not bFlag)
	self.m_VocationComBo:EnableWnd(not bFlag)
	self.m_SexComBo:EnableWnd(not bFlag)
	self.m_MinLevelEdit:EnableWnd(not bFlag)
	self.m_MaxLevelEdit:EnableWnd(not bFlag)
	
	self.m_BaseOnNameEdit:SetWndText("")
	self.m_BaseOnIdEdit:SetWndText("")
	self.m_PlaceComBo:SetSel(0)
	self.m_VocationComBo:SetSel(0)
	self.m_SexComBo:SetSel(0)
end

--查找玩家
function CAssociationFindPlayerWnd:SearchPlayer()
	g_GameMain.m_AssociationFindWnd.m_AssociationRetFindPlayerWnd.m_tblBeFoundPlayerInfo = {} --清空上次查找的玩家列表
	local textFltMgr = CTextFilterMgr:new()
	local bFlag = self.m_AccuratelyFindRadio:GetCheck()
	if(bFlag) then
		local name	= textFltMgr:RemoveTab1(self.m_BaseOnNameEdit:GetWndText())
		local id	= textFltMgr:RemoveTab1(self.m_BaseOnIdEdit:GetWndText())
		if("" == id) then id = 0 end
		id = tonumber(id)
		if("" == name) then
			if(0 == id or not IsNumber(id)) then MsgClient(5004) return end
		end
		Gac2Gas:SearchPlayerAccurately(g_Conn, name, id)
	else
		local place		= self.m_tblScene[self.m_PlaceComBo:GetCurrentSel()]
		local vocation	= self.m_tblClass[self.m_VocationComBo:GetCurrentSel()].classID
		local sex		= self.m_SexComBo:GetCurrentSel()
		local minLevel	= textFltMgr:RemoveTab1(self.m_MinLevelEdit:GetWndText())
		local maxLevel	= textFltMgr:RemoveTab1(self.m_MaxLevelEdit:GetWndText())
		minLevel = tonumber(minLevel)
		maxLevel = tonumber(maxLevel)
		Gac2Gas:SearchPlayerCondition(g_Conn, vocation, sex, minLevel, maxLevel, place)
	end
	self:ShowWnd(false)
end

--查找群界面**********************************************************************************************
function CAssociationFindWnd:CreateAssociationFindGroupWnd(parent)
	local wnd = CAssociationFindGroupWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationFindGroupWnd:Init(parent)
	self:CreateFromRes("AssociationFindGroup", parent)
	self:ShowWnd( true )
end

function CAssociationFindGroupWnd:OnChildCreated()
	self.m_SearchBtn			= self:GetDlgChild("SearchBtn")
	self.m_BaseOnNameEdit		= self:GetDlgChild("BaseOnNameEdit")
	self.m_BaseOnIdEdit			= self:GetDlgChild("BaseOnIDEdit")
	self.m_AccuratelyFindRadio	= self:GetDlgChild("AccuratelyFindRadio")
	self.m_ConditionFindRadio	= self:GetDlgChild("ConditionFindRadio")
	self.m_KeywordEdit			= self:GetDlgChild("KeywordEdit")
	self.m_GroupTypeComBo		= self:GetDlgChild("GroupTypeComBo")
	self.m_AccuratelyFindRadio:SetCheck(true)
	
	for i, v in ipairs( Lan_Asso_GroupType_Common:GetKeys() ) do
		self.m_GroupTypeComBo:InsertItem(Lan_Asso_GroupType_Common(v, "DisplayName"), v, -1)
	end
	self:ChangeFindTypeShow()
end

--消息
function CAssociationFindGroupWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_SearchBtn) then
			self:SearchGroup()
		elseif(Child == self.m_AccuratelyFindRadio or Child == self.m_ConditionFindRadio) then
			self:ChangeFindTypeShow()
		end
	end
end

function CAssociationFindGroupWnd:ChangeFindTypeShow()
	local bFlag = self.m_AccuratelyFindRadio:GetCheck()
	self.m_BaseOnNameEdit:EnableWnd(bFlag)
	self.m_BaseOnIdEdit:EnableWnd(bFlag)
	self.m_KeywordEdit:EnableWnd(not bFlag)
	self.m_GroupTypeComBo:EnableWnd(not bFlag)
	
	self.m_BaseOnNameEdit:SetWndText("")
	self.m_BaseOnIdEdit:SetWndText("")
	self.m_GroupTypeComBo:SetSel(0)
end

--查找群
function CAssociationFindGroupWnd:SearchGroup()
	g_GameMain.m_AssociationFindWnd.m_AssociationRetFindGroupWnd.m_tblBeFoundGroupInfo = {} --清空上次查找的群列表
	local textFltMgr = CTextFilterMgr:new()
	local bFlag = self.m_AccuratelyFindRadio:GetCheck()
	if(bFlag) then
		local name	= textFltMgr:RemoveTab1(self.m_BaseOnNameEdit:GetWndText())
		local id	= textFltMgr:RemoveTab1(self.m_BaseOnIdEdit:GetWndText())
		if("" == id) then id = 0 end
		id = tonumber(id)
		if("" == name) then
			if(0 == id or not IsNumber(id)) then MsgClient(5004) return end
		end
		Gac2Gas:SearchGroupAccurately(g_Conn, name, id)
	else
		local keyWord	= self.m_KeywordEdit:GetWndText()
		local type		= self.m_GroupTypeComBo:GetCurrentSel()
		Gac2Gas:SearchGroupCondition(g_Conn, keyWord, type)
	end
	self:ShowWnd(false)
end

--查找玩家的返回界面************************************************************************************************
function CAssociationFindWnd:CreateAssociationRetFindPlayerWnd(parent)
	local wnd = CAssociationRetFindPlayerWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationRetFindPlayerWnd:Init(parent)
	self.m_tblBeFoundPlayerInfo	= {}
	self.m_tblAssocRetFindItem	= {}
	self:CreateFromRes("AssociationRetFindPlayer", parent)
end

function CAssociationRetFindPlayerWnd:OnChildCreated()
	self.m_List			= self:GetDlgChild("List")
	self.m_AddFriendBtn	= self:GetDlgChild("AddFriendBtn")
	self.m_BackBtn		= self:GetDlgChild("BackBtn")
end

--消息
function CAssociationRetFindPlayerWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_AddFriendBtn) then
			local itemIndex = self.m_List:GetSelectItem(-1) + 1
			if(itemIndex < 1) then MsgClient(5005) return end
			
			local tbl = self.m_tblBeFoundPlayerInfo[itemIndex]
			local tblInfo	= {}
			tblInfo.id		= tbl[1]
			tblInfo.name	= tbl[2]
			g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, false)
			g_GameMain.m_AssociationFindWnd:ShowWnd(false)
		elseif(Child == self.m_BackBtn) then
			self:ShowWnd(false)
			g_GameMain.m_AssociationFindWnd.m_AssociationFindPlayerWnd:ShowWnd(true)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		
	end
end

--显示查找到的玩家
function CAssociationRetFindPlayerWnd:DrawPlayerList()
	self.m_List:DeleteAllItem()
	self.m_tblAssocRetFindItem = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	local tbl = self.m_tblBeFoundPlayerInfo
	for i = 1, #tbl do
		self.m_List:InsertItem(i-1, 20)
		local item = self.m_List:GetSubItem(i-1, 0)
		local itemWnd = g_GameMain.m_AssociationFindWnd:CreateAssocRetFindItemWnd(item)
		itemWnd:ShowBeSearchInfo(tbl[i][1], tbl[i][2], tbl[i][3])
		self.m_tblAssocRetFindItem[i] = itemWnd
	end
end

--查找好友群的返回界面************************************************************************************************
function CAssociationFindWnd:CreateAssociationRetFindGroupWnd(parent)
	local wnd = CAssociationRetFindGroupWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationRetFindGroupWnd:Init(parent)
	self.m_tblBeFoundGroupInfo	= {}
	self.m_tblAssocRetFindItem	= {}
	self:CreateFromRes("AssociationRetFindGroup", parent)
end

function CAssociationRetFindGroupWnd:OnChildCreated()
	self.m_List			= self:GetDlgChild("List")
	self.m_JoinBtn		= self:GetDlgChild("JoinBtn")
	self.m_BackBtn		= self:GetDlgChild("BackBtn")
end

--消息
function CAssociationRetFindGroupWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_JoinBtn) then
			local itemIndex = self.m_List:GetSelectItem(-1) + 1
			if(itemIndex < 1) then MsgClient(5006) return end
			local classId = self.m_tblBeFoundGroupInfo[itemIndex][1]
			Gac2Gas:RequestJoinIntoFriendGroup(g_Conn, classId)
			g_GameMain.m_AssociationFindWnd:ShowWnd(false)
		elseif(Child == self.m_BackBtn) then
			self:ShowWnd(false)
			g_GameMain.m_AssociationFindWnd.m_AssociationFindGroupWnd:ShowWnd(true)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		
	end
end

--显示查找到的群
function CAssociationRetFindGroupWnd:DrawGroupList()
	self.m_List:DeleteAllItem()
	self.m_tblAssocRetFindItem = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	local tbl = self.m_tblBeFoundGroupInfo
	for i = 1, #tbl do
		self.m_List:InsertItem(i-1, 20)
		local item = self.m_List:GetSubItem(i-1, 0)
		local itemWnd = g_GameMain.m_AssociationFindWnd:CreateAssocRetFindItemWnd(item)
		itemWnd:ShowBeSearchInfo(tbl[i][1], tbl[i][2], tbl[i][3])
		self.m_tblAssocRetFindItem[i] = itemWnd
	end
end
----------------------------------------------------------------
function CAssociationFindWnd:CreateAssocRetFindItemWnd(parent)
	local wnd = CAssociationRetFindListItem:new()
	wnd:CreateFromRes("AssociationRetFindListItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CAssociationRetFindListItem:OnChildCreated()
	self.m_IdLable		= self:GetDlgChild("IdLable")
	self.m_NameLable	= self:GetDlgChild("NameLable")
	self.m_TempLable	= self:GetDlgChild("TempLable")
end

function CAssociationRetFindListItem:ShowBeSearchInfo(id, name, level)
	self.m_IdLable:SetWndText(id)
	self.m_NameLable:SetWndText(name)
	self.m_TempLable:SetWndText(level)
end

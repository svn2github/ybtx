gac_require "message/message_list/MsgListWndInc"
gac_require "framework/texture_mgr/DynImageMgr"
cfg_load "image_res/MsgListImage_Client"
lan_load "message/Lan_SystemFriendMessage_Common"

local EMsgItemType = {}
EMsgItemType.EMsgItemType_PriChat		= "私聊"
EMsgItemType.EMsgItemType_PubChat		= "群聊"
EMsgItemType.EMsgItemType_FriendInv		= "好友邀请"
EMsgItemType.EMsgItemType_FriendGroInv	= "好友群邀请"
EMsgItemType.EMsgItemType_FriendGroApp	= "好友群申请"
EMsgItemType.EMsgItemType_TongInv		= "佣兵小队邀请"
EMsgItemType.EMsgItemType_TeamInv		= "组队邀请"
EMsgItemType.EMsgItemType_TeamApp		= "组队申请"
EMsgItemType.EMsgItemType_EMail			= "邮件"
EMsgItemType.EMsgItemType_ArmyCorpsInv	= "佣兵团邀请"
EMsgItemType.EMsgItemType_TongReq       = "佣兵小队申请入队"

local fl_ItemMaxNum = 8
local fl_ItemHeight = 20

--回复被邀请加入佣兵小队的MessageBox的回调函数
local function TongBeInviteMBoxClick(inviterId, index)
	if(index == 1) then --确定
		Gac2Gas:ResponseBeInviteToTong(g_Conn, inviterId, true)
	elseif(index == 2) then --拒绝
		Gac2Gas:ResponseBeInviteToTong(g_Conn, inviterId, false)
	end
	g_GameMain.m_MsgBox = nil
	return true
end

--回复被邀请加为好友的MessageBox的回调函数
local function AssoBeInviteMBoxClick(context, index)
	if(index == 1) then --确定
		local tblInfo	= {}
		tblInfo.id		= context.nId
		tblInfo.name	= context.sName
		tblInfo.class	= context.nJoinClassId
		g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, true) --true是被动接受邀请
	elseif(index == 2) then --拒绝
		Gac2Gas:RefuseBeAddFriend(g_Conn, context.nId) --拒绝好友邀请
	end
	return true
end

--回复申请加入好友群的MessageBox的回调函数
local function AssoJoinGroupMBoxClick(tblInfo, index)
	Gac2Gas:ResponseRequestJoinGroup(g_Conn, tblInfo.nId, tblInfo.nGroupId, index)
	return true
end

--回应被邀请加入好友群的MessageBox回调函数
local function AssoResbInviteToGroupMBoxClick(tblInfo, index)
	Gac2Gas:ResponseInviteToGroup(g_Conn, tblInfo.nGroupId, tblInfo.nId, index)
	return true
end

--回复被邀请加入佣兵团的MessageBox的回调函数
local function BeInviteMBoxClick(uInviterID,uButton)
	if uButton == MB_BtnOK then
		Gac2Gas:ResponseBeInviteToArmyCorps(g_Conn, uInviterID, true)
	elseif(uButton == MB_BtnCancel) then --拒绝
		Gac2Gas:ResponseBeInviteToArmyCorps(g_Conn, uInviterID, false)
	end
	g_GameMain.m_MsgBox = nil
	return true
end

function CreateMsgListParentWnd(parent)
	local wnd = CMsgListParentWnd:new()
	wnd:CreateFromRes("MsgListMinimum", parent)
	return wnd
end

function CMsgListParentWnd:OnChildCreated()
	self.m_ShowBtn		= self:GetDlgChild("ShowBtn")
	self.m_HideBtn		= self:GetDlgChild("HideBtn")
	self.m_MsgListWnd	= self:CreateMsgListWnd(self)
	self.m_ShowBtn:ShowWnd(false)
end

function CMsgListParentWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ShowBtn) then
			self:ShowMsgListWnd(true)
		elseif(Child == self.m_HideBtn) then
			self:ShowMsgListWnd(false)
		end
	end
end

function CMsgListParentWnd:ShowMsgListWnd(bFlag)
	self.m_MsgListWnd:ShowWnd(bFlag)
	self.m_HideBtn:ShowWnd(bFlag)
	self.m_ShowBtn:ShowWnd(not bFlag)
end

-------------------------------------------------------------------------------------------------
function CMsgListParentWnd:CreateMsgListWnd(parent)
	local wnd = CMsgListWnd:new()
	wnd:Init(parent)
	wnd:ShowWnd(true)
	return wnd
end

function CMsgListWnd:Init(parent)
	self:CreateFromRes("MsgList", parent)
	self.m_tblInfoAll	= {}
end

function CMsgListWnd:OnChildCreated()
	self.m_UpBtn	= self:GetDlgChild("UpBtn")
	self.m_DownBtn	= self:GetDlgChild("DownBtn")
	self.m_List		= self:GetDlgChild("List")
	self.m_VerticalScroll = self.m_List:GetVerticalScroll()
end

function CMsgListWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_UpBtn) then
			local pos = self.m_VerticalScroll:GetPos()
			self.m_VerticalScroll:SetPos(pos - 1)
			self:EnableUpAndDownBottomBtn()
		elseif(Child == self.m_DownBtn) then
			local pos = self.m_VerticalScroll:GetPos()
			self.m_VerticalScroll:SetPos(pos + 1)
			self:EnableUpAndDownBottomBtn()
		end
	end
end

function CMsgListWnd:ResetRect()
	local height	= math.min( #(self.m_tblItemWnd), fl_ItemMaxNum ) * fl_ItemHeight
	local bFlag		= (0 == height)
	g_GameMain.m_MsgListParentWnd:ShowWnd(not bFlag)
	if(bFlag) then return end
	local rt = CFRect:new()
	self:GetLogicRect(rt)
	rt.top = rt.bottom - height
	self:SetLogicRect(rt)
	self.m_List:SetLogicRect(rt)
	self:EnableUpAndDownBottomBtn()
end

function CMsgListWnd:EnableUpAndDownBottomBtn()
	self.m_UpBtn:ShowWnd(true)
	self.m_DownBtn:ShowWnd(true)
	if( self.m_VerticalScroll:Istop() ) then
		self.m_UpBtn:ShowWnd(false)
	end
	if(self.m_VerticalScroll:Isbottom()) then
		self.m_DownBtn:ShowWnd(false)
	end
end

function CMsgListWnd:DrawList()
	self.m_List:DeleteAllItem()
	self.m_tblItemWnd = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	local nFlashNum = 0
	local len = #(self.m_tblInfoAll)
	for i = len, 1, -1 do
		self.m_List:InsertItem(len-i, fl_ItemHeight)
		local item = self.m_List:GetSubItem(len-i, 0)
		local itemWnd = self:CreateListItem(item, self.m_tblInfoAll[i])
		table.insert(self.m_tblItemWnd, itemWnd)
		
		if(itemWnd.m_tblInfo.bFlash) then
			nFlashNum = nFlashNum + 1
		end
	end
	if(nFlashNum <= 0) then g_GameMain.m_MsgListParentWnd.m_ShowBtn:DelFlashAttention() end
	self:ResetRect()
end

-----------------------------------------------------------------------------
function CMsgListWnd:CloseOneType(sType)
	for i, v in ipairs(self.m_tblInfoAll) do
		if(v.sType == sType) then
			self:DeleteOneItem(i)
		end
	end
end

function CMsgListWnd:CloseOne(sType, nId)
	for i, v in ipairs(self.m_tblInfoAll) do
		if(v.nId == nId and v.sType == sType) then
			self:DeleteOneItem(i)
			return v.tblText
		end
	end
end

function CMsgListWnd:DeleteOneItem(nIndex)
	table.remove(self.m_tblInfoAll, nIndex)
	self:DrawList()
end

function CMsgListWnd:FlashOneItemByTypeAndId(sType, nId)
	for i, v in ipairs(self.m_tblInfoAll) do
		if(v.nId == nId and v.sType == sType) then
			self:FlashOneItemByIndex(i)
			break
		end
	end
end

function CMsgListWnd:FlashOneItemByIndex(nIndex)
	self.m_tblInfoAll[nIndex].bFlash = true
	local nItemIndex = #(self.m_tblInfoAll) - nIndex + 1
	self.m_tblItemWnd[nItemIndex].m_FlashBtn:AddFlashInfoByNameEx("MsgListFlash", true)
	
	g_GameMain.m_MsgListParentWnd.m_ShowBtn:DelFlashAttention()
	g_GameMain.m_MsgListParentWnd.m_ShowBtn:AddFlashInfoByName("MsgListShowBtn")
end

function CMsgListWnd:SetOneItemTextByIndex(nIndex, sText)
	local nItemIndex = #(self.m_tblInfoAll) - nIndex + 1
	self.m_tblItemWnd[nItemIndex].m_ShowBar.m_Name:SetWndText(sText)
end

-----------------------------------------------------------------------------
function CMsgListWnd:CreatePrivateChatMinimum(nId, sName, sText, wnd)
	local tbl = {}
	tbl.nId		= nId
	tbl.sName	= sName
	tbl.wnd		= wnd
	tbl.sType	= EMsgItemType.EMsgItemType_PriChat
	tbl.tblText	= {}
	
	table.insert(tbl.tblText, sText)
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreatePublicChatMinimum(nId, sText, wnd)
	local base			= g_GameMain.m_AssociationBase
	local tblGroupIndex	= base.m_tblFriendGroupIndex
	local sGroupName	= base.m_tblFriendGroup[tblGroupIndex[nId]][1][1]
	
	local tbl	= {}
	tbl.nId		= nId
	tbl.sName	= sGroupName
	tbl.wnd		= wnd
	tbl.sType	= EMsgItemType.EMsgItemType_PubChat
	tbl.tblText	= {}
	
	table.insert(tbl.tblText, sText)
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateTongInvMsgMinimum(nInviterId, sInviterName, sTongName)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_TongInv and v.nId == nInviterId and v.sTongName == sTongName ) then
			return
		end
	end
	
	local tbl		= {}
	tbl.nId			= nInviterId
	tbl.sName		= sInviterName
	tbl.sTongName	= sTongName
	tbl.sType		= EMsgItemType.EMsgItemType_TongInv
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateArmyCorpsInvMsgMinimum(nInviterId, sInviterName, sArmyCorpsName)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_ArmyCorpsInv and v.nId == nInviterId and v.sArmyCorpsName == sArmyCorpsName ) then
			return
		end
	end
	
	local tbl		= {}
	tbl.nId							= nInviterId
	tbl.sName						= sInviterName
	tbl.sArmyCorpsName 	= sArmyCorpsName
	tbl.sType						= EMsgItemType.EMsgItemType_ArmyCorpsInv
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateFriendInvMsgMinimum(nInviterId, sInviterName, nJoinClassId)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_FriendInv and v.nId == nInviterId ) then
			return
		end
	end
	
	local tbl			= {}
	tbl.nId				= nInviterId
	tbl.sName			= sInviterName
	tbl.nJoinClassId	= nJoinClassId
	tbl.sType			= EMsgItemType.EMsgItemType_FriendInv
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateFriendGroInvMsgMinimum(nInviterId, sInviterName, nGroupId, sGroupName)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_FriendGroInv and v.nId == nInviterId and v.nGroupId == nGroupId ) then
			return
		end
	end
	
	local tbl		= {}
	tbl.nId			= nInviterId
	tbl.sName		= sInviterName
	tbl.nGroupId	= nGroupId
	tbl.sGroupName	= sGroupName
	tbl.sType		= EMsgItemType.EMsgItemType_FriendGroInv
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateFriendGroAppMsgMinimum(nInviterId, sInviterName, nGroupId)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_FriendGroApp and v.nId == nInviterId and v.nGroupId == nGroupId ) then
			return
		end
	end
	
	local tbl		= {}
	tbl.nId			= nInviterId
	tbl.sName		= sInviterName
	tbl.nGroupId	= nGroupId
	tbl.sType		= EMsgItemType.EMsgItemType_FriendGroApp
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateTeamInvMsgMinimum(nInviterId, playerlev, class, sInviterName, eAM)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_TeamInv and v.nId == nInviterId ) then
			return
		end
	end
	
	local tbl	= {}
	tbl.nId		= nInviterId
	tbl.playerlev	= playerlev
	tbl.class	= class
	tbl.sName	= sInviterName
	tbl.eAM		= eAM
	tbl.sType	= EMsgItemType.EMsgItemType_TeamInv
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateTeamAppMsgMinimum(nApplicantID, playerlev, class, sApplicantName)
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_TeamApp and v.nId == nApplicantID ) then
			return
		end
	end
	
	local tbl	= {}
	tbl.nId		= nApplicantID
	tbl.playerlev	= playerlev
	tbl.class	= class
	tbl.sName	= sApplicantName
	tbl.sType	= EMsgItemType.EMsgItemType_TeamApp
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateEMailMsgMinimum(nMsgID)
	local text = Lan_SystemFriendMessage_Common(nMsgID, "Content")
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_EMail ) then
			v.nCount	= v.nCount + 1
			v.sName		= GetStaticTextClient(1147, v.nCount)
			v.sText		= v.sText .. os.date( "%X", os.time() ) .. "#r" .. text .. "#r"
			self:SetOneItemTextByIndex(i, v.sName)
			return
		end
	end
	
	local tbl	= {}
	tbl.nCount	= 1
	tbl.sName	= GetStaticTextClient(1147, tbl.nCount)
	tbl.sText	= os.date( "%X", os.time() ) .. "#r" .. text .. "#r"
	tbl.sType	= EMsgItemType.EMsgItemType_EMail
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:CreateTongReqMsgMinimum(nMsgID)
	local text = Lan_SystemFriendMessage_Common(nMsgID, "Content")
	for i, v in ipairs(self.m_tblInfoAll) do
		if( v.sType == EMsgItemType.EMsgItemType_TongReq ) then
			v.nCount	= v.nCount + 1
			v.sName		= GetStaticTextClient(9131, v.nCount)
			v.sText		= v.sText .. os.date( "%X", os.time() ) .. "#r" .. text .. "#r"
			self:SetOneItemTextByIndex(i, v.sName)
			return
		end
	end
	
	local tbl	= {}
	tbl.nCount	= 1
	tbl.sName	= GetStaticTextClient(9131, tbl.nCount)
	tbl.sText	= os.date( "%X", os.time() ) .. "#r" .. text .. "#r"
	tbl.sType	= EMsgItemType.EMsgItemType_TongReq
	self:InsertMinimumItem(tbl)
end

function CMsgListWnd:InsertMinimumItem(tblInfo)
	--PlayCue("WndOp_com03")
	table.insert(self.m_tblInfoAll, tblInfo)
	self:DrawList()
	
	if(tblInfo.sType == EMsgItemType.EMsgItemType_PriChat or tblInfo.sType == EMsgItemType.EMsgItemType_PubChat) then
		if( next(tblInfo.tblText) ) then
			self:FlashOneItemByIndex(#(self.m_tblItemWnd))
		end
	elseif(	tblInfo.sType == EMsgItemType.EMsgItemType_TongInv		or
			tblInfo.sType == EMsgItemType.EMsgItemType_FriendInv	or 
			tblInfo.sType == EMsgItemType.EMsgItemType_FriendGroInv	or
			tblInfo.sType == EMsgItemType.EMsgItemType_FriendGroApp	or
			tblInfo.sType == EMsgItemType.EMsgItemType_TeamInv		or
			tblInfo.sType == EMsgItemType.EMsgItemType_TeamApp		or
			tblInfo.sType == EMsgItemType.EMsgItemType_EMail			or
			tblInfo.sType == EMsgItemType.EMsgItemType_TongReq			or
			tblInfo.sType == EMsgItemType.EMsgItemType_ArmyCorpsInv) then
		self:FlashOneItemByIndex(#(self.m_tblItemWnd))
	end
end

-----------------------------------------------------------------------------
function CMsgListWnd:InsertChatMsg(sType, nId, sCharName, sText, nTime)
	local sTime = nTime and nTime or os.date("%X")
	local sNewText = g_ColorMgr:GetColor("好友聊天名") .. sCharName .. "(".. sTime .. "):#r" .. g_ColorMgr:GetColor("好友聊天内容") .. sText
	for i, v in ipairs(self.m_tblInfoAll) do
		if(v.nId == nId and v.sType == sType) then
			table.insert(v.tblText, sNewText)
			self:FlashOneItemByIndex(i)
			return
		end
	end
	if(sType == EMsgItemType.EMsgItemType_PriChat) then
		self:CreatePrivateChatMinimum(nId, sCharName, sNewText)
	elseif(sType == EMsgItemType.EMsgItemType_PubChat) then
		self:CreatePublicChatMinimum(nId, sNewText)
	end
end

--************************************************************************
--------------------------------------------------------------------------
--************************************************************************
function CMsgListWnd:CreateListItem(parent, tblInfo)
	local wnd = CMsgListItem:new()
	wnd:Init(parent, tblInfo)
	wnd:ShowWnd(true)
	return wnd
end

function CMsgListItem:Init(parent, tblInfo)
	self:CreateFromRes("MsgListItem", parent)
	self.m_tblInfo = tblInfo
	self.m_ShowBar.m_Name:SetWndText(tblInfo.sName)
	
	local rt = CFRect:new()
	local imagePath = MsgListImage_Client(tblInfo.sType, "ImagePathEnable")
	rt.left		= MsgListImage_Client(tblInfo.sType, "Left")
	rt.right	= MsgListImage_Client(tblInfo.sType, "Right")
	rt.top		= MsgListImage_Client(tblInfo.sType, "Top")
	rt.bottom	= MsgListImage_Client(tblInfo.sType, "Bottom")
	
	g_DynImageMgr:AddImageNeedCut(self.m_ShowBar.m_Icon, imagePath, IP_ENABLE, rt, CFPos:new())
	
	if(tblInfo.bFlash) then
		self.m_FlashBtn:AddFlashInfoByNameEx("MsgListFlash", true)
	end
end

function CMsgListItem:OnChildCreated()
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_FlashBtn	= self:GetDlgChild("FlashBtn")
	self.m_ShowBar	= self:CreateShowBar(self.m_FlashBtn)
end

function CMsgListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:Close()
		elseif(Child == self.m_FlashBtn) then
			if(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_PriChat) then
				g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(self.m_tblInfo.nId, self.m_tblInfo.sName)
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_PubChat) then
				g_GameMain.m_AssociationWnd:CreateAssociationPubChatWnd(self.m_tblInfo.nId, self.m_tblInfo.sName)
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_TongInv) then
				local string = MsgBoxMsg(17001, self.m_tblInfo.sName, self.m_tblInfo.sTongName)
				MessageBox(g_GameMain, string, BitOr(MB_BtnOK, MB_BtnCancel), TongBeInviteMBoxClick, self.m_tblInfo.nId)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_FriendInv) then
				local string = MsgBoxMsg(15001, self.m_tblInfo.sName)
				MessageBox(g_GameMain, string, BitOr(MB_BtnOK, MB_BtnCancel), AssoBeInviteMBoxClick, self.m_tblInfo)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_FriendGroInv) then
				local string = MsgBoxMsg(15003, self.m_tblInfo.sName, self.m_tblInfo.sGroupName)
				MessageBox(g_GameMain, string, BitOr(MB_BtnOK, MB_BtnCancel), AssoResbInviteToGroupMBoxClick, self.m_tblInfo)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_FriendGroApp) then
				local base			= g_GameMain.m_AssociationBase
				local tblGroupIndex	= base.m_tblFriendGroupIndex
				local groupName		= base.m_tblFriendGroup[tblGroupIndex[self.m_tblInfo.nGroupId]][1][1]
				local string		= MsgBoxMsg(15002, self.m_tblInfo.sName, groupName)
				MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), AssoJoinGroupMBoxClick, self.m_tblInfo)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_TeamInv) then
				g_GameMain.m_TeamBase:ShowMsgBox(self.m_tblInfo.nId, self.m_tblInfo.playerlev, self.m_tblInfo.class, self.m_tblInfo.sName, self.m_tblInfo.eAM)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_TeamApp) then
				g_GameMain.m_TeamBase:ShowMsgBox(self.m_tblInfo.nId, self.m_tblInfo.playerlev, self.m_tblInfo.class, self.m_tblInfo.sName)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_EMail) then
				g_GameMain.m_SystemFriendMsgBox:OpenPanel(self.m_tblInfo.sText)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_TongReq) then
				g_GameMain.m_SystemFriendMsgBox:OpenPanel(self.m_tblInfo.sText)
				self:Close()
			elseif(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_ArmyCorpsInv) then
				local ShowInfo = MsgBoxMsg(16502, self.m_tblInfo.sName, self.m_tblInfo.sArmyCorpsName)
				MessageBox(g_GameMain, ShowInfo, BitOr(MB_BtnOK, MB_BtnCancel), BeInviteMBoxClick, self.m_tblInfo.nId)
				self:Close()
			end
		end
	end
end

function CMsgListItem:Close()
	local wnd = g_GameMain.m_MsgListParentWnd.m_MsgListWnd
	for i, v in ipairs(wnd.m_tblItemWnd) do
		if(v == self) then
			if(self.m_tblInfo.sType == EMsgItemType.EMsgItemType_PriChat or self.m_tblInfo.sType == EMsgItemType.EMsgItemType_PubChat) then
				if(self.m_tblInfo.wnd) then
					self.m_tblInfo.wnd:VirtualExcludeWndClosed()
				end
			end
			wnd:DeleteOneItem(#(wnd.m_tblItemWnd) - i + 1)
			break
		end
	end
end

function CMsgListItem:CreateShowBar(parent)
	local wnd = CMsgListItemShowBar:new()
	wnd:CreateFromRes("MsgListItemShowBar", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CMsgListItemShowBar:OnChildCreated()
	self.m_Icon	= self:GetDlgChild("Icon")
	self.m_Name	= self:GetDlgChild("Name")
end


------------------------------------------------------------------------------------------

gac_require "relation/association/AssociationFriendGroupInc"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
lan_load "association/Lan_Asso_GroupType_Common"

function CreateAssociationFriendGroupCreateWnd(parent)
	local wnd = CAssociationCreateFriendGroupWnd:new()
	wnd:Init(parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
	return wnd
end

function CAssociationCreateFriendGroupWnd:Init(parent)
	self:CreateFromRes("AssociationCreateGroup", parent)
	self:ShowWnd( true )
end

function CAssociationCreateFriendGroupWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_CancelBtn		= self:GetDlgChild("CancelBtn")
	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_GroupNameEdit	= self:GetDlgChild("GroupNameEdit")
	self.m_GroupTypeCB		= self:GetDlgChild("GroupTypeCB")
	self.m_GroupLableEdit	= self:GetDlgChild("GroupLableEdit")
	self.m_GroupDeclareEdit	= self:GetDlgChild("GroupDeclareEdit")
	
	self.m_GroupNameEdit:SetMaxTextLenght(8)
	self.m_GroupLableEdit:SetMaxTextLenght(8)
	self.m_GroupDeclareEdit:SetMaxTextLenght(1024)
	
	for i, v in ipairs( Lan_Asso_GroupType_Common:GetKeys() ) do
		self.m_GroupTypeCB:InsertItem(Lan_Asso_GroupType_Common(v, "DisplayName"), v, -1)
	end
	self.m_GroupTypeCB:SetSel(1)
end

--消息
function CAssociationCreateFriendGroupWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn or Child == self.m_CancelBtn) then
			self:ShowWnd(false)
			self:DestroyWnd()
		elseif(Child == self.m_OkBtn) then
			self:CreateFriendGroup()
		end
	end
end

function CAssociationCreateFriendGroupWnd:CreateFriendGroup()
	local textFltMgr	= CTextFilterMgr:new()
	local groupType		= self.m_GroupTypeCB:GetCurrentSel() + 1
	
	local groupName		= self.m_GroupNameEdit:GetWndText()
	local groupLable	= self.m_GroupLableEdit:GetWndText()
	local groupDeclare	= self.m_GroupDeclareEdit:GetWndText()
	
	if( not textFltMgr:IsValidName(groupName) ) then
		MsgClient(130056)
		return
	elseif( not textFltMgr:IsValidName(groupLable) ) then
		MsgClient(130057)
		return
	elseif( not textFltMgr:IsValidMsg(groupDeclare) ) then
		MsgClient(130058)
		return
	end
	
	groupName		= textFltMgr:RemoveTab1(groupName)
	groupLable		= textFltMgr:RemoveTab1(groupLable)
	groupDeclare	= textFltMgr:RemoveTab1(groupDeclare)
	
	if("" == groupName) then
		MsgClient(5007)
	elseif("" == groupDeclare) then
		MsgClient(5008)
	else
		g_GameMain.m_AssociationBase.RetCreateFriendGroup = function (_, groupId)
			local base	= g_GameMain.m_AssociationBase
			local tbl	= base.m_tblFriendGroup
			local wnd	= g_GameMain.m_AssociationWnd.m_AssociationListWnd
			table.insert(tbl, {{groupName, groupId, groupType, groupLable, groupDeclare, 1}, {{g_MainPlayer.m_Properties:GetCharName(), g_MainPlayer.m_Properties:GetCharID(), 1, 1}}})
			base:ResetGroupIndex()
			if(2 == wnd.m_ListPage) then --好友群列表打开着
				wnd.m_tblList[2]:SetFriendGroupList()
			end
		end
		Gac2Gas:CreateFriendGroup(g_Conn, groupName, groupType, groupLable, groupDeclare)
		self:ShowWnd(false)
		self:DestroyWnd()
	end
end
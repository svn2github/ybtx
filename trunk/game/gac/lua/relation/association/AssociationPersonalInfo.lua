gac_require "relation/association/AssociationPersonalInfoInc"
lan_load "association/Lan_Asso_BodyShape_Common"
lan_load "association/Lan_Asso_Interest_Common"
lan_load "association/Lan_Asso_MakeFriendState_Common"
lan_load "association/Lan_Asso_Mood_Common"
lan_load "association/Lan_Asso_Personality_Common"
lan_load "association/Lan_Asso_Style_Common"

--***********************************************************************************************************
-------编辑个人信息窗口------
--***********************************************************************************************************
function CreateAssociationPersonalityInfoEditWnd(parent)
	local wnd = CAssociationPersonalInfoEditWnd:new()
	wnd:Init(parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CAssociationPersonalInfoEditWnd:Init(parent)
	self.m_TextFilter			= CTextFilterMgr:new()
	self.m_sShowSen				= ""
	self:CreateFromRes("AssociationPersonalInfo", parent)
	self.m_NameLable:SetWndText(g_MainPlayer.m_sName)
	self.m_IdLable:SetWndText("ID:" .. g_MainPlayer.m_uID)
end

function CAssociationPersonalInfoEditWnd:OnChildCreated()
	self.m_XBtn					= self:GetDlgChild("XBtn")
	self.m_OkBtn				= self:GetDlgChild("OkBtn")
	self.m_CancelBtn			= self:GetDlgChild("CancelBtn")
	self.m_NameLable			= self:GetDlgChild("NameLable")
	self.m_IdLable				= self:GetDlgChild("IdLable")
	self.m_MoodCB				= self:GetDlgChild("MoodCB")
	self.m_ShowSentenceEdit		= self:GetDlgChild("ShowSentenceEdit")
	self.m_BodyShapeCB			= self:GetDlgChild("BodyShapeCB")
	self.m_PersonalityCB		= self:GetDlgChild("PersonalityCB")
	self.m_MakeFriendsStateCB	= self:GetDlgChild("MakeFriendsStateCB")
	self.m_StyleCB				= self:GetDlgChild("StyleCB")
	self.m_DetailEdit			= self:GetDlgChild("DetailEdit")
	
	self.m_tblCheck = {}
	for i = 1, 9 do
		self.m_tblCheck[i] = self:GetDlgChild("Check" .. i)
	end
	-----------------------------------------------------------
	for i, v in ipairs(Lan_Asso_Mood_Common:GetKeys()) do
		self.m_MoodCB:InsertItem(Lan_Asso_Mood_Common(v, "DisplayName"), v, -1)
	end
	for i, v in ipairs(Lan_Asso_BodyShape_Common:GetKeys()) do
		self.m_BodyShapeCB:InsertItem(Lan_Asso_BodyShape_Common(v, "DisplayName"), v, -1)
	end
	for i, v in ipairs(Lan_Asso_Personality_Common:GetKeys()) do
		self.m_PersonalityCB:InsertItem(Lan_Asso_Personality_Common(v, "DisplayName"), v, -1)
	end
	for i, v in ipairs(Lan_Asso_MakeFriendState_Common:GetKeys()) do
		self.m_MakeFriendsStateCB:InsertItem(Lan_Asso_MakeFriendState_Common(v, "DisplayName"), v, -1)
	end
	for i, v in ipairs(Lan_Asso_Style_Common:GetKeys()) do
		self.m_StyleCB:InsertItem(Lan_Asso_Style_Common(v, "DisplayName"), v, -1)
	end
	
	self.m_MoodCB:SetSel(0)
	self.m_BodyShapeCB:SetSel(0)
	self.m_PersonalityCB:SetSel(0)
	self.m_MakeFriendsStateCB:SetSel(0)
	self.m_StyleCB:SetSel(0)
end

--消息
function CAssociationPersonalInfoEditWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn or Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			if( self:ChangeInfo() ) then
				self:ShowWnd(false)
			end
		end
	end
end

function CAssociationPersonalInfoEditWnd:VirtualExcludeWndClosed()
	self:ClearInfo()
end

function CAssociationPersonalInfoEditWnd:OpenPanel()
	self:ShowWnd(true)
	Gac2Gas:GetAssociationPersonalInfo(g_Conn)
end

function CAssociationPersonalInfoEditWnd:ChangeInfo()
	local showSen = self.m_ShowSentenceEdit:GetWndText()
	self.m_sShowSen = showSen
	if( not self.m_TextFilter:IsValidMsg(showSen) ) then
		MsgClient(5014)
		return false
	end
	local detail = self.m_DetailEdit:GetWndText()
	if(string.len(detail) > 255) then
		MsgClient(130055)
		return false
	end
	if( not self.m_TextFilter:IsValidMsg(detail) ) then
		MsgClient(5013)
		return false
	end
	
	local mood				= self.m_MoodCB:GetCurrentSel()
	local bodyShape			= self.m_BodyShapeCB:GetCurrentSel()
	local personality		= self.m_PersonalityCB:GetCurrentSel()
	local makeFriendsState	= self.m_MakeFriendsStateCB:GetCurrentSel()
	local style				= self.m_StyleCB:GetCurrentSel()
	
	local strInterest		= ""
	for i = 1, #(self.m_tblCheck) do
		local str = tostring(self.m_tblCheck[i]:GetCheck() and 1 or 0)
		strInterest = strInterest .. str
	end

	
	self.CallBackFuncChangePersonalInfo = function ()
		g_GameMain.m_AssociationWnd:ChangePersonalInfo(showSen)
	end
	Gac2Gas:SaveAssociationPersonalInfo(g_Conn, mood, showSen, strInterest, bodyShape, personality, makeFriendsState, style, detail)
	return true
end

function CAssociationPersonalInfoEditWnd:ClearInfo()
	local context = {0, "", "000000000", 0, 0, 0, 0, ""}
	self:ShowInfo(context)
end

function CAssociationPersonalInfoEditWnd:ShowInfo(context)
	local mood, showSen, strInterest, bodyShape, personality, makeFriendsState, style, detail = unpack(context)
	self.m_MoodCB:SetSel(mood)
	self.m_ShowSentenceEdit:SetWndText(showSen)
	self.m_BodyShapeCB:SetSel(bodyShape)
	self.m_PersonalityCB:SetSel(personality)
	self.m_MakeFriendsStateCB:SetSel(makeFriendsState)
	self.m_StyleCB:SetSel(style)
	self.m_DetailEdit:SetWndText(detail)
	for i = 1, #(self.m_tblCheck) do
		local subStr = tonumber(string.sub(strInterest, i, i))
		self.m_tblCheck[i]:SetCheck(1 == subStr and true or false)
	end
end

--***********************************************************************************************************
-------查看个人信息窗口------
--***********************************************************************************************************
function CreateAssociationPersonalInfoShowWnd(parent, memberId)
	local wnd = CAssociationPersonalInfoShowWnd:new()
	wnd:Init(parent, memberId)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CAssociationPersonalInfoShowWnd:Init(parent, memberId)
	self.m_MemberId				= memberId
	self:CreateFromRes("AssociationMemberInfo", parent)
	self:ShowWnd( true )
end

function CAssociationPersonalInfoShowWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_NameLable		= self:GetDlgChild("NameLable")
	self.m_IdLable			= self:GetDlgChild("IdLable")
	self.m_MoodIcon			= self:GetDlgChild("MoodIcon")
	self.m_ShowSen			= self:GetDlgChild("ShowSen")
	self.m_Vocation			= self:GetDlgChild("Vocation")
	self.m_Level			= self:GetDlgChild("Level")
	self.m_TongName			= self:GetDlgChild("TongName")
	self.m_Location			= self:GetDlgChild("Location")
	self.m_BodyShape		= self:GetDlgChild("BodyShape")
	self.m_Personality		= self:GetDlgChild("Personality")
	self.m_MakeFriendsState	= self:GetDlgChild("MakeFriendsState")
	self.m_Style			= self:GetDlgChild("Style")
	self.m_InterestRT		= self:GetDlgChild("InterestRT")
	self.m_GiftBtn			= self:GetDlgChild("GiftBtn")
	self.m_PrivateChatBtn	= self:GetDlgChild("PrivateChatBtn")
	self.m_MakeTeamBtn		= self:GetDlgChild("MakeTeamBtn")
	self.m_DetailRT			= self:GetDlgChild("DetailRT")
	
	self.m_GiftBtn:EnableWnd(false)
	self.m_PrivateChatBtn:EnableWnd(false)
	self.m_MakeTeamBtn:EnableWnd(false)
end

function CAssociationPersonalInfoShowWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CAssociationPersonalInfoShowWnd:VirtualExcludeWndClosed()
	self:DestroyWnd()
	local tbl = g_GameMain.m_tblPerInfoShowWnd
	for i = 1, #tbl do
		if(memberId == tbl.m_MemberId) then
			table.remove(tbl, i)
			break
		end
	end
end

function CAssociationPersonalInfoShowWnd:ShowInfo(context)
	local mood, showSen, class, level, tongName, cofcName, location,
		strInterest, bodyShape, personality, makeFriendsState, style, detail = unpack(context)
	self.m_MoodIcon:SetWndText(Lan_Asso_Mood_Common(mood, "DisplayName"))
	self.m_ShowSen:SetWndText(showSen)
	self.m_Vocation:SetWndText(class)
	self.m_Level:SetWndText(level)
	self.m_TongName:SetWndText(tongName)
	self.m_Location:SetWndText(GetSceneDispalyName(location))
	self.m_BodyShape:SetWndText(Lan_Asso_BodyShape_Common(bodyShape, "DisplayName"))
	self.m_Personality:SetWndText(Lan_Asso_Personality_Common(personality, "DisplayName"))
	self.m_MakeFriendsState:SetWndText(Lan_Asso_MakeFriendState_Common(makeFriendsState, "DisplayName"))
	self.m_Style:SetWndText(Lan_Asso_Style_Common(style, "DisplayName"))
	
	local interestStr = ""
	for i = 1, string.len(strInterest) do
		local subStr = tonumber(string.sub(strInterest, i, i))
		if(1 == subStr) then
			interestStr = interestStr .. Lan_Asso_Interest_Common(i, "DisplayName") .. ","
		end
	end
	self.m_InterestRT:SetWndText(interestStr)
	self.m_DetailRT:SetWndText(detail)
end
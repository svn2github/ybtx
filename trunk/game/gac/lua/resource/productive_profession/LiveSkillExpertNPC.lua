gac_gas_require "framework/common/CMoney"
lan_load "liveskill/Lan_Practiced_Common"

CLiveSkillExpertNPCWnd		= class(SQRDialog)
CLiveSkillExpertNPCListItem	= class(SQRDialog)

function CreateLiveSkillExpertNPCWnd(parent)
	local wnd = CLiveSkillExpertNPCWnd:new()
	wnd:Init(parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CLiveSkillExpertNPCWnd:Init(parent)
	self.m_SkillName			= ""
	self.m_tblTypeInfo			= {}
	self.m_tblPracticedValue	= {}
	self:CreateFromRes("LiveSkillExpertNpc", parent)
end

function CLiveSkillExpertNPCWnd:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_List			= self:GetDlgChild("List")
	self.m_LearnBtn		= self:GetDlgChild("LearnBtn")
	self.m_ForgetBtn	= self:GetDlgChild("ForgetBtn")
	self.m_CurExpert	= self:GetDlgChild("CurExpert")
	self.m_Money		= self:GetDlgChild("Money")
end

function CLiveSkillExpertNPCWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_LearnBtn) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160036)
				return 
			end
			local itemIndex = self.m_List:GetSelectItem(-1)+1
			if(not itemIndex or itemIndex < 1 or itemIndex > #(self.m_tblTypeInfo)) then return end
			Gac2Gas:LearnLiveSkillExpert(g_Conn, self.m_SkillName, self.m_tblTypeInfo[itemIndex])
			self:ShowWnd(false)
		elseif(Child == self.m_ForgetBtn) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160038)
				return 
			end
			Gac2Gas:ForgetLiveSkillExpert(g_Conn)
			self:ShowWnd(false)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_List) then
			local itemIndex = self.m_List:GetSelectItem(-1)+1
			if(self.m_sExpertName == self.m_tblTypeInfo[itemIndex]) then
				local nMoney = g_LiveSkillMgr:GetExpertInfoByLevelEx(self.m_nExpertLevel + 1, "MoneySpend")
				local sMoney = nMoney and g_MoneyMgr:ChangeMoneyToString(nMoney) or "---"
				self.m_Money:SetWndText(sMoney)
			else
				local nMoney = g_LiveSkillMgr:GetExpertInfoByLevelEx(1, "MoneySpend")
				local sMoney = g_MoneyMgr:ChangeMoneyToString(nMoney)
				self.m_Money:SetWndText(sMoney)
			end
		end
	end
end

function CLiveSkillExpertNPCWnd:OpenWnd(skillName)
	self:ShowWnd(true)
	self.m_SkillName = skillName
	self:RefreshList(skillName)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CLiveSkillExpertNPCWnd:OnUpdatePracticedInfo(expertName, expertLevel)
	self.m_sExpertName	= expertName
	self.m_nExpertLevel	= expertLevel
	local bFlag = ("" == expertName)
	if(bFlag) then
		text = GetStaticTextClient(3035)
	else
		text = GetStaticTextClient( 8326, Lan_Practiced_Common(MemH64(g_GameMain.m_LiveSkillProdMain.m_sType .. expertName), "DisplayName") )
		text = string.format("%s(%s)", text, Lan_SmithingExpert_Common(expertLevel, "DisplayName"))
	end
	self.m_CurExpert:SetWndText(text)
	for i, v in ipairs(self.m_tblItemWnd) do
		v:SetPracticedInfo(self.m_tblPracticedValue[v.m_sName])
		v.m_Mark:ShowWnd(v.m_sName == expertName)
	end
end

function CLiveSkillExpertNPCWnd:RefreshList(sSkillName)
	self.m_tblItemWnd = {}
	self.m_List:DeleteAllItem()
	self.m_List:InsertColumn(0, self.m_List:GetWorkAreaOrgWidth())
	self.m_tblTypeInfo = g_LiveSkillMgr:GetPracticedListByName(sSkillName)
	if(not self.m_tblTypeInfo) then return end
	for i, v in ipairs(self.m_tblTypeInfo) do
		self.m_List:InsertItem(i-1, 30)
		local item		= self.m_List:GetSubItem(i-1, 0)
		local itemWnd	= self:CreateLiveSkillExpertNPCListItem(item)
		table.insert(self.m_tblItemWnd, itemWnd)
		itemWnd:SetInfo(sSkillName, v)
	end
	local nType = g_LiveSkillMgr:GetSkillTypeByName(sSkillName)
	Gac2Gas:GetPracticedInfoForNpcPanel(g_Conn, nType)
end

function CLiveSkillExpertNPCWnd:CreateLiveSkillExpertNPCListItem(parent)
	local wnd = CLiveSkillExpertNPCListItem:new()
	wnd:CreateFromRes("LiveSkillExpertNpcListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CLiveSkillExpertNPCListItem:OnChildCreated()
	self.m_Icon		= self:GetDlgChild("Icon")
	self.m_Name		= self:GetDlgChild("Name")
	self.m_Progress	= self:GetDlgChild("Progress")
	self.m_Mark		= self:GetDlgChild("Mark")
end

function CLiveSkillExpertNPCListItem:SetInfo(sSkillName, sName)
	self.m_sName		= sName
	local sDisplayName	= Lan_Practiced_Common(MemH64(sSkillName .. sName), "DisplayName") 
	local nSmallIcon	= g_LiveSkillMgr:GetOneExpertInfoByColName(sSkillName, sName, "SmallIcon")
	g_LoadBufIconFromRes(nSmallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	self.m_Name:SetWndText(sDisplayName)
end

function CLiveSkillExpertNPCListItem:SetPracticedInfo(nPracticedValue)
	local practicedValue = nPracticedValue or 0
	self.m_Progress:SetPos(practicedValue)
	self.m_Progress:SetRange(g_GameMain.m_LiveSkillProdMain.m_LiveSkillProdPracticed.m_nMaxPracticedValue)
	self.m_Progress:SetWndText(practicedValue .. "/" .. g_GameMain.m_LiveSkillProdMain.m_LiveSkillProdPracticed.m_nMaxPracticedValue)
end

function Gas2Gac:ReturnGetPracticedInfoForNpcPanelBegin(Conn)
	g_GameMain.m_LiveSkillExpertNPC.m_tblPracticedValue = {}
end

function Gas2Gac:ReturnGetPracticedInfoForNpcPanel(Conn, name, value)
	g_GameMain.m_LiveSkillExpertNPC.m_tblPracticedValue[name] = value
end

function Gas2Gac:ReturnGetPracticedInfoForNpcPanelEnd(Conn, expertName, expertLevel)
	g_GameMain.m_LiveSkillExpertNPC:OnUpdatePracticedInfo(expertName, expertLevel)
end
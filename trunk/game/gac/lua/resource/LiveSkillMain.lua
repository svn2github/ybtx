gac_require "resource/LiveSkillMainInc"
gac_require "resource/productive_profession/LiveSkillDes"
cfg_load "liveskill/LiveSkillName_Client"
lan_load "liveskill/Lan_LiveSkillName_Client"

function CreateLiveSkillMainWnd(parent)
	local wnd = CLiveSkillMainWnd:new()
	wnd:Init(parent)
	wnd:ShowWnd(false)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CLiveSkillMainWnd:Init(parent)
	self.m_tblLiveSkillHasLearned = {}
	self:CreateFromRes("LiveSkillMainWnd", parent)
end

function CLiveSkillMainWnd:OnChildCreated()
	self.m_tblListItemWnd = {}
	self.m_List = self:GetDlgChild("List")
	self.m_XBtn = self:GetDlgChild("XBtn")
end

--消息
function CLiveSkillMainWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_List) then
			local itemIndex = self.m_List:GetSelectItem(-1)+1
			self:OpenTheSkillPanel(self.m_tblLiveSkillHasLearned[itemIndex])
		end
	end
end

function CLiveSkillMainWnd:GetSkillInfoByName(skillName)
	local tbl = self.m_tblLiveSkillHasLearned
	for i = 1, #tbl do
		if(tbl[i][1] == skillName) then
			return tbl[i]
		end
	end
end

function CLiveSkillMainWnd:DrawList()
	self.m_List:DeleteAllItem()
	self.m_tblListItemWnd = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	for i = 1, #(self.m_tblLiveSkillHasLearned) do
		self.m_List:InsertItem(i-1, 60)
		local itemWnd		= self:CreateLiveSkillListItem(self.m_List:GetSubItem(i-1, 0))
		local name			= self.m_tblLiveSkillHasLearned[i][1]
		local level			= self.m_tblLiveSkillHasLearned[i][2]
		local exp			= self.m_tblLiveSkillHasLearned[i][3]
		local nSmallIcon	= LiveSkillName_Client(name, "SmallIcon")
		itemWnd:SetInfo(name, level, exp, nSmallIcon)
		table.insert(self.m_tblListItemWnd, itemWnd)
	end
end

function CLiveSkillMainWnd:GetStateByName(sName)
	for i, v in ipairs(self.m_tblLiveSkillHasLearned) do
		if ( v[1] == sName ) then
			return true
		end
	end
	return false
end

function CLiveSkillMainWnd:BeOpenPanel()
	self:ShowWnd(not self:IsShow())
end

function CLiveSkillMainWnd:OpenTheSkillPanel(tbl)
	local skillName = tbl[1]
	if("花卉" == skillName or "采矿" == skillName) then
		if( not g_GameMain.m_LiveSkillDesWnd ) then
			g_GameMain.m_LiveSkillDesWnd = CreateLiveSkillDesWnd(g_GameMain)
		end
		g_GameMain.m_LiveSkillDesWnd:UpdateInfo(tbl)
	else
		g_GameMain.m_LiveSkillProdMain:OpenWnd(true, tbl)
	end
end

function CLiveSkillMainWnd:UpdateOneExpBar(sName, nExp)
	for k, v in ipairs(self.m_tblListItemWnd) do
		if( v.m_sName == sName ) then
			v:UpdateExpBar(nExp)
		end
	end
end

--------------------------------------------------------------------
function CLiveSkillMainWnd:CreateLiveSkillListItem(parent)
	local wnd = CLiveSkillMainListItem:new()
	wnd:CreateFromRes("LiveSkillMainWndListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CLiveSkillMainListItem:OnChildCreated()
	self.m_Icon		= self:GetDlgChild("Icon")
	self.m_Name		= self:GetDlgChild("Name")
	self.m_Level	= self:GetDlgChild("Level")
	self.m_Exp		= self:GetDlgChild("Exp")
end

function CLiveSkillMainListItem:SetInfo(sName, nLevel, nExp, nSmallIcon)
	g_LoadIconFromRes( nSmallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE )
	self.m_sName	= sName
	self.m_nLevel	= nLevel
	self.m_Name:SetWndText( Lan_LiveSkillName_Client(MemH64(sName), "DisplayName") )
	self.m_Level:SetWndText(GetStaticTextClient(8322, nLevel))
	self:UpdateExpBar(nExp)
end

function CLiveSkillMainListItem:UpdateExpBar(nExp)
	local nextLevelExpNeed	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sName, self.m_nLevel + 1, "ExperienceNeed")
	local curLevelExpNeed	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sName, self.m_nLevel, "ExperienceNeed")
	
	if(nextLevelExpNeed) then
		self.m_Exp:SetRange(nextLevelExpNeed - curLevelExpNeed)
		self.m_Exp:SetPos(nExp - curLevelExpNeed)
		self.m_Exp:SetWndText(nExp .. "/" .. nextLevelExpNeed)
	else
		self.m_Exp:SetRange(1)
		self.m_Exp:SetPos(1)
		self.m_Exp:SetWndText(nExp .. "/" .. nExp)
	end
end

-------------------------------------------------------------------------
function Gas2Gac:ReturnGetLiveSkillHasLearnedBegin(Conn)
	if(g_GameMain.m_LiveSkillMainWnd) then
		g_GameMain.m_LiveSkillMainWnd.m_tblLiveSkillHasLearned = {}
	end
	g_GameMain.m_LiveSkillProdMain.m_sType = ""
end

function Gas2Gac:ReturnGetLiveSkillHasLearned(Conn, strSkillName, nLevel, nExp)
	if(g_GameMain.m_LiveSkillMainWnd) then
		local tbl = g_GameMain.m_LiveSkillMainWnd.m_tblLiveSkillHasLearned
		table.insert(tbl, {strSkillName, nLevel, nExp})
	end
	
	if( not ("花卉" == strSkillName or "采矿" == strSkillName) ) then
		g_GameMain.m_LiveSkillProdMain:UpdateInfo({strSkillName, nLevel, nExp})
	end
end

function Gas2Gac:ReturnGetLiveSkillHasLearnedEnd(Conn)
	local wnd = g_GameMain.m_LiveSkillMainWnd
	if(wnd) then
		wnd:DrawList()
	end
end

function Gas2Gac:ReturnLiveSkillExp(Conn, nExp, sName)
	local liveSkillMainWnd = g_GameMain.m_LiveSkillMainWnd
	liveSkillMainWnd:UpdateOneExpBar(sName, nExp)
	
	local tbl = liveSkillMainWnd.m_tblLiveSkillHasLearned
	local wnd = g_GameMain.m_LiveSkillProdMain
	for i = 1, #(tbl) do
		if( sName == tbl[i][1] ) then
			tbl[i][3] = nExp
			if(wnd) then
				wnd:UpdateExpBar(tbl[i])
			end
			break
		end
	end
end
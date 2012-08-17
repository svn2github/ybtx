lan_load "liveskill/Lan_Practiced_Common"
lan_load "liveskill/Lan_SmithingExpert_Common"

CLiveSkillProdPracticedWnd = class(SQRDialog)
CLiveSkillProdPraListItem  = class(SQRDialog)

function CreateLiveSkillProdPracticedWnd(parent)
	local wnd = CLiveSkillProdPracticedWnd:new()
	wnd:Init(parent)
	return wnd
end

function CLiveSkillProdPracticedWnd:Init(parent)
	self.m_nMaxPracticedValue = 100
	self.m_tblPracticedValue = {}
	self:CreateFromRes("LiveSkillProdPracticed", parent)
end

function CLiveSkillProdPracticedWnd:OnChildCreated()
	self.m_Name				= self:GetDlgChild("Name")
	self.m_Icon				= self:GetDlgChild("Icon")
	self.m_Type				= self:GetDlgChild("Type")
	self.m_PracticedList	= self:GetDlgChild("PracticedList")
end

function CLiveSkillProdPracticedWnd:DrawPracticedList()
	self.m_PracticedList:DeleteAllItem()
	self.m_tblPracticedListItem = {}
	self.m_PracticedList:InsertColumn(0, self.m_PracticedList:GetWndWidth())
	for i, v in ipairs(g_LiveSkillMgr:GetPracticedListByName(g_GameMain.m_LiveSkillProdMain.m_sType)) do
		self.m_PracticedList:InsertItem(i-1, 30)
		local wnd = self.m_PracticedList:GetSubItem(i-1, 0)
		local practicedItem	= self:CreateLiveSkillProdPraListItem(wnd, self)
		practicedItem:SetInfo(v)
		table.insert(self.m_tblPracticedListItem, practicedItem)
	end
end

function CLiveSkillProdPracticedWnd:OnUpdatePracticedInfo()
	if(not self.m_tblPracticedListItem) then return end
	for i, v in ipairs(self.m_tblPracticedListItem) do
		v:SetPracticedInfo(self.m_tblPracticedValue[v.m_sName])
	end
end

function CLiveSkillProdPracticedWnd:OnUpgreadProgress(nMaxPracticedValue)
	self.m_nMaxPracticedValue = nMaxPracticedValue
	if(self.m_tblPracticedListItem) then
		for i, v in ipairs(self.m_tblPracticedListItem) do
			v:UpgreadProgress(self.m_tblPracticedValue[i])
		end
	end
end

function CLiveSkillProdPracticedWnd:OnGetPracticedInfo()
	local nType = g_LiveSkillMgr:GetSkillTypeByName(g_GameMain.m_LiveSkillProdMain.m_sType)
	Gac2Gas:GetPracticedInfo(g_Conn, nType)
end

function CLiveSkillProdPracticedWnd:ReturnGetPracticedInfoEnd(expertName, expertLevel)
	self:OnUpdatePracticedInfo()
	
	local bFlag	= ("" ~= expertName)
	if(bFlag) then
		self.m_Name:SetWndText( GetStaticTextClient( 8326, Lan_Practiced_Common(MemH64(g_GameMain.m_LiveSkillProdMain.m_sType .. expertName), "DisplayName") ) )
		local nSmallIcon = g_LiveSkillMgr:GetOneExpertInfoByColName(g_GameMain.m_LiveSkillProdMain.m_sType, expertName, "SmallIcon")
		g_LoadIconFromRes(nSmallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	else
		self.m_Name:SetWndText( GetStaticTextClient(3035) )
	end
	self.m_Type:SetWndText( Lan_SmithingExpert_Common(expertLevel, "DisplayName") )
	self.m_Icon:ShowWnd(bFlag)
	
	for i, v in ipairs(self.m_tblPracticedListItem) do
		v.m_Mark:ShowWnd(v.m_sName == expertName)
	end
end

function CLiveSkillProdPracticedWnd:OpenWnd(bFlag)
	if(bFlag) then
		self:DrawPracticedList()
		self:OnGetPracticedInfo()
	end
	self:ShowWnd(bFlag)
end
---------------------------------------------------------------
function CLiveSkillProdPracticedWnd:CreateLiveSkillProdPraListItem(parent, father)
	local wnd		= CLiveSkillProdPraListItem:new()
	wnd.m_fatherWnd	= father
	wnd:CreateFromRes("LiveSkillProdPraListItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CLiveSkillProdPraListItem:OnChildCreated()
	self.m_Label	= self:GetDlgChild("Label")
	self.m_Icon		= self:GetDlgChild("Icon")
	self.m_Mark		= self:GetDlgChild("Mark")
	self.m_Progress	= self:GetDlgChild("Progress")
	self.m_Mark:ShowWnd(false)
	self.m_Progress:SetProgressMode(0)
	self.m_Progress:SetRange(self.m_fatherWnd.m_nMaxPracticedValue)
end

function CLiveSkillProdPraListItem:SetInfo(sExpertName)
	self.m_sName		= sExpertName
	local nSmallIcon	= g_LiveSkillMgr:GetOneExpertInfoByColName(g_GameMain.m_LiveSkillProdMain.m_sType, sExpertName, "SmallIcon")
	g_LoadBufIconFromRes(nSmallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	self.m_Label:SetWndText( Lan_Practiced_Common(MemH64(g_GameMain.m_LiveSkillProdMain.m_sType .. sExpertName), "DisplayName") )
end

function CLiveSkillProdPraListItem:SetPracticedInfo(nPracticedValue)
	local practicedValue = nPracticedValue or 0
	self.m_Progress:SetPos(practicedValue)
	self.m_Progress:SetWndText(practicedValue .. "/" .. self.m_fatherWnd.m_nMaxPracticedValue)
end

function CLiveSkillProdPraListItem:UpgreadProgress(nPracticedValue)
	self.m_Progress:SetRange(self.m_fatherWnd.m_nMaxPracticedValue)
	self:SetPracticedInfo(nPracticedValue)
end

----------------------------------------------------------------
function Gas2Gac:ReturnGetPracticedInfoBegin(Conn)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProdPracticed.m_tblPracticedValue = {}
end

function Gas2Gac:ReturnGetPracticedInfo(Conn, name, value)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProdPracticed.m_tblPracticedValue[name] = value
end

function Gas2Gac:ReturnGetPracticedInfoEnd(Conn, expertName, expertLevel)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProdPracticed:ReturnGetPracticedInfoEnd(expertName, expertLevel)
end

function Gas2Gac:ReturnGetLiveSkillExpert(Conn, expertName, expertLevel)

end

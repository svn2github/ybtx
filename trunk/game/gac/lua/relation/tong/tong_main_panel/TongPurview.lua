cfg_load "tong/TongPurview_Common"
lan_load "tong/Lan_TongPurview_Common"

CTongPurviewWnd			= class(SQRDialog)	--导出类
CTongPurviewListItem	= class(SQRDialog)

function CTongPurviewWnd.CreateTongPurviewWnd(parent)
	local wnd = CTongPurviewWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongPurviewWnd:Init(parent)
	self.m_tblTongPurview = {}
	local tblCommon		= TongPurview_Common
	local tblLanCommon	= Lan_TongPurview_Common
	local keys = tblCommon:GetKeys()
	for k, key in pairs(keys) do
		local v = tblCommon(key)
		local sPurviewInfo = string.format("%d%d%d%d%d", v("Level5"), v("Level4"), v("Level3"), v("Level2"), v("Level1"))
		self.m_tblTongPurview[v("Order")] = { tblLanCommon(MemH64(v("Title")),"DisplayTitle"), sPurviewInfo }
	end
	self:CreateFromRes("TongPurview", parent)
	self:DrawPurviewList()
end

function CTongPurviewWnd:OnChildCreated()
	self.m_PurviewList  = self:GetDlgChild("PurviewList")
end

function CTongPurviewWnd:OpenPanel(bFlag)
	if(bFlag) then
		--Gac2Gas:GetTongPurview(g_Conn)
		self:SetPurviewInfo()
	end
	self:ShowWnd(bFlag)
end

function CTongPurviewWnd:SetPurviewInfo()
	local tbl = self.m_tblTongPurview
	for i = 1, #tbl do
		self.m_PurviewList[i]:ChangeInfo(tbl[i][2])
	end
end

----------------------------------------------------------------
function CTongPurviewWnd:DrawPurviewList()
	self.m_PurviewList:DeleteAllItem()
	self.tblPurviewItem = {}
	self.m_PurviewList:InsertColumn(0, self.m_PurviewList:GetWndOrgWidth())
	local tbl = self.m_tblTongPurview
	for i = 1, #tbl do
		self.m_PurviewList:InsertItem(i-1, 30)
		local item = self.m_PurviewList:GetSubItem(i-1, 0)
		local purviewItem = self:CreateTongPurviewListItemWnd(item)
		table.insert(self.m_PurviewList, purviewItem)
		purviewItem:SetTitle(tbl[i][1])
	end
end

----------------------------------------------------------------
function CTongPurviewWnd:CreateTongPurviewListItemWnd(parent)
	local wnd = CTongPurviewListItem:new()
	wnd:CreateFromRes("TongPurviewListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongPurviewListItem:OnChildCreated()
	self.m_Title = self:GetDlgChild("Title")
	self.m_tblCheck = {}
	for i = 1, 5 do
		self.m_tblCheck[i] = self:GetDlgChild("Check" .. i)
	end
end

function CTongPurviewListItem:SetTitle(sTitle)
	self.m_Title:SetWndText(sTitle)
end

function CTongPurviewListItem:ChangeInfo(sInfo)
	for i = 1, 5 do
		local bFlag = "1" == string.sub(sInfo, i, i)
		self.m_tblCheck[i]:SetCheck(bFlag)
	end
end

--*************************
---------------------------
--*************************
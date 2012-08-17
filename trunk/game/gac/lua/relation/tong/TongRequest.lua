gac_require "relation/tong/TongRequestInc"
gac_gas_require "relation/tong/TongMgr"
local textFltMgr = CTextFilterMgr:new()
------NPC申请入会面板------

function CreateTongRequestWnd(Parent)
	local Wnd = CTongRequestWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongRequestWnd:Init(Parent)
	self:CreateFromRes("TongRequest", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongRequestWnd:OnChildCreated()
	self.m_AllTongInfoTbl = {}
	self.m_AllTongList	= self:GetDlgChild("AllTongList")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_LastBtn	= self:GetDlgChild("LastBtn")
	self.m_NextBtn	= self:GetDlgChild("NextBtn")
	self.m_BackToFirst = self:GetDlgChild("BackToFirst")
	self.m_TongSearchName = self:GetDlgChild("TongSearchName")
	self.m_SearchBtn = self:GetDlgChild("SearchBtn")
	self.m_NowPage	= 0
	self.m_CurPage = self:GetDlgChild("CurPage")
	self.m_NowTongList	= {}
	
	self.m_tblSorBtn = {}
	self.m_tblSorBtn["sName"]		= self:GetDlgChild("NameBtn")
	self.m_tblSorBtn["nLevel"]		= self:GetDlgChild("LevelBtn")
	self.m_tblSorBtn["nMemNum"]		= self:GetDlgChild("NumBtn")
	self.m_tblSorBtn["sCaptainName"] = self:GetDlgChild("CaptainNameBtn")
	self.m_tblSorBtn["nStationLine"]	= self:GetDlgChild("StationBtn")
	for k, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
end

function CTongRequestWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(self:IsShow()) then
		if (uMsgID == BUTTON_LCLICK) then
			if(Child == self.m_XBtn) then
				self:ShowWnd(false)
			elseif(Child == self.m_OkBtn) then
				self:OnRequestJoinTong()
			elseif(Child == self.m_LastBtn) then
				self:ChangePage(-1)
				self:DrawRequestList()
			elseif(Child == self.m_NextBtn) then
				self:ChangePage(1)
				self:DrawRequestList()
			elseif(Child == self.m_BackToFirst) then
				self.m_AllTongInfoTbl = {}
				Gac2Gas:GetAllTongSomeInfo(g_Conn)
				self:TurnToPageOne()
				self:DrawRequestList()
			elseif(Child == self.m_SearchBtn) then
				local sName = self.m_TongSearchName:GetWndText()
				if("" == sName) or not textFltMgr:IsValidName(sName) then
					MsgClient(9068)
					return
				end
				Gac2Gas:SearchTong(g_Conn,sName)
			else
				for k, v in pairs(self.m_tblSorBtn) do
					if(Child == v) then
						self:SortList(k, v.bFlag)
						v.bFlag = not v.bFlag
						break
					end
				end
			end
		end
	end
end

function CTongRequestWnd:OpenPanel()
	self.m_NowPage	= 0
	self.m_AllTongInfoTbl = {} --清空游戏帮会列表
	Gac2Gas:GetAllTongSomeInfo(g_Conn)
	self:ShowWnd(true)
end

local uListCountOnePage = 14



	
function CTongRequestWnd:ChangePage(uAdd)
	if self.m_NowPage == 0 and uAdd < 0 then
		return
	end
	local uAllTongCount = #self.m_AllTongInfoTbl
	if (self.m_NowPage+1)*uListCountOnePage >= uAllTongCount and uAdd > 0 then
		return
	end
	self.m_NowPage = self.m_NowPage + uAdd
	self.m_NowTongList = {}
	local uNextBeginIndex = self.m_NowPage*uListCountOnePage + 1
	local uBackCount = uAllTongCount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllTongCount
	local tbl = self.m_AllTongInfoTbl
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_NowTongList,tbl[i])
	end
	self:ShowCurPage()
end

function CTongRequestWnd:TurnToPageOne()
	if self.m_NowPage == 0 then
		return
	end
	self.m_NowTongList = {}
	local tbl = self.m_AllTongInfoTbl
	for i=1, uListCountOnePage do
		table.insert(self.m_NowTongList,tbl[i])
	end
	self.m_NowPage = 0
	self:ShowCurPage()
end

function CTongRequestWnd:ShowCurPage()
	local sumOfPages = 0
	local uAllTongCount = #self.m_AllTongInfoTbl
	if uAllTongCount % uListCountOnePage > 0 then
		sumOfPages = uAllTongCount/uListCountOnePage + 1
	elseif uAllTongCount % uListCountOnePage == 0 then
		sumOfPages = uAllTongCount/uListCountOnePage
	end
	if sumOfPages == 0 then
		self.m_CurPage:SetWndText(sumOfPages.."/"..sumOfPages)
	else
		self.m_CurPage:SetWndText((self.m_NowPage+1).."/"..math.floor(sumOfPages))
	end
end

function CTongRequestWnd:TurntoPageBySearch(NowPage,i)
	self.m_NowTongList = {}
	local uAllTongCount = #self.m_AllTongInfoTbl
	local uNextBeginIndex = NowPage*uListCountOnePage + 1
	local uBackCount = uAllTongCount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllTongCount
	local tbl = self.m_AllTongInfoTbl
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_NowTongList,tbl[i])
	end
	self.m_NowPage = NowPage
	self:DrawRequestList()
	local index = i%uListCountOnePage
	self.m_AllTongList:SelectItem(index-1)
	self:ShowCurPage()
end

function CTongRequestWnd:SortList(index, bFlag)
	if(bFlag) then
		table.sort(self.m_AllTongInfoTbl, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_AllTongInfoTbl, function (a, b) return a[index] > b[index] end)
	end
	self.m_NowTongList = {}
	local uAllTongCount = #self.m_AllTongInfoTbl
	local uNextBeginIndex = self.m_NowPage*uListCountOnePage + 1
	local uBackCount = uAllTongCount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllTongCount
	local tbl = self.m_AllTongInfoTbl
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_NowTongList,tbl[i])
	end
	self:DrawRequestList()
end

function CTongRequestWnd:OnRequestJoinTong()
	local itemIndex = self.m_AllTongList:GetSelectItem(-1)+1
	local tbl = self.m_NowTongList
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id = tbl[itemIndex].nId
	if( g_MainPlayer.m_Properties:GetTongID() ~= 0 ) then
		MsgClient(9009)
		return
	else
		Gac2Gas:RequestJoinTong(g_Conn, id)
	end
	self:ShowWnd(false)
end

---------------------------------------------------------
function CTongRequestWnd:DrawRequestList()
	self.m_AllTongList:DeleteAllItem()
	self.tblTongListItem = {}
	self.m_AllTongList:InsertColumn(0, self.m_AllTongList:GetWndOrgWidth())
	local tbl = self.m_NowTongList
	for i, v in ipairs(tbl) do
		self.m_AllTongList:InsertItem(i-1, 30)
		local item = self.m_AllTongList:GetSubItem(i-1, 0)
		local tongListItem = self:CreateTongRequestListItemWnd(item)
		table.insert(self.tblTongListItem, tongListItem)
		tongListItem:SetTongRequestText(v)
	end
end

------创建列表的行窗体
function CTongRequestWnd:CreateTongRequestListItemWnd(Parent)
	local Wnd = CTongRequestListItem:new()
	Wnd:CreateFromRes("TongAllTongListItem", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongRequestListItem:OnChildCreated()
	self.m_Name		= self:GetDlgChild("Name")
	self.m_MemNum	= self:GetDlgChild("MemNum")
	self.m_Level	= self:GetDlgChild("Level")
	self.m_Station	= self:GetDlgChild("Station")
	self.m_CaptainName = self:GetDlgChild("CaptainName")
end

function CTongRequestListItem:SetTongRequestText(tblInfo)
	local strStation = g_GameMain.m_TongBase:GetStationDes(tblInfo.nWarZoneId,tblInfo.nStationId)
	local warZone = ""
	if tblInfo.nWarZoneId ~= 0 then
		warZone = GetStaticTextClient(10039, tblInfo.nWarZoneId)
	end
	self.m_Name:SetWndText(tblInfo.sName)
	self.m_Level:SetWndText(g_TongMgr:LevelN2S(tblInfo.nLevel))
	self.m_MemNum:SetWndText(tblInfo.nMemNum.."/"..tblInfo.nMaxNum)
	self.m_CaptainName:SetWndText(tblInfo.sCaptainName)
	self.m_Station:SetWndText(strStation)
end
--*************************
---------------------------
--*************************

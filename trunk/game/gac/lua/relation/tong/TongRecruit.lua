CTongRecruit = class(SQRDialog)
CTongRecruitListItem = class(SQRDialog)
local textFltMgr = CTextFilterMgr:new()
--------NPC佣兵队员招募------
function CTongRecruit:GetWnd(parent)
	local Wnd = g_GameMain.m_TongRecruit
	if not Wnd then
		Wnd = CTongRecruit:new(g_GameMain)
		Wnd:CreateFromRes("TongRecruit", g_GameMain)
		g_GameMain.m_TongRecruit = Wnd
	end
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, "非互斥")
	return Wnd
end

function CTongRecruit:OnChildCreated()
	self.m_AllPlayerInfoTbl = {}
	self.m_AllPlayerList	= self:GetDlgChild("AllPlayerList")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_LastBtn	= self:GetDlgChild("LastBtn")
	self.m_NextBtn	= self:GetDlgChild("NextBtn")
	self.m_BackToFirst = self:GetDlgChild("BackToFirstBtn")
	self.m_SearchName = self:GetDlgChild("SearchName")
	self.m_SearchBtn = self:GetDlgChild("SearchBtn")
	self.m_NowPage	= 0
	self.m_CurPage = self:GetDlgChild("CurPage")
	self.m_NowPlayerList	= {}
	
	self.m_tblSorBtn = {}
	self.m_tblSorBtn["sName"]		= self:GetDlgChild("NameBtn")
	self.m_tblSorBtn["nLevel"]		= self:GetDlgChild("LevelBtn")
	self.m_tblSorBtn["nClass"]		= self:GetDlgChild("ClassBtn")
	self.m_tblSorBtn["fFightPoint"] = self:GetDlgChild("FightPointBtn")
	self.m_tblSorBtn["nProffer"]	= self:GetDlgChild("ProfferBtn")
	self.m_tblSorBtn["nExploit"] = self:GetDlgChild("ExploitBtn")
	for k, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
end

function CTongRecruit:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(self:IsShow()) then
		if (uMsgID == BUTTON_LCLICK) then
			if(Child == self.m_XBtn) then
				self:ShowWnd(false)
			elseif(Child == self.m_OkBtn) then
				self:OnRecruitToTong()
			elseif(Child == self.m_LastBtn) then
				self:ChangePage(-1)
				self:DrawPlayerList()
			elseif(Child == self.m_NextBtn) then
				self:ChangePage(1)
				self:DrawPlayerList()
			elseif(Child == self.m_BackToFirst) then
				self.m_AllTongInfoTbl = {}
				Gac2Gas:GetAllPlayerSomeInfo(g_Conn)
				self:TurnToPageOne()
				self:DrawPlayerList()
			elseif(Child == self.m_SearchBtn) then
				local sName = self.m_SearchName:GetWndText()
				if("" == sName) or not textFltMgr:IsValidName(sName) then
					MsgClient(9064)
					return
				end
				Gac2Gas:SearchPlayer(g_Conn,sName)
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

function CTongRecruit:OpenPanel()
	self.m_NowPage	= 0
	self.m_AllTongInfoTbl = {} --清空游戏帮会列表
	Gac2Gas:GetAllPlayerSomeInfo(g_Conn)
	self:ShowWnd(true)
end

function CTongRecruit:DrawPlayerList()
	self.m_AllPlayerList:DeleteAllItem()
	self.tblPlayerListItem = {}
	self.m_AllPlayerList:InsertColumn(0, self.m_AllPlayerList:GetWndOrgWidth())
	local tbl = self.m_NowPlayerList
	for i, v in ipairs(tbl) do
		self.m_AllPlayerList:InsertItem(i-1, 30)
		local item = self.m_AllPlayerList:GetSubItem(i-1, 0)
		local playerListItem = self:CreateTongRecruitListItemWnd(item)
		table.insert(self.tblPlayerListItem, playerListItem)
		playerListItem:SetTongRecruitText(v)
	end
end

local uListCountOnePage = 10
	
function CTongRecruit:ChangePage(uAdd)
	if self.m_NowPage == 0 and uAdd < 0 then
		return
	end
	local uAllPlayercount = #self.m_AllPlayerInfoTbl
	if (self.m_NowPage+1)*uListCountOnePage >= uAllPlayercount and uAdd > 0 then
		return
	end
	self.m_NowPage = self.m_NowPage + uAdd
	self.m_NowPlayerList = {}
	local uNextBeginIndex = self.m_NowPage*uListCountOnePage + 1
	local uBackCount = uAllPlayercount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllPlayercount
	local tbl = self.m_AllPlayerInfoTbl
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_NowPlayerList,tbl[i])
	end
	self:ShowCurPage()
end

function CTongRecruit:TurnToPageOne()
	if self.m_NowPage == 0 then
		return
	end
	self.m_NowPlayerList = {}
	local tbl = self.m_AllPlayerInfoTbl
	for i=1, uListCountOnePage do
		table.insert(self.m_NowPlayerList,tbl[i])
	end
	self.m_NowPage = 0
	self:ShowCurPage()
end

function CTongRecruit:ShowCurPage()
	local sumOfPages = 0
	local uAllPlayercount = #self.m_AllPlayerInfoTbl
	if uAllPlayercount % uListCountOnePage > 0 then
		sumOfPages = uAllPlayercount/uListCountOnePage + 1
	elseif uAllPlayercount % uListCountOnePage == 0 then
		sumOfPages = uAllPlayercount/uListCountOnePage
	end
	if sumOfPages == 0 then
		self.m_CurPage:SetWndText(sumOfPages.."/"..sumOfPages)
	else
		self.m_CurPage:SetWndText((self.m_NowPage+1).."/"..math.floor(sumOfPages))
	end
end

function CTongRecruit:TurntoPageBySearch(NowPage, i)
	self.m_NowPlayerList = {}
	local uAllPlayercount = #self.m_AllPlayerInfoTbl
	local uNextBeginIndex = NowPage * uListCountOnePage + 1
	local uBackCount = uAllPlayercount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllPlayercount
	local tbl = self.m_AllPlayerInfoTbl
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_NowPlayerList,tbl[i])
	end
	self.m_NowPage = NowPage
	self:DrawPlayerList()
	local index = i%uListCountOnePage
	self.m_AllPlayerList:SelectItem(index-1)
	self:ShowCurPage()	
end

function CTongRecruit:SortList(index, bFlag)
	if(bFlag) then
		table.sort(self.m_AllPlayerInfoTbl, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_AllPlayerInfoTbl, function (a, b) return a[index] > b[index] end)
	end
	self.m_NowPlayerList = {}
	local uAllPlayercount = #self.m_AllPlayerInfoTbl
	local uNextBeginIndex = self.m_NowPage * uListCountOnePage + 1
	local uBackCount = uAllPlayercount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllPlayercount
	local tbl = self.m_AllPlayerInfoTbl
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_NowPlayerList,tbl[i])
	end
	self:DrawPlayerList()
end

function CTongRecruit:OnRecruitToTong()
	local itemIndex = self.m_AllPlayerList:GetSelectItem(-1)+1
	local tbl = self.m_NowPlayerList
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id = tbl[itemIndex].nId
	if( g_MainPlayer.m_Properties:GetTongID() == 0 ) then
		MsgClient(9067)
		return
	else
		Gac2Gas:InviteJoinTong(g_Conn, id)
	end
	self:ShowWnd(false)
end

------创建列表的行窗体
function CTongRecruit:CreateTongRecruitListItemWnd(Parent)
	local Wnd = CTongRecruitListItem:new()
	Wnd:CreateFromRes("TongAllPlayerListItem", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongRecruitListItem:OnChildCreated()
	self.m_Name		= self:GetDlgChild("PlayerName")
	self.m_Class	= self:GetDlgChild("Class")
	self.m_Level	= self:GetDlgChild("Level")
	self.m_FightPoint	= self:GetDlgChild("FightPoint")
	self.m_Proffer = self:GetDlgChild("Proffer")
	self.m_Exploit = self:GetDlgChild("Exploit")
end

function CTongRecruitListItem:SetTongRecruitText(tblInfo)
	self.m_Name:SetWndText(tblInfo.sName)
	self.m_Level:SetWndText(tblInfo.nLevel)
	local sClass	= g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(tblInfo.nClass)
	self.m_Class:SetWndText(sClass)
	self.m_FightPoint:SetWndText(string.format("%.2f",tblInfo.fFightPoint))
	self.m_Proffer:SetWndText(tblInfo.nProffer)
	self.m_Exploit:SetWndText(tblInfo.nExploit)
end
--*************************
---------------------------
--*************************
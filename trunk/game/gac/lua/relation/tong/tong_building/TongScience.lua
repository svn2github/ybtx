gac_gas_require "relation/tong/TongTechnologyMgr"
lan_load "tong/Lan_TongFightTech_Common"

CTongScienceWnd			= class(SQRDialog, CTongProductionBase)
CTongScienceListItem	= class(SQRDialog, CTongProductionListItemBase)

local fl_TblBtnText =
{
	["条件不足"]	= GetStaticTextClient(10033),
	["升级"]		= GetStaticTextClient(10034),
	["升级中"]		= GetStaticTextClient(10035),
	["排队中"]		= GetStaticTextClient(10036),
	["继续升级"]	= GetStaticTextClient(10037),
	["满级"]		= GetStaticTextClient(10038)
}

function CTongScienceWnd:Ctor()
	self:InitInfo()
	self:CreateFromRes("TongBuilldingItemCreate", g_GameMain)
	self:DrawItemCreateList()
	self:ShowFalseIcon()
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongScienceWnd:InitInfo()
	self.m_CurType				= 1 --当前页面展示的科技类型。1 战斗科技
	self.m_TongLevel			= 1
	self.m_TongLine				= 0
	self.m_tblScienceState		= {}
	self.m_tblFightTechNames	= g_TongTechMgr:GetFightTechNames()
end

function CTongScienceWnd:OnGetCreatingItemInfo()
	Gac2Gas:GetTongFightScienceInfo(g_Conn)
end

function CTongScienceWnd:OnCancelCreateItem(Child)
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160041)
		return 
	end
	Gac2Gas:CancelUpdateScience(g_Conn, Child.name, g_GameMain.m_TongScience.m_CurType)
end

function CTongScienceWnd:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
	if(bFlag) then
		self:OnGetCreatingItemInfo()
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
	end
end

function CTongScienceWnd:SetWaitingIcon(sName, nLevel, i)
	local nNextLevel = nLevel + 1
	local nSmallIcon = g_TongTechMgr:GetFightTechSmallIcon(sName, nNextLevel)
	g_LoadIconFromRes(nSmallIcon, self.m_tblWaitingIcon[i], -1, IP_ENABLE, IP_ENABLE)
	self.m_tblWaitingIconBtn[i]:SetMouseOverDescAfter( g_Tooltips:GetTongFightTechTips(sName, nLevel) )
end

function CTongScienceWnd:UpdateInfo()
	self:ShowFalseIcon()
	local tbl = self.m_tblScienceState[1] --1是战斗科技
	if(not tbl or not next(tbl) ) then return end
	local nReserchingIndex = g_TongTechMgr:GetStateIndex("Researching")
	if( tbl[nReserchingIndex] and next(tbl[nReserchingIndex]) ) then
		self:UpdateCreatingInfo(tbl[nReserchingIndex][1].name, tbl[nReserchingIndex][1].passedTime, tbl[nReserchingIndex][1].level)
	end
	
	for i, v in ipairs(tbl[g_TongTechMgr:GetStateIndex("Waiting")]) do
		self.m_tblWaitingIconBtn[i].name = v.name
		self.m_tblWaitingIconBtn[i]:ShowWnd(true)
		self:SetWaitingIcon(v.name, v.level, i)
	end
end

function CTongScienceWnd:ShowFalseIcon()
	self.m_CreatingIconBtn:ShowWnd(false)
	self.m_CreatingBar:ShowWnd(false)
	for i, v in pairs(self.m_tblWaitingIconBtn) do
		v:ShowWnd(false)
	end
end

--正在制造的物品信息
function CTongScienceWnd:UpdateCreatingInfo(sName, nPassedTime, nLevel)
	local nNextLevel = nLevel + 1
	
	if(self.CreatingBuildingItemTick) then
		UnRegisterTick(self.CreatingBuildingItemTick)
		self.CreatingBuildingItemTick = nil
	end
	
	self.m_CreatingIconBtn.name = sName
	self.m_CreatingIconBtn:ShowWnd(true)
	self.m_CreatingBar:ShowWnd(true)
	
	local nSmallIcon	= g_TongTechMgr:GetFightTechSmallIcon(sName, nNextLevel)
	local nTime			= g_TongTechMgr:GetFightTechNeedTime(sName, nNextLevel)
	
	local CreatingBuildingItemTick = function()
		if(self.m_CreatingBar:GetPos() >= nTime) then
			UnRegisterTick(self.CreatingBuildingItemTick)
			self.CreatingBuildingItemTick = nil
			self:OnGetCreatingItemInfo()
		else
			local newValue = self.m_CreatingBar:GetPos() + 1
			self.m_CreatingBar:SetPos(newValue)
			self.m_CreatingBar:SetWndText(newValue .. "/" .. nTime)
			self.m_ItemBar:SetPos(newValue)
			self.m_ItemBar:SetWndText(newValue .. "/" .. nTime)
		end
	end
	
	g_LoadIconFromRes(nSmallIcon, self.m_CreatingIcon, -1, IP_ENABLE, IP_ENABLE)
	self.m_CreatingIconBtn:SetMouseOverDescAfter( g_Tooltips:GetTongFightTechTips(sName, nLevel) )
	self.m_CreatingBar:SetRange(nTime)
	self.m_CreatingBar:SetPos(nPassedTime)
	self.m_CreatingBar:SetWndText(nPassedTime .. "/" .. nTime)
	self.m_ItemBar:SetRange(nTime)
	self.m_ItemBar:SetPos(nPassedTime)
	self.m_ItemBar:SetWndText(nPassedTime .. "/" .. nTime)
	self.CreatingBuildingItemTick = RegisterTick("CreatingBuildingItemTick", CreatingBuildingItemTick, 1000)
end

function CTongScienceWnd:InsertInfo(sName, nLevel, nMark, nPassedTime)
	for i, v in ipairs(self.m_tblScienceItem) do
		if(v.name == sName) then
			v:ChangeInfo(nLevel, nMark, nPassedTime)
		end
	end
	
	local tbltemp		= {}
	tbltemp.name		= sName
	tbltemp.level		= nLevel
	tbltemp.passedTime	= nPassedTime
	table.insert(self.m_tblScienceState[1][nMark], tbltemp) --1是战斗科技
end
---------------------------------------------------------------

function CTongScienceWnd:DrawItemCreateList()
	self.m_ItemList:DeleteAllItem()
	self.m_ItemList:InsertColumn(0, self.m_ItemList:GetWndOrgWidth())
	local tbl = self.m_tblFightTechNames
	self.m_tblScienceItem = {}
	for i, v in ipairs(self.m_tblFightTechNames) do
		self.m_ItemList:InsertItem(i-1, 50)
		local item		= self.m_ItemList:GetSubItem(i-1, 0)
		local itemWnd	= self:CreateListItemWnd(item, self)
		table.insert(self.m_tblScienceItem, itemWnd)
		itemWnd:SetInfo(v, 0, 0, 0)
	end
end

function CTongScienceWnd:ReturnGetTongFightScienceInfoBegin(nTongLevel, nTongLine)
	self.m_TongLevel	= nTongLevel
	self.m_TongLine		= nTongLine
	
	local tbl = {}
	for k, v in pairs(g_TongTechMgr.m_tblTechStat) do
		tbl[v] = {}
	end
	self.m_tblScienceState[1] = tbl
	
	for i, v in ipairs(self.m_tblScienceItem) do
		v:ChangeInfo(0, 0, 0)
	end
end

------创建建筑列表的行窗体
function CTongScienceWnd:CreateListItemWnd(parent, father)
	local wnd = CTongScienceListItem:new()
	wnd.m_fatherWnd = father
	wnd:Init(parent)
	wnd.m_CreateBtn:SetWndText(fl_TblBtnText["升级"])
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongScienceListItem:Init(parent)
	self.m_tblInfo		= nil
	self:CreateFromRes("TongTechListItem", parent)
	self:ShowWnd( true )
end

function CTongScienceListItem:OnChildCreated()
	self.m_CreateBtn	= self:GetDlgChild("CreateBtn")
	self.m_Progress		= self:GetDlgChild("Progress")
	self.m_Name			= self:GetDlgChild("Name")
	self.m_Icon			= self:GetDlgChild("Icon")
	
	self.m_tblNeedShow		= {}
	self.m_tblNeedShowLable	= {}
	for i = 1, 4 do
		self.m_tblNeedShow[i]		= self:GetDlgChild("NeedShow" .. i)
		self.m_tblNeedShowLable[i]	= self:GetDlgChild("NeedShowLable" .. i)
	end
end

function CTongScienceListItem:OnCreate()
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160041)
		return 
	end
	Gac2Gas:UpdateTongScience(g_Conn, self.name, g_GameMain.m_TongScience.m_CurType)
end

function CTongScienceListItem:CanUpgrade(sName, nNextLevel)
	local nNeedTongLevel	= g_TongTechMgr:GetFightTechNeedTongLevel(sName, nNextLevel)
	local nStationLine		= g_TongTechMgr:GetFightTechStationLine(sName, nNextLevel)
	return self.m_fatherWnd.m_TongLevel < nNeedTongLevel or self.m_fatherWnd.m_TongLine < nStationLine
end

function CTongScienceListItem:SetInfo(sName, nLevel, nMark, nPassedTime)
	self.name = sName
	self:ChangeInfo(nLevel, nMark, nPassedTime)
end

function CTongScienceListItem:ChangeInfo(nLevel, nMark, nPassedTime)
	local nNextLevel = nLevel + 1
	if( g_TongTechMgr:HasFightTechInfo(self.name, nNextLevel) or 0 == nLevel ) then --未满级
		local nNeedTongLevel	= g_TongTechMgr:GetFightTechNeedTime(self.name, nNextLevel)
		local nTime				= g_TongTechMgr:GetFightTechNeedTime(self.name, nNextLevel)
		local nMoney			= g_TongTechMgr:GetFightTechNeedTongMoney(self.name, nNextLevel)
		local nRes				= g_TongTechMgr:GetFightTechNeedRes(self.name, nNextLevel)
		local nExploit			= 0
		local nSmallIcon		= g_TongTechMgr:GetFightTechSmallIcon(self.name, nNextLevel)
		
		self.m_Name:SetWndText( Lan_TongFightTech_Common( MemH64(self.name .. nNextLevel), "DisplayName") )
		if( nMark == g_TongTechMgr:GetStateIndex("Normal") ) then
			local bFlag = self:CanUpgrade(self.name, nNextLevel)
			self.m_CreateBtn:EnableWnd(not bFlag)
			self.m_CreateBtn:SetWndText(bFlag and fl_TblBtnText["条件不足"] or fl_TblBtnText["升级"])
			self.m_Progress:ShowWnd(false)
		elseif( nMark == g_TongTechMgr:GetStateIndex("Researching") ) then
			self.m_CreateBtn:EnableWnd(false)
			self.m_CreateBtn:SetWndText(fl_TblBtnText["升级中"])
			self.m_Progress:ShowWnd(true)
			g_GameMain.m_TongScience.m_ItemBar = self.m_Progress
		elseif( nMark == g_TongTechMgr:GetStateIndex("Waiting") ) then
			self.m_CreateBtn:EnableWnd(false)
			self.m_CreateBtn:SetWndText(fl_TblBtnText["排队中"])
			self.m_Progress:ShowWnd(false)
		elseif( nMark == g_TongTechMgr:GetStateIndex("Stay") ) then
			local bFlag = self:CanUpgrade(self.name, nNextLevel)
			self.m_CreateBtn:EnableWnd(not bFlag)
			self.m_CreateBtn:SetWndText(bFlag and fl_TblBtnText["条件不足"] or fl_TblBtnText["继续升级"])
			self.m_Progress:ShowWnd(true)
			self.m_Progress:SetRange(nTime)
			self.m_Progress:SetPos(nPassedTime)
		end
		
		local sTime		= g_CTimeTransMgr:FormatTime(nTime)
		local sMoney	= g_MoneyMgr:ChangeMoneyToString(nMoney)
		local tblTemp	= {	{ GetStaticTextClient(10022), nTime, sTime },
							{ GetStaticTextClient(10023), nRes },
							{ GetStaticTextClient(10024), nExploit },
							{ GetStaticTextClient(10025), nMoney, sMoney }	}
		
		local a = 1
		for i = 1, #tblTemp do
			self.m_tblNeedShow[i]:ShowWnd(false)
			self.m_tblNeedShowLable[i]:ShowWnd(false)
			local bFlag = ( 0 ~= tblTemp[i][2] )
			if( bFlag ) then
				self.m_tblNeedShow[a]:SetWndText( tblTemp[i][3] or tblTemp[i][2] )
				self.m_tblNeedShowLable[a]:SetWndText( tblTemp[i][1] )
				self.m_tblNeedShow[a]:ShowWnd(true)
				self.m_tblNeedShowLable[a]:ShowWnd(true)
				a = a + 1
			end
		end
		
		g_LoadIconFromRes(nSmallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	else --满级
		self.m_Name:SetWndText( Lan_TongFightTech_Common(MemH64(self.name .. nLevel), "DisplayName") )
		self.m_CreateBtn:EnableWnd(false)
		self.m_CreateBtn:SetWndText(fl_TblBtnText["满级"])
		self.m_Progress:ShowWnd(false)
		for i = 1, 4 do
			self.m_tblNeedShow[i]:ShowWnd(false)
			self.m_tblNeedShowLable[i]:ShowWnd(false)
		end
	end
	
	self.m_Icon:SetMouseOverDescAfter( g_Tooltips:GetTongFightTechTips(self.name, nLevel) )
end
CTongLogWnd			= class(SQRDialog)	--导出类
CTongLogListItem	= class(SQRDialog)

function CTongLogWnd.CreateTongLogWnd(parent)
	local wnd = CTongLogWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongLogWnd:Init(parent)
	self.m_tblTongLog = {}
	self:CreateFromRes("TongLog", parent)
end

function CTongLogWnd:OnChildCreated()
	self.m_LogList	= self:GetDlgChild("LogList")
	self.m_All			= self:GetDlgChild("All")
	self.m_Member		= self:GetDlgChild("Member")
	self.m_Building	= self:GetDlgChild("Building")
	self.m_War			= self:GetDlgChild("War")
	self.m_Science	= self:GetDlgChild("Science")
	self.m_Mission	= self:GetDlgChild("Mission")
	self.m_Bag			= self:GetDlgChild("Bag")
	self.m_FrontBtn	= self:GetDlgChild("FrontBtn")
	self.m_BackBtn	= self:GetDlgChild("BackBtn")
	self.m_NowPage	= 0
	self.m_tblNowLogList	= {}
end

--消息
function CTongLogWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_All) then
			self:GetTongLog(0)
		elseif(Child == self.m_Member) then
			self:GetTongLog(1)
		elseif(Child == self.m_Building) then
			self:GetTongLog(2)
		elseif(Child == self.m_War) then
			self:GetTongLog(3)
		elseif(Child == self.m_Science) then
			self:GetTongLog(4)
		elseif(Child == self.m_Mission) then
			self:GetTongLog(5)
		elseif(Child == self.m_Bag) then
			self:GetTongLog(6)
		elseif(Child == self.m_FrontBtn) then
			self:ChangePage(-1)
		elseif(Child == self.m_BackBtn) then
			self:ChangePage(1)
		end
	end
end

function CTongLogWnd:OpenPanel(bFlag)
	if(bFlag) then
		self:GetTongLog(0)
	end
	self:ShowWnd(bFlag)
end

function CTongLogWnd:GetTongLog(iType)
	self.m_tblTongLog = {}
	self.m_NowPage = 0
	Gac2Gas:GetTongLog(g_Conn, iType)
end

----------------------------------------------------------------
local uListCountOnePage = 11
function CTongLogWnd:ChangePage(uAdd)
	if self.m_NowPage == 0 and uAdd < 0 then
		return
	end
	local uAllLogCount = # self.m_tblTongLog
	if (self.m_NowPage+1)*uListCountOnePage >= uAllLogCount and uAdd > 0 then
		return
	end

	self.m_NowPage = self.m_NowPage + uAdd
	self.m_tblNowLogList = {}
	local uNextBeginIndex = self.m_NowPage*uListCountOnePage + 1
	local uBackCount = uAllLogCount - self.m_NowPage*uListCountOnePage
	local uNextEndIndex = uBackCount > uListCountOnePage and (uNextBeginIndex + uListCountOnePage) or uAllLogCount
	for i=uNextBeginIndex, uNextEndIndex do
		table.insert(self.m_tblNowLogList,self.m_tblTongLog[i])
	end
	self:DrawLogList()
end

function CTongLogWnd:DrawLogList()
	self.m_LogList:DeleteAllItem()
	self.tblLogItem = {}
	self.m_LogList:InsertColumn(0, self.m_LogList:GetWndOrgWidth())
	local tbl = self.m_tblNowLogList
	for i = 1, #tbl do
		self.m_LogList:InsertItem(i-1, 30)
		local item = self.m_LogList:GetSubItem(i-1, 0)
		local logItem = self:CreateTongLogListItemWnd(item)
		table.insert(self.tblLogItem, logItem)
		logItem:SetTongLogText(tbl[i][1],tbl[i][2],tbl[i][3])
	end
end

function CTongLogWnd:ReturnGetTongLog(iData, iType, sDes)
	local textFilter = CTextFilterMgr:new()
	sDes = textFilter:GetRealStringByMessageID(sDes)
	local tbl = {iData, iType, sDes}
	table.insert(self.m_tblTongLog, tbl)
end

------创建日志列表的行窗体
function CTongLogWnd:CreateTongLogListItemWnd(Parent)
	local Wnd = CTongLogListItem:new()
	Wnd:CreateFromRes("TongLogListItem", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongLogListItem:OnChildCreated()
	self.m_Date = self:GetDlgChild("Date")
	self.m_Type = self:GetDlgChild("Type")
	self.m_Description = self:GetDlgChild("Description")
end

function CTongLogListItem:SetTongLogText(iData, iType, sDes)
	local tblLogType	= {	GetStaticTextClient(10008),		--人员
							GetStaticTextClient(10009),		--建设
							GetStaticTextClient(10010),		--战争
							GetStaticTextClient(10011),		--科技
							GetStaticTextClient(10012),		--任务
							GetStaticTextClient(10013)	}	--仓库
	local type			= tblLogType[iType]
	
	local sYear			= os.date("%Y", iData)
	local sMonth		= os.date("%m", iData)
	local sDay			= os.date("%d", iData)
	local sTime			= os.date("%X", iData)
	local date			= GetStaticTextClient(10014, sYear, sMonth, sDay, sTime)
	
	self.m_Date:SetWndText(date)
	self.m_Type:SetWndText(type)
	self.m_Description:SetWndText(sDes)
end

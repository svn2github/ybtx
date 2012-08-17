CFbGameStatisticsWnd = class(SQRDialog)

function CFbGameStatisticsWnd:Ctor(parent)
	self:CreateFromRes("FbGameStatisticsWnd",parent)
	self.m_Title = self:GetDlgChild("Title")
	self.m_TitleList = self:GetDlgChild("TitleList")
	self.m_DataList = self:GetDlgChild("DataList")
	self.m_ExitBtn = self:GetDlgChild("ExitBtn")
end

function CFbGameStatisticsWnd:GetWnd()
	local Wnd = g_GameMain.m_FbGameStatisticsWnd
	if not Wnd then
		Wnd = CFbGameStatisticsWnd:new(g_GameMain)
		g_GameMain.m_FbGameStatisticsWnd = Wnd
	end
	return Wnd
end

function CFbGameStatisticsWnd:CountDownTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime < 0 then
		self:Close()
		return
	end
	self.m_ExitBtn:SetWndText(GetStaticTextClient(1012, self.m_CountDownTime))
end

function CFbGameStatisticsWnd:InitWnd(caption, itemTbl, itemWidthTbl, dataTbl, exitTime, selIdex)
	self.m_Title:SetWndText(caption)
	self.m_DataList:DeleteAllItem()
	self.m_TitleList:DeleteAllItem()
	self.m_CountDownTime = exitTime
	
	local itemCount = #itemTbl
	if itemCount ~= 0 then
		
		if itemWidthTbl == nil then
			itemWidthTbl = {}
			for i = 1,  itemCount do
				itemWidthTbl[i] = 1
			end
		end
		assert(itemCount== #itemWidthTbl)
		local AllWidth = 0
		for i = 1,  itemCount do
				AllWidth = AllWidth + itemWidthTbl[i]
		end
		assert(AllWidth ~= 0)
	
		local rt = CFRect:new()
		self.m_DataList:GetLogicRect(rt)
		local wndWidth = rt.right-rt.left
	
		for i = 1, itemCount do
			local width = (itemWidthTbl[i]/AllWidth) * wndWidth
			self.m_TitleList:InsertColumn(i -1, width)
			self.m_DataList:InsertColumn(i - 1, width)
		end
		self.m_TitleList:InsertItem(0, 30)
		for i, name in ipairs(itemTbl) do
			self.m_TitleList:GetSubItem(0, i - 1):SetWndText(name)
		end
		local row = 0
		local col = 0
		for _, data in ipairs(dataTbl) do
			local insertPos = row
			if row == selIdex - 1 then
				insertPos = 0
			end
			if col == 0 then
				self.m_DataList:InsertItem(insertPos, 25)
			end
	
			self.m_DataList:GetSubItem(insertPos, col):SetWndText(data)
			col = col + 1
			if col == itemCount then
				col = 0
				row = row + 1
			end
		end
		
	end
	
	self.m_ExitBtn:SetWndText(GetStaticTextClient(1012, self.m_CountDownTime))
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	self.m_CountDownTick = RegClassTick("StatisticsWndCountDownTick", self.CountDownTick, 1000, self)
	self:ShowWnd(true)
end

function CFbGameStatisticsWnd:Close()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	self:ShowWnd(false)
end

function CFbGameStatisticsWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ExitBtn then
			Gac2Gas:NotJoinFbAction(g_Conn, self.m_GameName)
			self:Close()
		end
	end
end

function CFbGameStatisticsWnd:ShowMatchGameStatisticsWnd(gameName, dataList, exitTime, selIndex)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	local ShowStr =  GetStaticTextClient(9403)
	local caption = g_GetFbActionNameLanStr(gameName) .. " " .. ShowStr .. ":"
	local itemList = Lan_MatchGame_Common(MemH64(gameName), "StatisticsShow")
	local itemWidthList = MatchGame_Common(gameName, "ItemWidth")
	local itemTbl = GetCfgTransformValue(true, "Lan_MatchGame_Common", MemH64(gameName), "StatisticsShow")
	local itemWidthTbl = nil
	if itemWidthList ~= "" then
		itemWidthTbl = GetCfgTransformValue(true, "MatchGame_Common", gameName, "ItemWidth")
	end
	local dataTbl, msg = loadstring(" return { " .. dataList .. " }")
	if not dataTbl then
		LogErr("活动统计数据有问题 "..msg, "活动名:" .. gameName .. "  数据:" .. dataList)
		return
	end
	self.m_GameName = gameName
	self:InitWnd(caption, itemTbl, itemWidthTbl, dataTbl() , exitTime, selIndex)
end

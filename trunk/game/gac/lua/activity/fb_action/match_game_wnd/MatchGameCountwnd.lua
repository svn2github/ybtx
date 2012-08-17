gac_gas_require "activity/quest/LoadMatchGame"

local ItemHeights = 20
CMatchGameCountWnd = class(SQRDialog)

--创建小副本记分板
function CMatchGameCountWnd:Ctor()
	self:CreateFromRes("FbActionWnd",g_GameMain)

	self.m_TitleWnd = self:GetDlgChild("Title")
	self.m_ShowInfoBtn = self:GetDlgChild("ShowWndBtn")
	self.m_ShowInfoBtn:SetCheck(true)
	
	self.m_InfoWnd = SQRDialog:new()
	self.m_InfoWnd:CreateFromRes("FbActionInfoWnd", self)
	self.m_InfoWnd:ShowWnd( true )
	self.m_CountInfoList = self.m_InfoWnd:GetDlgChild("ListInfo")
end

function CMatchGameCountWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ShowInfoBtn then
			if self.m_InfoWnd then
				if self.m_InfoWnd:IsShow() then
					self.m_ShowInfoBtn:SetCheck(false)
					self.m_InfoWnd:ShowWnd(false)
				else
					self.m_ShowInfoBtn:SetCheck(true)
					self.m_InfoWnd:ShowWnd(true)
				end
			end
		end
	end
end

local function SetWndHeight( CurrWnd, ItemHeight, ItemCount)
	local height = ItemHeight * ItemCount + 10
	local rt = CFRect:new()
	CurrWnd.m_InfoWnd:GetLogicRect(rt)
	rt.bottom = rt.top + height
	CurrWnd.m_InfoWnd:SetLogicRect(rt)
	CurrWnd.m_CountInfoList:SetLogicRect(rt)
end

local function GetWndWidth(ChildWnd)
	local rt = CFRect:new()
	ChildWnd:GetLogicRect(rt)
	local width = rt.right-rt.left
	return width
end

function CMatchGameCountWnd:SetTitleText(str)
	if self.m_TitleText then
		self.m_TitleWnd:SetWndText(self.m_TitleText)
	else
		self.m_TitleWnd:SetWndText(str)
	end
end

function CMatchGameCountWnd:SetTime(Time)
	if self.m_ShowTimeTick then
		UnRegisterTick( self.m_ShowTimeTick) 
		self.m_ShowTimeTick = nil
	end
	
	if g_GameMain.m_FbActionMgrWnd.m_DownTimeTick then
		UnRegisterTick(g_GameMain.m_FbActionMgrWnd.m_DownTimeTick)
		g_GameMain.m_FbActionMgrWnd.m_DownTimeTick = nil
	end
	
	local pstTime = Time
	
	local function TimeTick()
		if g_MainPlayer then
			local tbl =  g_MonsterComeTimeTbl[self.m_GameName]
			
			if self.m_TimeType == 2 then
				pstTime = pstTime - 1
				if tbl then
					for i, v in ipairs(tbl) do
						if v[2] == nil then
							self:SetTitleText(GetStaticTextClient(v[1]))
						elseif (tbl.m_TotalTime - pstTime) < v[2] then
							self:SetTitleText(GetStaticTextClient(v[1] ,v[2] - (tbl.m_TotalTime - pstTime)))
							break
						end
					end
				else
					self:SetTitleText(GetStaticTextClient(1017, pstTime))
				end
				
				if pstTime == 0 then
					if self.m_ShowTimeTick then
						UnRegisterTick( self.m_ShowTimeTick)
						self.m_ShowTimeTick = nil
					end
					return
				end
				
				if pstTime <= 5 then
					MsgClient(191017,pstTime)
				elseif pstTime == 20 then
					MsgClient(191017,pstTime)
				elseif pstTime == 60 then
					if Time > 90 then
						MsgClient(191018,1)
					end
				elseif pstTime == 180 then
					if Time > 270 then
						MsgClient(191018,3)
					end
				end
			else
				pstTime = pstTime + 1
				if tbl then
					for i, v in pairs(tbl) do
						if v[2] == nil then
							self:SetTitleText(GetStaticTextClient(v[1]))
						elseif (pstTime) < v[2] then
							self:SetTitleText(GetStaticTextClient(v[1] ,v[2] - (pstTime)))
							break
						end
					end
				else
					self:SetTitleText(GetStaticTextClient(1018, pstTime))
				end
			end
		end
	end
	
	self.m_ShowTimeTick = RegisterTick( "MatchGameShowTime", TimeTick, 1000)
end

local function SetWndWidth( CurrWnd, WidthTbl)
	local ItemWidth = 0
	for i=1, #(WidthTbl) do
		ItemWidth = ItemWidth + WidthTbl[i]
	end
	local rt = CFRect:new()
	CurrWnd.m_InfoWnd:GetLogicRect(rt)
	rt.left = rt.right - ItemWidth
	--rt.right = rt.left + ItemWidth
	CurrWnd.m_InfoWnd:SetLogicRect(rt)
	CurrWnd.m_CountInfoList:SetLogicRect(rt)
end

function CMatchGameCountWnd:InitListWnd(itemTbl, WidthTbl)
	local ItemCount = #itemTbl
	SetWndWidth( self, WidthTbl)
	
	local width = GetWndWidth( self.m_CountInfoList)
	self.m_CountInfoList:DeleteAllItem()
	
	for i = 1, ItemCount do
		self.m_CountInfoList:InsertColumn(i - 1, WidthTbl[i])	
	end
	self.m_CountInfoList:InsertItem(0, ItemHeights)
	for i = 1, ItemCount do
		self:SetListText(0, i - 1, itemTbl[i])
	end
end

function CMatchGameCountWnd:SetListText(ItemNum, ColNum, ColTitle)
	local Item = self.m_CountInfoList:GetSubItem(ItemNum,ColNum)
	Item:SetWndText(ColTitle)
end


function CMatchGameCountWnd:SetItemData(index, dataTbl)
	local count = self.m_CountInfoList:GetItemCount()
	if index >= count then
		self.m_CountInfoList:InsertItem(index, ItemHeights)
	end
	for i, v in ipairs(dataTbl) do
		local item = self.m_CountInfoList:GetSubItem(index,i-1)
		item:SetWndText(v)
	end
end

local function SortFun(e1, e2)
	local scoreId =  g_GameMain.m_MatchGameWnd.m_ScoreIndex
	if e1.DataTbl[scoreId] > e2.DataTbl[scoreId] then
		return true
	elseif e1.DataTbl[scoreId] == e2.DataTbl[scoreId] then
		return e1.LastScoreId < e2.LastScoreId
	end
	return false
end

function CMatchGameCountWnd:UpdateAllList()
	local oldCount = self.m_CountInfoList:GetItemCount()
	local showTeamCount = 0
	
	for _, team in ipairs(self.m_SortTbl) do
		if team.IsShow then
			showTeamCount = showTeamCount + 1
		end
	end
	if self.m_OrderIndex then
		table.sort(self.m_SortTbl, SortFun)
		local order = 1
		for i, team in ipairs(self.m_SortTbl) do
			if team.IsShow then
				if team.DataTbl[self.m_ScoreIndex] == 0 then
					team.DataTbl[self.m_OrderIndex] = showTeamCount
				else
					team.DataTbl[self.m_OrderIndex] = order
					order = order + 1
				end
			end
		end
	end
	
	local index = 1  -- 第一行为标题
	for _, team in ipairs(self.m_SortTbl) do
		if team.IsShow then
			self:SetItemData(index, team.DataTbl)
			index = index + 1
			if team.MemberTbl then
				for _, member in pairs(team.MemberTbl) do
					if member.IsShow then
						self:SetItemData(index, member.DataTbl)
						index = index + 1
					end
				end
			end
		end
	end
	
	local count = self.m_CountInfoList:GetItemCount()
	for i = 1, count - index do
		self.m_CountInfoList:DeleteItem(index + i)
	end
	if oldCount ~= index then
		SetWndHeight(self, ItemHeights, index)
	end
end

function CMatchGameCountWnd:MatchGameSetWaitTime(Time)
	self.m_WaitTime = Time
end

function CMatchGameCountWnd:ShowDownTime(ResidualTime, gameState)
	local function DownTime()
		local StrInfo = ""
		self.m_WaitTime = self.m_WaitTime - 1
		if self.m_WaitTime > 70 then
			StrInfo = GetStaticTextClient(8811, self.m_WaitTime-70)
		elseif self.m_WaitTime > 10 and self.m_WaitTime <= 70 then
			StrInfo = GetStaticTextClient(8817, self.m_WaitTime-5)
		elseif self.m_WaitTime > 0 and self.m_WaitTime <= 10 then
			StrInfo = GetStaticTextClient(8818)
			if self.m_WaitTime <= 5 then
				MsgClient(191005, self.m_WaitTime)
			end
		elseif self.m_WaitTime <= 0 then
			MsgClient(191006)
			if g_GameMain.m_FbActionMgrWnd.m_DownTimeTick then
				UnRegisterTick(g_GameMain.m_FbActionMgrWnd.m_DownTimeTick)
				g_GameMain.m_FbActionMgrWnd.m_DownTimeTick = nil
			end
		end
		self:SetTitleText(StrInfo)
	end
	
	if gameState == 0 then --准备开始
		if g_GameMain.m_FbActionMgrWnd.m_DownTimeTick then
			UnRegisterTick(g_GameMain.m_FbActionMgrWnd.m_DownTimeTick)
			g_GameMain.m_FbActionMgrWnd.m_DownTimeTick = nil
		end
		self.m_WaitTime = ResidualTime
		g_GameMain.m_FbActionMgrWnd.m_DownTimeTick = RegisterTick("FbActionMgrWndTick",DownTime,1000)
	elseif gameState == 1 then --比赛中
		self:SetTime(ResidualTime)
	end
end

--首次进入 小副本初始化记分界面
function CMatchGameCountWnd:InitMatchGameCountWnd(Conn, gameName, timeType, ResidualTime, gameState)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self.m_GameName = gameName
	self.m_TimeType = timeType
	self.m_WaitTime = 0
	self.m_TeamTbl = {}
	self.m_SortTbl = {}
	local itemTbl = GetCfgTransformValue(true, "MatchGame_Common", gameName, "ScoreBoard")
	for i,v in ipairs(itemTbl) do
		if v == "总分" then
			self.m_ScoreIndex = i   --按总分排序
		end
		if v == "排名" then
			self.m_OrderIndex = i
		end
		if v == "玩家名称" then
			self.m_NameIndex = i
		end
	end
	if self.m_OrderIndex and not self.m_ScoreIndex  then
		LogErr("需要排队的活动没有总分显示, 无法排序", gameName)
	end
	self.m_ScoreSequenceId = 1
	local itemTbl = GetCfgTransformValue(true, "Lan_MatchGame_Common", MemH64(gameName), "ScoreBoardShow")
	local WidthTbl = GetCfgTransformValue(true, "MatchGame_Common", gameName, "ScoreWndRect")
	
	self.m_TitleText = nil
	self:InitListWnd(itemTbl,WidthTbl)
	self:ShowDownTime(ResidualTime, gameState)
	self:ShowWnd( true)
end

--其他人进入小副本初始化记分界面
function CMatchGameCountWnd:MatchGameAddTeam(Conn, isShow, teamId, dataList)
	if not IsCppBound(g_MainPlayer) then
		return
	end

	local dataTbl, msg = loadstring(" return { " .. dataList .. " }")
	if not dataTbl then
		LogErr("MatchGameAddTeam "..msg, "活动名:" .. self.m_GameName .. "  数据:" .. dataList)
	end
	assert(self.m_TeamTbl[teamId] == nil)
	self.m_TeamTbl[teamId] = {}
	self.m_TeamTbl[teamId].IsShow = isShow
	self.m_TeamTbl[teamId].DataTbl = dataTbl()
	self.m_TeamTbl[teamId].TeamId = teamId
	self.m_TeamTbl[teamId].LastScoreId = 0
	table.insert(self.m_SortTbl, self.m_TeamTbl[teamId])
	
	self:UpdateAllList()
end

function CMatchGameCountWnd:MatchGameAddTeamMember(Conn, isShow, teamId, playerId, dataList)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local MemberTbl = self.m_TeamTbl[teamId].MemberTbl
	if not MemberTbl then
		MemberTbl = {}
		self.m_TeamTbl[teamId].MemberTbl = MemberTbl
	end
	assert(MemberTbl[playerId] == nil)
	local dataTbl, msg = loadstring(" return { " .. dataList .. " }")
	if not dataTbl then
		LogErr("MatchGameAddTeamMember "..msg, "活动名:" .. self.m_GameName .. "  数据:" .. dataList)
	end
	MemberTbl[playerId] = {}
	MemberTbl[playerId].IsShow = isShow
	MemberTbl[playerId].DataTbl = dataTbl()
	self:UpdateAllList()
end

function CMatchGameCountWnd:MatchGameRemoveTeam(Conn, teamId)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self.m_TeamTbl[teamId] = nil
	for i, v in pairs(self.m_SortTbl) do
		if v.TeamId == teamId then
			table.remove(self.m_SortTbl, i)
		end
	end
	self:UpdateAllList()
end

function CMatchGameCountWnd:MatchGameRemoveTeamMember(Conn, teamId, playerId)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self.m_TeamTbl[teamId].MemberTbl[playerId] = nil
	self:UpdateAllList()
end

function CMatchGameCountWnd:MatchGameShowTeam(Conn, teamId)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self.m_TeamTbl[teamId].IsShow = true
	self:UpdateAllList()
end

function CMatchGameCountWnd:MatchGameShowTeamMember(Conn, teamId, playerId, playerName)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local team = self.m_TeamTbl[teamId]
	team.IsShow = true
	team.MemberTbl[playerId].IsShow = true
	local nameIndex = self.m_NameIndex
	if nameIndex then
		team.MemberTbl[playerId].DataTbl[nameIndex] = playerName
	end
	self:UpdateAllList()
end

function CMatchGameCountWnd:MatchGameUpdateTeamScore(Conn, teamId, dataIndex, value, isUpdate)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self.m_TeamTbl[teamId].DataTbl[dataIndex] = value
	if dataIndex == self.m_ScoreIndex  then
		self.m_TeamTbl[teamId].LastScoreId = self.m_ScoreSequenceId
		self.m_ScoreSequenceId = self.m_ScoreSequenceId + 1
	end
	if isUpdate then
		self:UpdateAllList()
	end
end

function CMatchGameCountWnd:MatchGameUpdatePlayerScore(Conn, teamId, playerId, dataIndex, value, isUpdate)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local team = self.m_TeamTbl[teamId]
	team.IsShow = true
	team.MemberTbl[playerId].DataTbl[dataIndex] = value
	if isUpdate then
		self:UpdateAllList()
	end
end

--离开游戏的玩家
function CMatchGameCountWnd:CloseList(Conn)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self:ShowWnd( false)
	self.m_GameName = nil
	self.m_TimeType = nil
	self.m_TitleText = nil
	self.m_TeamTbl = nil
	self.m_SortTbl = nil
	self.m_ScoreSequenceId = nil
	self.m_ScoreIndex = nil
	self.m_OrderIndex = nil
	self.m_NameIndex = nil
	self.m_WaitTime = nil
	
	if self.m_ShowTimeTick then
		UnRegisterTick( self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	if g_GameMain.m_FbActionMgrWnd.m_DownTimeTick then
		UnRegisterTick(g_GameMain.m_FbActionMgrWnd.m_DownTimeTick)
		g_GameMain.m_FbActionMgrWnd.m_DownTimeTick = nil
	end
end

--游戏开始  开始计时   如果游戏类型为 3 则 time = 0
function CMatchGameCountWnd:BeginGame(Conn, time)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	self:SetTime(time)
end
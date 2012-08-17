gac_require "relation/tong/TongWarBattleCountWndInc"
gac_require "relation/tong/TongBattleCountWnd"

------帮会国战作战统计------

function CreateTongWarExitBtnWnd(Parent)
	local Wnd = CTongWarExitBtnWnd:new()
	Wnd:CreateFromRes("TongWarBattleCountBtnWnd", Parent)
	Wnd.m_ShowBtn = Wnd:GetDlgChild("ShowBtn")
	return Wnd
end

function CTongWarExitBtnWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ShowBtn then
			Gac2Gas:RequestShowTongWarBattleCountWnd(g_Conn)
		end
	end
end

function CTongWarExitBtnWnd:OpenPanel()
	self:ShowWnd(true)
end

function CTongWarExitBtnWnd:ClosePanel()
	self:ShowWnd(false)
end

function CreateTongWarBattleCountWnd(Parent)
	local Wnd = CTongWarBattleCountWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongWarBattleCountWnd:Init(Parent)
	self.m_MemberTbl = {}
	self.m_SortStateTbl = {}
	self:CreateFromRes("TongWarCountWnd", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongWarBattleCountWnd:OnChildCreated()
	self.m_MemberList = self:GetDlgChild("MemberList")
	self.m_MemberNameBtn = self:GetDlgChild("MemberNameBtn")
	self.m_LevelBtn = self:GetDlgChild("LevelBtn")
	self.m_KillNumBtn = self:GetDlgChild("KillNumBtn")
	self.m_ContiKillNumBtn = self:GetDlgChild("ContiKillNumBtn")
	self.m_DeathTimesBtn = self:GetDlgChild("DeathTimesBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_GetExploitBtn = self:GetDlgChild("GetExploitBtn")
	self.m_AmGetExploit = self:GetDlgChild("AmGetExploit")
	self.m_SsGetExploit = self:GetDlgChild("SsGetExploit")
	self.m_XsGetExploit = self:GetDlgChild("XsGetExploit")
	self.m_TongGetExploit = self:GetDlgChild("TongGetExploit")
end

function CTongWarBattleCountWnd:OpenPanel()
	Gac2Gas:GetTongWarMemberListInfo(g_Conn)
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
	self.m_InfoUpdateTick = RegClassTick("TongWarBattleInfoUpdateTick", self.InfoUpdateTick, 5 * 1000, self)
	self:ShowWnd(true)
end

function CTongWarBattleCountWnd:ClosePanel()
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
	self:ShowWnd(false)
end

function CTongWarBattleCountWnd:VirtualExcludeWndClosed()
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
end

function CTongWarBattleCountWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_XBtn then
			self:ClosePanel()
		elseif Child == self.m_MemberNameBtn then
			self:DrawSortedMemberList(1)
		elseif Child == self.m_LevelBtn then
			self:DrawSortedMemberList(2)
		elseif Child == self.m_KillNumBtn then
			self:DrawSortedMemberList(3)
		elseif Child == self.m_ContiKillNumBtn then
			self:DrawSortedMemberList(4)
		elseif Child == self.m_DeathTimesBtn then
			self:DrawSortedMemberList(5)
		elseif Child == self.m_GetExploitBtn then
			self:DrawSortedMemberList(6)
		end
	end
end

function CTongWarBattleCountWnd:InfoUpdateTick(tick)
	if tick then
		self:OpenPanel()
	end
end

function CTongWarBattleCountWnd:DrawMemberList()
	table.sort(self.m_MemberTbl, function(a, b) return a[1] < b[1] end)
	
	self.m_MemberList:DeleteAllItem()
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())
	
	self.m_ItemWndTbl = {}
	for i = 1, #self.m_MemberTbl do
		self.m_MemberList:InsertItem(i - 1, 30)
		local item = self.m_MemberList:GetSubItem(i - 1, 0)
		local itemWnd = self:CreateMemeberListWnd(item)
		itemWnd:SetInfo(self.m_MemberTbl[i])
		table.insert(self.m_ItemWndTbl, itemWnd)
	end
end

function CTongWarBattleCountWnd:DrawSortedMemberList(nCol)
	if not self.m_SortStateTbl[nCol] then
		table.sort(self.m_MemberTbl, function(a, b) return a[nCol] > b[nCol] end)
		self.m_SortStateTbl[nCol] = not self.m_SortStateTbl[nCol]
	else
		table.sort(self.m_MemberTbl, function(a, b) return a[nCol] < b[nCol] end)
		self.m_SortStateTbl[nCol] = not self.m_SortStateTbl[nCol]
	end
	
	self.m_MemberList:DeleteAllItem()
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())
	
	self.m_ItemWndTbl = {}
	for i = 1, #self.m_MemberTbl do
		self.m_MemberList:InsertItem(i - 1, 30)
		local item = self.m_MemberList:GetSubItem(i - 1, 0)
		local itemWnd = self:CreateMemeberListWnd(item)
		itemWnd:SetInfo(self.m_MemberTbl[i])
		table.insert(self.m_ItemWndTbl, itemWnd)
	end
end

function CTongWarBattleCountWnd:CreateMemeberListWnd(parent)
	local wnd = CreateTongBattleCountWnd(parent)
	return wnd
end

function Gas2Gac:OpenTongWarExitBtnWnd(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongWarExitBtnWnd:OpenPanel()
	end
end

function Gas2Gac:OpenTongWarBattleCountWnd(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongWarBattleCountWnd:OpenPanel()
	end
end

function Gas2Gac:ReturnCampGetExploitSum(Conn, nAmGetExploitSum, nSsGetExploitSum, nXsGetExploitSum)
	if g_MainPlayer then
		g_GameMain.m_TongWarBattleCountWnd:SetCampExploitInfo(nAmGetExploitSum, nSsGetExploitSum, nXsGetExploitSum)
	end
end

function Gas2Gac:ReturnTongGetExploitSum(Conn,nTongGetExploitSum)
	if g_MainPlayer then
		g_GameMain.m_TongWarBattleCountWnd:SetTongExploitInfo(nTongGetExploitSum)
	end
end

function CTongWarBattleCountWnd:SetTongExploitInfo(nTongGetExploitSum)
	self.m_TongGetExploit:SetWndText(nTongGetExploitSum)
end

function CTongWarBattleCountWnd:SetCampExploitInfo(nAmGetExploitSum, nSsGetExploitSum, nXsGetExploitSum)
	self.m_AmGetExploit:SetWndText(nAmGetExploitSum)
	self.m_SsGetExploit:SetWndText(nSsGetExploitSum)
	self.m_XsGetExploit:SetWndText(nXsGetExploitSum)
end

function Gas2Gac:CloseTongWarPanel(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongWarExitBtnWnd:ClosePanel()
		g_GameMain.m_TongWarBattleCountWnd:ClosePanel()
		g_GameMain.m_TongBattlePersonalCountWnd:ClosePanel()
		g_GameMain.m_TongMonsAttackCountWnd:ClosePanel()
		g_GameMain.m_TongMonsAttackExitBtnWnd:ClosePanel()
		g_GameMain.m_TongDownTimeWnd:ClosePanel()
		g_MainPlayer:ChangeWarModel()
	end
end
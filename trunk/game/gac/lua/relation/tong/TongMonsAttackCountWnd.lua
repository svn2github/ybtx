gac_require "relation/tong/TongMonsAttackCountWndInc"
gac_require "relation/tong/TongMonsAttackListWnd"

------帮会怪物攻城统计------

function CreateTongMonsAttackExitBtnWnd(Parent)
	local Wnd = CTongMonsAttackExitBtnWnd:new()
	Wnd:CreateFromRes("FbActionQueueBtnWnd", Parent)
	Wnd.m_ShowBtn = Wnd:GetDlgChild("ShowBtn")
	return Wnd
end

function CTongMonsAttackExitBtnWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ShowBtn then
			if g_MainPlayer then
				g_GameMain.m_TongMonsAttackCountWnd:OpenPanel()
			end
		end
	end
end

function CTongMonsAttackExitBtnWnd:OpenPanel()
	self:ShowWnd(true)
end

function CTongMonsAttackExitBtnWnd:ClosePanel()
	self:ShowWnd(false)
end

function CreateTongMonsAttackCountWnd(Parent)
	local Wnd = CTongMonsAttackCountWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongMonsAttackCountWnd:Init(Parent)
	self.m_MemberTbl = {}
	self.m_SortStateTbl = {}
	self:CreateFromRes("TongMonsAttackCountWnd", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongMonsAttackCountWnd:OnChildCreated()
	self.m_MemberNameBtn = self:GetDlgChild("MemberNameBtn")
	self.m_LevelBtn = self:GetDlgChild("LevelBtn")
	self.m_KillNumBtn = self:GetDlgChild("KillNumBtn")
	self.m_MemberList = self:GetDlgChild("MemberList")
	self.m_XBtn = self:GetDlgChild("XBtn")
end

function CTongMonsAttackCountWnd:OpenPanel()
	Gac2Gas:GetTongMonsAttackListInfo(g_Conn)
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
	self.m_InfoUpdateTick = RegClassTick("TongMonsAttackInfoUpdateTick", self.InfoUpdateTick, 5 * 1000, self)
	self:ShowWnd(true)
end

function CTongMonsAttackCountWnd:ClosePanel()
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
	self:ShowWnd(false)
end

function CTongMonsAttackCountWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_XBtn then
			self:ClosePanel()
		elseif Child == self.m_MemberNameBtn then
			self:DrawSortedMemberList(1)
		elseif Child == self.m_LevelBtn then
			self:DrawSortedMemberList(2)
		elseif Child == self.m_KillNumBtn then
			self:DrawSortedMemberList(3)
		end
	end
end

function CTongMonsAttackCountWnd:InfoUpdateTick(tick)
	if tick then
		self:OpenPanel()
	end
end

function CTongMonsAttackCountWnd:DrawMemberList()
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

function CTongMonsAttackCountWnd:DrawSortedMemberList(nCol)
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

function CTongMonsAttackCountWnd:CreateMemeberListWnd(parent)
	local wnd = CreateTongMonsAttackListWnd(parent)
	return wnd
end

function Gas2Gac:OpenTongMonsAttackCountWnd(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongMonsAttackCountWnd:OpenPanel()
	end
end

function Gas2Gac:OpenTongMonsAttackExitBtnWnd(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongMonsAttackExitBtnWnd:OpenPanel()
	end
end
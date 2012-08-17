gac_require "relation/tong/TongChallengeBattleCountWndInc"
gac_require "relation/tong/TongBattleCountWnd"

------帮会挑战作战统计面板------
function CreateTongChallengeExitBtnWnd(Parent)
	local Wnd = CTongChallengeExitBtnWnd:new()
	Wnd:CreateFromRes("FbActionQueueBtnWnd", Parent)
	Wnd.m_ShowBtn = Wnd:GetDlgChild("ShowBtn")
	return Wnd
end

function CTongChallengeExitBtnWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ShowBtn then
--			local function IsContinue(GameWnd, uButton)
--				if uButton == MB_BtnOK then
--					Gac2Gas:ExitTongChallenge(g_Conn)
--				end
--				return true
--			end
--			MessageBox(g_GameMain, GetStaticTextClient(9114), BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
			Gac2Gas:RequestShowTongChallengeBattleCountWnd(g_Conn)
		end
	end
end

function CTongChallengeExitBtnWnd:OpenPanel()
	self:ShowWnd(true)
end

function CTongChallengeExitBtnWnd:ClosePanel()
	self:ShowWnd(false)
end

function CreateTongChallengeBattleCountWnd(Parent)
	local Wnd = CTongChallengeBattleCountWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongChallengeBattleCountWnd:Init(Parent)
	self.m_MemberTbl = {}
	self.m_SortStateTbl = {}
	self:CreateFromRes("TongChallengeCountWnd", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongChallengeBattleCountWnd:OnChildCreated()
	self.m_MemberList = self:GetDlgChild("MemberList")
	self.m_MemberNameBtn = self:GetDlgChild("MemberNameBtn")
	self.m_LevelBtn = self:GetDlgChild("LevelBtn")
	self.m_KillNumBtn = self:GetDlgChild("KillNumBtn")
	self.m_ContiKillNumBtn = self:GetDlgChild("ContiKillNumBtn")
	self.m_DeathTimesBtn = self:GetDlgChild("DeathTimesBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_QuitBtn = self:GetDlgChild("QuitBtn")
end

function CTongChallengeBattleCountWnd:OpenPanel()
	Gac2Gas:GetTongChallengeMemberListInfo(g_Conn)
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
	self.m_InfoUpdateTick = RegClassTick("TongChallengeBattleInfoUpdateTick", self.InfoUpdateTick, 5 * 1000, self)
	self:ShowWnd(true)
end

function CTongChallengeBattleCountWnd:ClosePanel()
	if self.m_InfoUpdateTick then
		UnRegisterTick(self.m_InfoUpdateTick)
		self.m_InfoUpdateTick = nil
	end
	self:ShowWnd(false)
end

function CTongChallengeBattleCountWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_XBtn then
			self:ClosePanel()
		elseif Child == self.m_QuitBtn then
			Gac2Gas:ExitTongChallenge(g_Conn)
			self:ShowWnd(false)
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
		end
	end
end

function CTongChallengeBattleCountWnd:InfoUpdateTick(tick)
	if tick then
		self:OpenPanel()
	end
end

function CTongChallengeBattleCountWnd:DrawMemberList()
	table.sort(self.m_MemberTbl, function(a, b) return a[1] < b[1] end)
	
	self.m_MemberList:DeleteAllItem()
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())
	
	self.m_ItemWndTbl = {}
	for i = 1, #self.m_MemberTbl do
		self.m_MemberList:InsertItem(i - 1, 30)
		local item = self.m_MemberList:GetSubItem(i - 1, 0)
		local itemWnd = self:CreateMemberListWnd(item)
		itemWnd:SetInfo(self.m_MemberTbl[i])
		table.insert(self.m_ItemWndTbl, itemWnd)
	end
end

function CTongChallengeBattleCountWnd:DrawSortedMemberList(nCol)
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
		local itemWnd = self:CreateMemberListWnd(item)
		itemWnd:SetInfo(self.m_MemberTbl[i])
		table.insert(self.m_ItemWndTbl, itemWnd)
	end
end

function CTongChallengeBattleCountWnd:CreateMemberListWnd(parent)
	local wnd = CreateTongBattleCountWnd(parent)
	return wnd
end

function Gas2Gac:OpenTongChallengeExitBtnWnd(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongChallengeExitBtnWnd:OpenPanel()
	end
end

function Gas2Gac:OpenTongChallengeBattleCountWnd(Conn)
	if g_MainPlayer then
		g_GameMain.m_TongChallengeBattleCountWnd:OpenPanel()
	end
end
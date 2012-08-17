gac_require "relation/tong/TongWarBattleAwardListWnd"
CTongWarBattleAwardWnd = class(SQRDialog)

function CTongWarBattleAwardWnd:GetWnd()
	local Wnd = g_GameMain.m_TongWarBattleAwardWnd
	if not Wnd then
		Wnd = CTongWarBattleAwardWnd:new()
		Wnd:CreateFromRes("TongWarBattleAwardWnd", g_GameMain)
		Wnd:Init()
		g_GameMain.m_TongWarBattleAwardWnd = Wnd
	end
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, "·Ç»¥³â")
	return Wnd
end

function CTongWarBattleAwardWnd:Init()
	self.m_MemberTbl = {}
	self.m_tblSorBtn = {}
	self.m_MemberList = self:GetDlgChild("MemberList")
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_tblSorBtn["sPlayerName"] = self:GetDlgChild("TongMemNameBtn")
	self.m_tblSorBtn["nLevel"] = self:GetDlgChild("LevelBtn")
	self.m_tblSorBtn["nGetExploit"] = self:GetDlgChild("GetExploitBtn")
	self.m_tblSorBtn["nAddProffer"] = self:GetDlgChild("GetAwardBtn")
	self.m_tblSorBtn["nCampAward"] = self:GetDlgChild("ExtendAwardBtn")
	for k, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
end

function CTongWarBattleAwardWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_XBtn then
			self:ClosePanel()
		else
			for k, v in pairs(self.m_tblSorBtn) do
				if(Child == v) then
					self:DrawSortedMemberList(k, v.bFlag)
					v.bFlag = not v.bFlag
					break
				end
			end
		end
	end
end

function CTongWarBattleAwardWnd:DrawMemberList()	
	self.m_MemberList:DeleteAllItem()
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())
	
	self.m_ItemWndTbl = {}
	for i , v in ipairs(self.m_MemberTbl) do
		self.m_MemberList:InsertItem(i - 1, 30)
		local item = self.m_MemberList:GetSubItem(i - 1, 0)
		local itemWnd = self:CreateMemeberListWnd(item)
		itemWnd:SetInfo(v)
		table.insert(self.m_ItemWndTbl, itemWnd)
	end
end

function CTongWarBattleAwardWnd:DrawSortedMemberList(index, bFlag)
	if(bFlag) then
		table.sort(self.m_MemberTbl, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_MemberTbl, function (a, b) return a[index] > b[index] end)
	end
	self:DrawMemberList()
end

function CTongWarBattleAwardWnd:CreateMemeberListWnd(parent)
	local wnd = CreateTongWarBattleAwardListWnd(parent)
	return wnd
end

function CTongWarBattleAwardWnd:OpenPanel()
	self:ShowWnd(true)
end

function CTongWarBattleAwardWnd:ClosePanel()
	self:ShowWnd(false)
end


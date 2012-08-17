gac_require "relation/tong/TongBattleListWndInc"

------帮会挑战对战表面板------

function CreateTongBattleListWnd(Parent)
	local Wnd = CTongBattleListWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongBattleListWnd:Init(Parent)
	self.m_BattleListInfo = {}
	self:CreateFromRes("TongBattleListWnd", Parent)
	self:ShowWnd(false)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongBattleListWnd:OnChildCreated()
	self.m_BattleList = self:GetDlgChild("BattleList")
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
end

function CTongBattleListWnd:OpenPanel()
	Gac2Gas:GetTongBattleListInfo(g_Conn)
	self:ShowWnd(true)
end

--消息
function CTongBattleListWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn or Child == self.m_XBtn then
			self:ShowWnd(false)
		end
	end
end

function CTongBattleListWnd:DrawTongBattleList()
	self.m_BattleList:DeleteAllItem()
	self.m_BattleList:InsertColumn(0, self.m_BattleList:GetWndOrgWidth())
	local tbl = self.m_BattleListInfo
	for i = 1, #tbl do
		self.m_BattleList:InsertItem(i - 1, 30)
		local item = self.m_BattleList:GetSubItem(i - 1, 0)
		local itemWnd = self:CreateListItemWnd(item)
		itemWnd:SetInfo(tbl[i])
	end
end

function CTongBattleListWnd:CreateListItemWnd(parent)
	local wnd = CTongBattleListItem:new()
	wnd:CreateFromRes("TongChallengeListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CTongBattleListItem:OnChildCreated()
	self.m_TongName		= self:GetDlgChild("TongName")
	self.m_LeaderName	= self:GetDlgChild("LeaderName")
	self.m_WarZone		= self:GetDlgChild("WarZone")
	self.m_Station		= self:GetDlgChild("Station")
end

function CTongBattleListItem:SetInfo(tongInfo)
	local tongName = ("" == tongInfo[1]) and GetStaticTextClient(10032) or tongInfo[1]
	self.m_TongName:SetWndText(tongName)
	self.m_LeaderName:SetWndText(tongInfo[2])
	self.m_WarZone:SetWndText(tongInfo[3])
	self.m_Station:SetWndText(g_GameMain.m_TongBase:GetStationDes(tongInfo[3],tongInfo[4]))
end
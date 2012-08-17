gac_require "relation/tong/TongChallengeInc"

------帮会挑战------
function CreateTongChallengeWnd(Parent)
	local Wnd = CTongChallengeWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongChallengeWnd:Init(Parent)
	self.m_tblTongChanllengeInfo = {}
	self:CreateFromRes("TongChallenge", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongChallengeWnd:OnChildCreated()
	self.m_WarZoneLable	= self:GetDlgChild("WarZoneLable")
	self.m_StationLable	= self:GetDlgChild("StationLable")
	self.m_SelfWarZone	= self:GetDlgChild("SelfWarZone")
	self.m_SelfStation	= self:GetDlgChild("SelfStation")
	self.m_TongList		= self:GetDlgChild("TongList")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_BattleListBtn = self:GetDlgChild("BattleListBtn")
	self.m_XBtn			= self:GetDlgChild("XBtn")
end

function CTongChallengeWnd:OpenPanel()
	Gac2Gas:GetTongChallengeInfoList(g_Conn)
	self:ShowWnd(true)
end

function CTongChallengeWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			Gac2Gas:JudgeOpenChallenge(g_Conn)
		elseif Child == self.m_BattleListBtn then
			if(not g_GameMain.m_TongBattleList) then
				gac_require "relation/tong/TongBattleListWnd"
				g_GameMain.m_TongBattleList = CreateTongBattleListWnd(g_GameMain)
			end
			g_GameMain.m_TongBattleList:OpenPanel()
		end
	end
end

function CTongChallengeWnd:OnPreChallenge()
	local itemIndex = self.m_TongList:GetSelectItem(-1)+1
	if(not itemIndex or 1 > itemIndex) then return end
	local tbl = self.m_tblTongChanllengeInfo[itemIndex]
	self:ShowChallengeMessageBox(tbl[3], tbl[4]) --WarZoneId, WarZoneStationId
end

function CTongChallengeWnd:DrawTongChallengeList()
	local wzsDisplay = g_GameMain.m_TongBase:GetStationDes(self.m_SelfWz,self.m_SelfWzs)
	self.m_SelfWarZone:SetWndText(self.m_SelfWz)
	self.m_SelfStation:SetWndText(wzsDisplay)
	self.m_TongList:DeleteAllItem()
	self.m_TongList:InsertColumn(0, self.m_TongList:GetWndOrgWidth())
	local tbl = self.m_tblTongChanllengeInfo
	self.m_tblTongChanllengeItem = {}
	for i = 1, #tbl do
		self.m_TongList:InsertItem(i-1, 30)
		local item = self.m_TongList:GetSubItem(i-1, 0)
		local itemWnd = self:CreateListItemWnd(item)
		table.insert(self.m_tblTongChanllengeItem, itemWnd)
		itemWnd:SetInfo(tbl[i])
	end
end

function CTongChallengeWnd:ShowChallengeMessageBox(WarZoneId, WarZoneStationId)
	if(not g_GameMain.m_TongChallengeMsgBox) then
		g_GameMain.m_TongChallengeMsgBox = CreateTongChallengeMsgBoxWnd(g_GameMain)
	end
	g_GameMain.m_TongChallengeMsgBox:OpenPanel(WarZoneId, WarZoneStationId)
end

-------------------------------------------------------------
function CTongChallengeWnd:CreateListItemWnd(parent)
	local wnd = CTongChallengeListItem:new()
	wnd:CreateFromRes("TongChallengeListItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CTongChallengeListItem:OnChildCreated()
	self.m_TongName		= self:GetDlgChild("TongName")
	self.m_LeaderName	= self:GetDlgChild("LeaderName")
	self.m_WarZone		= self:GetDlgChild("WarZone")
	self.m_Station		= self:GetDlgChild("Station")
end

function CTongChallengeListItem:SetInfo(tongInfo)
	local tongName = ("" == tongInfo[1]) and GetStaticTextClient(10032) or tongInfo[1]
	self.m_TongName:SetWndText(tongName)
	self.m_LeaderName:SetWndText(tongInfo[2])
	self.m_WarZone:SetWndText(tongInfo[3])
	self.m_Station:SetWndText(g_GameMain.m_TongBase:GetStationDes(tongInfo[3],tongInfo[4]))
end


--********************************************************************
--挑战对话框
--********************************************************************
function CreateTongChallengeMsgBoxWnd(parent)
	local wnd = CTongChallengeMsgBox:new()
	wnd:Init(parent)
	return wnd
end

function CTongChallengeMsgBox:Init(parent)
	self.m_tblTongChanllengeInfo = {}
	self:CreateFromRes("TongChallengeMsgBox", parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 3, false, nil, 1)
end

function CTongChallengeMsgBox:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_TongName		= self:GetDlgChild("TongName")
	self.m_Gold			= self:GetDlgChild("Gold")
	self.m_Silver		= self:GetDlgChild("Silver")
	self.m_Copper		= self:GetDlgChild("Copper")
	
	self.m_Gold:SetMaxTextLenght(8)
	self.m_Silver:SetMaxTextLenght(2)
	self.m_Copper:SetMaxTextLenght(2)
end

function CTongChallengeMsgBox:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			self:OnChallenge()
		end
	end
end

function CTongChallengeMsgBox:OnChallenge()
	local money = self:TransMoney()
	if money < 100000 then
		MsgClient(9445)
		return
	end
	Gac2Gas:SendChallenge(g_Conn, self.m_WarZoneId, self.m_WarZoneStationId, tonumber(money))
	self:ShowWnd(false)
end

function CTongChallengeMsgBox:OpenPanel(WarZoneId, WarZoneStationId)
	self.m_WarZoneId = WarZoneId
	self.m_WarZoneStationId = WarZoneStationId
	self.m_TongName:SetWndText(g_GameMain.m_TongBase:GetStationDes(WarZoneId, WarZoneStationId))
	self:SetFocus()
	self.m_Gold:SetWndText(0)
	self.m_Silver:SetWndText(0)
	self.m_Copper:SetWndText(0)
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CTongChallengeMsgBox:TransMoney()
	local gold		= tonumber( self.m_Gold:GetWndText() ) or 0
	local silver	= tonumber( self.m_Silver:GetWndText() ) or 0
	local copper	= tonumber( self.m_Copper:GetWndText() ) or 0
	return math.floor( gold*10000 + silver*100 + copper ) 
end

function Gas2Gac:ReturnTongLevelInfo(Conn, Flag)
	if not Flag then
		MsgClient(9447)
		return
	end
	g_GameMain.m_TongChallenge:OnPreChallenge()
end

CGameSettingWnd = class(SQRDialog)

function CGameSettingWnd:Ctor(parent)
	self:CreateFromRes("GameSetting", parent)
end

function CGameSettingWnd:OnChildCreated()
	--状态设置，1为选中，0为未选中
	self.m_uTeamState 	= 1
	self.m_uTransState 	= 1
	self.m_uGroupState 	= 1
	self.m_uFriendState = 1
	self.m_uTroopState 	= 1
	self.m_uAutoMakeTeamState 	= 1
	
	self.m_TeamBtn			= self:GetDlgChild("TeamCBtn")
	self.m_TranCBtn			= self:GetDlgChild("TranCBtn")
	self.m_GroupCBtn		= self:GetDlgChild("GroupCBtn")
	self.m_FriendCBtn		= self:GetDlgChild("FriendCBtn")
	self.m_TroopCBtn		= self:GetDlgChild("TroopCBtn")
	
	
	self.m_uSelectQuestSortState = 3--1表示按等级排序,2表示按类型排序,3表示按区域排序
	self.m_SelectQuestSortCBtn = {}
	for i=1, 3 do
		self.m_SelectQuestSortCBtn[i] = self:GetDlgChild("Sort"..i.."CBtn")
	end
	
	self.m_LangCB		= self:GetDlgChild("LangCB")
	self:InitLangCB()
	
	self.m_AutoMakeTeamCBtn = self:GetDlgChild("AutoMakeTeamCBtn")
	self.m_AutoMakeTeamText = self:GetDlgChild("AutoMakeTeamText")
	self.m_AutoMakeTeamCBtn:ShowWnd(false)
	self.m_AutoMakeTeamText:ShowWnd(false)
end

function CGameSettingWnd:InitLangCB()
	local lanCount = CLanguageSetting_Inst():GetLanguageCount()
	for i = 1, lanCount do
		local lanName = CLanguageSetting_Inst():GetLanguageName(i-1)
		self.m_LangCB:InsertItem(lanName, i-1, -1)
	end
	
	local curLan = CLanguageSetting_Inst():GetCurLanIndex();
	self.m_LangCB:SetSel(curLan);
	
end

function CGameSettingWnd:GetLangType()
	return self.m_LangCB:GetCurrentSel()
end


function CGameSettingWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_TeamBtn) then
			if self.m_uTeamState == 1 then
				self.m_uTeamState = 0 
				self.m_TeamBtn:SetCheck(false)
			elseif self.m_uTeamState == 0 then
				self.m_uTeamState = 1 
				self.m_TeamBtn:SetCheck(true)
			end
		elseif(Child == self.m_TranCBtn) then
			if self.m_uTransState == 1 then
				self.m_uTransState = 0 
				self.m_TranCBtn:SetCheck(false)
			elseif self.m_uTransState == 0 then
				self.m_uTransState = 1 
				self.m_TranCBtn:SetCheck(true)
			end
		elseif(Child == self.m_GroupCBtn) then
			if self.m_uGroupState == 1 then
				self.m_uGroupState = 0 
				self.m_GroupCBtn:SetCheck(false)
			elseif self.m_uGroupState == 0 then
				self.m_uGroupState = 1 
				self.m_GroupCBtn:SetCheck(true)
			end
		elseif(Child == self.m_FriendCBtn) then
			if self.m_uFriendState == 1 then
				self.m_uFriendState = 0 
				self.m_FriendCBtn:SetCheck(false)
			elseif self.m_uFriendState == 0 then
				self.m_uFriendState = 1 
				self.m_FriendCBtn:SetCheck(true)
			end
		elseif(Child == self.m_TroopCBtn) then
			if self.m_uTroopState == 1 then
				self.m_uTroopState = 0 
				self.m_TroopCBtn:SetCheck(false)
			elseif self.m_uTroopState == 0 then
				self.m_uTroopState = 1 
				self.m_TroopCBtn:SetCheck(true)
			end
		elseif(Child == self.m_AutoMakeTeamCBtn) then
			if self.m_uAutoMakeTeamState == 1 then
				self.m_uAutoMakeTeamState = 0 
				self.m_AutoMakeTeamCBtn:SetCheck(false)
			elseif self.m_uAutoMakeTeamState == 0 then
				self.m_uAutoMakeTeamState = 1 
				self.m_AutoMakeTeamCBtn:SetCheck(true)
			end
		else
			for i=1, 3 do
				if(Child == self.m_SelectQuestSortCBtn[i]) then
					self.m_uSelectQuestSortState = i
					self.m_SelectQuestSortCBtn[i]:SetCheck(true)
					break
				end
			end
		end
	end
end

--设置
function CGameSettingWnd:GameSetting()
	-- 设置语言
	--local langType = self:GetLangType()
	--CLanguageSetting_Inst():ChangeLanguage(langType)
	
	Gac2Gas:GameSetting(g_Conn,self.m_uTeamState,self.m_uTransState,self.m_uGroupState,self.m_uFriendState,self.m_uTroopState,self.m_uAutoMakeTeamState,self.m_uSelectQuestSortState)
	self:QuestSortSetting(self.m_uSelectQuestSortState)
end

--恢复默认设置
function CGameSettingWnd:SetDefault()
	self.m_uTeamState = 1
	self.m_uTransState = 1
	self.m_uGroupState = 1
	self.m_uFriendState = 1
	self.m_uTroopState = 1
	self.m_uAutoMakeTeamState = 1
	
	--默认都为选中状态
	self.m_TeamBtn:SetCheck(true)
	self.m_TranCBtn:SetCheck(true)
	self.m_GroupCBtn:SetCheck(true)
	self.m_FriendCBtn:SetCheck(true)
	self.m_TroopCBtn:SetCheck(true)
	self.m_AutoMakeTeamCBtn:SetCheck(true)
	
	self.m_uSelectQuestSortState = 3
	self.m_SelectQuestSortCBtn[3]:SetCheck(true)
	self:QuestSortSetting(self.m_uSelectQuestSortState)
	
	Gac2Gas:GameSetting(g_Conn,self.m_uTeamState,self.m_uTransState,self.m_uGroupState,self.m_uFriendState,self.m_uTroopState,self.m_uAutoMakeTeamState,self.m_uSelectQuestSortState)
end

function CGameSettingWnd:QuestSortSetting(SortType)
	if g_GameMain.m_QuestTraceBack.m_SortType ~= SortType then
		g_GameMain.m_QuestTraceBack.m_SortType = SortType
		g_GameMain.m_HandBookWnd.m_SortType = SortType
		g_GameMain.m_QuestTraceBack:ResetSortQuestList()
	end
end

function Gas2Gac:RetQuestSortSettingInfo(Conn,uQuestSortSet)
	g_GameMain.m_SysSetting.m_GameSettingWnd:QuestSortSetting(uQuestSortSet)
end

--返回游戏设置信息
function Gas2Gac:ReturnGetGameSettingInfo(Conn,uTeamSet,uTransSet,uGroupSet,uFriendSet,uTroopSet,uAutoMakeTeamSet,uQuestSortSet)
	local Wnd = g_GameMain.m_SysSetting.m_GameSettingWnd
	Wnd.m_uTeamState 	= uTeamSet
	Wnd.m_uTransState 	= uTransSet
	Wnd.m_uGroupState 	=	uGroupSet
	Wnd.m_uFriendState = uFriendSet
	Wnd.m_uTroopState 	= uTroopSet
	Wnd.m_uAutoMakeTeamState 	= uAutoMakeTeamSet
	Wnd.m_uSelectQuestSortState = uQuestSortSet
	
	local function ToBlooean(nIndex)
		return 0 ~= nIndex
	end
	
	Wnd.m_TeamBtn:SetCheck(ToBlooean(Wnd.m_uTeamState))

	Wnd.m_TranCBtn:SetCheck(ToBlooean(Wnd.m_uTransState))

	Wnd.m_GroupCBtn:SetCheck(ToBlooean(Wnd.m_uGroupState))

	Wnd.m_FriendCBtn:SetCheck(ToBlooean(Wnd.m_uFriendState))

	Wnd.m_TroopCBtn:SetCheck(ToBlooean(Wnd.m_uTroopState))

	Wnd.m_AutoMakeTeamCBtn:SetCheck(ToBlooean(Wnd.m_uAutoMakeTeamState))
	
	Wnd.m_SelectQuestSortCBtn[uQuestSortSet]:SetCheck(true)
end

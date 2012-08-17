CArmyCorpsMember 		= class(SQRDialog)
CArmyCorpsMemberListItem = class(SQRDialog)
CArmyCorpsTeamItem = class(SQRDialog)

function CArmyCorpsMember.CreateWnd(parent)
	local wnd = CArmyCorpsMember:new()
	wnd:Init(parent)
	return wnd
end

function CArmyCorpsMember:Init(parent)
	self:CreateFromRes("ArmyCorpsMember", parent)
end

function CArmyCorpsMember:OnChildCreated()
	self.m_TeamList				= self:GetDlgChild("TeamList")
	self.m_MemberList			= self:GetDlgChild("MemberList")
	self.m_CurNum					= self:GetDlgChild("CurNum")
	self.m_UpperLimitNum	= self:GetDlgChild("UpperLimitNum")
	self.m_ChangePosBtn		= self:GetDlgChild("ChangePosBtn")
	self.m_JoinTeam				= self:GetDlgChild("JoinTeam")
	self.m_AddFriend			= self:GetDlgChild("AddFriend")
	
	self.m_tblSorBtn = {}
	self.m_tblSorBtn["name"]			= self:GetDlgChild("SortBtn1")
	self.m_tblSorBtn["position"]		= self:GetDlgChild("SortBtn2")
	self.m_tblSorBtn["level"]			= self:GetDlgChild("SortBtn3")
	self.m_tblSorBtn["class"]			= self:GetDlgChild("SortBtn4")
	self.m_tblSorBtn["profferExpend"]	= self:GetDlgChild("SortBtn5")
	self.m_tblSorBtn["proffer"]			= self:GetDlgChild("SortBtn6")
	self.m_tblSorBtn["exploit"]			= self:GetDlgChild("SortBtn7")
	
	self.tblTeamInfoItem = {}
	self.tblTeamMemberItem = {}
	self.m_tblArmyCorpsTeamInfo = {}
	self.m_tblArmyCorpsTeamMemInfo = {}
	self.m_CurMemNum = 0
	self.m_tblTongPosMem = g_TongPurviewMgr:GetPosNameTableOrderByLevel()
	
	for p, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
end

--消息
function CArmyCorpsMember:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ChangePosBtn) then
			self:OnShowArmyCorpsChangePosWnd()
		elseif(Child == self.m_JoinTeam) then
			self:OnRequestTeam()
		elseif(Child == self.m_AddFriend) then
			self:OnRequestFriend()
		else
			for p, v in pairs(self.m_tblSorBtn) do
				if(Child == v) then
					self:SortMember(p, v.bFlag)
					v.bFlag = not v.bFlag
					break
				end
			end
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		if (Child == self.m_TeamList) then
			self:ShowArmyTeamMember(uParam1 + 1)
		--elseif (Child == self.m_MemberList) then
		end
	end
end

function CArmyCorpsMember:OpenPanel(bFlag)
	if(bFlag) then
		Gac2Gas:GetArmyCorpsTeamInfo(g_Conn)
	end
	self:ShowWnd(bFlag)
end

function CArmyCorpsMember:ShowArmyTeamMember(Index)
	local itemIndex = self.m_TeamList:GetSelectItem(-1) + 1
	local tbl = self.m_tblArmyCorpsTeamInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local tong_id = tbl[itemIndex].ID
	Gac2Gas:GetTargetTongMemberInfo(g_Conn, tong_id)
end

function CArmyCorpsMember:ShowTeamInfo()
	self.m_TeamList:DeleteAllItem()
	self.tblTeamInfoItem = {}
	self.m_tblArmyCorpsTeamInfo = g_GameMain.m_ArmyCorpsWnd.m_tblPanel[2].m_tblArmyCorpsMemInfo
	
	self.m_TeamList:InsertColumn(0, self.m_TeamList:GetWndOrgWidth())
	for i, v in ipairs(self.m_tblArmyCorpsTeamInfo) do
		self.m_TeamList:InsertItem(i-1, 30)
		local item = self.m_TeamList:GetSubItem(i-1, 0)
		local memberItem = self:CreateArmyCorpsTeamItemWnd(item)
		table.insert(self.tblTeamInfoItem, memberItem)
		memberItem:SetWndText(v.Name)
	end
	self.m_TeamList:SelectItem(0)
	self:ShowArmyTeamMember(1)
end

-----佣兵小队按照某属性排序------
function CArmyCorpsMember:SortMember(index, bFlag)
	if(bFlag) then
		table.sort(self.m_tblArmyCorpsTeamMemInfo, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_tblArmyCorpsTeamMemInfo, function (a, b) return a[index] > b[index] end)
	end
	local tempTeamMemTbl = g_GameMain.m_TongMainPan.m_tblPanel[2].m_tblTongMemInfo
	self.m_tblArmyCorpsTeamMemInfo = {}
	for i, v in ipairs(tempTeamMemTbl) do
		if v.nOnline == 1 then
			table.insert(self.m_tblArmyCorpsTeamMemInfo,v)
		end
	end
	for i, v in ipairs(tempTeamMemTbl) do
		if v.nOnline ~= 1 then
			table.insert(self.m_tblArmyCorpsTeamMemInfo,v)
		end
	end
	
	self:DrawArmyCorpsTeamMemberList()
end

------创建佣兵小队列表的行窗体
function CArmyCorpsMember:CreateArmyCorpsTeamItemWnd(parent)
	local wnd = CArmyCorpsTeamItem:new()
	wnd:CreateFromRes("ArmyCorpsTeamItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CArmyCorpsMember:ShowArmyCorpsMemberInfoWnd(Conn, maxNum)
	self.m_MaxMemNum = maxNum
	if self:IsShow() then
		self:SortMember("name", true)
	end
end

function CArmyCorpsMember:DrawArmyCorpsTeamMemberList()
	self.m_MemberList:DeleteAllItem()
	self.tblTeamMemberItem = {}
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())
	self.m_CurMemNum = 0
	for i, v in ipairs(self.m_tblArmyCorpsTeamMemInfo) do
		self.m_MemberList:InsertItem(i-1, 30)
		local item = self.m_MemberList:GetSubItem(i-1, 0)
		local memberItem = self:CreateArmyCorpsMemberListItemWnd(item)
		table.insert(self.tblTeamMemberItem, memberItem)
		local sPos = self.m_tblTongPosMem[v.position][1]
		memberItem:SetTongMemListText(v.name, sPos, v.level, v.class, v.profferExpend, v.proffer, v.exploit, v.nOnline)
		if v.nOnline == 1 then
			self.m_CurMemNum = self.m_CurMemNum + 1
		end
	end
	self.m_MemberList:SelectItem(0)
	self.m_CurNum:SetWndText(self.m_CurMemNum)
	self.m_UpperLimitNum:SetWndText(self.m_MaxMemNum)
end

------创建佣兵小队成员列表的行窗体
function CArmyCorpsMember:CreateArmyCorpsMemberListItemWnd(parent)
	local wnd = CArmyCorpsMemberListItem:new()
	wnd:CreateFromRes("ArmyCorpsMemberListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CArmyCorpsMemberListItem:OnChildCreated()
	self.m_tblLabel = {}
	for i = 1, 7 do
		self.m_tblLabel[i] = self:GetDlgChild("Label" .. i)
	end
end

function CArmyCorpsMemberListItem:SetTongMemListText(sName, sPos, nLevel, nClass, nProfferExpend, nProffer, nExploit, nOnline)
	local sClass	= g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(nClass)
	local tbl		= {sName, sPos, nLevel, sClass, nProfferExpend, nProffer, nExploit}
	for i, v in ipairs(self.m_tblLabel) do
		local color = (1 == nOnline) and 0xffffffff or 0xff808080
		v:SetWndTextColor(IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE), color)
		v:SetWndText(tbl[i])
	end
end

function CArmyCorpsMember:OnRequestTeam()
	local itemIndex = self.m_MemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblArmyCorpsTeamMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local uTargetID = tbl[itemIndex].id
	if(0 == uTargetID) or uTargetID == g_MainPlayer.m_uID then return end
		
	Gac2Gas:InviteMakeTeam(g_Conn, uTargetID)
end

function CArmyCorpsMember:OnRequestFriend()
	local itemIndex = self.m_MemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblArmyCorpsTeamMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local uTargetID = tbl[itemIndex].id
	local uTargetName = tbl[itemIndex].name
	if(0 == uTargetID) or uTargetID == g_MainPlayer.m_uID then return end
		
	local tblInfo	= {}
	tblInfo.id		= uTargetID
	tblInfo.name 	= uTargetName
	g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, false)
end

------显示修改职位面板------
function CArmyCorpsMember:OnShowArmyCorpsChangePosWnd()
	local itemIndex = self.m_MemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblArmyCorpsTeamMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local uTargetID = tbl[itemIndex].id
	local uTargetName = tbl[itemIndex].name
	if(0 == uTargetID) or uTargetID == g_MainPlayer.m_uID then return end
	
	if (g_ArmyCorpsMgr:GetPosIndexByName("团长") ~= g_GameMain.m_ArmyCorpsWnd.m_ArmyCorps_Pos) then
		MsgClient(8526)
		return
	end
	CArmyCorpsChangePosWnd.GetWnd():OpenPanel(true,uTargetID,uTargetName)
end

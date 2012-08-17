CTongMemberAppWnd		= class(SQRDialog)	--导出类
CTongMemberAppListItem	= class(SQRDialog)

function CTongMemberAppWnd.CreateTongMemberAppWnd(parent)
	local wnd = CTongMemberAppWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongMemberAppWnd:Init(parent)
	self.m_CurMemNum			= 0
	self.m_tblTongAppMemInfo	= {}
	self:CreateFromRes("TongMemberApp", parent)
end

function CTongMemberAppWnd:OnChildCreated()
	self.m_MemberList		= self:GetDlgChild("MemberList")
	self.m_CurNum			= self:GetDlgChild("CurNum")
	self.m_UpperLimitNum	= self:GetDlgChild("UpperLimitNum")
	self.m_AgreeBtn			= self:GetDlgChild("AgreeBtn")
	self.m_RefuseBtn		= self:GetDlgChild("RefuseBtn")
	
	self.m_tblSorBtn = {}
	self.m_tblSorBtn["name"]	= self:GetDlgChild("SortBtn1")
	self.m_tblSorBtn["level"]	= self:GetDlgChild("SortBtn2")
	self.m_tblSorBtn["class"]	= self:GetDlgChild("SortBtn3")
	self.m_tblSorBtn["proffer"]	= self:GetDlgChild("SortBtn4")
	self.m_tblSorBtn["exploit"]	= self:GetDlgChild("SortBtn5")
	
	for p, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
end

function CTongMemberAppWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_AgreeBtn) then
			self:OnBeJoinTong(true)
		elseif(Child == self.m_RefuseBtn) then
			self:OnBeJoinTong(false)
		else
			for p, v in pairs(self.m_tblSorBtn) do
				if(Child == v) then
					self:SortMember(p, v.bFlag)
					v.bFlag = not v.bFlag
					break
				end
			end
		end
	end
end

function CTongMemberAppWnd:OpenPanel(bFlag)
	if(bFlag) then
		Gac2Gas:GetTongRequestInfo(g_Conn)
	end
	self:ShowWnd(bFlag)
end

function CTongMemberAppWnd:UpdateTongMemAppInfo(maxNum)
	self.m_CurNum:SetWndText(self.m_CurMemNum)
	self.m_UpperLimitNum:SetWndText(maxNum)
	self:DrawMemberList()
end

-----申请入团成员按照某属性排序------
function CTongMemberAppWnd:SortMember(index, bFlag)
	if(bFlag) then
		table.sort(self.m_tblTongAppMemInfo, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_tblTongAppMemInfo, function (a, b) return a[index] > b[index] end)
	end
	self:DrawMemberList()
end

------申请入会人员列表行----------------------------------------------------------------
function CTongMemberAppWnd:DrawMemberList()
	self.m_MemberList:DeleteAllItem()
	self.tblMemberItem = {}
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())--行宽度大于列表宽度
	for i, v in ipairs(self.m_tblTongAppMemInfo) do
		self.m_MemberList:InsertItem(i-1, 30)
		local item = self.m_MemberList:GetSubItem(i-1, 0)
		local memberItem = self:CreateTongMemAppListItemWnd(item)
		table.insert(self.tblMemberItem, memberItem)
		memberItem:SetTongMemAppListText(v.name, v.level, v.class, v.proffer, v.exploit)
	end
end

------是否批准加入帮会------
function CTongMemberAppWnd:OnBeJoinTong(beJoin)
	local itemIndex = self.m_MemberList:GetSelectItem(-1)+1
	local tbl = self.m_tblTongAppMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	Gac2Gas:BeJoinTong(g_Conn, tbl[itemIndex].id, beJoin)
end

------创建帮会成员列表的行窗体------
function CTongMemberAppWnd:CreateTongMemAppListItemWnd(parent)
	local wnd = CTongMemberAppListItem:new()
	wnd:CreateFromRes("TongMemberAppListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongMemberAppListItem:OnChildCreated()
	self.m_tblLabel = {}
	for i = 1, 5 do
		self.m_tblLabel[i] = self:GetDlgChild("Label" .. i)
	end
end

function CTongMemberAppListItem:SetTongMemAppListText(sName, nLevel, nClass, nProffer, nExploit)
	local sClass	= g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(nClass)
	local tbl		= {sName, nLevel, sClass, nProffer, nExploit}
	for i, v in ipairs(self.m_tblLabel) do
		v:SetWndText(tbl[i])
	end
end

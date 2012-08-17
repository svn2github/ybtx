gac_gas_require "relation/tong/TongPurviewMgr"

CTongMemberWnd		= class(SQRDialog)	--导出类
CTongMemberListItem	= class(SQRDialog)

function CTongMemberWnd.CreateTongMemberWnd(parent)
	local wnd = CTongMemberWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongMemberWnd:Init(parent)
	self.m_CurMemNum		= 0
	self.m_OnlineMemNum = 0
	self.m_tblTongMemInfo	= {}
	self:InitTblTongPosMember()
	self:CreateFromRes("TongMemberMgr", parent)
end

function CTongMemberWnd:OnChildCreated()
	self.m_MemberList		= self:GetDlgChild("MemberList")
	self.m_CurNum			= self:GetDlgChild("CurNum")
	self.m_OnlineNum  = self:GetDlgChild("OnlineNum")
	self.m_UpperLimitNum	= self:GetDlgChild("UpperLimitNum")
	self.m_ChangePosBtn		= self:GetDlgChild("ChangePosBtn")
	self.m_KickOutBtn		= self:GetDlgChild("KickOutBtn")
	self.m_LeaveBtn			= self:GetDlgChild("LeaveBtn")
	self.m_AddFriend		= self:GetDlgChild("AddFriend")
	self.m_JoinTeam			= self:GetDlgChild("JoinTeam")
	self.m_PrivateChat  = self:GetDlgChild("PrivateChatBtn")
	self.m_SendEmail    = self:GetDlgChild("SendEmailBtn")
	
	self.m_tblSorBtn = {}
	self.m_tblSorBtn["name"]			     = self:GetDlgChild("SortBtn1")
	self.m_tblSorBtn["position"]		   = self:GetDlgChild("SortBtn2")
	self.m_tblSorBtn["level"]			     = self:GetDlgChild("SortBtn3")
	self.m_tblSorBtn["class"]			     = self:GetDlgChild("SortBtn4")
	self.m_tblSorBtn["proffer"]	       = self:GetDlgChild("SortBtn5")
	self.m_tblSorBtn["profferExpend"]	 = self:GetDlgChild("SortBtn6")
	self.m_tblSorBtn["tongProffer"]    = self:GetDlgChild("SortBtn7")
	self.m_tblSorBtn["exploit"]			   = self:GetDlgChild("SortBtn8")
	self.m_tblSorBtn["latestOnLine"]   = self:GetDlgChild("SortBtn9")
	
	for p, v in pairs(self.m_tblSorBtn) do
		v.bFlag = true
	end
end

function CTongMemberWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ChangePosBtn) then
			self:OnShowTongChangePosWnd()
		elseif(Child == self.m_KickOutBtn) then
			self:OnKickOutOfTong()
		elseif(Child == self.m_LeaveBtn) then
			self:OnLeaveTong()
		elseif(Child == self.m_AddFriend) then
			self:OnRequestFriend()
		elseif(Child == self.m_JoinTeam) then
			self:OnRequestTeam()
		elseif(Child == self.m_PrivateChat) then
			self:OnShowPrivateChatWnd()
		elseif(Child == self.m_SendEmail) then
			self:OnShowSendEmailWnd()
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
		if( g_GameMain.m_TongChangePos and g_GameMain.m_TongChangePos:IsShow() ) then
			self:OnShowTongChangePosWnd()
		end
	end
end

function CTongMemberWnd:InitTblTongPosMember()
	self.m_tblTongPosMem = g_TongPurviewMgr:GetPosNameTableOrderByLevel()
end

function CTongMemberWnd:OpenPanel(bFlag)
	if(bFlag) then
		Gac2Gas:GetTongMemberInfo(g_Conn)
	end
	self:ShowWnd(bFlag)
end

function CTongMemberWnd:UpdateTongMemInfo(maxNum)
	self.m_CurNum:SetWndText(self.m_CurMemNum)
	self.m_UpperLimitNum:SetWndText(maxNum)
	self.m_OnlineNum:SetWndText(self.m_OnlineMemNum)
	self:DrawMemberList()
end

------开除------
function CTongMemberWnd:OnKickOutOfTong()
	local itemIndex = self.m_MemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblTongMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id = tbl[itemIndex].id
	Gac2Gas:KickOutOfTong(g_Conn, id)
end

------退团------
function CTongMemberWnd:OnLeaveTong()
	local LeaveTong = function(wnd, uButton)
		if(uButton == MB_BtnOK) then
			Gac2Gas:LeaveTong(g_Conn)
		end
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(17004), BitOr(MB_BtnOK, MB_BtnCancel), LeaveTong, g_GameMain)
end

--------组队----------
function CTongMemberWnd:OnRequestTeam()
	local itemIndex = self.m_MemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblTongMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id = tbl[itemIndex].id
	if(0 == id) or id == g_MainPlayer.m_uID then return end
		
	Gac2Gas:InviteMakeTeam(g_Conn, id)
end

----------好友------
function CTongMemberWnd:OnRequestFriend()
	local itemIndex = self.m_MemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblTongMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id = tbl[itemIndex].id
	local Name = tbl[itemIndex].name
	if(0 == id) or id == g_MainPlayer.m_uID then return end
		
	local tblInfo	= {}
	tblInfo.id		= id
	tblInfo.name 	= Name
	g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, false)
end

------显示修改职位面板------
function CTongMemberWnd:OnShowTongChangePosWnd()
	local itemIndex = self.m_MemberList:GetSelectItem(-1)+1
	local tbl = self.m_tblTongMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id   = tbl[itemIndex].id
	local name = tbl[itemIndex].name
	local pos  = tbl[itemIndex].position
	if(not g_GameMain.m_TongChangePos) then
		gac_require "relation/tong/TongChangePos" 
		g_GameMain.m_TongChangePos = CreateTongChangePosWnd(g_GameMain)
	end
	g_GameMain.m_TongChangePos:OpenPanel(id, name, pos)
end

-----显示私聊窗口---------
function CTongMemberWnd:OnShowPrivateChatWnd()
	local itemIndex = self.m_MemberList:GetSelectItem(-1)+1
	local tbl = self.m_tblTongMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local id   = tbl[itemIndex].id
	local name = tbl[itemIndex].name
	g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(id, name) --私聊 
end

----显示发送邮件窗口--------
function CTongMemberWnd:OnShowSendEmailWnd()
	local itemIndex = self.m_MemberList:GetSelectItem(-1)+1
	local tbl = self.m_tblTongMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local name = tbl[itemIndex].name
	local PlayerName = g_MainPlayer.m_Properties:GetCharName()
	if name == PlayerName then
		return
	end
	g_GameMain.m_SendBox.m_EmailTitle:SetWndText("")
	g_GameMain.m_SendBox.m_ReceiverName:SetWndText(name)
	g_GameMain.m_SendBox.m_EmailText:SetWndText("")
	g_GameMain.m_SendBox.m_Jin:SetWndText("")
	g_GameMain.m_SendBox.m_Yin:SetWndText("")
	g_GameMain.m_SendBox.m_Tong:SetWndText("")
	g_GameMain.m_SendBox:CleanAllGoodsInformation()
	g_GameMain.m_SendBox.m_SendBtn:EnableWnd(false)
	g_GameMain.m_SendBox:SetFocus()
	g_GameMain.m_SendBox:ShowWnd(true)
	g_GameMain.m_WndMainBag:ShowWnd(true)
	g_GameMain.m_WndMainBag:SetFocus()
end

-----帮会成员按照某属性排序------
function CTongMemberWnd:SortMember(index, bFlag)
	if(bFlag) then
		table.sort(self.m_tblTongMemInfo, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_tblTongMemInfo, function (a, b) return a[index] > b[index] end)
	end
	self:DrawMemberList()
end

------帮会成员列表行----------------------------------------------------------------
function CTongMemberWnd:DrawMemberList()
	self.m_MemberList:DeleteAllItem()
	self.tblMemberItem = {}
	self.m_MemberList:InsertColumn(0, self.m_MemberList:GetWndOrgWidth())
	for i, v in ipairs(self.m_tblTongMemInfo) do
		self.m_MemberList:InsertItem(i-1, 30)
		local item = self.m_MemberList:GetSubItem(i-1, 0)
		local memberItem = self:CreateTongMemListItemWnd(item)
		table.insert(self.tblMemberItem, memberItem)
		local sPos = self.m_tblTongPosMem[v.position][1]
		memberItem:SetTongMemListText(v.name, sPos, v.level, v.class, v.proffer, v.profferExpend,v.tongProffer, v.exploit, v.nOnline, v.latestOnLine)
	end
	self.m_MemberList:SelectItem(0)
end

------创建帮会成员列表的行窗体
function CTongMemberWnd:CreateTongMemListItemWnd(parent)
	local wnd = CTongMemberListItem:new()
	wnd:CreateFromRes("TongMemberMgrListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongMemberListItem:OnChildCreated()
	self.m_tblLabel = {}
	for i = 1, 9 do
		self.m_tblLabel[i] = self:GetDlgChild("Label" .. i)
	end
end

function CTongMemberListItem:SetTongMemListText(sName, sPos, nLevel, nClass, nProffer, nProfferExpend, nTongProffer, nExploit, nOnline, latestOnLine)
	local sClass	= g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(nClass)
	local tbl		= {sName, sPos, nLevel, sClass, nProffer,nProfferExpend, nTongProffer, nExploit, latestOnLine}
	for i, v in ipairs(self.m_tblLabel) do
		local color = (1 == nOnline) and 0xffffffff or 0xff808080
		v:SetWndTextColor(IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE), color)
		v:SetWndText(tbl[i])
	end
end

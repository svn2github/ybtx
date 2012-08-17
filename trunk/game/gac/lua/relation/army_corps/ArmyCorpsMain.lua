CArmyCorpsMain = class(SQRDialog)

function CArmyCorpsMain.CreateWnd(parent)
	local wnd = CArmyCorpsMain:new()
	wnd:Init(parent)
	return wnd
end

function CArmyCorpsMain:Init(parent)
	self:CreateFromRes("ArmyCorpsMain", parent)
end

function CArmyCorpsMain:OnChildCreated()
	self.m_Name				= self:GetDlgChild("Name")
	self.m_LeaderName		= self:GetDlgChild("LeaderName")
	self.m_Level			= self:GetDlgChild("Level")
	self.m_Exploit			= self:GetDlgChild("Exploit")
	self.m_MemberNum		= self:GetDlgChild("MemberNum")
	self.m_Honor			= self:GetDlgChild("Honor")
	self.m_Money			= self:GetDlgChild("Money")
	self.m_PurposeEdit		= self:GetDlgChild("PurposeEdit")
	self.m_PurposeRichText	= self:GetDlgChild("PurposeRichText")
	self.m_ChangePurpBtn	= self:GetDlgChild("ChangePurposeBtn")
	self.m_ChangePurpOk		= self:GetDlgChild("ChangePurposeOk")
	self.m_PurposeEdit:ShowWnd(false)
	self.m_ChangePurpOk:ShowWnd(false)
	self.m_ChangeToTeamBtn = self:GetDlgChild("ChangeToTeam")
end

--消息
function CArmyCorpsMain:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ChangePurpBtn) then
			self:OnChangePurpose()
		elseif(Child == self.m_ChangePurpOk) then
			self:OnChangePurposeOk()
		elseif(Child == self.m_ChangeToTeamBtn) then
			g_GameMain.m_TongMainPan:ShowWnd(true)
			g_GameMain.m_TongMainPan:ChangePageTo(1)
		end
	end
end

------刷新佣兵团主面板信息------
--[[
团id
团名称
团长
团当前小队数量
团小队数量上限
团等级
团荣誉
团声望
团资金
团宗旨
--]]

function CArmyCorpsMain:UpdateArmyCorpsInfo( sName, uLevel, uMoney, sPurpose, sAdminName, 
																						uMemberCount, uExploit, uHonor )
	self.m_Name:SetWndText(sName)
	self.m_LeaderName:SetWndText(sAdminName)
	self.m_Level:SetWndText(uLevel)
	local uMaxMemberCount = g_ArmyCorpsMgr:GetTongCountLimit(uLevel)
	self.m_MemberNum:SetWndText( string.format("%d/%d", uMemberCount, uMaxMemberCount ) )
	self.m_Exploit:SetWndText(uExploit)
	self.m_Honor:SetWndText( uHonor )
	self.m_PurposeEdit:SetWndText(sPurpose)
	self.m_PurposeRichText:SetWndText(sPurpose)
	self.m_Money:SetWndText(g_MoneyMgr:ChangeMoneyToString(uMoney))
	
	local bShowChangePurp = g_ArmyCorpsMgr:JudgePurview(g_GameMain.m_ArmyCorpsWnd.m_ArmyCorps_Pos, "UpdatePurpose")
	self.m_ChangePurpBtn:ShowWnd(bShowChangePurp)
end

------允许修改佣兵团宗旨------
function CArmyCorpsMain:OnChangePurpose()
	self.m_PurposeRichText:ShowWnd(false)
	self.m_PurposeEdit:ShowWnd(true)
	self.m_ChangePurpBtn:ShowWnd(false)
	self.m_ChangePurpOk:ShowWnd(true)
end

------确认修改佣兵团宗旨------
function CArmyCorpsMain:OnChangePurposeOk()
	self.m_PurposeEdit:ShowWnd(false)
	self.m_PurposeRichText:ShowWnd(true)
	self.m_ChangePurpBtn:ShowWnd(true)
	self.m_ChangePurpOk:ShowWnd(false)
	local sText = self.m_PurposeEdit:GetWndText()
	if string.len(sText) > 1024 then
		MsgClient(9178)
		return
	end
	Gac2Gas:ChangeArmyCorpsPurpose(g_Conn, sText)
end

function CArmyCorpsMain:UpdateArmyCorpsPurpose(sPurpose)
	self.m_PurposeEdit:SetWndText(sPurpose)
	self.m_PurposeRichText:SetWndText(sPurpose)
end

------显示佣兵团信息面板------
function CArmyCorpsMain:OpenPanel(bFlag)
	if(bFlag) then
		self.m_Name:SetWndText("")
		self.m_LeaderName:SetWndText("")
		self.m_Level:SetWndText("")
		self.m_MemberNum:SetWndText("")
		self.m_Exploit:SetWndText("")
		self.m_Honor:SetWndText( "" )
		self.m_PurposeEdit:SetWndText("")
		self.m_PurposeRichText:SetWndText("")
		self.m_Money:SetWndText("")
		self.m_ChangePurpBtn:ShowWnd(false)
		Gac2Gas:GetArmyCorpsInfo(g_Conn)
	end
	self:ShowWnd(bFlag)
end

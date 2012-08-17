CArmyCorpsPanel = class(SQRDialog)

local fl_PageNum = 3

function CArmyCorpsPanel:Ctor(parent)
	self:CreateFromRes("ArmyCorpsPanel",parent)
	self.m_CloseBtn = self:GetDlgChild("Close")

	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
	--self:ShowWnd(false)
end

function CArmyCorpsPanel.GetWnd()
	local Wnd = g_GameMain.m_ArmyCorpsWnd
	if not Wnd then
		Wnd = CArmyCorpsPanel:new(g_GameMain)
		g_GameMain.m_ArmyCorpsWnd = Wnd
	end
	return Wnd
end

function CArmyCorpsPanel:OnChildCreated()
	local tblCreatePanelFun = {	CArmyCorpsMain.CreateWnd,
								CArmyCorpsTeamInfo.CreateWnd,
								CArmyCorpsMember.CreateWnd}
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_Static	= self:GetDlgChild("Static")
	self.m_tblCheck = {}
	self.m_tblPanel = {}
	for i = 1, fl_PageNum do
		self.m_tblCheck[i] = self:GetDlgChild("Check" .. i)
		self.m_tblPanel[i] = tblCreatePanelFun[i](self.m_Static)
	end
	self:GetDlgChild("Check4"):ShowWnd(false)
	self.m_tblCheck[1]:SetCheck(true)
	self.m_CurrentPos = 0
end

function CArmyCorpsPanel:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		else
			for i = 1, fl_PageNum do
				if(Child == self.m_tblCheck[i] and self.m_CurrentPos ~= i) then
					self:ChangePage()
					self.m_CurrentPos = i
				end
			end
		end
	end
end

function CArmyCorpsPanel:ChangePageTo(nPage)
	self.m_tblCheck[nPage]:SetCheck(true)
	self:ChangePage()
end

function CArmyCorpsPanel:ChangePage()
	for i = 1, fl_PageNum do
		self.m_tblPanel[i]:OpenPanel( self.m_tblCheck[i]:GetCheck() )
	end
end

function CArmyCorpsPanel:OpenPanel(bFlag)
	if(bFlag) then
		self.m_CurrentPos = 0
		self:ChangePage()
	end
	self:ShowWnd(bFlag)
end

function CArmyCorpsPanel:OnCreateArmyCorps()
	g_GameMain.m_TongCreate:ShowWnd(false)
	--g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit() --主角头顶信息
	g_GameMain.m_ArmyCorpsWnd:ChangePageTo(1)
	g_GameMain.m_ArmyCorpsWnd:OpenPanel(true)
end

function CArmyCorpsPanel:OnArmyCorpsPosChanged(uPos)
	self.m_ArmyCorps_Pos = uPos
end

function CArmyCorpsPanel:CanInviteArmyCorps()
	if self.m_ArmyCorps_Pos == 1 or self.m_ArmyCorps_Pos == 2 then
		return true
	end
	return false
end

function CArmyCorpsPanel:ReturnArmyCorpsInfo( Name,Level,Money,Purpose,AdminName,MemberCount,Exploit,Honor )
	self.m_tblInfo = {Name,Level,Money,Purpose,AdminName,MemberCount,Exploit,Honor}
	self.m_tblPanel[1]:UpdateArmyCorpsInfo( Name,Level,Money,Purpose,AdminName,MemberCount,Exploit,Honor )
end

function CArmyCorpsPanel:OnArmyCorpsPurposeChanged(sPurpose)
	self.m_tblPanel[1]:UpdateArmyCorpsPurpose(sPurpose)
end

function CArmyCorpsPanel:OnInviteJoinArmyCorps(uInviterID, sInviterName,sArmyCorpsName)
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateArmyCorpsInvMsgMinimum(uInviterID, sInviterName, sArmyCorpsName)
end

--function CArmyCorpsPanel:OnMsgInviteJoinArmyCorps(uInviterID, sInviterName,sArmyCorpsName)
--	local function BeInviteMBoxClick(g_GameMain,uButton)
--		if uButton == MB_BtnOK then
--			Gac2Gas:ResponseBeInviteToArmyCorps(g_Conn, uInviterID, true)
--		elseif(uButton == MB_BtnCancel) then --拒绝
--			Gac2Gas:ResponseBeInviteToArmyCorps(g_Conn, uInviterID, false)
--		end
--		g_GameMain.m_MsgBox = nil
--		return true
--	end
--	local ShowInfo = MsgBoxMsg(17001, sInviterName, sArmyCorpsName)
--	g_GameMain.m_MsgBox = MessageBox(g_GameMain, ShowInfo, BitOr(MB_BtnOK, MB_BtnCancel), BeInviteMBoxClick, g_GameMain)
--end

function CArmyCorpsPanel:ReturnArmyCorpsTeamInfo(tong_type,tong_name,leader_name,tong_level,member_num,tong_exploit,tong_id,warZoneId,stationId)
	self.m_tblPanel[2]:UpdateArmyCorpsTeamInfo(tong_type,tong_name,leader_name,tong_level,member_num,tong_exploit,tong_id,warZoneId,stationId)
end
function CArmyCorpsPanel:ReturnArmyCorpsTeamInfoBegin(MemberCount,MemberCountLimit)
	self.m_tblPanel[2]:UpdateArmyCorpsTeamInfoBegin(MemberCount,MemberCountLimit)
end

function CArmyCorpsPanel:ReturnArmyCorpsTeamInfoEnd()
	if self.m_tblPanel[2]:IsShow() then
		self.m_tblPanel[2]:SortMember("Level",true)
	elseif self.m_tblPanel[3]:IsShow() then
		self.m_tblPanel[3]:ShowTeamInfo()
	end
end
function CArmyCorpsPanel:OnLeaveArmyCorps(bSucc)
	self.m_ArmyCorps_Pos = 0
	if bSucc then
		g_GameMain.m_TongMainPan:ChangePageTo(1)
		g_GameMain.m_TongMainPan:OpenPanel(true)
	end
end

function CArmyCorpsPanel:OnKickOutOfArmyCorps(bSucc)
	self.m_tblPanel[2]:OnKickOutOfArmyCorps(bSucc)
end

function CArmyCorpsPanel:OnBeKickOutOfArmyCorps()
	g_GameMain.m_TongMainPan:ChangePageTo(1)
	g_GameMain.m_TongMainPan:OpenPanel(true)
end

function CArmyCorpsPanel:ShowArmyCorpsInfo()
	if g_MainPlayer.m_Properties:GetArmyCorpsName() == "" then
		MsgClient(8509)
		return
	end
	self:ShowWnd(true)
	self:ChangePageTo(1)
end
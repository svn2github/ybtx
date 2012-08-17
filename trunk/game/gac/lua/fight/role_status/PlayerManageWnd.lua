CPlayerManageWnd= class( SQRDialog )

function CPlayerManageWnd:Ctor()
	self:CreateFromRes("PlayerManageWnd",g_GameMain)
	self.m_PlayerNameStatic = self:GetDlgChild("PlayerNameStatic")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_ActionCheckBtnTbl = {}
	for i = 1 ,3 do
		local ActionCheckBtn = self:GetDlgChild("ActionCheckBtn" .. i)
		table.insert(self.m_ActionCheckBtnTbl,ActionCheckBtn)
	end
	self.m_ActionCheckBtnTbl[1]:SetCheck(true)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2)
end

function CPlayerManageWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.m_ActionCheckBtnTbl[1] then
			g_GameMain.m_RoleStatus:OpenSelfWnd(true)
			g_GameMain.m_FightingEvaluationWnd:ShowWnd(false)
			g_GameMain.m_JiFenDetailWnd:ShowWnd(false)
		elseif Child == self.m_ActionCheckBtnTbl[2] then
			g_GameMain.m_FightingEvaluationWnd:ShowWnd(true)
			g_GameMain.m_RoleStatus:ShowWnd(false)
			g_GameMain.m_JiFenDetailWnd:ShowWnd(false)
		elseif Child == self.m_ActionCheckBtnTbl[3] then
			Gac2Gas:GetTongProfferCount(g_Conn)
			g_GameMain.m_JiFenDetailWnd:ShowWnd(true)
			g_GameMain.m_JiFenDetailWnd:UpdateJiFenWnd()
			g_GameMain.m_RoleStatus:ShowWnd(false)
			g_GameMain.m_FightingEvaluationWnd:ShowWnd(false)
		elseif Child == self.m_CloseBtn then
			g_GameMain.m_PlayerManageWnd:ShowWnd(false)
		end
	end
end
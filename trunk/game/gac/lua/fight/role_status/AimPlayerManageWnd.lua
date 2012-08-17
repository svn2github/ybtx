CAimPlayerManageWnd= class( SQRDialog )

function CAimPlayerManageWnd:Ctor()
	self:CreateFromRes("PlayerManageWnd",g_GameMain)
	self.m_PlayerNameStatic = self:GetDlgChild("PlayerNameStatic")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_ActionCheckBtnTbl = {}
	for i = 1 ,3 do
		local ActionCheckBtn = self:GetDlgChild("ActionCheckBtn" .. i)
		table.insert(self.m_ActionCheckBtnTbl,ActionCheckBtn)
	end
	self.m_ActionCheckBtnTbl[1]:SetCheck(true)
	g_ExcludeWndMgr:InitExcludeWnd(self, 3)
end

function CAimPlayerManageWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.m_ActionCheckBtnTbl[1] then
			g_GameMain.m_AimStatusWnd:ShowWnd(true)
			g_GameMain.m_AimFightingEvaluationWnd:ShowWnd(false)
		elseif Child == self.m_ActionCheckBtnTbl[2] then
			g_GameMain.m_AimStatusWnd:ShowWnd(false)
			g_GameMain.m_AimFightingEvaluationWnd:ShowWnd(true)
		elseif Child == self.m_CloseBtn then
			g_GameMain.m_AimStatusWnd:OnClosePanel()
			g_GameMain.m_AimPlayerManageWnd:ShowWnd(false)
		end
	end
end
CJiFenSaiWnd = class(SQRDialog)

local m_FbName = "竞技场"--GetStaticTextClient(9401)--竞技场

function CJiFenSaiWnd:Ctor(parent)
	self:CreateFromRes("Fb_PVPApplyWnd",parent) 
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_MultiJoinBtn = self:GetDlgChild("JoinBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("FbGameInfo")
	self.m_Title = self:GetDlgChild("WndName")
	
	self.m_Title:SetWndText(GetStaticTextClient(9401))
	self.m_FbGameInfo:SetWndText(GetDescriptionDisplayName("竞技场介绍"))
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CJiFenSaiWnd:GetWnd()
	local Wnd = g_GameMain.m_JiFenSaiWnd
	if not Wnd then
		Wnd = CJiFenSaiWnd:new(g_GameMain)
		g_GameMain.m_JiFenSaiWnd = Wnd
	end
	return Wnd
end

function CJiFenSaiWnd:InitJiFenSaiWnd(IsInWait,IsCancel)
	self:ShowWnd(true)
	
	local IsShow1 = not IsInWait
	local IsShow2 = not IsCancel
	
	--self.m_SingleJoinBtn:EnableWnd(IsShow1)
	self.m_MultiJoinBtn:EnableWnd(IsShow1)
	self.m_CancelBtn:EnableWnd(IsShow2)

	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CJiFenSaiWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		--if Child == self.m_SingleJoinBtn then
			--报名参加积分赛
		--	Gac2Gas:SingleJoinJiFenSai(g_Conn, self.m_NpcID)
		--	self:ShowWnd(false)
		if Child == self.m_MultiJoinBtn then
			Gac2Gas:JoinJiFenSai(g_Conn, self.m_NpcID)
			self:ShowWnd(false)
		elseif Child == self.m_CancelBtn then
			--放弃参加积分赛
			Gac2Gas:NotJoinFbAction(g_Conn,m_FbName)
			if g_GameMain.m_FbActionMsgWnd[m_FbName] then
				g_GameMain.m_FbActionMsgWnd[m_FbName]:DeleteTick(m_FbName)
			end
			self:ShowWnd(false)
		elseif Child == self.m_CloseBtn then
			--关闭窗口
			self:ShowWnd(false)
			--self:ShowWnd(false)
		end
	end
end

function CJiFenSaiWnd:RetShowJiFenSaiWnd(IsInWait, EntityID)
	self.m_NpcID = EntityID
	self:InitJiFenSaiWnd(IsInWait, not IsInWait)
end

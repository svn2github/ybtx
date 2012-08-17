CAreaFbInfoWnd = class(SQRDialog)

function CAreaFbInfoWnd:Ctor(parent)
	self:CreateFromRes("AreaFbInfoWnd",parent)
	self.m_CloseBtn = self:GetDlgChild("Close")
	self.m_ShowBtn = self:GetDlgChild("JoinFbBtn")
	self.m_QuestBtn = self:GetDlgChild("ReferQuestBtn")
	self.m_RichEditText = self:GetDlgChild("InfoText")
	self.m_WndFace = self:GetDlgChild("WndFace")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CAreaFbInfoWnd:GetWnd()
	local Wnd = g_GameMain.m_AreaFbInfoWnd
	if not Wnd then
		Wnd = CAreaFbInfoWnd:new(g_GameMain)
		g_GameMain.m_AreaFbInfoWnd = Wnd
	end
	return Wnd
end


function CAreaFbInfoWnd:InitAreaFbInfoWnd(NpcID)
	self.m_TargetNpcId = NpcID
	--self.m_RichEditText:SetWndText(Description_Client["地下城探险介绍"].Content)
	self.m_RichEditText:SetWndText(GetDescriptionDisplayName("地下城探险介绍"))
  self.m_ShowBtn:SetWndText(GetStaticTextClient(1001))
	self.m_QuestBtn:ShowWnd(false)

	self:ShowWnd(true)
	
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CAreaFbInfoWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ShowBtn then
--			local Wnd = CAreaFbSelWnd.GetWnd()
--			Wnd:InitAreaFbSelWnd()
			g_GameMain.m_AreaFbSelWndNew:InitAreaFbSelWnd()
		end
		--关闭窗口
		self:ShowWnd(false)
	end
end

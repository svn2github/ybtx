CTongMainPanWnd = class(SQRDialog) --导出类

local fl_PageNum = 7

function CTongMainPanWnd:Ctor()
	self:CreateFromRes("TongMainPan", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongMainPanWnd:OnChildCreated()
	local tblCreatePanelFun = {	CTongMainWnd.CreateTongMainWnd,
								CTongMemberWnd.CreateTongMemberWnd,
								CTongMemberAppWnd.CreateTongMemberAppWnd,
								CTongPurviewWnd.CreateTongPurviewWnd,
								CTongLogWnd.CreateTongLogWnd,
								CTongPrivilegeWnd.CreateTongPrivilegeWnd,
								CTongHandBookWnd.CreateTongHandBookWnd	}
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_Static	= self:GetDlgChild("Static")
	self.m_tblCheck = {}
	self.m_tblPanel = {}
	for i = 1, fl_PageNum do
		self.m_tblCheck[i] = self:GetDlgChild("Check" .. i)
		self.m_tblPanel[i] = tblCreatePanelFun[i](self.m_Static)
	end
	self.m_tblCheck[1]:SetCheck(true)
	self.m_CurrentPos = 0
end

function CTongMainPanWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
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

function CTongMainPanWnd:ChangePageTo(nPage)
	self.m_tblCheck[nPage]:SetCheck(true)
	self:ChangePage()
end

function CTongMainPanWnd:ChangePage()
	for i = 1, fl_PageNum do
		self.m_tblPanel[i]:OpenPanel( self.m_tblCheck[i]:GetCheck() )
	end
end

------判断是否打开佣兵团面板------
function CTongMainPanWnd:BeShowTongMain()
	if( not self:IsShow() ) then
		local tongId = g_MainPlayer.m_Properties:GetTongID()
		if(0 ~= tongId) then
			self:OpenPanel(true)
		else
			MsgClient(9022)
		end
	else
		self:OpenPanel(false)
	end
end

function CTongMainPanWnd:OpenPanel(bFlag)
	if(bFlag) then
		self.m_CurrentPos = 0
		self:ChangePage()
	end
	self:ShowWnd(bFlag)
end

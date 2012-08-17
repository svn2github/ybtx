--gac_require "relation/tong/TongRobResourceWndInc"
CRobResourceWnd = class(SQRDialog)

function CRobResourceWnd:Ctor(Parent, sceneName)
	self:CreateFromRes("RobResourceWnd", Parent)
	self.m_TitleWnd			 	= self:GetDlgChild("TitleWnd")
	self.m_SignBtn			 	= self:GetDlgChild("SignBtn")
	--self.m_SignQueryBtn	 	= self:GetDlgChild("SignQueryBtn")
	self.m_ProfferBtn	 	= self:GetDlgChild("ProfferBtn")
	--self.m_CancelSignBtn	 	= self:GetDlgChild("CancelSignBtn")
	self.m_Exit      		  = self:GetDlgChild("Exit")
	self.m_EnterFb      		  = self:GetDlgChild("EnterFb")
	self.m_HeaderGet		  = self:GetDlgChild("HeaderGet")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CRobResourceWnd:GetWnd(sceneName)
	
	if not g_GameMain.m_RobResourceWnd then
		g_GameMain.m_RobResourceWnd = CRobResourceWnd:new(g_GameMain, sceneName)
	end
	local wnd = g_GameMain.m_RobResourceWnd
	wnd:ShowWnd(true)
	wnd:SetSceneName(sceneName)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
	return wnd
end

--function CreateRobResourceWnd(Parent)
--	local Wnd = CRobResourceWnd:new()	
--	Wnd:Init(Parent)	
--	Wnd:ShowWnd(false)
--	return Wnd
--end

--function CRobResourceWnd:Init(Parent)
--	self:CreateFromRes("RobResourceWnd", Parent)
--	self.m_TitleWnd			 	= self:GetDlgChild("TitleWnd")
--	self.m_SignBtn			 	= self:GetDlgChild("SignBtn")
--	self.m_SignQueryBtn	 	= self:GetDlgChild("SignQueryBtn")
--	self.m_ProfferBtn	 	= self:GetDlgChild("ProfferBtn")
--	self.m_CancelSignBtn	 	= self:GetDlgChild("CancelSignBtn")
--	self.m_Exit      		  = self:GetDlgChild("Exit")
--	self.m_EnterFb      		  = self:GetDlgChild("EnterFb")
--end

function CRobResourceWnd:Destroy()
	self:ShowWnd(false)
end

function CRobResourceWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_SignBtn == Child then
--			self:Destroy()
--			Gac2Gas:JoinRobResFb(g_Conn) --判断是否能够报名	
			self:Destroy()
			Gac2Gas:OpenSignQueryWnd(g_Conn)
--		elseif self.m_SignQueryBtn == Child then
--			self:Destroy()
--			Gac2Gas:OpenSignQueryWnd(g_Conn)
		elseif self.m_ProfferBtn == Child then
			Gac2Gas:GetProffer(g_Conn) 
			self:Destroy()
		elseif self.m_Exit == Child then
			self:Destroy()
--		elseif self.m_CancelSignBtn == Child then
--			Gac2Gas:ShowCancelWnd(g_Conn) 
--			self:Destroy()
		elseif self.m_EnterFb == Child then
			Gac2Gas:EnterRobScene(g_Conn, true)   --立即进入场景 
			self:Destroy()
		elseif self.m_HeaderGet == Child then
			Gac2Gas:GetHeaderAward(g_Conn) 
			self:Destroy()
		end
	end
end

function CRobResourceWnd:SetSceneName(sceneName)
	local sceneName = GetSceneDispalyName(sceneName)
	self.m_TitleWnd:SetWndText(GetStaticTextClient(190001, sceneName))
end



--发一个奖励的消息
function Gas2Gac:DayAwardMsgFromRobRes(Conn)
	local str = GetStaticTextClient(15400)--"抢夺资源点奖励已发放,请注意查收"
	DisplayMsg(2,str)
	SysRollAreaMsg(str)
end

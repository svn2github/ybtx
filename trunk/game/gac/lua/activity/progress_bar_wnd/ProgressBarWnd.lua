CProgressBarWnd=class(SQRDialog)
local PosRange = 100

function CProgressBarWnd:Ctor()
	self:CreateFromRes("Action_Loading",g_GameMain)
	self.m_ProgressBar = self:GetDlgChild( "Progress_Load" )
	self.m_ProgressBar:SetProgressMode(0)
	self.m_ProgressBar:SetRange(PosRange)
	self.m_PerfectArea1 = self:GetDlgChild( "PerfectArea1" )
	self.m_PerfectArea2 = self:GetDlgChild( "PerfectArea2" )
	self.m_PerfectArea1:SetMouseOverDescAfter(ToolTipsText(505))
	self.m_PerfectArea2:SetMouseOverDescAfter(ToolTipsText(506))
	self.m_IsInPerfectCollState = false
	self.m_LoadTime = nil
	self.Pos = 0
	self.m_PerfectArea1:ShowWnd(false)
	self.m_PerfectArea2:ShowWnd(false)
end

function CProgressBarWnd:SetPos( nNum )
	if self.m_ProgressBar.SetPos then
		self.m_ProgressBar:SetPos(nNum)
	end
end
function CProgressBarWnd:GetPos(  )
	return self.m_ProgressBar:GetPos()
end
function CProgressBarWnd:LoadProgressByTime(time,IsPerfectColl, ActionName) --
	time = time/PosRange
	if ActionName and ActionName ~= "" then
		self.m_ActionName = ActionName
		CPlaySoundMgr.PlayLodingSound(ActionName)
	end
	
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
	end
	self:ShowWnd(true)
	self.m_PerfectArea1:ShowWnd(IsPerfectColl)
	self.m_PerfectArea2:ShowWnd(IsPerfectColl)
	self.m_IsInPerfectCollState = IsPerfectColl
	self.Pos=0
	self:SetPos( self.Pos )
	local Loading = function()
		self.Pos = self.Pos + 1
		self:SetPos( self.Pos )
		if self.m_IsInPerfectCollState then
			self.m_PerfectArea1:SetFocus()
			self.m_PerfectArea2:SetFocus()
		end
		if(self.Pos >= PosRange) then
			self:SetPos( PosRange )
			self.m_PerfectArea1:ShowWnd(false)
			self.m_PerfectArea2:ShowWnd(false)
			self.m_IsInPerfectCollState = false
			self:ShowWnd(false)
			UnRegisterTick( self.m_LoadTime )
			self.m_LoadTime = nil
			if self.m_ActionName then
				CPlaySoundMgr.StopLodingSound(self.m_ActionName)
				self.m_ActionName = nil
			end
			--UnRegisterTick( self.m_LoadTime )
			return
		end
	end
	self.m_LoadTime = RegisterTick( "Loading", Loading , time)  --注册时钟
end

function CProgressBarWnd:OnDesLoading()
	self.Pos = 999
	self.m_PerfectArea1:ShowWnd(false)
	self.m_PerfectArea2:ShowWnd(false)
--	self.m_IsInPerfectCollState = false
	
	if self.m_ActionName then
		CPlaySoundMgr.StopLodingSound(self.m_ActionName)
		self.m_ActionName = nil
	end
	if self.m_LoadTime~=nil then
		UnRegisterTick( self.m_LoadTime )
		self.m_LoadTime = nil
	end
	
	local function callback()
		if self.m_LoadTime == nil then
			self:ShowWnd(false)
			self:SetPos( 0 )
		end
	end
	
	if not self.m_IsInPerfectCollState then
		RegisterOnceTick(g_GameMain, "StopProgressBar", callback, 500)
	else
		callback()
	end
end

function CProgressBarWnd:StopPerfetCollState()
--	print("CProgressBarWnd:StopPerfetCollState()")
	self.m_PerfectArea1:ShowWnd(false)
	self.m_PerfectArea2:ShowWnd(false)
	self.m_IsInPerfectCollState = false
	
	local PosNum = self:GetPos()
	local rt = CFRect:new()
	self.m_ProgressBar:GetLogicRect(rt)
	local BarWidth = rt.right - rt.left
	local PosWidth = BarWidth/PosRange
	
	local Perfect_rt1 = CFRect:new()
	self.m_PerfectArea1:GetLogicRect(Perfect_rt1)
	local Perfect_rt2 = CFRect:new()
	self.m_PerfectArea2:GetLogicRect(Perfect_rt2)
	
	local ClickPos = rt.left+PosNum*PosWidth
--	print(ClickPos,Perfect_rt2.left,Perfect_rt2.right)
	if ClickPos >= Perfect_rt2.left and ClickPos <= Perfect_rt2.right then
--		print("成功完美采集！获得3倍物品！")
		if self.m_LoadTime then
			UnRegisterTick( self.m_LoadTime )
			self.m_LoadTime = nil
		end
		Gac2Gas:SendSuccPerfetColl(g_Conn,3)
	elseif ClickPos >= Perfect_rt1.left and ClickPos <= Perfect_rt1.right then
--		print("成功完美采集！获得2倍物品！")
		if self.m_LoadTime then
			UnRegisterTick( self.m_LoadTime )
			self.m_LoadTime = nil
		end
		Gac2Gas:SendSuccPerfetColl(g_Conn,2)
	end
end

function CProgressBarWnd.NotifyLoadProgressBar(Conn, time, IsPerfectColl, ActionName)
	if IsPerfectColl then
		g_GameMain.m_ProgressBarWnd:LoadProgressByTime(time,IsPerfectColl, ActionName)
	else
		g_GameMain.m_NormalProgressBarWnd:LoadProgressByTime(time, ActionName)
	end
end

function CProgressBarWnd.NotifyStopProgressBar(Conn)
	if g_GameMain then
		if g_GameMain.m_ProgressBarWnd:IsShow() then
			g_GameMain.m_ProgressBarWnd:OnDesLoading()
		elseif g_GameMain.m_NormalProgressBarWnd:IsShow() then
			g_GameMain.m_NormalProgressBarWnd:OnDesLoading()
		elseif g_GameMain.m_ResourceProgressWnd:IsShow() then
			g_GameMain.m_ResourceProgressWnd:OnDesLoading()
		end
	end
end

CNormalProgressBarWnd=class(SQRDialog)

local PosRange = 100

function CNormalProgressBarWnd:Ctor()
	self:CreateFromRes("NormalAction_Loading",g_GameMain)
	self.m_NormalProgressBarWnd = self:GetDlgChild( "Progress_Load" )
	self.m_NormalProgressBarWnd:SetProgressMode(0)
	self.m_NormalProgressBarWnd:SetRange(PosRange)
	self.m_LoadTime = nil
	self.Pos = 0
end

function CNormalProgressBarWnd:SetPos( nNum )
	if self.m_NormalProgressBarWnd.SetPos then
		self.m_NormalProgressBarWnd:SetPos(nNum)
	end
end
function CNormalProgressBarWnd:GetPos(  )
	return self.m_NormalProgressBarWnd:GetPos()
end
function CNormalProgressBarWnd:LoadProgressByTime(time, ActionName) --
	time = time/PosRange
	if ActionName and ActionName ~= "" then
		self.m_ActionName = ActionName
		CPlaySoundMgr.PlayLodingSound(ActionName)
	end
	
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
	end
	self:ShowWnd(true)
	self.Pos=0
	self:SetPos( self.Pos )
	local Loading = function()
		self.Pos = self.Pos + 1
		self:SetPos( self.Pos )
		if(self.Pos >= PosRange) then
			self:SetPos( PosRange )
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
	self.m_LoadTime = RegisterTick( "Loading", Loading , time)  --×¢²áÊ±ÖÓ
end

function CNormalProgressBarWnd:OnDesLoading()
	self.Pos = 999
	self:ShowWnd(false)
	self:SetPos( 0 )
	if self.m_ActionName then
		CPlaySoundMgr.StopLodingSound(self.m_ActionName)
		self.m_ActionName = nil
	end
	
	if self.m_LoadTime~=nil then
		UnRegisterTick( self.m_LoadTime )
		self.m_LoadTime = nil
	end
end
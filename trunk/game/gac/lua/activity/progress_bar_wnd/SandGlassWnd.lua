CSandGlassWnd = class(SQRDialog)

function CSandGlassWnd:Ctor()	
	self:CreateFromRes("SandGlassWnd",g_GameMain)
	self.m_PosRange = 500
	self.Pos = 0
	self.m_TenorBarUpWnd = self:GetDlgChild("TenorBarUp")
	self.m_TenorBarDownWnd = self:GetDlgChild("TenorBarDown")
	self.m_TenorBarUpWnd:SetProgressMode(3)
	self.m_TenorBarUpWnd:SetRange(self.m_PosRange)
	self.m_TenorBarDownWnd:SetProgressMode(3)
	self.m_TenorBarDownWnd:SetRange(self.m_PosRange)
	self.m_LoadTime = nil
end

function CSandGlassWnd:SetPos()
	if self.Pos then
		self.m_TenorBarDownWnd:SetPos(self.Pos)
		self.m_TenorBarUpWnd:SetPos(self.m_PosRange-self.Pos)
	end
end

function CSandGlassWnd:LoadProgressByTime(time) --
	
	time = time/self.m_PosRange
	self:ShowWnd(true)
	self.Pos = 0
	self:SetPos()
	local Loading = function()
		self.Pos = self.Pos + 1
		self:SetPos()
		if(self.Pos >= self.m_PosRange) then
			self:SetPos( self.m_PosRange )
			self:ShowWnd(false)
			UnRegisterTick(self.m_LoadTime)
			self.m_LoadTime = nil
			return
		end
	end
	if self.m_LoadTime then
		UnRegisterTick(self.m_LoadTime)
	end
	self.m_LoadTime = RegisterTick( "Loading", Loading , time)  --×¢²áÊ±ÖÓ
end

CUseItemProgressWnd=class(SQRDialog)

local PosRange = 500

function CUseItemProgressWnd:Ctor()
	self:CreateFromRes("UseItem_Loading",g_GameMain)
	self.m_UseItemProgressWnd = self:GetDlgChild( "Progress_Load" )
	self.m_UseItemProgressWnd:SetProgressMode(0)
	self.m_UseItemProgressWnd:SetRange(PosRange)
	self.m_LoadTime = nil
	self.Pos = 0
end

function CUseItemProgressWnd:SetPos( nNum )
	if self.m_UseItemProgressWnd.SetPos then
		self.m_UseItemProgressWnd:SetPos(nNum)
	end
end
function CUseItemProgressWnd:GetPos(  )
	return self.m_UseItemProgressWnd:GetPos()
end
function CUseItemProgressWnd:LoadProgressByTime(time, ActionName) --
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
	end
	self:ShowWnd(true)
	self.Pos=0
	self:SetPos( self.Pos )
	if ActionName and ActionName ~= "" then
		self.m_ActionName = ActionName
		CPlaySoundMgr.PlayLodingSound(ActionName)
	end
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

function CUseItemProgressWnd:OnDesLoading()
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

function CUseItemProgressWnd.NotifyUseItemProgressLoad(Conn, time, ActionName)
	local ticktime = time/PosRange
	g_GameMain.m_UseItemProgressWnd:LoadProgressByTime(ticktime, ActionName)
end

function CUseItemProgressWnd.NotifyUseItemProgressStop(Conn)
	if g_GameMain.m_UseItemProgressWnd:IsShow() then
		g_GameMain.m_UseItemProgressWnd:OnDesLoading()
	end
end
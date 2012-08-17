CMessageShapeEffectWnd =  class( SQRDialog )

function CMessageShapeEffectWnd:Ctor()
	self:CreateFromRes("MessageShapeEffectWnd",g_GameMain)
	self.m_RichText = self:GetDlgChild("RichText")
end

function CMessageShapeEffectWnd:SetMsgInfo(MsgStr)
	local function StartShape(Tick)
		if self.m_StartMessageShapeTick then
			UnRegisterTick(self.m_StartMessageShapeTick)
			self.m_StartMessageShapeTick = nil
		end
		self:SetTransparentObj(250, true)
	end
	
	if self.m_StartMessageShapeTick then
		UnRegisterTick(self.m_StartMessageShapeTick)
		self.m_StartMessageShapeTick = nil
	end
	PlayCue("dxc_alert")
	self:EndTransparent(false)
	self:SetTransparent(1)
	self:ShowWnd(true)	
	self.m_RichText:SetWndText(MsgStr)
	self.m_StartMessageShapeTick = RegisterTick( "StartMessageShapeTick", StartShape, 5000 )
end
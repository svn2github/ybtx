CMessageMiniTextWnd = class(SQRDialog)

function CMessageMiniTextWnd:Ctor()
	self:CreateFromRes("MessageMiniTextWnd",g_GameMain)
	self.m_RichText = self:GetDlgChild("RichText")
end

function CMessageMiniTextWnd:SetMsgInfo(MsgStr)
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
	self:EndTransparent(false)
	self:SetTransparent(1)
	self:ShowWnd(true)	
	self.m_RichText:SetWndText(MsgStr)
	self.m_StartMessageShapeTick = RegisterTick( "StartMessageShapeTick", StartShape, 5000 )
end
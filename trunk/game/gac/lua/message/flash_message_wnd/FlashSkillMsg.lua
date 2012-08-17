CFlashSkillMsgWnd =  class( SQRDialog )

function CFlashSkillMsgWnd:Ctor()
	self:CreateFromRes("FlashSkillMsgWnd",g_GameMain)
	self.m_RichText = self:GetDlgChild("RichText")
end

local function StartTransFun( Tick, Wnd)
	if Wnd.m_StartTransTick then
		UnRegisterTick(Wnd.m_StartTransTick)	
		Wnd.m_StartTransTick = nil
	end
	Wnd:SetTransparentObj(1500,true)
end

function CFlashSkillMsgWnd:SetFlashSkillMsgWndText(Msg)		
	if self.m_StartTransTick then
		UnRegisterTick(self.m_StartTransTick)	
		self.m_StartTransTick = nil
		self:EndTransparent(true)
	end
	self:DelFlashAttention()
	self.m_RichText:SetWndText(Msg)
	self:AddFlashInfoByName("shanguang4")
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	local StrTex = g_ImageMgr:GetImagePath(1333)
	DefaultImage:AddImage(self:GetGraphic(), -1, StrTex, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self:SetWndBkImage( Flag, DefaultImage )
	self:EndTransparent(false)
	self:SetTransparent(1)
	self:ShowWnd(true)
	self.m_StartTransTick = RegisterTick( "StartTransTick", StartTransFun, 1000,self)	
end


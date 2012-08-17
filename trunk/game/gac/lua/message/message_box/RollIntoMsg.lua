cfg_load "message/SystemFriendMessage_Common"
lan_load "message/Lan_SystemFriendMessage_Common"
gac_require "framework/texture_mgr/DynImageMgr"

CRollIntoMsg = class(SQRDialog) --µ¼³öÀà

function CRollIntoMsg.CreateRollIntoMsgWnd(nMessageId)
	local text = Lan_SystemFriendMessage_Common(nMessageId, "Content")
	if(not text or "" == text) then return end
		
	local wnd = CRollIntoMsg:new(nMessageId)
	if(not g_GameMain.m_tblRollIntoMsg) then
		g_GameMain.m_tblRollIntoMsg = {}
	end
	g_GameMain.m_tblRollIntoMsg[wnd] = wnd
end

function CRollIntoMsg:Ctor(nMessageId)
	self:CreateFromRes("RollIntoMsg", g_GameMain)
	self:ShowWnd(true)
	self:SetInfo(nMessageId)
	self:RollInto()
end

function CRollIntoMsg:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_Image		= self:GetDlgChild("Image")
	self.m_Des			= self:GetDlgChild("Des")
end

function CRollIntoMsg:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:DestroyWnd()
			g_GameMain.m_tblRollIntoMsg[self] = nil
		end
	end
end

function CRollIntoMsg:SetKeyRect()
	self.m_EndRect		= CFRect:new()
	self.m_StartRect	= CFRect:new()
	self:GetWndRect(self.m_EndRect)
	local height = self.m_EndRect.bottom - self.m_EndRect.top
	self.m_StartRect.left	= self.m_EndRect.left
	self.m_StartRect.right	= self.m_EndRect.right
	self.m_StartRect.top	= self.m_EndRect.bottom
	self.m_StartRect.bottom	= self.m_StartRect.top + height
end

function CRollIntoMsg:SetInfo(nMsgID)
	local text		= Lan_SystemFriendMessage_Common(nMsgID, "Content")
	local imagePath	= SystemFriendMessage_Common(nMsgID, "ImagePath")
	if(imagePath and "" ~= imagePath) then
		local imageW	= SystemFriendMessage_Common(nMsgID, "ImageWidth")
		local imageH	= SystemFriendMessage_Common(nMsgID, "ImageHeight")
		local rt		= CFRect:new()
		rt.left			= SystemFriendMessage_Common(nMsgID, "ImagePosX")
		rt.right		= rt.left + imageW
		rt.top			= SystemFriendMessage_Common(nMsgID, "ImagePosY")
		rt.bottom		= rt.top + imageH
		g_DynImageMgr:AddImageNeedCut( self.m_Image, imagePath, IP_ENABLE, rt, CFPos:new() )
	end
	self.m_Des:SetWndText(text)
end

function CRollIntoMsg:RollInto()
	self:ShowWnd(true)
	self:SetKeyRect()
	self:SetShapeEffect(1000, self.m_StartRect, self.m_EndRect)
end

function CRollIntoMsg:RollOut()
	self:ShowWnd(true)
	self:SetKeyRect()
	self:SetShapeEffect(1000, self.m_EndRect, self.m_StartRect)
end
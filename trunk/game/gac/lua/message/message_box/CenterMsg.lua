cfg_load "image_res/CenterMsgImage_Client"

CCenterMsg = class(SQRDialog) --导出类

function CCenterMsg:Ctor()
	self:CreateFromRes("CenterMsg", g_GameMain)
end

function CCenterMsg:OnChildCreated()
	self.m_Icon			= self:GetDlgChild("Icon")
	self.m_RichText		= self:GetDlgChild("RichText")
	self.m_FlashBar		= self:GetDlgChild("Z001")
	self.m_MoveFlash	= self:GetDlgChild("Z002")
end

function CCenterMsg:WndProc( uMsgID, uParam1, uParam2 )
	if (uMsgID == WM_LBUTTONDOWN) then
		if(13 == self.m_uType) then
			g_GameMain.m_GuideWnd:OpenPanel(true)
		end
		self:ShowWnd(false)
	end
end

function CCenterMsg:OpenPanel(sText, uType)
	local arg = g_ParseCommandline()[3] --鲜血试炼服
	if arg and arg.gastype == 1 then
		return
	end
	
	local icon = CenterMsgImage_Client(uType, "ImagePath")
	g_DynImageMgr:AddImageByIP(self.m_Icon, icon, IP_ENABLE)
	self.m_uType = uType
	self.m_Icon:DelFlashAttention()
	self.m_FlashBar:DelFlashAttention()
	self.m_MoveFlash:DelFlashAttention()
	self.m_Icon:AddFlashInfoByName("Sparkel")
	self.m_FlashBar:AddFlashInfoByName("HolyLight")
	self.m_MoveFlash:AddFlashInfoByName("Glory")
	self.m_RichText:SetWndText("#c8f7039" .. sText)
	self:SetTransparent(1)
	self:SetTransparentObj(0, false)
	self:ShowWnd(true)
	self:DoEffect()
end

function CCenterMsg:DoEffect()
	local rect = CFRect:new()
	self:GetWndRect(rect)
	
	local rectStart = CFRect:new()
	self.m_MoveFlash:GetWndRect(rectStart)
	width = rectStart.right - rectStart.left
	rectStart.left	= rect.left - width/2
	rectStart.right	= rect.left + width
	
	local rectEnd = CFRect:new()
	self.m_MoveFlash:GetWndRect(rectEnd)
	rectEnd.right	= rect.right + width/2
	rectEnd.left	= rect.right - width
	
	self.m_MoveFlash:SetShapeEffect(500, rectStart, rectEnd)
end
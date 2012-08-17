cfg_load "quest/HeadHelp_Client"
lan_load "quest/Lan_HeadHelp_Client"

CHeadHelpWnd = class(SQRDialog)

function CHeadHelpWnd:Ctor()
	self:CreateFromRes( "HeadHelpWnd", g_GameMain )
	self.m_DisappearTime = 0
	self.m_Image = self:GetDlgChild("Image")
	self.m_Text = self:GetDlgChild("Text")
end

function CHeadHelpWnd:CloseHeadHelpWnd()
	self:SetTransparent(1)
	self:SetTransparentObj(self.m_DisappearTime, true)
	--g_MainPlayer:GetRenderObject():DelChild(self)
end

local function CloseHeadHelpWnd()
	g_GameMain.m_HeadHelpWnd:CloseHeadHelpWnd()
end

function CHeadHelpWnd:ShowHeadHelpWnd(Name)
	if self:IsShow() then
		return
	end
	self.m_DisappearTime = HeadHelp_Client(Name, "DisappearTime")
	local LastTime = HeadHelp_Client(Name, "LastTime")
	local TexPath = HeadHelp_Client(Name, "TexPath")
	local Text = Lan_HeadHelp_Client( MemH64(Name), "Text" )
	
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self.m_Image:GetGraphic(), -1,  TexPath, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self.m_Image:SetWndBkImage( Flag, DefaultImage )
	self.m_Image:SetWndText("")
	self.m_Text:SetWndText(Text)
	self:SetTransparent(1)
	self:ShowWnd(true)
	
	--g_MainPlayer:GetRenderObject():AddChild( self, eLinkType.LT_SKELETAL, "Bip01 Head" )
	RegisterOnceTick(g_GameMain, "CloseHeadHelpWnd", CloseHeadHelpWnd, LastTime*1000)
end


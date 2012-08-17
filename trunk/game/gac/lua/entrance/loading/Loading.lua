cfg_load "tips/LoadingRandomImage_Client"
lan_load "tips/Lan_Tips_Client"
lan_load "StaticText_Common"

CLoading	= class( SQRDialog )

function CLoading:Ctor(parent)
	self:CreateLoading(parent)
end

function CLoading:CreateLoading( parent )
	self:CreateFromResEx( "Login_Loading", parent, false, false )
	self:ShowWnd( true )
	self.m_ProgressLoad	= self:GetDlgChild( "Progress_Load" )
	self.m_wndTips		= self:GetDlgChild( "Wnd_Tips" )
	self.m_wndProcessStatus = self:GetDlgChild("Wnd_ShowProcessStatus")
end

function CLoading:SetRandomText()
	local keys = Lan_Tips_Client:GetKeys()
	local r = math.random(# keys)
	self.m_wndTips:SetWndText(Lan_Tips_Client(keys[r], "DisplayTips"))
	self:AddImage()
end

function CLoading:AddImage()
	local keys = LoadingRandomImage_Client:GetKeys()
	local randomNo = math.random(# keys)
	local strTexture = LoadingRandomImage_Client(keys[randomNo],"ImagePath")
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 0 ) 	
	self:SetWndBkImage( Flag, DefaultImage )
end

function CLoading:SetPos(nNum)
	self.m_ProgressLoad:SetPos(nNum)
	self.m_ProgressLoad:SetWndText(self:GetPos())
end

function CLoading:GetPos()
	return self.m_ProgressLoad:GetPos()
end

function CLoading:SetProcessStatusInfo(nIndex)
	local content = GetStaticTextClient(nIndex)
	self.m_wndProcessStatus:SetWndText(content)
	CRenderSystemClient_Inst():Refresh()
	CRenderSystemClient_Inst():Refresh()
end

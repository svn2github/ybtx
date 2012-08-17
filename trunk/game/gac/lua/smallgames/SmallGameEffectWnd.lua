gac_require "smallgames/SmallGameEffectWndInc"

local function SetWndRect(ChildWnd)
	local rt_1 = CFRect:new()
	ChildWnd:GetLogicRect(rt_1)
	rt_1.right = rt_1.left + 200
	rt_1.bottom = rt_1.top + 200
	ChildWnd:SetLogicRect(rt_1)
end

function CreatSmallGameEffectWnd(ParentWnd)
	local Wnd = CSmallGameEffectWnd:new()
	Wnd:CreateFromRes("SmallGameEffectWnd", ParentWnd)
	Wnd.m_GifWnd = Wnd:GetDlgChild("GifWnd")
	--SetWndRect(Wnd)
	g_MainPlayer:GetRenderObject():AddChild( Wnd, eLinkType.LT_SKELETAL, "Bip01 Head" )
	--Wnd:SetRenderWndHeight(20)
	Wnd:SetOff(-100,-50)

	return Wnd
end

function CSmallGameEffectWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		
	end
end

function CSmallGameEffectWnd:AddImage(strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self.m_GifWnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self.m_GifWnd:SetWndBkImage( Flag, DefaultImage )
end

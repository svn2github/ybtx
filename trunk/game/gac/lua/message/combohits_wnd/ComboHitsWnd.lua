CComboHitsWnd= class( SQRDialog )

local WndNumber = 3
local TickTime = 10

local function UnRegisterTickFun(Wnd)
	if Wnd.m_MoveComboHitsWndTick then
		UnRegisterTick(Wnd.m_MoveComboHitsWndTick)
	end
	Wnd.m_MoveComboHitsWndTick = nil
	
	if Wnd.m_HoldStateWndTick then
		UnRegisterTick(Wnd.m_HoldStateWndTick)
	end
	Wnd.m_HoldStateWndTick = nil	
end

function CComboHitsWnd.HideComboHitsWnd()
	for i = 1, WndNumber do 
		if g_GameMain.m_ComboHitsWndTbl[i] then
			UnRegisterTickFun(g_GameMain.m_ComboHitsWndTbl[i])		
		end
		g_GameMain.m_ComboHitsWndTbl[i]:ShowWnd(false)
	end
end

function CComboHitsWnd:Ctor()
	self:CreateFromResEx("ComboHitsWnd",g_GameMain,false,false)
	self.m_NumberTxt = self:GetDlgChild("NumberTxt")
end

local function HoldStatefun(Tick,Wnd)
	UnRegisterTickFun(Wnd)
	Wnd:SetTransparentObj(5000,true)
end

local function MoveComboHitsWndFun(Tick,Wnd)
	Wnd:ShowWnd(true)
	Wnd:SetTransparent(1)
	
	local InitRt = CFRect:new()
	g_GameMain.m_ComboHitsRectWnd:GetWndRect(InitRt)
	local ParentRt = CFRect:new()
	ParentRt.left  = InitRt.left - 180
	ParentRt.right  = InitRt.right - 180 
	ParentRt.top = InitRt.top
	ParentRt.bottom = InitRt.bottom

	local WndRt = CFRect:new()
	Wnd:GetWndRect(WndRt)
	if Wnd.m_StepWidth < 0 then
		if ParentRt.right - WndRt.right < 3 then
			UnRegisterTickFun(Wnd)
			if g_GameMain.ComboHitsInfo.m_IsCrazyState then
				Wnd:EndTransparent(false)
			else
				Wnd.m_HoldStateWndTick = RegisterTick("HoldStateWndTick", HoldStatefun, 50000,Wnd)
			end
		end
		WndRt.left = WndRt.left - Wnd.m_StepWidth
		WndRt.right = WndRt.right - Wnd.m_StepWidth
		Wnd:SetWndRect(WndRt)			
	elseif (ParentRt.right - WndRt.right < 15 or ParentRt.right - WndRt.right < 0 )and Wnd.m_StepWidth > 0  then 
		WndRt.left = WndRt.left - Wnd.m_StepWidth
		WndRt.right = WndRt.right - Wnd.m_StepWidth
		Wnd:SetWndRect(WndRt)	
		Wnd.m_StepWidth = Wnd.m_StepWidth + 1
	elseif ParentRt.right - WndRt.right >= 15 and Wnd.m_StepWidth > 0 then
		Wnd.m_StepWidth = - Wnd.m_StepWidth
	end
end

local function GetComboHitsWnd()
	for i = 1 ,WndNumber do 
		if not  g_GameMain.m_ComboHitsWndTbl[i]:IsShow() then
			local WndRt = CFRect:new()
			g_GameMain.m_ComboHitsRectWnd:GetWndRect(WndRt)
			g_GameMain.m_ComboHitsWndTbl[i]:SetWndRect(WndRt)
			g_GameMain.m_ComboHitsWndTbl[i]:SetTransparent(1)
			return g_GameMain.m_ComboHitsWndTbl[i]
		end
	end
end

local function AddImageFun(Wnd,StrTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  StrTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

function CComboHitsWnd.ShowComboHitsWnd(Number)
	if Number == 4  and not g_GameMain.ComboHitsInfo.m_IsCrazyState then 
		g_GameMain.ComboHitsInfo.m_IsCrazyState  = true
		CComboHitsWnd.HideComboHitsWnd()
		local Wnd = GetComboHitsWnd()
		Wnd:ShowWnd(true)
		Wnd.m_NumberTxt:ShowWnd(false)
		Wnd.m_StepWidth = 2
		local StrTex = g_ImageMgr:GetImagePath(1328)
		AddImageFun(Wnd,StrTex)
		Wnd.m_MoveComboHitsWndTick = RegisterTick("MoveComboHitsWndFun", MoveComboHitsWndFun, TickTime,Wnd)
	elseif Number < 4 then
		g_GameMain.ComboHitsInfo.m_IsCrazyState  = false
		CComboHitsWnd.HideComboHitsWnd()
		local Wnd = GetComboHitsWnd()
		Wnd:ShowWnd(true)
		Wnd.m_NumberTxt:ShowWnd(true)
		local ImageID = 1044 + Number
		Wnd.m_NumberTxt:SetWndText(CTextureFontMgr_Inst():GetEmtStrByIdx("#" .. ImageID))
		Wnd.m_StepWidth = 2
		local StrTex = g_ImageMgr:GetImagePath(1329)
		AddImageFun(Wnd,StrTex)
		Wnd.m_MoveComboHitsWndTick = RegisterTick("MoveComboHitsWndFun", MoveComboHitsWndFun, TickTime,Wnd)
	end
end

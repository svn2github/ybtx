CFlashMsgWnd =  class( SQRDialog )

function CFlashMsgWnd:Ctor()
	self:CreateFromRes("FlashMsgWnd",g_GameMain )
	self.m_RichText = self:GetDlgChild("RichText")
end

local function StartTransFun( Tick, Wnd)
	if Wnd.m_StartTransTick then
		UnRegisterTick(Wnd.m_StartTransTick)	
		Wnd.m_StartTransTick = nil
	end
	Wnd:SetTransparentObj(2000,true)
end

local texTbl = {
				[6] = {["BackTex"] = g_ImageMgr:GetImagePath(1330),["FlashStr"] =  "shanguang1"},
				[7] = {["BackTex"] = g_ImageMgr:GetImagePath(1331),["FlashStr"] =  "shanguang2"},
				[8] = {["BackTex"] = g_ImageMgr:GetImagePath(1332),["FlashStr"] =  "shanguang3"}
				}

function CFlashMsgWnd:SetFlashMsgWndText(Msg,MsgType)
	local function ShowFlashMsgWnd(Tick,Wnd)
		if Wnd.m_ShowFlashMsgWndTick then
			UnRegisterTick(Wnd.m_ShowFlashMsgWndTick)	
			Wnd.m_ShowFlashMsgWndTick = nil
		end			
		if Wnd.m_StartTransTick then
			UnRegisterTick(Wnd.m_StartTransTick)	
			Wnd.m_StartTransTick = nil
			Wnd:EndTransparent(true)
		end
		PlayCue("WndOp_com04")
		Wnd.m_RichText:SetWndText(Msg)
		Wnd:AddFlashInfoByName(texTbl[MsgType].FlashStr)
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE	
		local DefaultImage = WND_IMAGE_LIST:new()
		
		DefaultImage:AddImage(Wnd:GetGraphic(), -1,  texTbl[MsgType].BackTex, nil, CFPos:new(), "0xffffffff", 15 ) 	
		Wnd:SetWndBkImage( Flag, DefaultImage )
		Wnd:EndTransparent(false)
		Wnd:SetTransparent(1)
		Wnd:ShowWnd(true)
		Wnd.m_StartTransTick = RegisterTick( "StartTransTick", StartTransFun, 4500,Wnd)	
	end
	if self.m_ShowFlashMsgWndTick then
		UnRegisterTick(self.m_ShowFlashMsgWndTick)	
		self.m_ShowFlashMsgWndTick = nil
	end
	if MsgType ~= 8 then
		g_App:GetRootWnd():PlayScreenFx("baiguang2/b_w")
	end
	self.m_ShowFlashMsgWndTick = RegisterTick( "ShowFlashMsgWndTick", ShowFlashMsgWnd, 380,self)	
end


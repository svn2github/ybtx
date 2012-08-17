CTongItemUseTrans = class(SQRDialog)

function CTongItemUseTrans:Ctor(Parent)
	self:CreateFromRes("TongItemUseWnd", Parent)
	self.m_TextInfo		= self:GetDlgChild("TextInfo")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_CancelBtn    	= self:GetDlgChild("CancelBtn")
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongItemUseTrans.GetWnd(sceneName, posx, posy, name, sceneId, serverId)
	if not g_GameMain.m_TongItemUseWnd then
		g_GameMain.m_TongItemUseWnd = CTongItemUseTrans:new(g_GameMain, 15)
	end
	local wnd = g_GameMain.m_TongItemUseWnd
	if wnd:IsShow() then
		return
	end
	wnd.m_SceneName = sceneName
	wnd.m_PosX = posx
	wnd.m_Posy = posy
	wnd.m_Name = name
	wnd.m_SceneId = sceneId
	wnd.m_ServerId = serverId
	wnd.m_CountDownTime = 15
	wnd.m_TextInfo:SetWndText(GetStaticTextClient(400002, name, GetSceneDispalyName(sceneName)))
	wnd.m_OkBtn:SetWndText(GetStaticTextClient(400001, wnd.m_CountDownTime))
	if wnd.m_ShowTimeTick then
		UnRegisterTick(wnd.m_ShowTimeTick)
		wnd.m_ShowTimeTick = nil
	end
	wnd.m_ShowTimeTick = RegClassTick("TongItemUseTick", wnd.ShowTimeTick, 1000, wnd)
	wnd:ShowWnd(true)
end

function CTongItemUseTrans:ShowTimeTick()
	self.m_CountDownTime = self.m_CountDownTime - 1
	if self.m_CountDownTime < 0 then
		self:Destroy()
		 --´«ËÍ
		self:Destroy()
		--Gac2Gas:ChangeSceneByMsg(g_Conn, self.m_SceneName, self.m_PosX, self.m_Posy)
		return
	end
	self.m_OkBtn:SetWndText(GetStaticTextClient(400001, self.m_CountDownTime))
	self:ShowWnd(true)
end

function CTongItemUseTrans:Destroy()
	if self.m_ShowTimeTick then
		UnRegisterTick(self.m_ShowTimeTick)
		self.m_ShowTimeTick = nil
	end
	self:ShowWnd(false)
end

function CTongItemUseTrans:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_OkBtn == Child then
			self:Destroy()
			Gac2Gas:ChangeSceneByMsg(g_Conn, self.m_PosX, self.m_Posy, self.m_SceneId, self.m_ServerId)
		elseif self.m_CancelBtn == Child then
			self:Destroy()
		end
	end
end

function CTongItemUseTrans.UnRegisterItemUsetTick()
	if g_GameMain.m_TongItemUseWnd then
		UnRegisterTick(g_GameMain.m_TongItemUseWnd.m_ShowTimeTick)
		g_GameMain.m_TongItemUseWnd.m_ShowTimeTick = nil
	end
end

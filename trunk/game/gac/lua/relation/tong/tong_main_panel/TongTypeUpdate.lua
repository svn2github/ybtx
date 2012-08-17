
CTongTypeUpdateWnd		= class(SQRDialog)

---------------------- 类型提升 --------------------
function CTongTypeUpdateWnd.GetWnd()
	local Wnd = g_GameMain.m_TongTypeUpdatWnd
	if Wnd == nil then
		g_GameMain.m_TongTypeUpdatWnd = CTongTypeUpdateWnd:new(g_GameMain)
		Wnd = g_GameMain.m_TongTypeUpdatWnd
	end
	return Wnd
end

function CTongTypeUpdateWnd:Ctor(Parent)
	self:CreateFromRes("TongTypeUpdateWnd", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongTypeUpdateWnd:OnChildCreated()
	self.m_UpdateBtn1	= self:GetDlgChild("UpdateBtn1")
	self.m_UpdateBtn2	= self:GetDlgChild("UpdateBtn2")
	self.m_CloseBtn		= self:GetDlgChild("Close")
	self.m_ConText1		= self:GetDlgChild("StaticText4")
	self.m_ConText2		= self:GetDlgChild("StaticText8")
	self:ShowConText()
	self.m_UpdateToType = 0
end

function CTongTypeUpdateWnd:ShowConText()
	self.m_ConText1:SetWndText(GetStaticTextClient(9125))
	self.m_ConText2:SetWndText(GetStaticTextClient(9126))
end
--消息
function CTongTypeUpdateWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_UpdateBtn1) then
			self.m_UpdateToType = g_TongMgr:GetTongTypeByName("精英战斗")
			self:OnUpdateTongTypeBtn(1)
		elseif(Child == self.m_UpdateBtn2) then
			self.m_UpdateToType = g_TongMgr:GetTongTypeByName("精英生产")
			self:OnUpdateTongTypeBtn(2)
		elseif(Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		end
	end
end

function CTongTypeUpdateWnd:OnUpdateTongTypeBtn(uType)
	local function IsContinue(GameWnd, uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:UpTongType(g_Conn,self.m_UpdateToType)
		end
		return true
	end
	if self.m_UpdateToType == g_TongMgr:GetTongTypeByName("精英战斗") and 
		g_GameMain.m_TongBase.m_TongType ~= g_TongMgr:GetTongTypeByName("战斗") then
		MsgClient(9366)
		return
	end
	if self.m_UpdateToType == g_TongMgr:GetTongTypeByName("精英生产") and 
		g_GameMain.m_TongBase.m_TongType ~= g_TongMgr:GetTongTypeByName("生产") then
		MsgClient(9367)
		return
	end
	
	local strMsg = uType == 1 and GetStaticTextClient(10052) or GetStaticTextClient(10056)
	MessageBox(g_GameMain, strMsg, BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
end

function CTongTypeUpdateWnd:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
end

function CTongTypeUpdateWnd:OnUpTongTypeFun()
	if g_GameMain.m_TongBase.m_TongType == g_TongMgr:GetTongTypeByName("普通") then
		MsgClient(9363)
		return false
	end
	if g_GameMain.m_TongBase.m_TongType == g_TongMgr:GetTongTypeByName("精英战斗") or
		 g_GameMain.m_TongBase.m_TongType == g_TongMgr:GetTongTypeByName("精英生产") then
		MsgClient(9364)
		return false
	end
	self:OpenPanel(true)
end
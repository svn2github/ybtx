
CTongTypeChangeWnd		= class(SQRDialog)

function CTongTypeChangeWnd.GetWnd()
	local Wnd = g_GameMain.m_TongTypeChangeWnd
	if Wnd == nil then
		g_GameMain.m_TongTypeChangeWnd = CTongTypeChangeWnd:new(g_GameMain)
		Wnd = g_GameMain.m_TongTypeChangeWnd
	end
	return Wnd
end

function CTongTypeChangeWnd:Ctor(Parent)
	self:CreateFromRes("TongTypeChangeWnd", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongTypeChangeWnd:OnChildCreated()
	self.m_ChangeBtn1	= self:GetDlgChild("ChangeBtn1")
	self.m_ChangeBtn2	= self:GetDlgChild("ChangeBtn2")
	self.m_CloseBtn		= self:GetDlgChild("Close")
	self.m_ConText1		= self:GetDlgChild("StaticWnd7")
	self.m_ConText2		= self:GetDlgChild("StaticWnd8")
	self:ShowConText()
end

function CTongTypeChangeWnd:ShowConText()
	self.m_ConText1:SetWndText(GetStaticTextClient(9123))
	self.m_ConText2:SetWndText(GetStaticTextClient(9124))
end
--消息
function CTongTypeChangeWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ChangeBtn1) then
			self:OnChangeTongTypeBtn(1)
		elseif(Child == self.m_ChangeBtn2) then
			self:OnChangeTongTypeBtn(2)
		elseif(Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		end
	end
end

function CTongTypeChangeWnd:OnChangeTongTypeBtn(TongType)
	if g_GameMain.m_TongBase.m_TongType ~= g_TongMgr:GetTongTypeByName("普通") then
		local uMsgId = TongType==1 and 9368 or 9369
		MsgClient(uMsgId)
		return
	end
	
	local function IsContinue(GameWnd, uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:ChangeTongType(g_Conn, TongType)
		end
		return true
	end
	local strMsg = TongType==1 and GetStaticTextClient(10048) or GetStaticTextClient(10049)
	MessageBox(g_GameMain, strMsg, BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
end

function CTongTypeChangeWnd:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
end

function CTongTypeChangeWnd:ResetTongType()
	if g_GameMain.m_TongBase.m_TongType == g_TongMgr:GetTongTypeByName("普通") then
		MsgClient(9371)
		return
	end
	local function IsContinue(GameWnd, uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:ResetTongType(g_Conn)
		end
		return true
	end
	local strMsg = GetStaticTextClient(10058)
	MessageBox(g_GameMain, strMsg, BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
end

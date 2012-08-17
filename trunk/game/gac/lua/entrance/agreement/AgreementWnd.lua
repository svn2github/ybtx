CUserAgreementWnd = class(SQRDialog)

function CUserAgreementWnd:Ctor(parent)
	self:CreateFromRes("User_Agreement", parent)
	self:SetAgreementText()
end

function CUserAgreementWnd:OnChildCreated()
	self.m_btnDeny		= self:GetDlgChild("Btn_Deny")
	self.m_btnAgree		= self:GetDlgChild("Btn_Agree")
	self.m_CAgreement	= self:GetDlgChild("Agreement")
	self.m_btnDeny:EnableWnd(true)
	self.m_btnAgree:EnableWnd(false)
end

--响应用户协议窗口中的事件
function CUserAgreementWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_btnAgree) then
			self:BackUpAnimation()
		elseif(Child == self.m_btnDeny) then 
			local function CallBack(Context, Button)
				if MB_BtnOK == Button then
					g_Conn:ShutDown()
					coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
				else
					return true
				end
			end
			MessageBox(self, MsgBoxMsg(403), BitOr( MB_BtnOK, MB_BtnCancel), CallBack)			
		end
	elseif( uMsgID == RICH_SCROLL ) then --滚动条被拖动到底部
		if(Child == self.m_CAgreement) then
			if( self.m_CAgreement:Isbottom() ) then
				self.m_btnAgree:EnableWnd(true)
			end
		end
	end
end

function CUserAgreementWnd:OnDestroy()
	if(self.Tick_BackUpAnimationEnd) then
		UnRegisterTick(self.Tick_BackUpAnimationEnd)
		self.Tick_BackUpAnimationEnd = nil
	end
end

function CUserAgreementWnd:SetAgreementText()
	local m_AgreementFilePath = g_RootPath .. "var/gac/Agreement.txt"
	local file = CLuaIO_Open(m_AgreementFilePath, "r")
	if(file) then
		local text = file:ReadAll()
		file:Close()
		self.m_CAgreement:SetWndText(text)
	else
		local function CallBack(Context, Button)
			if (Button == MB_BtnOK) then
				g_Conn:ShutDown()
				coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
			end
		end
		MessageBox(g_Login, MsgBoxMsg(404), MB_BtnOK, CallBack)
	end
end

function CUserAgreementWnd:Animation()
	local zoom			= self:GetRootZoomSize()
	local RectEnd		= CFRect:new()
	local RectStart		= CFRect:new()
	self:GetWndRect(RectEnd)
	RectStart.top		= RectEnd.top - 20*zoom
	RectStart.bottom	= RectEnd.bottom - 20*zoom
	RectStart.left		= RectEnd.left
	RectStart.right		= RectEnd.right
	self:ShowWnd(true)
	self:SetTransparent(0.1)
	self:SetTransparentObj(350, false)
	self:SetShapeEffect(350, RectStart, RectEnd)
end

function CUserAgreementWnd:BackUpAnimation()
	local zoom			= self:GetRootZoomSize()
	local RectEnd		= CFRect:new()
	local RectStart		= CFRect:new()
	self:GetWndRect(RectEnd)
	RectStart.top		= RectEnd.top - 20*zoom
	RectStart.bottom	= RectEnd.bottom - 20*zoom
	RectStart.left		= RectEnd.left
	RectStart.right		= RectEnd.right
	self:SetTransparentObj(600, true)
	self:SetShapeEffect(600, RectEnd, RectStart)
	
	local function BackUpAnimationEnd()
		UnRegisterTick(self.Tick_BackUpAnimationEnd)
		self.Tick_BackUpAnimationEnd = nil
		self:ShowWnd(false)
		Gac2Gas:SetUserAgreement(g_Conn)
		coroutine.resume( g_App.MainRoutine, EGameState.eToSelectChar )
	end
	
	------------------------------------------------------------------------------
	if(self.Tick_BackUpAnimationEnd) then
		return
	end
	self.Tick_BackUpAnimationEnd = RegisterTick("BackUpAnimationEnd", BackUpAnimationEnd, 800)
	------------------------------------------------------------------------------
end
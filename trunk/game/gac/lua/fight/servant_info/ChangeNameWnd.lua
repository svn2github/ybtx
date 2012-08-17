CChangeNameWnd			= class( SQRDialog )

function CChangeNameWnd:Ctor( Parent )
	self:CreateFromRes( "ChangeSTWnd",Parent )
	self.m_pWndText 	= self:GetDlgChild("wndtext")
	self.m_pOK				= self:GetDlgChild("ok")
	self.m_pClose			= self:GetDlgChild("close")
	self.m_pNameText	= self:GetDlgChild("nameTest")
end

function CChangeNameWnd.GetWnd()
	local Wnd = g_GameMain.m_ChangeServantNameWnd
	if not Wnd then
		Wnd = CChangeNameWnd:new(g_GameMain)
		g_GameMain.m_ChangeServantNameWnd = Wnd
	end
	return Wnd
end

function CChangeNameWnd:Init(eType)
	self.eNpcType = eType
end

function CChangeNameWnd:SetNameText(text)
	self.m_pWndText:SetWndText(text)
end
--ÕÙ»½ÊÞ¸ÄÃûÃæ°å
function CChangeNameWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( (uMsgID == BUTTON_LCLICK and Child == self.m_pOK) or uMsgID == EDIT_RETURN ) then
		local name = self.m_pNameText:GetWndText()
		if (name == nil or name == "" or not self:CheckAllCharAreBlankChar(name))then
			self.m_pNameText:SetWndText("")
			self.m_MsgBox  = MessageBox( self, MsgBoxMsg(2002), MB_BtnOK )
			return
		end
		local textFltMgr	= CTextFilterMgr:new()
		if( not textFltMgr:IsValidName(name) ) then
			MsgClient(197005)
			return
		end
		if(string.len(name) > 32 )then
			self.m_pNameText:SetWndText("")
			self.m_MsgBox  = MessageBox( self, MsgBoxMsg(2001), MB_BtnOK )
			return
		end
		local pServant = g_MainPlayer:GetServant(self.eNpcType)
		if pServant and pServant.m_Properties then
			Gac2Gas:RequestSetServantName(g_Conn, pServant:GetEntityID(), pServant.m_Properties:GetCharName(), name)
		end
		self.m_pNameText:SetWndText("")
		self:ShowWnd(false)
	elseif ( uMsgID == BUTTON_LCLICK and Child ==self.m_pClose ) then
		self.m_pNameText:SetWndText("")
		self:ShowWnd(false)
	end
end
function CChangeNameWnd:CheckAllCharAreBlankChar(content)
	local contentNotAllBlankChar = false
	local contentLen = string.len(content)
	for i=1, contentLen do
		local char = string.sub(content,i, i)
		if char ~= ' ' and char~= '\n' then
			contentNotAllBlankChar = true 
			break
		end
	end
	return contentNotAllBlankChar
end

function CChangeNameWnd:ShowAndSetText(Text)
	self:SetNameText(Text)
	self:ShowWnd(true)
	self.m_pNameText:SetFocus()
end
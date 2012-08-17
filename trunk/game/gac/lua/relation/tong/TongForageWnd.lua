--gac_require "relation/tong/TongForageWndInc"
CTongForageWnd = class(SQRDialog)
	
function CTongForageWnd:Ctor(parent)
	self:CreateFromRes("TongForageWnd",parent)
	self.m_ShowText = self:GetDlgChild("ShowText")
	self.m_ExitBtn = self:GetDlgChild("ExitBtn")
	self.m_EnterBtn = self:GetDlgChild("EnterBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
--	self.m_ShowText:SetWndText(GetStaticTextClient(210000, toolNum,addForageNum)) 
end

function CTongForageWnd:GetWnd(toolNum,addForageNum)
	local Wnd = g_GameMain.m_HandForage
	if not Wnd then
		Wnd = CTongForageWnd:new(g_GameMain)
		g_GameMain.m_HandForage = Wnd
	end
	Wnd:ShowWnd(true)
	return Wnd
end


function CTongForageWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if(Child == self.m_ExitBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		else
			Gac2Gas:DeleteHandForage(g_Conn)
			self:ShowWnd(false)
		end
	end
end

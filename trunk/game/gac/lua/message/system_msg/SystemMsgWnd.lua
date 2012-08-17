gac_require "message/system_msg/SystemMsgWndInc"

function CreateSystemMsgWnd(Parent)
	local Wnd = CSystemMsgWnd:new()
	Wnd:CreateFromRes("SystemMsgWnd",Parent)
	Wnd.m_SystemMsgBtn = Wnd:GetDlgChild("SystemMsgBtn")
	return Wnd
end

function CreateSystemMsgInfo(Parent)
	local Wnd = CSystemMsgInfo:new()
	Wnd:CreateFromRes("SystemMsgInfo",Parent)
	Wnd.m_SystemMsgText = Wnd:GetDlgChild("SystemMsgText")
	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
	return Wnd
end

function CSystemMsgWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_SystemMsgBtn) then
			g_GameMain.m_SystemMsgInfo:ShowWnd(true)
			self:ShowWnd(false)
		end
	end
end

function CSystemMsgInfo:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		end
	end
end

function SendSystemMsg(msg)
	if msg and msg ~= "" then
		g_GameMain.m_SystemMsgWnd:ShowWnd(true)
		g_GameMain.m_SystemMsgInfo.m_SystemMsgText:SetWndText(msg)
	end
end
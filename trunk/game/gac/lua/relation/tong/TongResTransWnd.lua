gac_require "relation/tong/TongResTransWndInc"

------我的订单面板------

function CreateTongResTransWnd(Parent)
	local Wnd = CTongResTransWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongResTransWnd:Init(Parent)
	self:CreateFromRes("TongResTransWnd", Parent)
	self:ShowWnd(false)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongResTransWnd:OnChildCreated()
	self.m_TongName = self:GetDlgChild("TongNameEdit")
	self.m_TongName:SetMaxTextLenght(8)
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
end

function CTongResTransWnd:OpenPanel()
	self:ShowWnd(true)
end

--消息
function CTongResTransWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CancelBtn or Child == self.m_XBtn then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			self:TransToTongArea()
		end
	end
end

function CTongResTransWnd:TransToTongArea()
	local TongName = self.m_TongName:GetWndText()
	if not TongName or TongName == "" then
		MsgClient(9040)
		return
	end
	Gac2Gas:TransToTongArea(g_Conn, TongName)
	self:ShowWnd(false)
end
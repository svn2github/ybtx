gac_require "relation/tong/TongMonsAttackListWndInc"

function CreateTongMonsAttackListWnd(Parent)
	local Wnd = CTongMonsAttackListWnd:new()
	Wnd:CreateFromRes("TongMonsAttackListWnd", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

function CTongMonsAttackListWnd:OnChildCreated()
	self.m_MemberName = self:GetDlgChild("MemberName")
	self.m_Level = self:GetDlgChild("Level")
	self.m_KillNum = self:GetDlgChild("KillNum")
end

function CTongMonsAttackListWnd:SetInfo(ListInfo)
	self.m_MemberName:SetWndText(ListInfo[1])
	self.m_Level:SetWndText(ListInfo[2])
	self.m_KillNum:SetWndText(ListInfo[3])
end

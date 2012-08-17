gac_require "relation/tong/TongBattleCountWndInc"

------帮会战斗统计面板------

function CreateTongBattleCountWnd(Parent)
	local Wnd = CTongBattleCountWnd:new()
	Wnd:CreateFromRes("TongBattleCountListWnd", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

function CTongBattleCountWnd:OnChildCreated()
	self.m_MemberName = self:GetDlgChild("MemberName")
	self.m_Level = self:GetDlgChild("Level")
	self.m_KillNum = self:GetDlgChild("KillNum")
	self.m_MaxContiNum = self:GetDlgChild("MaxContiNum")
	self.m_DeadTimes = self:GetDlgChild("DeadTimes")
	self.m_GetExploit = self:GetDlgChild("GetExploit")
end

function CTongBattleCountWnd:SetInfo(ListInfo)
	self.m_MemberName:SetWndText(ListInfo[1])
	self.m_Level:SetWndText(ListInfo[2])
	self.m_KillNum:SetWndText(ListInfo[3])
	self.m_MaxContiNum:SetWndText(ListInfo[4])
	self.m_DeadTimes:SetWndText(ListInfo[5])
	self.m_GetExploit:SetWndText(ListInfo[6])
end

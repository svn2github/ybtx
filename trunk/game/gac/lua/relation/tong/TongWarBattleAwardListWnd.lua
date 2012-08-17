CTongWarBattleAwardListWnd = class(SQRDialog)
------¹úÕ½½±ÀøÃæ°å------

function CreateTongWarBattleAwardListWnd(Parent)
	local Wnd = CTongWarBattleAwardListWnd:new()
	Wnd:CreateFromRes("TongWarBattleAwardListWnd", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

function CTongWarBattleAwardListWnd:OnChildCreated()
	self.m_TongMemName = self:GetDlgChild("TongMemName")
	self.m_Level = self:GetDlgChild("Level")
	self.m_GetExploit = self:GetDlgChild("GetExploit")
	self.m_GetAward = self:GetDlgChild("GetAward")
	self.m_ExtendAward = self:GetDlgChild("ExtendAward")
end

function CTongWarBattleAwardListWnd:SetInfo(ListInfo)
	self.m_TongMemName:SetWndText(ListInfo.sPlayerName)
	self.m_Level:SetWndText(ListInfo.nLevel)
	self.m_GetExploit:SetWndText(ListInfo.nGetExploit)
	self.m_GetAward:SetWndText(ListInfo.nAddProffer)
	self.m_ExtendAward:SetWndText(ListInfo.nCampAward)
end

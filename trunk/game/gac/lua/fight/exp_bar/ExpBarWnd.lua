gac_require "fight/exp_bar/ExpBarWndInc"

function CreateExpBar(Parent)
	local wnd = CExpBar:new()
	wnd:Init( Parent )
	wnd:SetIsBottom(true)
	return wnd
end

function CExpBar:Init(Parent)
	self:CreateFromRes("ExpWnd", Parent)
	self:ShowWnd( true )
	self.m_ExpLong	= self:GetDlgChild("Exp_Pro")
	self.m_ExpValue	= self:GetDlgChild("ExpValue")
	self.m_ExpValue:InserBefore(self.m_ExpLong)
	self.m_ExpLong:SetProgressMode(0)
end

--设置经验条的长度
function CExpBar:SetExpLong(nowExp, maxExp)
	self.m_ExpLong:SetRange(maxExp)
	self.m_ExpLong:SetPos(nowExp)
	self.m_ExpValue:SetWndText(nowExp .. "/" .. maxExp)
end

function CExpBar:OnMouseMove(nFlags, posx, posy)
	
end
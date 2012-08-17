CSmallMapBG = class( SQRDialog )

------用于绘制地图窗口------
function CSmallMapBG:Ctor(parentWnd)
	self:CreateFromRes("SmallMapBG", parentWnd)
	self.m_Static = self:GetDlgChild("Static")
	self:ShowWnd(true)
end

function CSmallMapBG:OnChildCreated()
	self.m_Static = self:GetDlgChild("Static")
	self.m_Static:SetSmallMapWnd(self)
	self.m_WndPlayer = self:GetDlgChild("WndPlayer")
	self.m_WndPlayer:ShowWnd(false)
end
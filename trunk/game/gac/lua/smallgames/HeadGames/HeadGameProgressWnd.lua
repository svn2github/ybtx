gac_require "smallgames/HeadGames/HeadGameProgressWndInc"

function CreateHeadProgressWnd(parent)
	local Wnd = CHeadProgressWnd:new()
	Wnd:CreateFromRes("HeadGameProgressWnd",parent)
	Wnd.m_ProgressPer = Wnd:GetDlgChild( "UpProgress" )
	Wnd.m_SignalProgressPer = Wnd:GetDlgChild( "SignalProgress" )
	Wnd.m_GoodSignal = Wnd:GetDlgChild( "GoodSignal" )
	Wnd.m_BadSignal = Wnd:GetDlgChild( "BadSignal" )
	
	Wnd.m_ProgressPer:SetProgressMode(3)
	Wnd.m_ProgressPer:SetRange(100)
	Wnd.m_SignalProgressPer:SetProgressMode(3)
	Wnd.m_SignalProgressPer:SetRange(100)
	return Wnd
end

function CHeadProgressWnd:BeginHeadProgress()
	self.m_ProgressPer:SetPos(0)
	self.m_SignalProgressPer:SetPos(0)
	self:ShowWnd(true)
end

function CHeadProgressWnd:SetPerPos(Pos, SignalPos)
	self.m_ProgressPer:SetPos(Pos)
	self.m_SignalProgressPer:SetPos(SignalPos)
end

function CHeadProgressWnd:StopHeadProgress()
	self:ShowWnd(false)
end
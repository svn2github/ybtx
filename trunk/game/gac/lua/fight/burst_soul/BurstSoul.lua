gac_require "fight/burst_soul/BurstSoulInc"

function CreatBurstSoulWnd(parent)
	local wnd = CBurstSoul:new()
	wnd:Init( parent )
	return wnd
end

function CBurstSoul:Init(parent)
	self.m_tblUpperLimit = {[0] = 0,
	CBurstSoulCfgClient_GetNeedBurstSoulTimesByLevel(1),
	CBurstSoulCfgClient_GetNeedBurstSoulTimesByLevel(2),
	CBurstSoulCfgClient_GetNeedBurstSoulTimesByLevel(3)}
	self:CreateFromRes( "BurstSoul", parent )
end

function CBurstSoul:OnChildCreated()
	self.m_BurstSoulBar = self:GetDlgChild("BurstSoulBar")
	self.m_tblLight = {}
	for i = 1, 3 do
		self.m_tblLight[i] = self:GetDlgChild("Light" .. i)
		self.m_tblLight[i]:EnableWnd(false)
	end
	self:SetBarInfo(0, self.m_tblUpperLimit[0], self.m_tblUpperLimit[1])
end

function CBurstSoul:UpdateInfo(nowValue)
	local maxLevel	= #(self.m_tblUpperLimit)
	local maxVelue	= self.m_tblUpperLimit[maxLevel]
	local nowValue	= math.min(nowValue, maxVelue)
	local nLevel	= self:GetLevelByValue(nowValue)
	
	for i = 1, maxLevel do
		local bFlag = nLevel > i
		self.m_tblLight[i]:EnableWnd(bFlag)
	end
	nLevel = math.min(nLevel, maxLevel)
	
	local upperValue	= self.m_tblUpperLimit[nLevel]
	local lowerValue	= self.m_tblUpperLimit[nLevel - 1]
	self:SetBarInfo(nowValue, lowerValue, upperValue)
end

function CBurstSoul:SetBarInfo(nowValue, minValue, maxValue)
	local barMaxValue = maxValue - minValue
	local barNowValue = nowValue - minValue
	self.m_BurstSoulBar:SetRange( barMaxValue )
	self.m_BurstSoulBar:SetPos( barNowValue )
	self.m_BurstSoulBar:SetWndText( nowValue .. "/" .. maxValue )
end

function CBurstSoul:GetLevelByValue(nowValue)
	local maxLevel = #(self.m_tblUpperLimit)
	for i = 1, maxLevel do
		if (nowValue >= self.m_tblUpperLimit[i-1] and nowValue < self.m_tblUpperLimit[i]) then
			return i
		end
	end
	return maxLevel + 1
end

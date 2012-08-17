gac_require "information/tooltips/ToolTipsInc"

function CTooltips:GetSmashTips(sType, uAttackType, uSmashValue, fSmashRate)

	local tblTemp1 = {	[EAttackType.eATT_Puncture]		= ToolTipsText(102),
						[EAttackType.eATT_Chop]			= ToolTipsText(102),
						[EAttackType.eATT_BluntTrauma]	= ToolTipsText(102),
						[EAttackType.eATT_Nature]		= ToolTipsText(103),
						[EAttackType.eATT_Destroy]		= ToolTipsText(104),
						[EAttackType.eATT_Evil]			= ToolTipsText(105)	}
						
	local tblTemp2 = {	["非常安全"]	= ToolTipsText(106),
						["安全"]		= ToolTipsText(107),
						["危险"]		= ToolTipsText(108)	}

	local sAttackType = tblTemp1[uAttackType]
	local strParameter, Color, Tips = "", "", {}
	strParameter = ToolTipsText(101, tblTemp2[sType], sAttackType, uSmashValue, fSmashRate)
	
	self:SetFirstTipsInfo( Tips, Color, strParameter, 0 )
	return self:Completed( Tips )
end
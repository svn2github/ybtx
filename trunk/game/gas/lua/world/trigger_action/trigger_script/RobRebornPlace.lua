local function Script(Arg, Trigger, Player)	
	local timeLength = 60 * 1000
	
	local function ProgressSucceed()
		Trigger.m_Occupyer = Player.m_Properties:GetTongID()
	end
	
	local function ProgressFail()
		
	end
	TongLoadProgress(Player, timeLength, ProgressSucceed, ProgressFail,{})
end

return Script 
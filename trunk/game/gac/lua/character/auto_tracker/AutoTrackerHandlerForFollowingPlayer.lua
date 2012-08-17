AutoTrackerHandlerForFollowingPlayer = class(IAutoTrackerHandler)

function AutoTrackerHandlerForFollowingPlayer:OnReached(Attacker, Target)
end

function AutoTrackerHandlerForFollowingPlayer:OnCancel(Attacker, Target)
	if Target then
		MsgClient(3054,Target.m_Properties:GetCharName())
	else
		MsgClient(3054,"")
	end
end

function AutoTrackerHandlerForFollowingPlayer:OnCheckStopCond(Attacker, Target)
	return true
end

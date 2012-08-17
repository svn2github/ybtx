AutoTrackerHandlerForDeadNpc = class(IAutoTrackerHandler)

function AutoTrackerHandlerForDeadNpc:OnReached(Attacker, Target)
	--print("AutoTrackerHandlerForDeadNpc:OnReached")
	Attacker:StopMoving()
	OnRClickDeadNpc(Target)
	Attacker:CancelAutoTracker(false)
end

function AutoTrackerHandlerForDeadNpc:OnCheckStopCond(Attacker, Target)
	--print("AutoTrackerHandlerForDeadNpc:OnCheckStopCond")
	return true
--	if g_GetDistance(Target, Attacker) <=3 then
--		return true
--	end
end

AutoTrackerHandlerForIntObj = class(IAutoTrackerHandler)

function AutoTrackerHandlerForIntObj:OnReached(Attacker, Target)
--	print("AutoTrackerHandlerFroIntObj:OnReached")
	Attacker:StopMoving()
	OnLClickIntObj(Target)
	Attacker:CancelAutoTracker(false)
end

function AutoTrackerHandlerForIntObj:OnCheckStopCond(Attacker, Target)
--	print("AutoTrackerHandlerFroIntObj:OnCheckStopCond")
	return true
--	if g_GetDistance(Attacker,Target)>5 then
--		return false
--	end
end

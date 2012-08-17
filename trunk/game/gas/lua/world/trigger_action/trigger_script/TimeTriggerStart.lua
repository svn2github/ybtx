local function Script(Arg, Trigger, Player)

	if Trigger.m_TimeTrigger then
		return
	end
	
	Trigger.m_TimeTrigger = CTimeTrigger:new(Arg[1], Trigger)
end

return Script

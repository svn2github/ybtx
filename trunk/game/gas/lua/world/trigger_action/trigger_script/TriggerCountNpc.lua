local function Script(Arg, Trigger, Player)

	if Trigger.m_Scene.m_CountTrigger then
		return
	end
	
	Trigger.m_Scene.m_CountTrigger = CCountTrigger:new(Arg[1])
end

return Script

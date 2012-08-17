local function Script(Arg, Trigger, Player)

	local Target = nil
	local MsgID = Arg[1]
	
	if Arg[2] then
		Target = g_TriggerMgr:GetTarget(Arg[2], Trigger, Player, Arg[3])
	end
	if not Target then
		return
	end
	local strDisplay = GetStaticTextServer(Arg[1])
	Gas2Gac:PlayHeadEffect(Target:GetIS(0), "green_2", strDisplay, Target:GetEntityID())
end

return Script
local function Script(Arg, Trigger, Player)

	local Npc = nil
	if IsNpc(Trigger) then
		Npc = Trigger
	end
	if IsNpc(Player) then
		Npc = Player
	end
	if not Npc then
		return
	end
	
	Npc:SetNpcSleepState(false)
	local Num = #Arg
	
	local Result = nil
	local MixDis = nil
	for i=2, Num do
		local FirstPos = g_DynamicCreateMgr:GetPathFirstPos(Npc, Arg[1], Arg[i])
		local Dis = g_GetDistanceByPos(Npc, FirstPos)
		if not MixDis then
			MixDis = Dis
			Result = {Arg[1], Arg[i]}
		end
		if MixDis > Dis then
			MixDis = Dis
			Result = {Arg[1], Arg[i]}
		end
	end
	
	g_DynamicCreateMgr:CreateMercenaryPath(Npc, Result[1], Result[2])

end 

return Script
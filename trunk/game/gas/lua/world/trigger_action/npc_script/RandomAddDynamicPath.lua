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
	
	local RandomNum  = math.random(3, Num)
	--print("Ñ¡ÔñÂ·¾¶"..RandomNum)
	if not IsTable(Arg[2]) then
		Arg[2] = nil
	end
	g_DynamicCreateMgr:CreateMercenaryPath(Npc, Arg[1], Arg[RandomNum], Arg[2])

end

return Script

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
	
	local RandomNum  = math.random(2, Num)
	--print("Ñ¡ÔñÂ·¾¶"..RandomNum)
	
	g_DynamicCreateMgr:CreateMercenaryPath(Npc, Arg[1], Arg[RandomNum])

end

return Script
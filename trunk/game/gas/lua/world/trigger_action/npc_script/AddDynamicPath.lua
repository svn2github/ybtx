local function Script(Arg, Trigger, Player)
	local scene = nil
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
	local pos = nil
	if Arg[3] == "相对坐标" then --相对坐标
		pos = GetCreatePos(Npc)
		pos.x = pos.x + Arg[4]
		pos.y = pos.y + Arg[5]
	elseif Arg[3] == "绝对坐标" then
		local ID = Arg[4]
		pos.x, pos.y, scene = GetScenePosition(ID)
	elseif Arg[3] == "随机" then
		pos = GetCreatePos(Npc)
		pos = RandomCreatPos(pos,Arg[4])
	end
	g_DynamicCreateMgr:CreatePath(Npc, Arg[1], Arg[2], pos)
end

return Script
local function NpcDoSkill(Arg, Trigger, Player)
	
	local Target = Player
	local SkillName = Arg[1]
	
	if Arg[2] then
		Target = g_TriggerMgr:GetTarget(Arg[2], Trigger, Player, Arg[3])
		if Arg[2] == "OnlyNpc" then
			Trigger = Target
		end
	end
	
	if not Target then
		return
	end
	
	Trigger:ServerDoNoRuleSkill(SkillName, Target)
end

local function TrapDoSkill(Arg, Trigger, Player)
	
	local Target = Player
	local SkillName = Arg[1]
	
	if Arg[2] then
		Target = g_TriggerMgr:GetTarget(Arg[2], Trigger, Player, Arg[3])
		if Arg[2] == "OnlyNpc" then
			Trigger = Target
		end
	end 
	
	if not IsCppBound(Trigger) then
		return
	end
	if not IsCppBound(Target) then
		return
	end
	
	Trigger:DoIntObjSkill(SkillName,1,Target)
end


local funTbl = {
	[ECharacterType.Npc] = NpcDoSkill,
	[ECharacterType.Trap] = TrapDoSkill,
}

local function Script(Arg, Trigger, Player)
	local type = Trigger.m_Properties:GetType()
	if IsFunction (funTbl[type]) then
		funTbl[type](Arg, Trigger, Player)
	end
end

return Script
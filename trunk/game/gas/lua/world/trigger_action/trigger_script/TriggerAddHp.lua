--º”…˙√¸÷µ
local function Script(Arg, Trigger, Player)
	if Player.m_Properties:GetCharName() == Arg[2] then
		local FireNpc = Trigger.m_FireNpc
		if not FireNpc then
			return
		end
		local fullHp = FireNpc:CppGetMaxHP()
		local curHp = FireNpc:CppGetHP()
		FireNpc:CppSetHP(fullHp * 0.05 + curHp)
	end
end

return Script
	
	
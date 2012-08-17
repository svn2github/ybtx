local function DeleteObject(Arg, Trigger, Player)
	if Player.m_Properties:GetType() == ECharacterType.IntObj then   --Npc进入Trap删除Npc，Npc离开Trap自删除
		return 
	end
	Player:SetOnDisappear(true) 
end


local function DeleteNpcIn(Arg, Trigger, Player)   --Npc进入Trap自删除
	if Player.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end
	if Player.m_TgtSentryIndex then
		if Trigger.m_SentryIndex == Player.m_TgtSentryIndex and
			Trigger.m_InBattleNum == 0 then
			Player:SetOnDisappear(false)
		end
	end
end

local funcTbl = {
	["Npc进入Trap自删除"] = DeleteNpcIn,
	["对象自删除"] = DeleteObject,
}	

local function Script(Arg, Trigger, Player)
	local type = Arg[1]
	if IsFunction (funcTbl[type]) then
		funcTbl[type](Arg, Trigger, Player)
	end
end

return Script
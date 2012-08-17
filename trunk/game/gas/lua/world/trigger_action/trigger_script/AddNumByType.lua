local function AddSameNpcNum(Arg, Trigger, Player)  --相同Npc加计数

	if not IsCppBound(Player) then
		return
	end
	-- 计数
	if not IsCppBound(Trigger.FirstNpc) then
		if Player.m_CreateAnotherPigTick then
			UnRegisterTick(Player.m_CreateAnotherPigTick)
			Player.m_CreateAnotherPigTick = nil
		end
		Trigger.NpcCount = 1
		Trigger.FirstNpc	= Player
		Trigger.ExistNpcName = Player.m_Name -- 记下NPC名字
	else
		-- 放正确 不重生
		if Player.m_Name == Trigger.ExistNpcName
				and Trigger.NpcCount == 1 then				-- 成功后Trigger.NpcCount设为负数
					
			Trigger.NpcCount = Trigger.NpcCount + 1
				-- 取消Tick不在原处创建NPC
			if Player.m_CreateAnotherPigTick then
				UnRegisterTick(Player.m_CreateAnotherPigTick)
				Player.m_CreateAnotherPigTick = nil
			end
			
			if Trigger.FirstNpc.m_CreateAnotherPigTick then
				UnRegisterTick(Trigger.FirstNpc.m_CreateAnotherPigTick)
				Trigger.FirstNpc.m_CreateAnotherPigTick = nil
			end	
			
			Player.m_IsNotEmbrace = true
			Trigger.FirstNpc.m_IsNotEmbrace = true
			
			-- 特效
			local ModelString = "fx/setting/other/other/fuwen_green/explode.efx"
			local StateString = "fuwen_green/explode"
			local PlayerID = Player.m_OwnerId
			local NewPlayer = g_GetPlayerInfo(PlayerID)
			local NpcID = Player:GetEntityID()
			local FirstNpcID = Trigger.FirstNpc:GetEntityID()
			if IsCppBound(NewPlayer) then
				Gas2Gac:UseItemTargetEffect(NewPlayer.m_Conn, ModelString,StateString,NpcID)
				Gas2Gac:UseItemTargetEffect(NewPlayer.m_Conn, ModelString,StateString,FirstNpcID)
			end
			--Gas2Gac:UseItemTargetEffect(Player:GetIS(0), ModelString, StateString, Player:GetEntityID())
			
			local function DeleteTwoNpc()
				-- 抱进来的猪消失
				Player:SetNoDissolve()
				g_NpcServerMgr:DestroyServerNpcNow(Player, false)
				-- 第一个猪消失
				if IsCppBound(Trigger.FirstNpc) then
					Trigger.FirstNpc:SetNoDissolve()
					g_NpcServerMgr:DestroyServerNpcNow(Trigger.FirstNpc, false)
				end		
			end
			
			RegisterOnceTick(Player, "DeleteTwoNpcTick", DeleteTwoNpc, 1000)
		end
	end
	
	-- 如果够了2个
	if Trigger.NpcCount == 2 then
		-- 加分
		local TargetName = Arg
		if not TargetName or TargetName == "" then
			return
		end
		local Player = g_GetPlayerInfo(Player.m_OwnerId)
		if not IsCppBound(Player) then
			return		
		end
		CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, TargetName)	
	end
end

local function AddNumByBuff(Arg, Trigger, Player) --带buff进Trap加活动计数
	if not IsCppBound(Player) then
		return
	end
	
	local stateCascade = Player:GetStateCascade(Arg)
	if stateCascade > 0 then
		g_MatchGameMgr:AddMatchGameCount(Player, 11, Arg, stateCascade)
	end
end

local function AddMercActionNum(Arg, Trigger, Player)  --佣兵训练活动加计数,佣兵训练活动Npc加计数
	if Player.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(Player.m_OwnerId)
	end
	
	if not IsCppBound(Player) then
		return
	end
	local TargetName = Arg
	if not TargetName or TargetName == "" then
		return
	end
	CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, TargetName)
end

local function NpcAddQuestNum(Arg, Trigger, Player)
	local varname = Arg[2]
	local questname = Arg[1]
	if not IsCppBound(Player) then
		Player = g_GetPlayerInfo(Trigger.m_OwnerId)
		if Trigger.m_IsTestDeadOnTimeOver then
			local str = "Npc OwnerID:"..Trigger.m_OwnerId.."玩家ID:"..Player.m_uID
			LogErr("添加任务计数",str)
		end
	else
		if Trigger.m_IsTestDeadOnTimeOver then
			local str = "玩家ID:"..Player.m_uID
			LogErr("添加任务计数",str)
		end
	end
	
	if not Player then
		return
	end
	
	if Trigger.m_IsTestDeadOnTimeOver then
		Trigger.m_IsTestDeadOnTimeOver = nil
	end
	
	g_AddQuestVarNumForTeam(Player, questname, varname, 1)
end

local function TrapAddQuestNum(Arg, Trigger, Player)
	if Player.m_Properties:GetType() == ECharacterType.Npc then
		local PlayerId = Player.m_OwnerId 
		if PlayerId == nil then
			PlayerId = Trigger.m_OwnerId
			if PlayerId == nil then
				return 
			end
		end
		Player = g_GetPlayerInfo(Player.m_OwnerId)
		if not Player then
			return
		end
	else
		if Player.m_Properties:GetType() ~= ECharacterType.Player then
			return 
		end 
	end
	if Player.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	local QuestName = Arg[1]
	local VarName = Arg[2]
	local AddNum = Arg[3]
	g_AddQuestVarNumForTeam(Player, QuestName,VarName,AddNum)
end

local funTbl = {
	[ECharacterType.Npc]		= NpcAddQuestNum,
	[ECharacterType.Trap]		= TrapAddQuestNum,
}

local function AddQuestNum(Arg, Trigger, Player) --Npc踩天然Trap加任务计数,玩家踩Trap加任务计数
	local type = Trigger.m_Properties:GetType()
	if IsFunction (funTbl[type]) then
		funTbl[type](Arg, Trigger, Player)
	end
end

local function AddSmallFbNum(Arg, Trigger, Player)  --小副本,matchgame,虫子从虫洞跑出
	if Player.m_Properties:GetType() == ECharacterType.Player then
		if Player.m_OwnerId ~= nil then
			Player = g_GetPlayerInfo(Player.m_OwnerId)
		else
			Player = Player
		end
		if not Player or not IsCppBound(Player) then
			return
		end
		
		if not Player.m_MatchGameCount then
			Player.m_MatchGameCount = 0
		end
		
		g_MatchGameMgr:AddMatchGameCount(Player, 2, Trigger.m_Properties:GetCharName())
		return
	end
	
	if Player.m_Properties:GetType() == ECharacterType.Npc then
		if not IsCppBound(Player) then
			return
		end
		local scene = Trigger.m_Scene
		g_MatchGameMgr:AllTeamAddCount(scene, 2, Trigger.m_Properties:GetCharName())
		return
	end
end


local AddNumFunTbl = {
	["相同Npc加计数"] = AddSameNpcNum,
	["带buff进Trap加活动计数"] = AddNumByBuff,
	["佣兵训练活动加计数"] = AddMercActionNum,
	["小副本加计数"] = AddSmallFbNum,
	["任务加计数"] = AddQuestNum,
}	

local function Script(Arg, Trigger, Player)
	local type = Arg[1]
	if IsFunction (AddNumFunTbl[type]) then
		AddNumFunTbl[type](Arg[2], Trigger, Player)
	end
end

return Script
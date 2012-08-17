CClaspBehavior = class()

local function DestroyPig(data)
	local TargetType,EntityID = data[1],data[2]
	local Target = CEntityServerManager_GetEntityByID(EntityID)
	if TargetType == ECharacterType.Npc then
		if IsServerObjValid(Target) then
			g_NpcServerMgr:DestroyServerNpcNow(Target, false)
		end
	end
end

--抱和放
function CClaspBehavior.ClaspBehaviorActualize(player, TargetEntityID)
	local Target = CEntityServerManager_GetEntityByID(TargetEntityID)

	if IsServerObjValid(Target) then
		if Target.m_IsNotEmbrace then
			--print("isn\'t embrace !!!! ...... ")
			return false
		end
		
		-- 抱起来后 清除Tick
		if Target.m_CreateAnotherPigTick then
			UnRegisterTick(Target.m_CreateAnotherPigTick)
			Target.m_CreateAnotherPigTick = nil
		end
	
		player.m_EmbraceNpc = Target.m_Properties:GetCharName()
		player.m_EmbraceType = Target.m_Properties:GetType()
		
		if player.m_EmbraceType == ECharacterType.Npc then
			local RebornType = nil
			local SkillName
			local ReSkillName
			local NpcName = player.m_EmbraceNpc
			local ArgInfo = GetNpcTriggerArg(NpcName, "点击", "抱")
			if ArgInfo then
				RebornType = ArgInfo.Arg[1][2]
				SkillName = ArgInfo.Arg[1][3]
				ReSkillName = ArgInfo.Arg[1][4]
			end
			
			if Target.m_IsDownTarget then
				Target:SetNoDissolve()
				g_NpcServerMgr:DestroyServerNpcNow(Target, false)--不重生
				player.m_TargetRebornPos = Target.m_RebornPos
			else
				Target:SetNoDissolve()
				
				-- 判断参数，抱起来后是否重生
				
				if RebornType == "不复活不消失" or RebornType == "不复活消失" then
					g_NpcServerMgr:DestroyServerNpcNow(Target, false)--不重生
					player.m_TargetRebornPos = Target.m_RebornPos
				else
					g_NpcServerMgr:DestroyServerNpcNow(Target, true)--重生	
				end
			end
			
			if SkillName then
--				print(SkillName)
				player:PlayerDoItemSkill(SkillName,1)
			end
			if ReSkillName then
				player.m_EmbraceSkillName = ReSkillName
			end
		elseif player.m_EmbraceType == ECharacterType.IntObj then
			if not Target.m_IsDownTarget then
				g_IntObjServerMgr:Destroy(Target,true)
			else
				g_IntObjServerMgr:Destroy(Target,false)
			end
		end
		
		Target.m_IsDownTarget = nil
		return true,player.m_EmbraceType
	end
	return false
end

local function GetTargetPos(PlayerPos,PlayerDir)
	local iAngle = PlayerDir * (3.1415926*2/256)
	local x = math.sin(iAngle)
	local y = math.cos(iAngle)
	PlayerPos.x = PlayerPos.x + x * EUnits.eGridSpanForObj
	PlayerPos.y = PlayerPos.y + y * EUnits.eGridSpanForObj
	return PlayerPos
end

function CClaspBehavior.DroptBehaviorActualize(Player)
	local SelfEntityID = Player:GetEntityID()
	local Pos = CFPos:new()
	Player:GetPixelPos(Pos)
	local Dir = Player:GetActionDir()
	local TargetPos = CFPos:new()
	TargetPos = GetTargetPos(Pos,Dir)
	
	Pos.x = TargetPos.x / EUnits.eGridSpanForObj
	Pos.y = TargetPos.y / EUnits.eGridSpanForObj

	local Scene = Player.m_Scene
	
	--加一个猪的模型
	local PigNpc = nil
	local NpcName = Player.m_EmbraceNpc
	Player.m_EmbraceNpc = nil

	if not NpcName then
		return false
	end
	
	if Player.m_EmbraceType == ECharacterType.Npc then
		local fPos = CFPos:new( Pos.x * EUnits.eGridSpan, Pos.y * EUnits.eGridSpan )	
		local otherData = {["m_CreatorEntityID"]=SelfEntityID}
		PigNpc = g_NpcServerMgr:CreateServerNpc(NpcName,1,Scene,fPos,otherData)
		if not IsServerObjValid(PigNpc) then
			return false
		end
		PigNpc.m_IsDownTarget = true
		PigNpc.m_RebornPos = Player.m_TargetRebornPos
		if Player.m_EmbraceSkillName then
--			print("1",Player.m_EmbraceSkillName)
			Player:PlayerDoItemSkill(Player.m_EmbraceSkillName,1)
			Player.m_EmbraceSkillName = nil
		end
	elseif Player.m_EmbraceType == ECharacterType.IntObj then
		local Obj = CreateIntObj( Scene, Pos, NpcName, true, nil, nil)
		if not Obj then
			return false
		end
		Obj.m_IsDownTarget = true
		return true
	end

	PigNpc.m_CreatorEntityID = SelfEntityID
	PigNpc.m_OwnerId = Player.m_uID
	
	local data = {Player.m_EmbraceType,PigNpc:GetEntityID()}

	local RebornType = nil
	local ArgInfo = GetNpcTriggerArg(NpcName, "点击", "抱")
	if ArgInfo then
		RebornType = ArgInfo.Arg[1][2]
	end
	-- 放下是否消失
	if RebornType == "不复活不消失" then
		return true
	elseif RebornType == "不复活消失" then
		local Target = CEntityServerManager_GetEntityByID(PigNpc:GetEntityID())
		if Player.m_EmbraceType == ECharacterType.Npc then
			if IsServerObjValid(Target) then

				local function CreateAnotherPig()
					--print("CreateAnotherPig")
					if Target.m_CreateAnotherPigTick then
						UnRegisterTick(Target.m_CreateAnotherPigTick)
						Target.m_CreateAnotherPigTick = nil
					end
					
					-- 原来的猪消失
					if IsCppBound(Target) then
						Target:SetNoDissolve()
						g_NpcServerMgr:DestroyServerNpcNow(Target, false)
					end
					-- 在原处创建NPC
					local Pos = CFPos:new()
					Pos.x = PigNpc.m_RebornPos.x
					Pos.y = PigNpc.m_RebornPos.y
					local fPos = CFPos:new( Pos.x * EUnits.eGridSpan, Pos.y * EUnits.eGridSpan )
					local otherData = {["m_CreatorEntityID"]=SelfEntityID}
					local OhterPigNpc = g_NpcServerMgr:CreateServerNpc(NpcName,1,Scene,fPos,otherData)
					if not IsServerObjValid(OhterPigNpc) then
						return
					end
					OhterPigNpc.m_RebornPos = CFPos:new()
					OhterPigNpc.m_RebornPos.x = PigNpc.m_RebornPos.x
					OhterPigNpc.m_RebornPos.y = PigNpc.m_RebornPos.y
				end
				
				if Target.m_CreateAnotherPigTick then
					UnRegisterTick(Target.m_CreateAnotherPigTick)
				end
				Target.m_CreateAnotherPigTick = RegisterTick ("CreateAnotherPigTick", CreateAnotherPig, 1000)

			end
		end	
	else
		-- 消失
		RegisterOnceTick(Scene,"EmbraceAndDroptTick",DestroyPig,6000,data)
	end
	
	return true
end

function CClaspBehavior.DorpBeforClear(Player)
	local NpcName = Player.m_EmbraceNpc
	if not NpcName then
		return
	end
	local RebornType = nil
	local ArgInfo = GetNpcTriggerArg(NpcName, "点击", "抱")
	if ArgInfo then
		RebornType = ArgInfo.Arg[1][2]
	end
	-- 放下是否消失
	if RebornType == "不复活不消失" then
		if Player.m_EmbraceNpc then
			local Pos = CFPos:new()
			Player:GetPixelPos(Pos)
			local Scene = Player.m_Scene
			Pos.x = Pos.x / EUnits.eGridSpanForObj
			Pos.y = Pos.y / EUnits.eGridSpanForObj
			
			if Player.m_EmbraceType == ECharacterType.Npc then
				local fPos = CFPos:new(Pos.x * EUnits.eGridSpan, Pos.y * EUnits.eGridSpan)	
				local otherData = {["m_CreatorEntityID"]=Player:GetEntityID()}
				local PigNpc = g_NpcServerMgr:CreateServerNpc(NpcName, 1, Scene, fPos, otherData)
				if PigNpc then
					PigNpc.m_IsDownTarget = true
					PigNpc.m_RebornPos = Player.m_TargetRebornPos
				end
			elseif Player.m_EmbraceType == ECharacterType.IntObj then
				local Obj = CreateIntObj( Scene, Pos, NpcName, true, nil, nil)
				if Obj then
					Obj.m_IsDownTarget = true
				end
			end	
		end
	end
end

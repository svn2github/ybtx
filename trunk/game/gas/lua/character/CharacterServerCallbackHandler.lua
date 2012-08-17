CCharacterServerCallbackHandler = class()

function CCharacterServerCallbackHandler:OnCreateServant(uObjID,PetName, Level, PosX, PosY, bNormalAI)
	--print( "CCharacterServerCallbackHandler:OnCreateServant",PosX, PosY)
	local pCharacter = CCharacterDictator_GetCharacterByID(uObjID)
	if not IsServerObjValid(pCharacter) then
		return
	end
	local ServantRealName = ""
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		ServantRealName = pCharacter:GetPetName(PetName)
	end
	local Pos = CFPos:new( PosX, PosY )
	local otherData = {["m_CreatorEntityID"]=uObjID}
	local Pet = g_NpcServerMgr:CreateServerNpc(PetName, Level,pCharacter.m_Scene, Pos, otherData, uObjID,  nil, nil, ServantRealName)
	if IsServerObjValid(Pet) then
		g_NpcServerMgr:SkillCreateNpcEnd(Pet)
	end
end

function CCharacterServerCallbackHandler:OnCreateServantWithTarget(uObjID, PetName, Level, uTaregtID,PosX, PosY, bNormalAI)
	--print( "CCharacterServerCallbackHandler:OnCreateServant",PosX, PosY)
	local pCharacter = CCharacterDictator_GetCharacterByID(uObjID)
	if not IsServerObjValid(pCharacter) then
		return
	end
	local ServantRealName = ""
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		ServantRealName = pCharacter:GetPetName(PetName)
	end
	local Pos = CFPos:new( PosX, PosY )
	local otherData = {["m_CreatorEntityID"]=uObjID}
	local Pet = g_NpcServerMgr:CreateServerNpc(PetName, Level,pCharacter.m_Scene, Pos, otherData, uObjID,  nil, nil, ServantRealName)
	local Target = CCharacterDictator_GetCharacterByID(uTaregtID)
	if IsServerObjValid(Pet) and IsServerObjValid(Target) then
		Pet:ForceLock(Target, 10000)
		g_NpcServerMgr:SkillCreateNpcEnd(Pet)
	end
end

function CCharacterServerCallbackHandler:OnCreateBattleHorse( uObjID, PetName, PetType, Level, PosX, PosY)
	--print( "CCharacterServerCallbackHandler:OnCreateBattleHorse",PosX, PosY)
	local pCharacter = CCharacterDictator_GetCharacterByID(uObjID)
	if not IsServerObjValid(pCharacter) then
		return
	end
	local ServantRealName = ""
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		ServantRealName = pCharacter:GetPetName(PetName)
	end
	local Pos = CFPos:new( PosX, PosY )
	local otherData = {["m_CreatorEntityID"]=uObjID}
	local Pet = g_NpcServerMgr:CreateServerNpc( PetName, Level, pCharacter.m_Scene, Pos, otherData, uObjID, nil, nil, ServantRealName, true)
	if IsServerObjValid(Pet) and IsServerObjValid(pCharacter) then
		pCharacter:RemoteInitBattleHorse(Pet)
		g_NpcServerMgr:SkillCreateNpcEnd(Pet)
	end
end

function CCharacterServerCallbackHandler:OnCreateDeadNPC(uObjID, Level, PosX, PosY)
	--print( "CCharacterServerCallbackHandler:OnCreateDeadNPC",PosX, PosY)
	local pCharacter = CCharacterDictator_GetCharacterByID(uObjID)
	if not IsServerObjValid(pCharacter) then
		return
	end
	local Pos = CFPos:new( PosX, PosY )
	--应该实现一个创建尸体的函数
	local otherData = {["m_CreatorEntityID"]=uObjID}
	local NPC = g_NpcServerMgr:CreateServerNpc("马思阳测试用死尸", Level, pCharacter.m_Scene, Pos , otherData, 0)
	if not IsServerObjValid(NPC) then
		return
	end
	NPC:KillNpcBySpecialAction(pCharacter,nil)
	NPC:SetAndSyncActionState(EActionState.eAS_Dead)
	g_NpcServerMgr:SkillCreateNpcEnd(NPC)
end

function CCharacterServerCallbackHandler:OnCreateNPCWithTarget(uObjID,uTargetID, uMasterID, NPCName, Level, PosX, PosY)
	--print( "CCharacterServerCallbackHandler:OnCreateNPC",PosX, PosY)

	local pCharacter = CCharacterDictator_GetCharacterByID(uObjID)
	if not IsServerObjValid(pCharacter) then
		return
	end
	local Pos = CFPos:new( PosX, PosY )
	local otherData = {["m_CreatorEntityID"]=uObjID,["m_OwnerId"]=pCharacter.m_uID}
	local NPC = g_NpcServerMgr:CreateServerNpc(NPCName, Level, pCharacter.m_Scene, Pos ,otherData, uMasterID)
	if not IsServerObjValid(NPC) then
		return
	end
	
	local targetOfNPC = nil
	if uTargetID ~= 0 then
		
		 targetOfNPC = CCharacterDictator_GetCharacterByID(uTargetID)
	end
	
	--NPC:CppSetCamp(pCharacter:CppGetCamp())

	if IsServerObjValid(targetOfNPC) then
		NPC:ForceLock(targetOfNPC, 10000)
	end
	
	g_NpcServerMgr:SkillCreateNpcEnd(NPC)
end

function CCharacterServerCallbackHandler:OnCreateShadow(uObjID,PetName, Level, PosX, PosY, bNormalAI)
	--print( "CCharacterServerCallbackHandler:OnCreateShadow",PosX, PosY)
	local Player=CCharacterDictator_GetCharacterByID(uObjID)
	if not IsServerObjValid(Player) then
		return
	end
	if NpcInfoMgr_GetTypeByTypeName(Npc_Common(PetName,"Type")) ~= ENpcType.ENpcType_Shadow then
		LogErr("技能创建分身操作参数错误,NpcType必须是ENpcType_Shadow",PetName)
		return
	end
	local Pos = CFPos:new( PosX, PosY )
	local ServantRealName = Player.m_Properties:GetCharName()
	local otherData = {["m_CreatorEntityID"]=uObjID}
	local ShadowNpc = g_NpcServerMgr:CreateServerNpc(PetName, Level, Player.m_Scene, Pos, otherData, uObjID, nil, nil, ServantRealName)
	if IsServerObjValid(ShadowNpc) then
		g_NpcServerMgr:SkillCreateNpcEnd(ShadowNpc)
	end
	local MasterTarget = Player:GetTarget()
	if IsServerObjValid(MasterTarget) then
		ShadowNpc:ForceLock(MasterTarget, 10000)
	end
end

function CCharacterServerCallbackHandler:OnDestroyNpc(Npc)
--	print("CCharacterDictatorCallbackHandler:OnDestroyNpc", Npc:GetEntityID())
	g_NpcServerMgr:DestroyServerNpcNow(Npc, true)
end

function CCharacterServerCallbackHandler:PosCreateNpcWithMaster(uMasterID, sNpcName, fPosX, fPosY)
	local Master = CCharacterDictator_GetCharacterByID(uMasterID)
	if not IsServerObjValid(Master) then
		return
	end
	local otherData = 		{
								["m_CreatorEntityID"] = uMasterID
							}
	local Pos = CFPos:new( fPosX, fPosY)
	g_NpcServerMgr:CreateServerNpc(sNpcName, nil, Master.m_Scene, Pos, otherData, uMasterID)
end


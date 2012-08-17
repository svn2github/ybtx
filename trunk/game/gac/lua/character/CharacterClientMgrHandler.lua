gac_require "relation/team/TeamClientMgrHandler"

CEntityClientMgrHandler = class( IEntityClientManagerHandler )

function CEntityClientMgrHandler:OnGameEntityAdded( GameEntity )
	if GameEntity:GetGameEntityType() == EGameEntityType.eGET_CharacterDirector  or GameEntity:GetGameEntityType() == EGameEntityType.eGET_CharacterFollower then
		if GameEntity:GetType() == EClientObjectType.eCOT_Director then
			GameEntity.m_CallbackHandler = CCharacterDirectorCallbackHandler:new()
			TeamHandler = TeamClientMgrHandler:new()
			CTeamClientManager_SetHandler(TeamHandler)
		else
			GameEntity.m_CallbackHandler = CCharacterFollowerCallbackHandler:new()
		end
	elseif GameEntity:GetGameEntityType() == EGameEntityType.eGET_IntObject then
		GameEntity.m_CallbackHandler = CIntObjCallbackHandler:new()
	end
	GameEntity:SetCallbackHandler( GameEntity.m_CallbackHandler )
end
CCharacterInViewMgr = class()

--当Character进入玩家视野时调用该函数
function CCharacterInViewMgr:CharacterInView(Character)
	self:OnSetTeamAgileValueBeCare(Character, true)
end

--当Character离开玩家视野时调用该函数
function CCharacterInViewMgr:CharacterOutView(Character)
	self:OnSetTeamAgileValueBeCare(Character, false)
end

function CCharacterInViewMgr:OnSetTeamAgileValueBeCare(Character, bFlag)

	if(not Character) then return end
	if(Character.m_Properties:GetType() ~= ECharacterType.Player) then return end
	
	local target = g_MainPlayer.m_LockCenter.m_LockingIntObj
	if(target) then
		if(Character == target) then return end
			
		local targetType = target.m_Properties:GetType()
		if(targetType == ECharacterType.Npc or targetType == ECharacterType.Player) then
			local targetOfTarget = target.m_LockCenter.m_LockingIntObj
			if(targetOfTarget) then
				if(Character == targetOfTarget) then return end
			end
		end
	end
	
	local entityId = Character:GetEntityID()
	local tbl = g_GameMain.m_TeamBase.m_tblTeamMemberInfo
	for i = 1, #tbl do
		if(entityId == tbl[i][4]) then
			Character:SetAgileValueBeCare(bFlag)
			break
		end
	end
end

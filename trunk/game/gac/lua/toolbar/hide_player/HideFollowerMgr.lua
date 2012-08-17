
CHideFollowerMgr = class()
--------------   隐藏视野内玩家功能   --------------------

function CHideFollowerMgr:HidePlayer(pCharacter, bHide)
	if pCharacter == g_MainPlayer then
		return
	end
	if not pCharacter or pCharacter.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	if g_MainPlayer:IsEnemy(pCharacter) and bHide then
		return
	end
	
	if bHide and pCharacter:GetHorseRenderObj() then
		--pCharacter:GetRenderObject():SetAlphaValue(0)
		pCharacter:GetHorseRenderObj():SetAlphaValue(0)
		pCharacter:GetHorseRenderObj():Update()
--		g_GameMain.m_CharacterInSyncMgr:SetHeadInfoTransparent(pCharacter, 1)
		if(pCharacter.m_HeadInfoWnd) then
			pCharacter.m_HeadInfoWnd:SetTransparent(1)
		end
	elseif pCharacter:GetHorseRenderObj() then
		local Type = pCharacter.m_Properties:GetType()
		if Type == ECharacterType.Player then
			local bStealth = pCharacter.m_Properties:GetStealth()
			pCharacter:SetStealthState(bStealth)
--			g_GameMain.m_CharacterInSyncMgr:SetHeadInfoTransparent(pCharacter, bStealth and 0 or 1)
			if(pCharacter.m_HeadInfoWnd) then
				pCharacter.m_HeadInfoWnd:SetTransparent(bStealth and 0 or 1)
			end
		else
			pCharacter:GetHorseRenderObj():SetAlphaValue(255)
			pCharacter:GetHorseRenderObj():Update()
--			g_GameMain.m_CharacterInSyncMgr:SetHeadInfoTransparent(pCharacter, 1)
			if(pCharacter.m_HeadInfoWnd) then
				pCharacter.m_HeadInfoWnd:SetTransparent(1)
			end
		end
	end
	SetCanBeSelectedForLua(pCharacter, not bHide)
end

local function GetChararacterCanBeSelected(Character)
	local Type = Character.m_Properties:GetType()
	if Type == ECharacterType.Player then
		local bFriendCanSelected = g_UISettingMgr:GetOnePropState("友方不可选中")
		if bFriendCanSelected and g_MainPlayer:IsFriend(Character)then
			return false
		end
		if not g_MainPlayer:IsEnemy(Character) and not Character:CppIsAlive() and not g_MainPlayer.m_CanLookDead then
			return false
		end
		if g_GameMain.HidePlayer and not g_MainPlayer:IsEnemy(Character) then
			return false
		end
		if g_GameMain.HideAllPlayerExceptTeammate and not g_MainPlayer:IsEnemy(Character) then
			local SelfTeamID = g_MainPlayer.m_Properties:GetTeamID()
			local TargetTeamID = Character.m_Properties:GetTeamID()
			if SelfTeamID == TargetTeamID and SelfTeamID ~= 0 then
				return false
			end
		end
	elseif Type == ECharacterType.Npc and Character:GetNpcType() ~= ENpcType.ENpcType_BattleHorse then
		local NpcName = Character.m_Properties:GetCharName()
		local bFriendCanSelected = g_UISettingMgr:GetOnePropState("友方不可选中")
		if bFriendCanSelected and g_MainPlayer:IsFriend(Character) and Character:GetMaster() ~= g_MainPlayer then
			return false
		end
		local NpcCfg = Npc_Common(NpcName)
		if NpcCfg("ShowBloodType") == 1 or NpcCfg("ShowBloodType") == 2 then
			return false
		end
		if not Character:CppIsAlive() then
			return false
		end
	end
	return true
end

function SetCanBeSelectedForLua(Character, bSelected)
	if Character then
		if bSelected then
			if GetChararacterCanBeSelected(Character) then
				Character:SetCanBeSelected(true)
			end
		else
			Character:SetCanBeSelected(false)
		end
	end
end

function CHideFollowerMgr:IsHideNpcType(npc_type)
	if npc_type == ENpcType.ENpcType_Summon or
			npc_type == ENpcType.ENpcType_BattleHorse or
			npc_type == ENpcType.ENpcType_Truck or
			npc_type == ENpcType.ENpcType_LittlePet then
				return true
	end
	return false
end

--隐藏所有Player
function CHideFollowerMgr:HidePlayerFollower(bHide)
	g_GameMain.HidePlayer = bHide
	local tbl = g_GameMain.m_CharacterInSyncMgr.m_tblCharacter
	for i, p in pairs(tbl) do
		if p and IsCppBound(p) then
			local Type = p.m_Properties:GetType()
			if Type == ECharacterType.Player then
					self:HidePlayer(p, bHide)
			elseif Type == ECharacterType.Npc then
				if self:IsHideNpcType(p:GetNpcType()) then
					local pMaster = p:GetMaster()
					if pMaster ~= g_MainPlayer then
						self:HidePlayer(p, bHide)
					end
				end
			elseif Type == ECharacterType.IntObj then
				self:HideIntObj(p, bHide)
			end
		end
	end
	if( bHide ) then
		if g_MainPlayer.m_LockCenter.m_LockingObj then
			g_MainPlayer:UnLockObj()
		end
	end
end

--隐藏除队友的所有Player
function CHideFollowerMgr:HidePlayerFollowerExceptTeammate(bHide)
	g_GameMain.HideAllPlayerExceptTeammate = bHide
	local tbl = g_GameMain.m_CharacterInSyncMgr.m_tblCharacter
	for i, p in pairs(tbl) do
		if p and IsCppBound(p) then
			local Type = p.m_Properties:GetType()
			if Type == ECharacterType.Player then
				local SelfTeamID = g_MainPlayer.m_Properties:GetTeamID()
				local TargetTeamID = p.m_Properties:GetTeamID()
				if SelfTeamID ~= TargetTeamID or SelfTeamID == 0 then
					self:HidePlayer(p, bHide)
				elseif bHide == false then
					self:HidePlayer(p, bHide)
				end
			elseif Type == ECharacterType.Npc then
				if self:IsHideNpcType(p:GetNpcType()) then
					local pMasterID = p:GetMasterID()
					if pMasterID ~= g_MainPlayer:GetEntityID() and not g_GameMain.m_TeamBase:IsTeamMember(pMasterID) then
						self:HidePlayer(p, bHide)
					end
				end
			elseif Type == ECharacterType.IntObj then
				self:HideIntObj(p, bHide)
			end
		end
	end
	if( bHide ) then
		if g_MainPlayer.m_LockCenter.m_LockingObj then
			g_MainPlayer:UnLockObj()
		end
	end
end

function CHideFollowerMgr:HideAllPlayerFollowerEx(bHide)
	if g_MainPlayer then
		g_GameMain.HideAllPlayerExceptTeammate = false
		self:HidePlayerFollower(bHide)
		g_GameMain.m_HidePlayerWnd:HideOrShowPlayer()
	end
end


function CHideFollowerMgr:HideIntObj(pIntObj, bHide)
	local ObjName = pIntObj.m_Properties:GetCharName()
	if ObjName == "挑战旗" and pIntObj.m_Properties:GetCreatorName() ~= g_MainPlayer.m_Properties:GetCharName() then
		local RenderObject = pIntObj:GetRenderObject()
		if bHide then
			RenderObject:SetAlphaValue(0)
			pIntObj:SetCanBeSelected(false)
		else
			RenderObject:SetAlphaValue(255)
			pIntObj:SetCanBeSelected(true)
		end
	end
end

function CHideFollowerMgr:DealHideFollowerIntoSync(pCharacter)
	local Type = pCharacter.m_Properties:GetType()
	if Type == ECharacterType.Player then
		if g_GameMain.HidePlayer then
			self:HidePlayer(pCharacter, true)
		elseif g_GameMain.HideAllPlayerExceptTeammate then
			local SelfTeamID = g_MainPlayer.m_Properties:GetTeamID()
			local TargetTeamID = pCharacter.m_Properties:GetTeamID()
			if SelfTeamID ~= TargetTeamID or SelfTeamID == 0 then
				self:HidePlayer(pCharacter, true)
			end
		else
			local bStealth = pCharacter.m_Properties:GetStealth()
			pCharacter:SetStealthState(bStealth)
		end
	elseif Type == ECharacterType.Npc then
		if self:IsHideNpcType(pCharacter:GetNpcType()) then
			local pMasterID = pCharacter:GetMasterID()
			if pMasterID ~= g_MainPlayer:GetEntityID() then
				if g_GameMain.HidePlayer then
					self:HidePlayer(pCharacter, true)
				elseif g_GameMain.HideAllPlayerExceptTeammate then
					if not g_GameMain.m_TeamBase:IsTeamMember(pMasterID) then
						self:HidePlayer(pCharacter, true)
					end
				end
			end
		end
	end
end

function CHideFollowerMgr:ChangeHideModeOnRelationChanged(pCharacter)
	if pCharacter.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	if g_MainPlayer == pCharacter then
		if g_GameMain.HidePlayer then
			self:HidePlayerFollower(false)
			self:HidePlayerFollower(true)
		elseif g_GameMain.HideAllPlayerExceptTeammate then
			self:HidePlayerFollowerExceptTeammate(false)
			self:HidePlayerFollowerExceptTeammate(true)
		end
	else
		self:HidePlayer(pCharacter, false)
		self:DealHideFollowerIntoSync(pCharacter)
	end
end

function CHideFollowerMgr:DealHideFollowerOnTeamChanged(pCharacter, bHide)
	if not g_GameMain.HideAllPlayerExceptTeammate or not pCharacter then
		return
	end
	self:HidePlayer(pCharacter, bHide)
	local tbl = g_GameMain.m_CharacterInSyncMgr.m_tblCharacter
	for i, p in pairs(tbl) do
		if p and IsCppBound(p) then
			local Type = p.m_Properties:GetType()
			if Type == ECharacterType.Npc and self:IsHideNpcType(p:GetNpcType()) then
				local pMasterID = p:GetMasterID()
				if pMasterID ~= g_MainPlayer:GetEntityID() and g_GameMain.m_TeamBase:IsTeamMember(pMasterID) then
					self:HidePlayer(p, bHide)
				end
			end
		end
	end
end

function CHideFollowerMgr:DealHideIntObjIntoSync(pIntObj)
	local bHideIntObj = g_GameMain.HidePlayer or g_GameMain.HideAllPlayerExceptTeammate
	if pIntObj and bHideIntObj then
		self:HideIntObj(pIntObj, true)
	end
end

function CHideFollowerMgr:ExposePlayer(EntityID)
	local pCharacter = CCharacterFollower_GetCharacterByID(EntityID)
	if pCharacter then
		pCharacter:GetRenderObject():SetAlphaValue(255)
		pCharacter:GetRenderObject():Update()
		pCharacter:SetCanBeSelected(true)
		g_MainPlayer.m_ChallengeID = EntityID
	end
end

function CHideFollowerMgr:SetFriendCanNotBeSelected(pCharacter)
	if not pCharacter or pCharacter.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	
	local function IsRightObj(Char)
		local Type = Char.m_Properties:GetType()
		if (Type == ECharacterType.Player) then
			return true
		elseif(Type == ECharacterType.Npc and Char:GetMaster() ~= g_MainPlayer ) then
			local NpcName = Char.m_Properties:GetCharName()
			if Npc_Common(NpcName,"ShowBloodType") ~= 5 then
				return true
			end
		end
		return false
	end
	
	local bFriendCanSelected = g_UISettingMgr:GetOnePropState("友方不可选中")
	if bFriendCanSelected then
		if g_MainPlayer ~= pCharacter and IsRightObj(pCharacter) then
			if g_MainPlayer:IsFriend(pCharacter) then
				pCharacter:SetCanBeSelected(false)
				if g_MainPlayer.m_LockCenter.m_LockingObj == pCharacter then
					g_MainPlayer:UnLockObj()
				end
			elseif g_MainPlayer:IsEnemy(pCharacter) then
				SetCanBeSelectedForLua(pCharacter,true)
			end
		end
	else
		SetCanBeSelectedForLua(pCharacter,true)
	end
end



gac_require "fight/target_info/TargetInfoInc"
gac_require "fight/skill_loading/BaseSkillLoading"

function CreateTargetOfTargetInfo( parent )
	local wnd = CTargetOfTargetInfo:new()
	wnd:CreateFromRes("TargetHeadOfAim", parent)
	wnd:SetIsBottom(true)
	return wnd
end

function CTargetOfTargetInfo:Update(TargetFollower)
	local Target = TargetFollower
	if(not Target) then return end

	self.m_TargetLevel = Target:CppGetLevel()
	self.m_WndLevel:SetWndText(self.m_TargetLevel)
	
	--******Ñª**********************************************************************
	local nCurHP	= Target:CppGetCtrlState(EFighterCtrlState.eFCS_FeignDeath) and 0 or Target:CppGetPropertyValueByName("HealthPointAgile")
	local nUpperHP	= Target:CppGetPropertyValueByName("HealthPoint")
	self.m_WndHP:SetRange(nUpperHP)
	self.m_WndHP:SetWndText( string.format("%d/%d", nCurHP, nUpperHP) )
	self.m_WndHP:SetPos(nCurHP)
	
	g_GameMain.m_CharacterInSyncMgr:ChangeHPByMe(self.m_WndHP)
end

function CTargetOfTargetInfo:ShowAndUpdate(TargetFollower)
	if TargetFollower then
		self.uTargetOfTargetId = TargetFollower:GetEntityID()
	end
	self:Update(TargetFollower)
	self:SetTargetInfo(TargetFollower)--¸üÐÂÍ·Ïñ
	self:ShowWnd(true)
end

function CTargetOfTargetInfo:LockTargetOfTarget(TargetFollower)
	local CharacterFollower = g_MainPlayer.m_LockCenter.m_LockingObj
	if not CharacterFollower then 
		return
	end
	CharacterFollower.m_LockCenter.m_LockingObj = TargetFollower
	self:ShowAndUpdate(TargetFollower)
	self:OnCastEnd()
	CTargetOfTargetBuffWnd.GetWnd():ClearAllBuffState()
	--TargetFollower:SetTargetBuff()
	--g_MainPlayer:GetTargetCastingProcess(TargetFollower:GetEntityID())
	TargetFollower:SetAgileValueBeCare(true)
end

function CTargetOfTargetInfo:ReLockTargetOfTarget(TargetFollower)
	if(not TargetFollower) then return end
	if self.uTargetOfTargetId ~= nil and self.uTargetOfTargetId == TargetFollower:GetEntityID() then
		self:LockTargetOfTarget(TargetFollower)
	end
end

function CTargetOfTargetInfo:Close()
	if not self:IsShow() then
		return
	end
	local target = g_MainPlayer.m_LockCenter.m_LockingObj
	self:ShowWnd(false)
	CTargetOfTargetBuffWnd.GetWnd():ClearAllBuffState()
	g_GameMain.m_TargetOfTargetInfo:OnCastEnd()
	if target == g_MainPlayer then
		return
	end
	if target then
		if target.m_LockCenter.m_LockingObj then
			if target.m_LockCenter.m_LockingObj ~= g_MainPlayer and
				target.m_LockCenter.m_LockingObj ~= target and
				target.m_LockCenter.m_LockingObj~=g_MainPlayer:GetServant(ENpcType.ENpcType_BattleHorse) and
				target.m_LockCenter.m_LockingObj~=g_MainPlayer:GetServant(ENpcType.ENpcType_Summon) then
				local selectedEntityType = target.m_LockCenter.m_LockingObj:GetGameEntityType()
				if selectedEntityType == EGameEntityType.eGET_CharacterFollower then
					local targetOfTarget = target.m_LockCenter.m_LockingObj
					if(not g_GameMain.m_TeamBase:IsTeamMember(targetOfTarget:GetEntityID())) then
						targetOfTarget:SetAgileValueBeCare(false)
					end
				end
			end
			target.m_LockCenter.m_LockingObj = nil
		end
	end
end

function CTargetOfTargetInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
end
function CTargetOfTargetInfo:OnRButtonClick( wParam, x, y )
end

function CTargetOfTargetInfo:OnLButtonClick( wParam, x, y )
	self:OnLButtonUp( wParam, x, y )
end

function CTargetOfTargetInfo:OnLButtonUp( wParam, x, y )
	local Character = CCharacterFollower_GetCharacterByID(g_GameMain.m_TargetOfTargetInfo.m_TargetEntityID)
	if Character then
		g_MainPlayer:LockObj(Character)
		g_MainPlayer:CancelNormalAttack()
	end
end
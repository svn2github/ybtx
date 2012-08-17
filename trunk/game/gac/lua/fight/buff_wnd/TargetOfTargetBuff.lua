
CTargetOfTargetBuffWnd	= class ( CBuffWnd )
--目标的目标的Buff
function CTargetOfTargetBuffWnd:Ctor( Parent )
	self:Init( Parent, g_GameMain.m_TargetOfTargetInfo )
end

function CTargetOfTargetBuffWnd.GetWnd()
	local Wnd = g_GameMain.m_TargetOfTargetBuff
	if not Wnd then
		Wnd = CTargetOfTargetBuffWnd:new(g_GameMain)
		g_GameMain.m_TargetOfTargetBuff = Wnd
	end
	return Wnd
end

function CTargetOfTargetBuffWnd:DeleteBuffDebuffIfCharacterDestroyed(BuffID)
	local Character = CCharacterFollower_GetCharacterByID(g_GameMain.m_TargetOfTargetInfo.m_TargetEntityID)
	if (g_GameMain.m_TargetOfTargetInfo:IsShow() and g_MainPlayer.m_LockCenter.m_LockingObj ~= nil and
		g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter and g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj ~= Character) then
		self:DeleteBuffDebuff(BuffID)
		return true
	end
end

function CTargetOfTargetBuffWnd:GetRemainTime(ID, buff_name)
	if (g_GameMain.m_TargetOfTargetInfo:IsShow() and g_MainPlayer.m_LockCenter.m_LockingObj ~= nil and
		g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter and g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj) then
		local RemainTime = g_MainPlayer.m_LockCenter.m_LockingObj.m_LockCenter.m_LockingObj:GetRemainTime(buff_name, ID)
		return RemainTime or 0
	end
	return 0
end

function CTargetOfTargetBuffWnd:InitPos(ParentRectWnd)
	local Rect	= CFRect:new()
	ParentRectWnd:GetLogicRect(Rect)
	self:SetPos(self, Rect.left + 30, Rect.bottom - 4)
end
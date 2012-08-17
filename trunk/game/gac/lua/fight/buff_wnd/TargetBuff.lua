
CTargetBuffWnd 						= class ( CBuffWnd )
------- ´´½¨ CTargetBuffWnd
function CTargetBuffWnd:Ctor( Parent )
	self:Init( Parent, g_GameMain.m_TargetInfo )
end

function CTargetBuffWnd.GetWnd()
	local Wnd = g_GameMain.m_TargetBuff
	if not Wnd then
		Wnd = CTargetBuffWnd:new(g_GameMain)
		g_GameMain.m_TargetBuff = Wnd
	end
	return Wnd
end

function CTargetBuffWnd:DeleteBuffDebuffIfCharacterDestroyed(BuffID)
	local Character = CCharacterFollower_GetCharacterByID(g_GameMain.m_TargetInfo.m_TargetEntityID)
	if g_MainPlayer.m_LockCenter.m_LockingObj == nil or Character ~= g_MainPlayer.m_LockCenter.m_LockingObj then
		self:DeleteBuffDebuff(BuffID)
		return true
	end
end

function CTargetBuffWnd:GetRemainTime(ID, buff_name)
	if g_MainPlayer.m_LockCenter.m_LockingObj then
		local RemainTime = g_MainPlayer.m_LockCenter.m_LockingObj:GetRemainTime(buff_name, ID)
		return RemainTime or 0
	end
	return 0
end

function CTargetBuffWnd:InitPos(ParentRectWnd)
	local Rect	= CFRect:new()
	ParentRectWnd:GetLogicRect(Rect)
	self:SetPos(self, Rect.left + 62, Rect.bottom - 1)
end

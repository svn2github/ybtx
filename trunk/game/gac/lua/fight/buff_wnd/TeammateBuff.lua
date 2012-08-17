CTeammateBuff						= class ( CBuffWnd )

function CTeammateBuff:Ctor( Parent )
	self:Init( Parent, Parent )
	self.m_Parent = Parent
end

function CTeammateBuff:DeleteBuffDebuffIfCharacterDestroyed(BuffID)
	local Character = CCharacterFollower_GetCharacterByID(self.m_Parent.m_uEntityId)
	if(not Character) then
		self:DeleteBuffDebuff(BuffID)
		return true
	end
end

function CTeammateBuff:GetRemainTime(ID, buff_name)
	local Character = CCharacterFollower_GetCharacterByID(self.m_Parent.m_uEntityId)
	if(Character) then
		local RemainTime = Character:GetRemainTime(buff_name, ID)
		return RemainTime or 0
	end
	return 0
end

----------------------------------------------------------------------
function CTeammateBuff:InitData()
	self.m_RowNum			= 1
	self.m_ColNum			= 9
	self.m_IconSize			= 22
	self.m_OneTypeLineNum	= 1
	self.m_tblBuffs = {{}}
	self.m_tblBuffs[1].n = 0
end

function CTeammateBuff:GetBuffList(BuffType)
	return self.m_tblBuffs[1]
end

function CTeammateBuff:SetListItemIndex(BuffList, ID, BuffType)
	BuffList[ID].listItemIndex = BuffList.n
end

function CTeammateBuff:InitPos(ParentRectWnd)
	local Rect	= CFRect:new()
	ParentRectWnd:GetLogicRect(Rect)
	self:SetPos(self, Rect.right, Rect.bottom - 25)
end
----------------------------------------------------------------------

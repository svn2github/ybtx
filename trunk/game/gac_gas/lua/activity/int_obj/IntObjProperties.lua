CIntObjProperties = class(CCharacterProperties)

function CIntObjProperties:Init(IntObj)
	CCharacterProperties.Init(self, IntObj)
end

function CIntObjProperties:GetModelName()
	return self.m_StaticViewRootVariant:GetString("Attribute/ModelName")
end

function CIntObjProperties:GetCanUseState()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/CanUseState")
end

function CIntObjProperties:GetActionNum()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/ActionNum")
end

function CIntObjProperties:GetDropOwnerId()
	return self.m_StaticViewRootVariant:GetNumber("Attribute/DropOwnerId")
end

function CIntObjProperties:GetDropTime()
	return self.m_StaticViewRootVariant:GetNumber("Attribute/DropTime")
end

function CIntObjProperties:GetBelongToTeam()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/BelongToTeam")
end

function CIntObjProperties:GetResourceCount()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/ResourceCount")
end

function CIntObjProperties:GetDuelWinCount()
	return self.m_StaticViewRootVariant:GetString("Attribute/DuelWinCount")
end

function CIntObjProperties:GetCreatorName()
	return self.m_StaticViewRootVariant:GetString("Attribute/CreatorName")
end

function CIntObjProperties:GetBornAction()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/BornAction")
end

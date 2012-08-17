CBattleHorseProperties = class(CCharacterProperties)

function CBattleHorseProperties:Init(BattleHorse)
	CCharacterProperties.Init(self, BattleHorse)
end

function CBattleHorseProperties:GetSex()
	return self.m_ViewRootVarian:GetUInt8("Attribute/Sex")
end


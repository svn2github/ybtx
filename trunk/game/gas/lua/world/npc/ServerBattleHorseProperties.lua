CServerBattleHorseProperties =class(CBattleHorseProperties)

function CServerBattleHorseProperties:Init(BattleHorse, sName)
	CBattleHorseProperties.Init(self, BattleHorse)
	if (sName ~= nil) then
		self:SetCharName(sName)
	end
	self:SetType(ECharacterType.Player)
end

function CServerBattleHorseProperties:SetCharName(sName)
	self.m_StaticViewRootVariant:SetString("Attribute/CharName",sName)
end

function CServerBattleHorseProperties:SetType(uType)
	self.m_StaticViewRootVariant:SetUInt8("Attribute/Type",uType)
end

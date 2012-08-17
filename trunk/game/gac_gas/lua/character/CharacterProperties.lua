CCharacterProperties = class()

function CCharacterProperties:Ctor()
end

function CCharacterProperties:Init(Character)
	self.m_Character = Character
	self.m_StaticViewRootVariant = Character:GetViewVariantHolder(ECodeTableIndex.eCTI_Static)
	self.m_AgileViewRootVariant = Character:GetViewVariantHolder(ECodeTableIndex.eCTI_Agile)
end

function CCharacterProperties:InitSync()
	self.m_StaticSyncRootVariant = self.m_Character:GetSyncVariantHolder(ECodeTableIndex.eCTI_Static)
	self.m_AgileSyncRootVariant = self.m_Character:GetSyncVariantHolder(ECodeTableIndex.eCTI_Agile)
end

function CCharacterProperties:OutOfSync()
	self.m_StaticSyncRootVariant = nil
	self.m_AgileSyncRootVariant = nil
end

function CCharacterProperties:GetType()
	return self.m_StaticViewRootVariant:GetUInt8("Attribute/Type")
end

function CCharacterProperties:GetCharName()
	return self.m_StaticViewRootVariant:GetString("Attribute/CharName")
end
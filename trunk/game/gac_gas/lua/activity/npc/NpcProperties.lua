CNpcProperties = class(CCharacterProperties)

function CNpcProperties:Init(Npc)
	CCharacterProperties.Init(self, Npc)
end

function CNpcProperties:InitSync(Npc)
	CCharacterProperties.InitSync(self)
end

function CNpcProperties:GetCharName()
	return self.m_StaticViewRootVariant:GetString("Attribute/CharName")
end

function CNpcProperties:GetSex()
	if nil == self.m_StaticViewRootVariant then
		return ECharSex.eCS_NoSex
	end
	return self.m_StaticViewRootVariant:GetUInt8("Attribute/Sex")
end

function CNpcProperties:GetMasterTeamID()
	return self.m_AgileViewRootVariant:GetUInt32("Master/MasterTeamID")
end

function CNpcProperties:GetMasterTroopID()
	return self.m_AgileViewRootVariant:GetUInt32("Master/MasterTroopID")
end

function CNpcProperties:GetMasterTongID()
	return self.m_AgileViewRootVariant:GetUInt32("Master/MasterTongID")
end

function CNpcProperties:GetDropOwnerId()
	return self.m_StaticViewRootVariant:GetNumber("Attribute/DropOwnerId")
end

function CNpcProperties:GetDropTime()
	return self.m_StaticViewRootVariant:GetNumber("Attribute/DropTime")
end

function CNpcProperties:GetBelongToTeam()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/BelongToTeam")
end


-- SyncRootVariant --目前分身专用
function CNpcProperties:GetIsShadow()
	return self.m_StaticSyncRootVariant:GetBool("MasterRes/bIsShadow")
end

function CNpcProperties:GetMasterClass()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/MasterClass")
end

function CNpcProperties:GetMasterSex()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/MasterSex")
end

function CNpcProperties:GetHairResID()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/HairResID")
end

function CNpcProperties:GetFaceResID()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/FaceResID")
end

function CNpcProperties:GetWeaponIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/WeaponIndexID")
end

function CNpcProperties:GetOffWeaponIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/OffWeaponIndexID")
end

function CNpcProperties:GetOffWeaponBigID()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/OffWeaponBigID")
end

function CNpcProperties:GetHorseResID()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/HorseResID")
end

function CNpcProperties:GetHandsBehaviorName()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/HandsBehaviorName")
end

function CNpcProperties:GetHorseType()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/HorseType")
end

function CNpcProperties:GetBehaviorType()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/BehaviorType")
end

function CNpcProperties:GetHeadResID()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/HeadResID")
end

function CNpcProperties:GetBodyResID()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/BodyResID")
end

function CNpcProperties:GetArmResID()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/ArmResID")
end

function CNpcProperties:GetShoeResID()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/ShoeResID")
end

function CNpcProperties:GetShoulderResID()
	return self.m_StaticSyncRootVariant:GetString("MasterRes/ShoulderResID")
end

function CNpcProperties:GetWeaponResID()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/WeaponResID")
end

function CNpcProperties:GetOffWeaponResID()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/OffWeaponResID")
end

function CNpcProperties:GetBackResID()
	return self.m_StaticSyncRootVariant:GetUInt32("MasterRes/BackResID")
end

function CNpcProperties:GetNpcMaterialNum()
	return self.m_AgileSyncRootVariant:GetUInt32("Attribute1/MaterialNum")
end

function CNpcProperties:GetWeaponPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/WeaponPhase")
end

function CNpcProperties:GetOffWeaponPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/OffWeaponPhase")
end

function CNpcProperties:GetHeadPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/HeadPhase")
end

function CNpcProperties:GetBodyPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/BodyPhase")
end

function CNpcProperties:GetShoePhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/ShoePhase")
end

function CNpcProperties:GetShoulderPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/ShoulderPhase")
end

function CNpcProperties:GetArmPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("MasterRes/ArmPhase")
end
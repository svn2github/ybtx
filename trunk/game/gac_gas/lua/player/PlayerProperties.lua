CPlayerProperties = class(CCharacterProperties)

function CPlayerProperties:Init(Character)
	CCharacterProperties.Init(self, Character)
	self.m_uRollRes = 0   --ROLL值
end

--ViewVariant
function CPlayerProperties:GetTeamID()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/TeamID")
end

---------- PK值
function CPlayerProperties:GetPkValue()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/PkValue")
end

------------ 帮会
function CPlayerProperties:GetTongID()
	return self.m_StaticViewRootVariant:GetUInt32("Attribute/TongID")
end

function CPlayerProperties:GetTongName()
	return self.m_StaticViewRootVariant:GetString("Attribute/TongName")
end

function CPlayerProperties:GetCharID()
	return self.m_StaticViewRootVariant:GetNumber("Attribute/CharID")
end

function CPlayerProperties:GetUserID()
	return self.m_StaticViewRootVariant:GetNumber("Attribute/UserID")
end

function CPlayerProperties:GetCharAppellation1()
	return self.m_StaticViewRootVariant:GetUInt16("Attribute/CharAppellation1")
end

function CPlayerProperties:GetCharAppellation2()
	return self.m_StaticViewRootVariant:GetUInt16("Attribute/CharAppellation2")
end

function CPlayerProperties:GetSex()
	return self.m_StaticViewRootVariant:GetUInt8("Attribute/Sex")
end

function CPlayerProperties:GetArmyCorpsName()
	return self.m_StaticViewRootVariant:GetString("Attribute/ArmyCorpsName")
end

-- SyncRootVariant
function CPlayerProperties:GetHairResID()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/HairResID")
end

function CPlayerProperties:GetFaceResID()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/FaceResID")
end

function CPlayerProperties:GetShowArmet()
	return self.m_StaticSyncRootVariant:GetBool("Res/ShowArmet")
end

function CPlayerProperties:GetHeadIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/HeadIndexID")
end

function CPlayerProperties:GetBodyIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/BodyIndexID")
end

function CPlayerProperties:GetShoulderIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/ShoulderIndexID")
end

function CPlayerProperties:GetArmIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/ArmIndexID")
end

function CPlayerProperties:GetShoeIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/ShoeIndexID")
end

function CPlayerProperties:GetWeaponIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/WeaponIndexID")
end

function CPlayerProperties:GetOffWeaponIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/OffWeaponIndexID")
end

function CPlayerProperties:GetOffWeaponBigID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/OffWeaponBigID")
end

function CPlayerProperties:GetBackIndexID()
	return self.m_StaticSyncRootVariant:GetUInt32("Res/BackIndexID")
end

function CPlayerProperties:GetHorseResID()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/HorseResID")
end

function CPlayerProperties:GetHandsBehaviorName()
	return self.m_StaticSyncRootVariant:GetString("Res/HandsBehaviorName")
end

function CPlayerProperties:GetHorseType()
	return self.m_StaticSyncRootVariant:GetString("Res/HorseType")
end

function CPlayerProperties:GetBehaviorType()
	return self.m_StaticSyncRootVariant:GetString("Res/BehaviorType")
end

function CPlayerProperties:GetWeaponPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/WeaponPhase")
end

function CPlayerProperties:GetOffWeaponPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/OffWeaponPhase")
end

function CPlayerProperties:GetHeadPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/HeadPhase")
end

function CPlayerProperties:GetBodyPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/BodyPhase")
end

function CPlayerProperties:GetShoePhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/ShoePhase")
end

function CPlayerProperties:GetShoulderPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/ShoulderPhase")
end

function CPlayerProperties:GetArmPhase()
	return self.m_StaticSyncRootVariant:GetUInt8("Res/ArmPhase")
end

function CPlayerProperties:GetStealth()
	return self.m_AgileSyncRootVariant:GetBool("Attribute1/Stealth")
end

function CPlayerProperties:InitFightEnergy()
       self.m_uRollRes = 0   --ROLL值
end

function CPlayerProperties:GetRollRes()
	return self.m_uRollRes
end

function CPlayerProperties:SetRollRes(uRollRes)
	self.m_uRollRes = uRollRes
end

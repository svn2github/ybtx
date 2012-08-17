CServerNpcProperties = class( CNpcProperties )

function CServerNpcProperties:Init(Npc)
	self.m_InTrapTblName = {}
	CNpcProperties.Init(self, Npc)
	CNpcProperties.InitSync(self)
end

function CServerNpcProperties:InitNpcStatic(sName,uSex,uType)
   self:SetCharName(sName)
   self:SetSex(uSex)
   self:SetType(uType)
   self:SetDropOwnerId(0)
   self:SetDropTime(0)
   self:SetBelongToTeam(0)
   self:SetDynRoot()
   self:SetIsShadow(false)
end

function CServerNpcProperties:SetDynRoot()
	self.m_AgileSyncRootVariant:SetDynRoot("State")
end

function CServerNpcProperties:SetCharName(sName)
	self.m_StaticViewRootVariant:SetString("Attribute/CharName",sName)
end

function CServerNpcProperties:SetSex(uSex)
	self.m_StaticViewRootVariant:SetUInt8("Attribute/Sex",uSex)
end

function CServerNpcProperties:SetType(uType)
	self.m_StaticViewRootVariant:SetUInt8("Attribute/Type",uType)
end

function CServerNpcProperties:SetDropOwnerId(OwnerId)
	self.m_StaticViewRootVariant:SetNumber("Attribute/DropOwnerId",OwnerId)
end

function CServerNpcProperties:SetDropTime(DropTime)
	self.m_StaticViewRootVariant:SetNumber("Attribute/DropTime",DropTime)
end

function CServerNpcProperties:SetBelongToTeam(TeamID)
	self.m_StaticViewRootVariant:SetUInt32("Attribute/BelongToTeam",TeamID)
end

function CServerNpcProperties:SetMasterTeamID(MasterTeamID)
	self.m_AgileViewRootVariant:SetUInt32("Master/MasterTeamID",MasterTeamID)
end

function CServerNpcProperties:SetMasterTroopID(MasterTroopID)
	self.m_AgileViewRootVariant:SetUInt32("Master/MasterTroopID",MasterTroopID)
end

function CServerNpcProperties:SetMasterTongID(MasterTongID)
	self.m_AgileViewRootVariant:SetUInt32("Master/MasterTongID",MasterTongID)
end

-- SyncRootVariant --目前分身专用
function CServerNpcProperties:SetIsShadow(IsShadow)
	self.m_StaticSyncRootVariant:SetBool("MasterRes/bIsShadow",IsShadow)
end

function CServerNpcProperties:SetMasterClass(MasterClass)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/MasterClass",MasterClass)
end

function CServerNpcProperties:SetMasterSex(MasterSex)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/MasterSex",MasterSex)
end

function CServerNpcProperties:SetHairResID(HairResID)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/HairResID",HairResID)
end

function CServerNpcProperties:SetFaceResID(FaceResID)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/FaceResID",FaceResID)
end

function CServerNpcProperties:SetWeaponIndexID(WeaponIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/WeaponIndexID",WeaponIndexID)
end

function CServerNpcProperties:SetOffWeaponIndexID(OffWeaponIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/OffWeaponIndexID",OffWeaponIndexID)
end

function CServerNpcProperties:SetOffWeaponBigID(OffWeaponBigID)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/OffWeaponBigID",OffWeaponBigID)
end

function CServerNpcProperties:SetHorseResID(HorseResID)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/HorseResID",HorseResID)
end

function CServerNpcProperties:SetHandsBehaviorName(HandsBehaviorName)
	self.m_StaticSyncRootVariant:SetString("MasterRes/HandsBehaviorName",HandsBehaviorName)
end

function CServerNpcProperties:SetHorseType(HorseType)
	self.m_StaticSyncRootVariant:SetString("MasterRes/HorseType",HorseType)
end

function CServerNpcProperties:SetBehaviorType(BehaviorType)
	self.m_StaticSyncRootVariant:SetString("MasterRes/BehaviorType",BehaviorType)
end

function CServerNpcProperties:SetHeadResID(HeadIndexID)
	self.m_StaticSyncRootVariant:SetString("MasterRes/HeadResID",HeadIndexID)
end

function CServerNpcProperties:SetBodyResID(BodyResID)
	self.m_StaticSyncRootVariant:SetString("MasterRes/BodyResID",BodyResID)
end

function CServerNpcProperties:SetArmResID(ArmResID)
	self.m_StaticSyncRootVariant:SetString("MasterRes/ArmResID",ArmResID)
end

function CServerNpcProperties:SetShoeResID(ShoeResID)
	self.m_StaticSyncRootVariant:SetString("MasterRes/ShoeResID",ShoeResID)
end

function CServerNpcProperties:SetShoulderResID(ShoulderResID)
	self.m_StaticSyncRootVariant:SetString("MasterRes/ShoulderResID",ShoulderResID)
end

function CServerNpcProperties:SetWeaponResID(WeaponResID)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/WeaponResID",WeaponResID)
end

function CServerNpcProperties:SetOffWeaponResID(OffWeaponResID)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/OffWeaponResID",OffWeaponResID)
end

function CServerNpcProperties:SetBackResID(BackResID)
	self.m_StaticSyncRootVariant:SetUInt32("MasterRes/BackResID",BackResID)
end

function CServerNpcProperties:SetMaterialNum(MaterialNum)
	self.m_AgileSyncRootVariant:SetUInt32("Attribute1/MaterialNum", MaterialNum)
end

function CServerNpcProperties:SetWeaponPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/WeaponPhase",Phase)
end

function CServerNpcProperties:SetOffWeaponPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/OffWeaponPhase",Phase)
end

function CServerNpcProperties:SetHeadPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/HeadPhase",Phase)
end

function CServerNpcProperties:SetBodyPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/BodyPhase",Phase)
end

function CServerNpcProperties:SetShoePhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/ShoePhase",Phase)
end

function CServerNpcProperties:SetShoulderPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/ShoulderPhase",Phase)
end

function CServerNpcProperties:SetArmPhase(BehaviorType)
	self.m_StaticSyncRootVariant:SetUInt8("MasterRes/ArmPhase",Phase)
end

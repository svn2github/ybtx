CServerPlayerProperties = class( CPlayerProperties )

function CServerPlayerProperties:Init(Character)
	self.m_InTrapTblName = {}
	CCharacterProperties.Init(self, Character)
	CCharacterProperties.InitSync(self)
	self:SetDynRoot()
end

function CServerPlayerProperties:InitCharStatic(uUserId,uSex)
       self:SetUserID(uUserId)
       self:SetSex(uSex)       --性别在 character上
end

function CServerPlayerProperties:InitCommon(uID,sName)
       self:SetCharID(uID)
       self:SetCharName(sName)
end

function CServerPlayerProperties:SetDynRoot()
	self.m_AgileSyncRootVariant:SetDynRoot("State")
end

function CServerPlayerProperties:SetCharID(uID)
	self.m_StaticViewRootVariant:SetNumber("Attribute/CharID",uID)
	--self.m_ViewRootVariant:SetEmptyTable("Attribute/State")
	--self.m_ViewRootVariant:AddSubTree("Attribute/State", "state1", "NewState1")
	--self.m_ViewRootVariant:AddSubTree("Attribute/State", "state2", "NewState2")
	--self.m_ViewRootVariant:AddSubTree("Attribute/State", "state1", "NewState3")
	--self.m_ViewRootVariant:SetUInt8("Attribute/State/NewState1/hp", 10)
end

function CServerPlayerProperties:SetCharName(sName)
	self.m_StaticViewRootVariant:SetString("Attribute/CharName",sName)
end

function CServerPlayerProperties:SetType(uType)
	self.m_StaticViewRootVariant:SetUInt8("Attribute/Type",uType)
end

function CServerPlayerProperties:SetSex(uSex)
	self.m_StaticViewRootVariant:SetUInt8("Attribute/Sex",uSex)
end

function CServerPlayerProperties:SetUserID(uUserID)
	self.m_StaticViewRootVariant:SetNumber("Attribute/UserID",uUserID)
end

function CServerPlayerProperties:SetCharAppellation1(uAppellation)
	self.m_StaticViewRootVariant:SetUInt16("Attribute/CharAppellation1",uAppellation)
end

function CServerPlayerProperties:SetCharAppellation2(uAppellation)
	self.m_StaticViewRootVariant:SetUInt16("Attribute/CharAppellation2",uAppellation)
end

function CServerPlayerProperties:SetTongName(TongName)
	self.m_StaticViewRootVariant:SetString("Attribute/TongName",TongName)
end

--/**旧PK代码**/
function CServerPlayerProperties:SetPkValue(PkValue)
	self.m_StaticViewRootVariant:SetUInt32("Attribute/PkValue",PkValue)
end

function CServerPlayerProperties:SetHairResID(HairResID)
	self.m_StaticSyncRootVariant:SetUInt8("Res/HairResID",HairResID)
end

function CServerPlayerProperties:SetFaceResID(FaceResID)
	self.m_StaticSyncRootVariant:SetUInt8("Res/FaceResID",FaceResID)
end

function CServerPlayerProperties:SetShowArmet(bShowArmet)
	self.m_StaticSyncRootVariant:SetBool("Res/ShowArmet", bShowArmet)
end

function CServerPlayerProperties:SetHeadIndexID(HeadIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/HeadIndexID",HeadIndexID)
end

function CServerPlayerProperties:SetBodyIndexID(BodyIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/BodyIndexID",BodyIndexID)
end

function CServerPlayerProperties:SetShoulderIndexID(ShoulderIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/ShoulderIndexID",ShoulderIndexID)
end

function CServerPlayerProperties:SetArmIndexID(ArmIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/ArmIndexID",ArmIndexID)
end

function CServerPlayerProperties:SetShoeIndexID(ShoeIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/ShoeIndexID",ShoeIndexID)
end

function CServerPlayerProperties:SetWeaponIndexID(WeaponIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/WeaponIndexID",WeaponIndexID)
end

function CServerPlayerProperties:SetOffWeaponIndexID(OffWeaponIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/OffWeaponIndexID",OffWeaponIndexID)
end

function CServerPlayerProperties:SetOffWeaponBigID(OffWeaponBigID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/OffWeaponBigID",OffWeaponBigID)
end

function CServerPlayerProperties:SetBackIndexID(BackIndexID)
	self.m_StaticSyncRootVariant:SetUInt32("Res/BackIndexID",BackIndexID)
end

function CServerPlayerProperties:SetHorseResID(HorseResID)
	self.m_StaticSyncRootVariant:SetUInt8("Res/HorseResID",HorseResID)
end

function CServerPlayerProperties:SetHandsBehaviorName(HandsBehaviorName)
	self.m_StaticSyncRootVariant:SetString("Res/HandsBehaviorName",HandsBehaviorName)
end

function CServerPlayerProperties:SetHorseType(HorseType)
	self.m_StaticSyncRootVariant:SetString("Res/HorseType",HorseType)
end

function CServerPlayerProperties:SetBehaviorType(BehaviorType)
	self.m_StaticSyncRootVariant:SetString("Res/BehaviorType",BehaviorType)
end

function CServerPlayerProperties:SetWeaponPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/WeaponPhase",Phase)
end

function CServerPlayerProperties:SetOffWeaponPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/OffWeaponPhase",Phase)
end

function CServerPlayerProperties:SetHeadPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/HeadPhase",Phase)
end

function CServerPlayerProperties:SetBodyPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/BodyPhase",Phase)
end

function CServerPlayerProperties:SetShoePhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/ShoePhase",Phase)
end

function CServerPlayerProperties:SetShoulderPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/ShoulderPhase",Phase)
end

function CServerPlayerProperties:SetArmPhase(Phase)
	self.m_StaticSyncRootVariant:SetUInt8("Res/ArmPhase",Phase)
end

function CServerPlayerProperties:SetArmyCorpsName(sArmyCorpsName)
	self.m_StaticViewRootVariant:SetString("Attribute/ArmyCorpsName",sArmyCorpsName)
end

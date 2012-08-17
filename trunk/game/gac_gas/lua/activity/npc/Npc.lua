cfg_load "npc/Npc_Common"

CNpc = class()

function CNpc:Ctor()
end

function CNpc:GetType()
	if self.m_sType==nil then
		return EMapRole.eMR_Npc
	end
	return self.m_sType
end
function CNpc:SetType(sType)
	self.m_sType=sType
end
function CNpc:GetCharName()
	return self.m_sName
end

function CNpc:SetCharName(sName)
	self.m_sName = sName
end

function CNpc:SetIntObjType(ObjType)
	self.m_IntObjType=ObjType
end
function CNpc:GetIntObjType(ObjType)
	return self.m_IntObjType
end
--得到Npc基本信息的table
--NpcName	Notes	NpcType	TaskNpcType	FuncNpcType	FightNpcType	NpcRace	NpcCamp	NpcClass	NpcMap	NpcIsTurn	NpcSize	MinLevel	MaxLevel	MoveType	MoveRange	WalkSpeed	RunSpeed	Vision	NpcType	DefyDistance	FightRange	ScanLevel 	FightType	ChaseDistance	Return	EscapeRate	CallRadius	FirstHate	RebornTime	NormalSkill	InCombat	Skill1	Condition1	Interval1	UseRate1	Skill2	Condition2	Interval2	UseRate2	Skill3	Condition3	Interval3	UseRate3	ModelFilePath	FxFilePath	HurtFxFilePath	HitDownFxFilePath	SoundFilePath	RenderStyleID	GoldenRenderStyleID	GoldenFxFilePath
function CNpc:GetNpcCommonInfo()
	assert(Npc_Common(self.m_sName))	-- 说明 NPC 名字填错，NPC 总表里面没有这个 NPC
	return  Npc_Common(self.m_sName)
end


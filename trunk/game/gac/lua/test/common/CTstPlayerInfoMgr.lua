gac_require "test/common/CTstPlayerInfoMgrInc"

--毒抗
function CTstPlayerInfoMgr:GetPoisonDefence()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_PoisonDefence - self.m_DefaultInfo.m_PoisonDefence
end
--毒抗
function CTstPlayerInfoMgr:GetFireDefence()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_FireDefence - self.m_DefaultInfo.m_FireDefence
end
--冰抗
function CTstPlayerInfoMgr:GetIceDefence()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_IceDefence - self.m_DefaultInfo.m_IceDefence
end
--雷抗
function CTstPlayerInfoMgr:GetThunderDefence()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_ThunderDefence - self.m_DefaultInfo.m_ThunderDefence
end

--敏捷	
function CTstPlayerInfoMgr:GetAddAgility()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_Agility - self.m_DefaultInfo.m_Agility
end
--智力
function CTstPlayerInfoMgr:GetAddIntellect()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_Intellect - self.m_DefaultInfo.m_Intellect
end
--精神
function CTstPlayerInfoMgr:GetAddSpirit()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_Spirit - self.m_DefaultInfo.m_Spirit
end
--体质
function CTstPlayerInfoMgr:GetAddStamina()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_Stamina - self.m_DefaultInfo.m_Stamina
end
--力量
function CTstPlayerInfoMgr:GetAddStrength()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_Strength - self.m_DefaultInfo.m_Strength
end
--防御
function CTstPlayerInfoMgr:GetAddShield()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()].m_Shield - self.m_DefaultInfo.m_Shield
end

function CTstPlayerInfoMgr:GetNewInfo()
	return g_PlayerInfos[g_MainPlayer:GetEntityID()]
end

function CTstPlayerInfoMgr:GetDefaultInfo()
	return self.m_DefaultInfo
end
--这里采用了比较恶心的设计，因为需要判断下线再上线后的人物数字是否正确
--所有新的数据都通过实时用g_MainPlayer:GetEntityID()来获得，而
--上次登录的人物id和这次的人物id不一样，而旧的数据一直保存在self.m_DefaultInfo中
--通过差值来判断人物的属性是因为人物的基本属性可能策划会一直调整。通过装备获得的属性
--就只能用差值的方式了
function CTstPlayerInfoMgr:Ctor()
	self.m_DefaultInfo = g_PlayerInfos[g_MainPlayer:GetEntityID()]
end
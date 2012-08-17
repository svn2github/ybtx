#pragma once

// 存储Fighter的属性的BaseValue
class CStaticAttribs
{
public:
	//--------------------------------------------------------------------------------//
	//		基础属性																  //
	//--------------------------------------------------------------------------------//
	uint32 m_MaxHealthPoint;				// 最高血量
	uint32 m_MaxManaPoint;					// 最高蓝量
	uint32 m_MaxEnergyPoint;				// 最大能量
	uint32 m_MaxRagePoint;					// 最大怒气 
	uint32 m_MaxComboPoint;					// 最大连击

	float m_HPUpdateRate;					// 回血率( N%*上限/秒 )
	float m_MPUpdateRate;					// 回魔率( N%*上限/秒 )
	uint32 m_RPUpdateValue;					// 减怒值( N/秒 )
	uint32 m_EPUpdateValue;					// 回能值( N/秒 )
	float m_RPProduceRate;					// 怒气产生系数

	float m_RevertPer;						// 战斗状态回复速度 = m_RevertPer * 非战斗状态回复速度

	float m_RunSpeed;						// 移动速度
	float m_WalkSpeed;						// 行走速度

	//--------------------------------------------------------------------------------//
	//		物理攻击效果属性（未命中、物理躲避、招架、格挡、暴击、命中）			  //
	//--------------------------------------------------------------------------------//
	int32 m_PhysicalDodgeValue;			// 物理伤害躲闪值，计算出普攻躲闪率的东西	
	int32 m_ParryValue;					// 招架值，算出招架率的东西
	uint32 m_StrikeValue;					// 爆击值，计算出爆击率的东西
	uint32 m_AccuratenessValue;				// 精准值

	//--------------------------------------------------------------------------------//
	//		法术攻击效果属性（法术躲避、完全抵抗、抵抗、暴击、命中）				  //
	//--------------------------------------------------------------------------------//
	int32 m_MagicDodgeValue;				// 法术躲闪值，计算出魔法躲闪率的东西	
	uint32 m_MagicHitValue;					// 攻击方的法术命中值，用于抵消防御方的法术躲避值

	//--------------------------------------------------------------------------------//
	//		物理攻击伤害计算														  //
	//--------------------------------------------------------------------------------//
	uint32 m_PhysicalDPS;					// 攻擊力

	int32  m_Defence;						// 护甲值
	uint32 m_StrikeMultiValue;				// 爆击倍数值，计算出爆击伤害倍数值的东西
	int32 m_ResilienceValue;				// 韧性值，抵消爆击伤害倍数的东西
	int32 m_StrikeResistanceValue;		//免暴值

	//--------------------------------------------------------------------------------//
	//		法术攻击伤害计算														  //
	//--------------------------------------------------------------------------------//
	uint32 m_MagicDamageValue;				// 全系法伤，乘以法伤加成系数后得出伤害值

	int32 m_NatureResistanceValue;			// 自然抗性
	int32 m_DestructionResistanceValue;	// 破坏抗性
	int32 m_EvilResistanceValue;			// 黑暗抗性

	uint32 m_ValidityCoefficient;			// 有效性系数

	uint32 m_NatureSmashValue;				//自然碾压值
	uint32 m_DestructionSmashValue;			//破坏碾压值
	uint32 m_EvilSmashValue;				//暗黑碾压值
	uint32 m_DefenceSmashValue;				//护甲碾压值
};

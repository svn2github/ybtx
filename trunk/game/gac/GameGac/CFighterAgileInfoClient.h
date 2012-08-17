#pragma once
#include "CFighterBaseInfo.h"
#include "AgilePropertyClient.h"


template<typename ImpClass>
class TPropertyHolder;

//只保存玩家看得见的数据,例如血/蓝
class CFighterAgileInfo 
	:public CFighterBaseInfo
{
public:

	typedef		CHealthPoint	HealthPoint_t;
	typedef		CManaPoint		ManaPoint_t;
	typedef		CComboPoint		ComboPoint_t;
	typedef		CRagePoint		RagePoint_t;
	typedef		CEnergyPoint	EnergyPoint_t;	

	CHealthPoint		m_HealthPoint;
	CManaPoint			m_ManaPoint;
	CComboPoint			m_ComboPoint;
	CRagePoint			m_RagePoint;
	CEnergyPoint		m_EnergyPoint;

	void Reset();

protected:
	CFighterAgileInfo();

};
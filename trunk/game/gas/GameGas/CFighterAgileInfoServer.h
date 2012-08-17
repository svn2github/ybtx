#pragma once
#include "CFighterBaseInfo.h"
#include "AgilePropertyServer.h"

// 服务端角色易变属性
class CFighterAgileInfoServer
	:public CFighterBaseInfo
{
public:
	typedef		CHealthPointServer		HealthPoint_t;
	typedef		CManaPointServer		ManaPoint_t;
	typedef		CComboPointServer		ComboPoint_t;
	typedef		CRagePointServer		RagePoint_t;
	typedef		CEnergyPointServer		EnergyPoint_t;

	CHealthPointServer		m_HealthPoint;
	CManaPointServer		m_ManaPoint;
	CComboPointServer		m_ComboPoint;
	CRagePointServer		m_RagePoint;
	CEnergyPointServer		m_EnergyPoint;
};

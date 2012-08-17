#pragma once
#include "GameDef.h"
#include "CStaticAttribs.h"

// 管理(根据等级&职业计算)Fighter的属性的BaseValue
class CStaticAttribMgr
{
public:
	static CStaticAttribMgr& Inst();

	static uint32 CalcBasicMaxHealthPoint(EClass eClass,uint32 uLevel);
	static uint32 CalcBasicMaxManaPoint(EClass eClass,uint32 uLevel);

	void InitAllClassStaticAttrib();
	CStaticAttribs& GetStaticAttribs(EClass eClass, uint32 uLevel);

private:
	CStaticAttribMgr();
	void InitStaticAttribs(EClass eClass, uint32 uLevel);
	void InitBaseAttribs(EClass eClass, uint32 uLevel);
	CStaticAttribs	m_aryBasicAttrValue[eMaxLevel + 1][eCL_Count];
	bool			m_bStaticAttribsInited[eMaxLevel + 1][eCL_Count];
};


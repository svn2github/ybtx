#include "stdafx.h"
#include "CTempVarMgr.h"
#include "CCfgCalcOptrTable.h"

set<string>	CTempVarMgr::m_setName;				
CTempVarMgr::SET_VAR_PTR_FUNC CTempVarMgr::m_pFuncSetVarPtr = NULL;

bool CTempVarMgr::__init = CTempVarMgr::Init();

bool CTempVarMgr::Init()
{
	//注意：如果名字中有字母则必须用小写
	m_setName.insert("伤害");
	m_setName.insert("被伤害");
	m_setName.insert("未变更伤害");
	m_setName.insert("被未变更伤害");
	m_setName.insert("减蓝");
	m_setName.insert("被减蓝");
	m_setName.insert("治疗");
	m_setName.insert("被治疗");
	m_setName.insert("未变更治疗");
	m_setName.insert("被未变更治疗");
	m_setName.insert("魔法操作计算值");
	return true;
}

bool CTempVarMgr::SetVarPtr(UUnitValue* pVarPtr, const string& sName)
{
	if(m_pFuncSetVarPtr)
	{
		return m_pFuncSetVarPtr(pVarPtr, sName);
	}
	else
	{
		//类成员指针问题2010-12-21
		//pVarPtr->pOff = NULL;
		pVarPtr->lng = 0;
		return false;
	}
}




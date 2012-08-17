#include "stdafx.h"
#include "CBaseUnit.h"

#include "IApplication.h"
#include "EditorExp.h"

void UpdateTime(AutoDataHelper* pBaseUnit)
{
	((CBaseUnit*)pBaseUnit)->SetTick();
}

CBaseUnit::CBaseUnit(CEditContext* pContext)
{
	m_pModelPool = NULL;
	m_pContext = pContext;
	_RegCtrlData(E_INTEGER,UpdateTime,this,"¼ä¸ôºÁÃë",NULL,&m_pCyc);
}

CBaseUnit::~CBaseUnit()
{
	SetTick(0);
}

void sqr_tools::CBaseUnit::SetTick( uint32 Tick )
{
	if( Registered() && Tick == this->GetInterval() )
		return;

	if(Registered())
		IApplication::GetInst()->UnRegisterTick(this);

	if(Tick)
	{
		if(Tick != DEFAULT_CYC)
			m_pCyc = Tick;
		IApplication::GetInst()->RegisterTick(this,m_pCyc);
	}
}

ModelPool* CBaseUnit::GetModelPool( void )
{
	 return m_pModelPool;
}

void CBaseUnit::SetModelPool( ModelPool* pPool )
{
	m_pModelPool = pPool;
}

void CBaseUnit::OnTick( void )
{
	EDITOR_TRY
	{	
		_OnTick();
	}
	EDITOR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	EDITOR_TRY_END;
}

void CBaseUnit::_OnTick()
{

}


CBaseUnit::NamePool			CBaseUnit::st_NamePool;
CBaseUnit::UnitCreateMap	CBaseUnit::st_CreateMap;
const char*		CBaseUnit::GetUnitName(unsigned int idx)
{
	if(idx < st_NamePool.size())
		return st_NamePool[idx].c_str();

	return NULL;
}

CBaseUnit*		CBaseUnit::CreateUnit(const char* uName , CEditContext* pContext)
{
	UnitCreateMap::iterator it = st_CreateMap.find(uName);
	if( it != st_CreateMap.end() )
	{
		return (*it->second)(pContext);
	}
	return NULL;
}

void	CBaseUnit::DestroyUnit(CBaseUnit* pUnit)
{
	delete pUnit;
}
//--------------------------------------------------
#include "CCreateObjUnit.h"
#include "CChangePieceUnit.h"
#include "CChgAniGrpUnit.h"
#include "CEfxTestUnit.h"
#include "CChangeSceneUnit.h"



#define REG_TEST_UNIT(class_name) \
	st_CreateMap.insert(UnitCreateMap::value_type(class_name::st_strName,class_name::CreateNewUnit)); \
	st_NamePool.push_back(class_name::st_strName);


size_t CBaseUnit::InitUnit(void)
{
	REG_TEST_UNIT(CCreateObjUnit);
	REG_TEST_UNIT(CChangePieceUnit);
	REG_TEST_UNIT(CChgAniGrpUnit);
	REG_TEST_UNIT(CEfxTestUnit);
	REG_TEST_UNIT(CChangeSceneUnit);
	return st_NamePool.size();
}


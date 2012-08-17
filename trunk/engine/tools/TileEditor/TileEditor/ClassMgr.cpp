#include "StdAfx.h"
#include "classmgr.h"
#include "Module.h"
#include "Property.h"
#include "ServerObj.h"

typedef ClassDef** (*ObjectDLLSetupFn)(int * nDefs, int * version);

CClassMgr::CClassMgr( ) : m_nClassTotalCount(0),
						  m_nClassBind2MapCount(0),
						  m_pClassDefs(NULL),
						  m_nVersion(0)
{
}

CClassMgr::~CClassMgr() { }

void CClassMgr::VerifyClassDefProperties()
{
	int			i, j;
	PropDef *	pProp;

	for (i=0; i < m_nClassTotalCount; i++)
	{
		for (j=0; j < m_pClassDefs[i]->m_nProps; j++)
		{
			pProp = &m_pClassDefs[i]->m_Props[j];

			if (pProp->m_PropType >= NUM_PROPERTYTYPES || pProp->m_PropType < 0)
				pProp->m_PropType = NUM_PROPERTYTYPES;
		}
	}
}

void CClassMgr::Load(std::string strFileName)
{
	m_module.Bind(strFileName.c_str());
	
	ObjectDLLSetupFn	theFunction;
	
	// get the function.
	theFunction = (ObjectDLLSetupFn)m_module.GetFunctionPointer("ObjectDLLSetup");
	
	if ( !theFunction )
	{
		LogConsole("导入" << strFileName.c_str() << "失败");
	}
	
	m_pClassDefs = theFunction((int*)&m_nClassTotalCount, &m_nVersion);
	
	if ( m_nVersion != _OBJ_VERSION )
	{
		LogConsole("DLL版本不匹配");
	}
	
	VerifyClassDefProperties();
	StatNumClassBind2Map();
}

int CClassMgr::GetNumClassDefs()
{
	return m_nClassTotalCount;
}

ClassDef** CClassMgr::GetClassDefs()
{
	return m_pClassDefs;
}

int CClassMgr::GetNumClassBind2Map()
{
	return m_nClassBind2MapCount;
}

int CClassMgr::FindClassIdx(const char* pClassName)
{
	for( int i = 0; i < m_nClassTotalCount; i++)
	{
		if (strcmp(m_pClassDefs[i]->m_ClassName, pClassName) == 0)
			return i;
	}
	return INVALID;
}

ClassDef * CClassMgr::FindClass(const char * pClassName)
{
	int i;

	for ( i=0; i < m_nClassTotalCount; i++ )
		if (strcmp(m_pClassDefs[i]->m_ClassName, pClassName) == 0)
			return m_pClassDefs[i];

	return NULL;
}

PropDef * CClassMgr::FindVar(ClassDef *pClass, const char *pVarName)
{
	int i;

	while (pClass)
	{
		for (i=0; i < pClass->m_nProps; i++)
			if (strcmp(pClass->m_Props[i].m_PropName, pVarName) == 0)
				return &pClass->m_Props[i];

		pClass = pClass->m_ParentClass;
	}

	return NULL;
}

bool CClassMgr::IsClassFlagSet(ClassDef *pClass, const uint32 dwClassFlag)
{
	while (pClass)
	{
		if (pClass->m_ClassFlags & dwClassFlag)
			return true;

		pClass = pClass->m_ParentClass;
	}

	return false;
}

void CClassMgr::StatNumClassBind2Map()
{
	for(int i = 0; i < m_nClassTotalCount; i++)
	{
		if( m_pClassDefs[i]->m_ClassFlags & CF_ATTACHTOMAP )
			m_nClassBind2MapCount++;
	}
}
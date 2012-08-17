#include "stdafx.h"
#include "Brain.h"
#include "ClassMgr.h"
#include "Property.h"
#include "ServerObj.h"

CBrain::~CBrain()
{
	for ( int i = 0; i < m_nPropCnt; ++i )
	{
		SAFE_DELETE(m_vecProps[i]);
	}
}


void CBrain::Init(std::string strClassName,CClassMgr * pClassMgr)
{
	this->AddProps(strClassName,pClassMgr);
	m_nPropCnt = m_vecProps.size();
}

void CBrain::AddProps(std::string strClassName,CClassMgr * pClassMgr)
{
	ClassDef * pClass;
	
	if ( pClass = pClassMgr->FindClass(strClassName.c_str()) )
	{
		for ( int i = 0; i < pClass->m_nProps; i++ )
		{
			CBaseProp * pProp = NULL;
			PropDef prop = pClass->m_Props[i];
			
			if ( prop.m_PropType == PT_REAL )
				pProp = new CRealProp(prop.m_PropName,prop.m_PropFlags,prop.m_DefaultValueFloat);
			else if ( prop.m_PropType == PT_BOOL )
				pProp = new CBoolProp(prop.m_PropName,prop.m_PropFlags,prop.m_DefaultValueFloat);
			else if ( prop.m_PropType == PT_LONGINT )
				pProp = new CIntProp(prop.m_PropName,prop.m_PropFlags,prop.m_DefaultValueFloat);
			else if ( prop.m_PropType == PT_STRING )
				pProp = new CStringProp(prop.m_PropName,prop.m_PropFlags,prop.m_DefaultValueString);
			else if ( prop.m_PropType == PT_BYTE )
				pProp = new CDirProp(prop.m_PropName,prop.m_PropFlags,prop.m_DefaultValueFloat);
			else if ( prop.m_PropType == PT_POS )
				pProp = new CPosProp(prop.m_PropName,prop.m_PropFlags,prop.m_DefaultValuePos);
		
			if ( pProp ) m_vecProps.push_back(pProp);
		}
	
		if ( pClass->m_ParentClass )
			this->AddProps(pClass->m_ParentClass->m_ClassName,pClassMgr);
		return;
	}
}
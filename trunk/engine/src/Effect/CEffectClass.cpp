#include "stdafx.h"
#include "CEffectClass.h"
#include "TSqrAllocator.inl"

EMap< EString, CEffectClass* >*	g_pEffectUnitCreateMap = NULL;
CEffectClassEx*							g_EffectBaseClass[eBaseUnitCount];


IEffectUnitProp* CEffectClassEx::CreateEffectUnitProp( EBaseEffectUnit eBaseType, IEffectClassDesc* pClassDesc )
{
	if( eBaseUnitCount <= eBaseType )
		return NULL;
	CEffectClassEx *pEffectClass = NULL;
	pEffectClass = g_EffectBaseClass[eBaseType];
	Ast(pEffectClass);
	return pEffectClass->m_pEffectUnitPropCreateFun( pClassDesc );
}

const char* CEffectClassEx::GetEffectName( EBaseEffectUnit eBaseType )
{
	if( eBaseType >= eBaseUnitCount )
		return "";

	static EString szEffectName [eBaseUnitCount];
	if( szEffectName[eBaseType].empty() )
		szEffectName[eBaseType] = LoadResString( g_EffectBaseClass[eBaseType]->m_EffectUnitNameResID ).c_str();
	return szEffectName[eBaseType].c_str();
}

CEffectClass* CEffectClassEx::GetEffectClass( EBaseEffectUnit eBaseType )
{
	return eBaseType < eBaseUnitCount ? g_EffectBaseClass[eBaseType] : NULL;
}

CEffectClassEx::CEffectClassEx( EBaseEffectUnit eBaseType, WORD uResID, const char* szClassName, 
							   LPCREATE_EFFECT_FUN pEffectFun, LPCREATE_EFFECTPROP_FUN pEffectPropFun )
							   : CEffectClass( szClassName, pEffectFun, NULL )
{
	g_EffectBaseClass[eBaseType]	= this;
	m_pEffectUnitPropCreateFun		= pEffectPropFun;
	m_EffectUnitNameResID			= uResID;
}

CEffectClass::CEffectClass( const char* pszWndClass, LPCREATE_EFFECT_FUN lpCreatefun, CEffectClass* pParentClass )
{
	m_ClassName		= pszWndClass;
	m_pParent		= pParentClass;
	m_pCreateFun	= lpCreatefun;

	if( !g_pEffectUnitCreateMap )
		g_pEffectUnitCreateMap = new EMap< EString, CEffectClass* >();
	if( g_pEffectUnitCreateMap->find( m_ClassName ) != g_pEffectUnitCreateMap->end() )
		GenErr("注册相同类型的特效");
		
	(*g_pEffectUnitCreateMap)[m_ClassName] = this;
}

CEffectClass::~CEffectClass()
{
	g_pEffectUnitCreateMap->erase( m_ClassName );
	if( g_pEffectUnitCreateMap->size() == 0 )
	{
		delete g_pEffectUnitCreateMap;
		g_pEffectUnitCreateMap = NULL;
	}
}

bool CEffectClass::FindParentClass( const char* szClassName ) const
{
	if( m_ClassName == szClassName )
		return TRUE;
	return m_pParent ? m_pParent->FindParentClass( szClassName ) : FALSE;
}

IEffectUnit* CEffectClass::CreateEffectUnit( IEffectUnitProp* pProp )
{
	const char* pszEffectClass = pProp->GetEffectClassName();
	uint32 eBaseType = pProp->GetID(); 

	EMap< EString, CEffectClass* >::iterator it = g_pEffectUnitCreateMap->find( pszEffectClass );
	if( it == g_pEffectUnitCreateMap->end() || !it->second->FindParentClass( g_EffectBaseClass[eBaseType]->m_ClassName.c_str() ) )
		pszEffectClass = g_EffectBaseClass[eBaseType]->m_ClassName.c_str();

	return (*g_pEffectUnitCreateMap)[pszEffectClass]->m_pCreateFun( pProp );
}

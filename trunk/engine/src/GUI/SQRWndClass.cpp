#include "stdafx.h"
#include "SQRWndclass.h"
#include "SQRWnd.h"
#include "TSqrAllocator.inl"

UIMap< UIString, LPREGISTER_FUN >* SQRWndClass::ms_pWndCreateMap = NULL;

SQRWndClass::SQRWndClass( const char* pszWndClass, LPREGISTER_FUN lpCreatefun, SQRWndClass* pParentClass )
{
	m_ClassName = pszWndClass;
	m_pParent	= pParentClass;

	if( !ms_pWndCreateMap )
		ms_pWndCreateMap = new UIMap< UIString, LPREGISTER_FUN >();
	if( ms_pWndCreateMap->find( m_ClassName ) != ms_pWndCreateMap->end() )
		GenErr("注册相同类型的窗口");

	(*ms_pWndCreateMap)[m_ClassName] = lpCreatefun;
}

SQRWndClass::~SQRWndClass()
{
	ms_pWndCreateMap->erase( m_ClassName );
	if( ms_pWndCreateMap->size() == 0 )
	{
		delete ms_pWndCreateMap;
		ms_pWndCreateMap = NULL;
	}
}

SQRWnd* SQRWndClass::CreateWndObject( const char* pszWndClass )
{
	if( !ms_pWndCreateMap )
		return NULL;
	UIString name = pszWndClass;
	if( ms_pWndCreateMap->find( name ) == ms_pWndCreateMap->end() )
		return NULL;
	LPREGISTER_FUN CreateClassObject = (*ms_pWndCreateMap)[name];
	return CreateClassObject();
}

bool SQRWndClass::FindParentClass( const char* szClassName ) const
{
	if( m_ClassName == szClassName )
		return TRUE;
	return m_pParent ? m_pParent->FindParentClass( szClassName ) : FALSE;
}
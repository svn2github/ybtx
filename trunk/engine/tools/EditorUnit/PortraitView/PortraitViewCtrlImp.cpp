#include "stdafx.h"
#include "PortraitView\PortraitViewCtrlImp.h"
#include "PortraitView\CPortraitView.h"

DEF_UI_IMP(CPortraitViewCtrlImp);
CPortraitViewCtrlImp::CPortraitViewCtrlImp( void* param )
{
	m_pPortraitView = NULL;
}

CPortraitViewCtrlImp::~CPortraitViewCtrlImp( void )
{

}


void CPortraitViewCtrlImp::SetPhotoFrameTexPath( const std::string& path )
{
	m_pPortraitView->m_strPhotoFrame = path;
}

void CPortraitViewCtrlImp::GetPhotoFrameTexPath( std::string& path )
{
	path = m_pPortraitView->m_strPhotoFrame;
}

void CPortraitViewCtrlImp::SetIsShowPhotoFrame( const bool b )
{
	m_pPortraitView->m_bShowFrame = b;
}

bool CPortraitViewCtrlImp::GetIsShowPhotoFrame() const
{
	return m_pPortraitView->m_bShowFrame;
}

void CPortraitViewCtrlImp::SetIsShowPortraitView( const bool b )
{
	m_pPortraitView->m_bVisible = b;
}

bool CPortraitViewCtrlImp::GetIsShowPortraitView() const
{
	return m_pPortraitView->m_bVisible;
}

void CPortraitViewCtrlImp::SetPortraitView( CPortraitView* pPortraitView )
{
	if( pPortraitView == NULL )
		return;

	m_pPortraitView = pPortraitView;
}
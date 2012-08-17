#include "stdafx.h"
#include "CWorldMap.h"

#define MAP_TEXTURE_GROUP	"interface/tex/worldmap%02d.tex"

CWorldMap::CWorldMap(void)
: m_pWnd(NULL)
{
	memset( m_pTexture, 0, sizeof( m_pTexture ) );
}

CWorldMap::~CWorldMap(void)
{
	Clearup();
}

void CWorldMap::Clearup()
{
	for( int i = 0; i < WORLD_MAP_ROW; i++ )
	{
		for( int j = 0; j < WORLD_MAP_COL; j++ )
		{
			SafeRelease( m_pTexture[i][j] );
		}
	}
}

void CWorldMap::InitData(SQRWnd* pWnd)
{
	m_pWnd = pWnd;

	TCHAR szbuf[256];
	for( int i = 0; i < WORLD_MAP_ROW; i++ )
	{
		for( int j = 0; j < WORLD_MAP_COL; j++ )
		{
			sprintf( szbuf, MAP_TEXTURE_GROUP, i*WORLD_MAP_COL + j + 1 );
			m_pWnd->GetGraphic()->CreateTexture( PATH_ALIAS_RES.c_str(), szbuf, &m_pTexture[i][j] );
		}
	}
}

void CWorldMap::Render() 
{
	if(m_pWnd == NULL)
		return;
	for( int i = 0; i < WORLD_MAP_ROW; i++ )
	{
		for( int j = 0; j < WORLD_MAP_COL; j++ )
		{
			CFRect rt;
			rt.left		= m_pWnd->GetWndWidth()/float(WORLD_MAP_COL);
			rt.top		= i*m_pWnd->GetWndHeight()/float(WORLD_MAP_ROW);
			rt.right	= ( j + 1 )*m_pWnd->GetWndWidth()/float(WORLD_MAP_COL);
			rt.bottom	= ( i + 1 )*m_pWnd->GetWndHeight()/float(WORLD_MAP_ROW);

			CFRect TexRect = CFRect(0.5f, 0.5f, m_pTexture[i][j]->GetWidth() - 0.5f, m_pTexture[i][j]->GetHeight() - 0.5f);
			m_pWnd->DrawRect( m_pTexture[i][j], rt, 0xffffffff, &TexRect, false );
		}
	}
}
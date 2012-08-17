#include "stdafx.h"
#include "MagicBall.h"
#include "render.h"
#include "CGraphic.h"
#include "MathAlgorithm.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "CEventState.h"

namespace sqr
{
	DWORD VectortoRGBA( CVector3f * v, FLOAT fHeight )
	{
		DWORD r = (DWORD)( 127.0f * v->x + 128.0f );
		DWORD g = (DWORD)( 127.0f * v->y + 128.0f );
		DWORD b = (DWORD)( 127.0f * v->z + 128.0f );
		DWORD a = (DWORD)( 255.0f * fHeight );

		return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
	}
}

BOOL CMagicBall::Create(const TCHAR * pszName,const RECT * pRect,DWORD dwStyle,CBlyWnd *pParent)
{
	CBlyWnd::Create(pszName, pRect, dwStyle,pParent);

	CGraphic::GetInst()->CreateEmptyTexture(128, 128, 1, TF_ARGB32, &m_pITexture);

	SMagicBlock * pBlock = &m_clrDB;

	for ( DWORD j = 0; j < MAGICBALL_SIZE; j++  )
	{
		for ( DWORD i = 0; i < MAGICBALL_SIZE; i++ )
		{
			FLOAT x = ( 2.0f * (i / (float)(MAGICBALL_SIZE-1.0f)) - 1.0f );
			FLOAT z = -( 2.0f * (j / (float)(MAGICBALL_SIZE-1.0f)) - 1.0f ) ;
			FLOAT y = sqrtf( 1.0f - x*x - z*z );

			if ( (x*x + z*z) <= 1.0f )
			{
				CVector3f vVector( x, y, z );
				pBlock->clrBranch[j*MAGICBALL_SIZE+i] = VectortoRGBA( &vVector, 1.0f );
			}
			else
				pBlock->clrBranch[j*MAGICBALL_SIZE+i] = 0x80808080;
		}
	}

	m_pITexture->FillFromMemory(pBlock->clrBranch, TF_ARGB32, MAGICBALL_SIZE, pBlock->rc, pBlock->rc);

	m_vLight = CVector3f(0.0f,-1.0f,0.0f);

	return true;
}

void CMagicBall::Render()
{
	if ( this->GetStyle() & BWS_VISIBLE )
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();
		Ast(pRender);

		DWORD dwFactor = VectortoRGBA( &m_vLight, 0.0f );

		RECT & rc = m_wndInfo.rcRect;

		int iX = rc.left;
		int iY = rc.top;
		int iW = rc.right - rc.left;
		int iH = rc.bottom - rc.top;

		pRender->DrawLayer2DSolidQuad(&m_pRS, iX, iY, MAGICBALL_SIZE, MAGICBALL_SIZE, 0xffffffff, m_pITexture);

		m_pRS->m_Texturefactor = dwFactor;
		m_pRS->m_TexArg1_S0 = TA_TEXTURE;
		m_pRS->m_TexArg2_S0 = TA_TFACTOR;
		m_pRS->m_TexOP_S0 = TOP_DOTPRODUCT3;

		m_pRS->m_AlphaBlendEnable = true;
		m_pRS->m_SrcBlend = BLEND_SRCALPHA;
		m_pRS->m_DestBlend = BLEND_INVSRCALPHA;

		pRender->DrawLayer2DWireQuad(iX-1, iY-1, iW+1, iH+1, 0x40555555);
		pRender->DrawLayer2DWireQuad(iX-2, iY-2, iW+2, iH+2, 0x40555555);
	}
}

void CMagicBall::Update(const CMsg& msg)
{
	POINT ptMousePos;
	MSG_MOUSE_INF inf = CEventState::GetInst()->GetMousePosition();
	ptMousePos.x = inf.X;
	ptMousePos.y = inf.Y;

	DWORD & dwStyle = m_wndInfo.dwStyle;

	if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE))
	{
		BOOL bMouseOver = ( PtInRect(&ptMousePos) > 0 );

		if ( bMouseOver )
		{
			switch ( msg.msgSource )
			{
			case EUIMT_MS_MOVE:
				{
					int x,y;
					HitOffToCenter(x, y,&ptMousePos);
					m_vLight.x = ( 2.0f * x / MAGICBALL_SIZE );
					m_vLight.z = ( 2.0f * -y / MAGICBALL_SIZE );
					m_vLight.y = 0.0f;

					if ( m_vLight.Mag() > 1.0f )
					{
						m_vLight.normalize();
					}
					else
					{
						m_vLight.y = sqrtf( 1.0f - m_vLight.x*m_vLight.x - m_vLight.z*m_vLight.z );
					}
					CSceneEnvMgr::GetInst()->SetDirDir(-m_vLight);
				}
				break;

			case EUIMT_MS_BTNDOWN:
				{
					this->ShowWnd(false);
					//CMapEditApp::GetInst()->GetOperator()->SetIsSettingSunDir(false);
				}
				break;
			}
		}
	}
}

CVector3f* CMagicBall::getLightDir()
{
	return &m_vLight;
}

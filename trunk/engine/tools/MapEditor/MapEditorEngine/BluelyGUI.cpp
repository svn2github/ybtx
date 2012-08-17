#include "stdafx.h"
#include "BluelyGUI.h"
#include "render.h"
#include "engine.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "CMapEditApp.h"
#include "Picker.h"
#include "TerrainMesh.h"
#include "CTexture.h"
#include "CGraphic.h"
#include "ModelGroup.h"
#include "BaseHelper.h"
#include "CEventState.h"



namespace sqr
{
	CBlyWnd::CBlyWnd()
	{
		m_pParent = NULL;
		ZeroMemory(&m_wndInfo,sizeof(SWndInfo));

		m_wndInfo.colorBack =  BWC_BACK;
		m_wndInfo.colorText =	BWC_TEXT;

		m_wndInfo.colorLow  =   BWC_LOW;
		m_wndInfo.colorMiddle = BWC_MIDDLE;
		m_wndInfo.colorHigh   = BWC_HIGH;
	}

	CBlyWnd::~CBlyWnd()
	{
		size_t nSize = m_listChild.size();
		list<CBlyWnd *>::iterator iter = m_listChild.begin();
		for ( iter; iter != m_listChild.end(); ++iter )
		{
			if ( *iter )
				*iter = NULL;
		}

		if ( m_pParent )
			m_pParent = NULL;
	}

	void CBlyWnd::Update(const CMsg& msg)
	{
		UpdateDefault();

		DWORD & dwStyle = m_wndInfo.dwStyle;
		BOOL  & bActive = m_wndInfo.bActive;

		if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE) )
		{
			UpdateChild(msg);
		}
	}

	void CBlyWnd::Render()
	{
		DWORD &dwStyle = m_wndInfo.dwStyle;
		if (dwStyle & BWS_VISIBLE)
		{
			RenderDefault();
			RenderChild();
		}
	}

	void CBlyWnd::UpdateDefault()
	{
	}

	void CBlyWnd::SizeWnd(int iCX, int iCY)
	{
		RECT & rc = m_wndInfo.rcRect;
		rc.right  += iCX;
		rc.bottom += iCY;
	}

	void CBlyWnd::UpdateChild(const CMsg& msg)
	{
		for ( list<CBlyWnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); ++it )
		{
			CBlyWnd * pWnd = *it;
			pWnd->Update(msg);
		}
	}

	void CBlyWnd::AddChild(CBlyWnd * pWnd)
	{
		if ( pWnd->GetStyle() & BWS_TOPMOST )
			m_listChild.push_front(pWnd);
		else
		{
			list<CBlyWnd *>::iterator it;
			for ( it = m_listChild.begin(); it != m_listChild.end(); ++it )
			{
				CBlyWnd * p = *it;
				if ( !(p->GetStyle() & BWS_TOPMOST) )
				{
					m_listChild.insert(it,pWnd);
					break;
				}
			}
			if ( it == m_listChild.end() )
			{
				m_listChild.push_back(pWnd);
			}
		}
	}

	void CBlyWnd::RemoveChild(CBlyWnd * pWnd)
	{
		m_listChild.remove(pWnd);
	}

	void CBlyWnd::HitTest(const POINT * pPoint, CBlyWnd ** ppWnd)
	{
		if ( m_wndInfo.dwStyle & BWS_DISABLE )
		{
			return;
		}

		if ( *ppWnd ) return;

		if ( m_wndInfo.dwStyle & BWS_VISIBLE )
		{
			for ( list<CBlyWnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); ++it )
			{
				CBlyWnd * pWnd = *it;
				pWnd->HitTest(pPoint,ppWnd);
				if (*ppWnd) return;
			}

			if (PtInRect(pPoint) != BWH_OUTSIDE)
			{
				*ppWnd = this;
			}
		}
	}

	BOOL CBlyWnd::Create(const TCHAR * pszName, const RECT * pRect, DWORD dwStyle, CBlyWnd * pParent)
	{
		Ast(pszName);
		Ast(pRect);

		strncpy(m_wndInfo.szName, pszName, 255);
		m_wndInfo.szName[255] = 0;

		m_wndInfo.dwStyle = dwStyle;

		m_pParent = pParent;

		if ( pParent )
		{
			RECT  rc;
			pParent->GetWndRect(&rc);

			m_wndInfo.rcRect.left = rc.left + pRect->left;
			m_wndInfo.rcRect.top  = rc.top  + pRect->top;
			m_wndInfo.rcRect.right= m_wndInfo.rcRect.left + (pRect->right-pRect->left);
			m_wndInfo.rcRect.bottom = m_wndInfo.rcRect.top  + (pRect->bottom-pRect->top);

			pParent->AddChild(this);
		}
		else
			memcpy(&m_wndInfo.rcRect,pRect,sizeof(RECT));

		return TRUE;
	}


	void CBlyWnd::GetWndRect(RECT * pRect)
	{
		memcpy(pRect, &m_wndInfo.rcRect, sizeof(RECT));
	}

	void CBlyWnd::SetWndRect(RECT *pRect)
	{
		memcpy(&m_wndInfo.rcRect, pRect, sizeof(RECT));
	}

	void CBlyWnd::Destroy()
	{
		if (m_pParent)
			m_pParent->RemoveChild(this);
	}

	int CBlyWnd::PtInRect(const POINT * pPoint)
	{
		if ( !(m_wndInfo.dwStyle & BWS_VISIBLE) ) return BWH_OUTSIDE;

		RECT & rc = m_wndInfo.rcRect;
		int x = pPoint->x;
		int y = pPoint->y;
		if ( x < rc.left || y < rc.top || x > rc.right || y > rc.bottom ) return BWH_OUTSIDE;
		if ( x > rc.left + BWA_FRAMESIZE && x < rc.right - BWA_FRAMESIZE && y > rc.top + BWA_FRAMESIZE && y < rc.bottom - BWA_FRAMESIZE ) return BWH_INSIDE;
		return BWH_INTERSECT;
	}

	void CBlyWnd::HitOffToCenter(int & x, int & y, const POINT * pPoint)
	{
		RECT & rc = m_wndInfo.rcRect;
		x = pPoint->x - (rc.right + rc.left)/2;
		y = pPoint->y - (rc.bottom + rc.top)/2 ;
	}

	void CBlyWnd::MoveWnd(int iOX, int iOY)
	{
		RECT & rc = m_wndInfo.rcRect;
		rc.left += iOX;
		rc.top  += iOY;
		rc.right += iOX;
		rc.bottom += iOY;

		for ( list<CBlyWnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); ++it )
		{
			CBlyWnd * p = *it;
			p->MoveWnd(iOX,iOY);
		}
	}

	void CBlyWnd::ShowWnd(BOOL bShow)
	{
		if ( bShow )
			m_wndInfo.dwStyle |= BWS_VISIBLE;
		else
			m_wndInfo.dwStyle &= ~BWS_VISIBLE;
	}

	void CBlyWnd::MoveWndTo(int iX, int iY)
	{
		int iOX = iX - m_wndInfo.rcRect.left;
		int iOY = iY - m_wndInfo.rcRect.top;

		MoveWnd(iOX,iOY);
	}

	void CBlyWnd::CenterWndTo(int iX, int iY)
	{
		DWORD iW = m_wndInfo.rcRect.right - m_wndInfo.rcRect.left + 1;
		DWORD iH = m_wndInfo.rcRect.bottom- m_wndInfo.rcRect.top + 1;

		int iOX = iX - m_wndInfo.rcRect.left - iW / 2;
		int iOY = iY - m_wndInfo.rcRect.top  - iH / 2;

		MoveWnd(iOX,iOY);
	}

	void CBlyWnd::RenderDefault(void)
	{
		DWORD & dwStyle = m_wndInfo.dwStyle;
	}

	void CBlyWnd::RenderChild(void)
	{
		DWORD & dwStyle = m_wndInfo.dwStyle;
		if ( !(dwStyle & BWS_VISIBLE) ) return ;

		for ( list<CBlyWnd *>::reverse_iterator it = m_listChild.rbegin(); it != m_listChild.rend(); ++it )
		{
			CBlyWnd * pWnd = *it;
			pWnd->Render();
		}
	}

	void CBlyWnd::ModifyStyle(DWORD dwAdd, DWORD dwRemove)
	{
		DWORD & dwStyle = m_wndInfo.dwStyle;
		dwStyle |= dwAdd;
		dwStyle &= ~dwRemove;
	}

	BOOL CBlyWnd::IsActive()
	{
		return m_wndInfo.bActive;
	}

	DWORD CBlyWnd::GetStyle()
	{
		return m_wndInfo.dwStyle;
	}

	void CBlyWnd::SetStyle( DWORD dwStyle )
	{
		m_wndInfo.dwStyle = dwStyle;
	}

	void CBlyWnd::SetBkColor( D3DCOLOR dwColor )
	{
		m_wndInfo.colorBack = dwColor;
	}

	void CBlyWnd::SetTextColor( D3DCOLOR dwColor )
	{
		m_wndInfo.colorBack = dwColor;
	}

	//rtt
	CRTTTextureBox::CRTTTextureBox() : m_pITexture(NULL)
	{
	}

	CRTTTextureBox::~CRTTTextureBox()
	{
		if ( m_pITexture )
			SafeRelease(m_pITexture);
	}

	BOOL CRTTTextureBox::Create(const TCHAR * pszName,const RECT * pRect,DWORD dwStyle,CBlyWnd *pParent)
	{
		CBlyWnd::Create(pszName, pRect, dwStyle,pParent);

		m_pITexture = CGraphic::GetInst()->CreateRenderTarget();
		m_pITexture->SetWidth(512);
		m_pITexture->SetHeight(512);
		m_pITexture->Create();

		return 1;
	}

	void CRTTTextureBox::DrawRTTTexture()
	{
		if ( !m_pITexture )
			return;
	}

	void CRTTTextureBox::Render()
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();
		Ast(pRender);

		if ( this->GetStyle() & BWS_VISIBLE )
		{

			RECT & rc = m_wndInfo.rcRect;

			int iX = rc.left;
			int iY = rc.top;
			int iW = rc.right - rc.left;
			int iH = rc.bottom - rc.top;

			pRender->DrawLayer2DSolidQuad(iX, iY, iW, iH, 0xffffffff, m_pITexture);
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// checkbox
	//////////////////////////////////////////////////////////////////////////

	int CCheckBox::ms_iIDMother = 0;
	int CCheckBox::ms_iHotID = INVALID;

	CCheckBox::CCheckBox()
	{
		m_bChecked = false;
		m_iID = ms_iIDMother++;
		m_bHover = false;
		m_pTex = NULL;
	}

	CCheckBox::~CCheckBox()
	{
		SafeRelease(m_pTex);
	}

	void CCheckBox::SetTextureName(vector<string> & vTextureNames)
	{
		if ( vTextureNames.empty() )
			return;

		m_vecTextureNames = vTextureNames;

		m_pTex =  CRenderDevice::GetInst()->GetTexture(vTextureNames[0]);
	}

	void CCheckBox::Render()
	{
		CRenderDevice *pRender = CRenderDevice::GetInst();
		Ast(pRender);

		DWORD & dwStyle = m_wndInfo.dwStyle;
		if ( dwStyle & BWS_VISIBLE )
		{
			CBlyWnd::RenderDefault();

			RECT & rc = m_wndInfo.rcRect;

			int iX = rc.left, iY = rc.top, iW = rc.right - rc.left + 1, iH = rc.bottom - rc.top + 1;

			int iTextStartX = iX + iW/2;
			int iTextStartY = iY + iH/2;

			// shadow
			if ( ms_iHotID != INVALID && m_iID == ms_iHotID )
			{
				m_bChecked = true;
				if ( !m_vecTextureNames.empty() )
				{
					pRender->DrawLayer2DSolidQuad(iX-2, iY - 2, iW + 4, iH + 4, 0xffffffff, m_pTex, NULL);
				}
				pRender->DrawLayer2DWireQuad(iX-3, iY - 3, iW + 6, iH + 6, m_wndInfo.colorHigh);
				pRender->DrawLayer2DWireQuad(iX-2, iY - 2, iW + 4, iH + 4, m_wndInfo.colorHigh);
				pRender->DrawLayer2DWireQuad(iX-1, iY - 1, iW + 2, iH + 2, m_wndInfo.colorHigh);

				m_bHover = false;
			}
			else
			{
				m_bChecked = false;

				if ( !m_vecTextureNames.empty()  )
				{
					pRender->DrawLayer2DSolidQuad(iX, iY, iW, iH, 0xffffffff, m_pTex);
				}
			}

			if ( m_bHover )
			{
				pRender->DrawLayer2DWireQuad(iX - 2, iY - 2, iW + 4, iH + 4,m_wndInfo.colorHigh);
			}
		}
	}

	void CCheckBox::Update(const CMsg& msg)
	{
		CBlyWnd::UpdateDefault();

		POINT ptMousePos;
		MSG_MOUSE_INF inf = CEventState::GetInst()->GetMousePosition();
		ptMousePos.x = inf.X;
		ptMousePos.y = inf.Y;

		DWORD & dwStyle = m_wndInfo.dwStyle;
		BOOL  & bActive = m_wndInfo.bActive;

		if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE))
		{
			BOOL bMouseOver = ( PtInRect(&ptMousePos) > 0 );

			if ( bMouseOver )
				m_bHover = true;
			else
				m_bHover = false;

			if ( bMouseOver && m_pParent->IsActive() || bActive )
			{
				switch ( msg.msgSource )
				{
				case EUIMT_MS_BTNDOWN:
					if ( msg.msgInfo.key_info == MK_LBUTTON )
					{
						ms_iHotID = m_iID;
						m_bClicked = true;
					}
					break;

				default:
					break;
				}
			}
		}
	}

	bool CCheckBox::IsClicked()
	{
		bool bClicked = m_bClicked;
		m_bClicked = false;
		return bClicked;
	}

	void CCheckBox::SetChecked(bool bFlag)
	{
		m_bChecked = bFlag;
	}

	vector<string> & CCheckBox::GetTextureName()
	{
		return m_vecTextureNames;
	}

	void CTitleTextureBox::Render()
	{
		if (  !(this->GetStyle() & BWS_VISIBLE) )
		{
			return;
		}
		CRenderDevice * pRender = CRenderDevice::GetInst();
		Ast(pRender);

		RECT & rc = m_wndInfo.rcRect;

		int iX = rc.left;
		int iY = rc.top;
		int iW = rc.right - rc.left;
		int iH = rc.bottom - rc.top;

		//pLayer2D->DrawTextureSubRect(iX,iY,&m_rcPVS,m_dwTextureHandle);
		pRender->DrawLayer2DSolidQuad(iX, iY, iW, iH, 0xffffffff, m_strTextureName);
	}

	string CTitleTextureBox::GetTextureName()
	{
		return m_strTextureName;
	}

	void CTitleTextureBox::SetTextureName( string & name )
	{
		m_strTextureName = name;
	}

	BOOL CMiniMap::Create(const TCHAR * pszName,const RECT * pRect,DWORD dwStyle,CBlyWnd *pParent)
	{
		CBlyWnd::Create(pszName, pRect, dwStyle,pParent);

		CGraphic::GetInst()->CreateEmptyTexture(1024, 1024, 1, TF_ARGB32, &m_pITexture);

		RECT desRect = { 0, 0, 1024, 1024 };
		RECT srcrect = { 0, 0, 1024, 1024 };

		DWORD dwBitmapSize = 1024 * 1024;
		m_pBitmap = new DWORD[dwBitmapSize];

		if ( !m_pBitmap )
		{
			return false;
		}

		DWORD * pDest = m_pBitmap;
		DWORD dwSize = dwBitmapSize;

		__asm
		{
			mov edi,[pDest]
			mov eax,BWC_BACK
				mov ecx,dwSize
				cld
				rep stosd
		}

		//m_pITexture->FillFromMemory(m_pBitmap, TF_ARGB32, 1024, srcrect, desRect);

		//DWORD dwEffectColor  = 0xffff0000;
		//RECT ta = {0, 0, 1, 1};
		//RECT tb = {32, 993, 38, 999};
		//m_pITexture->FillFromMemory(&dwEffectColor, TF_ARGB32, 0, ta, tb);


		for ( int y = 0; y < MINIMAP_BLOCK_SIZE; ++y )
		{
			for ( int x = 0; x < MINIMAP_BLOCK_SIZE; ++x )
			{
				SMiniBlock * pBlock = &m_clrDB[y*MINIMAP_BLOCK_SIZE+x];
				pBlock->rcDest.left = x * MINIMAP_BLOCK_GRID_SIZE;
				pBlock->rcDest.top = (MINIMAP_BLOCK_SIZE - y - 1) * MINIMAP_BLOCK_GRID_SIZE;
				pBlock->rcDest.right = pBlock->rcDest.left + MINIMAP_BLOCK_GRID_SIZE;
				pBlock->rcDest.bottom = pBlock->rcDest.top +  MINIMAP_BLOCK_GRID_SIZE;

				for ( int i = 0; i < MINIMAP_BLOCK_GRID_SIZE*MINIMAP_BLOCK_GRID_SIZE; ++i )
				{
					pBlock->clrBranch[i] = BWC_BACK;
				}
			}
		}

		return true;
	}

	void CMiniMap::Render()
	{
		if (  !(this->GetStyle() & BWS_VISIBLE) )
		{
			return;
		}
		CRenderDevice * pRender = CRenderDevice::GetInst();
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if(pTerrainMesh==NULL)
			return;
		Ast(pRender);
		CIRect	rc;
		CMainWindowTarget::GetInst()->GetRect(rc);
		//RECT & rc = ;m_wndInfo.rcRect;
		float w = (float)(pTerrainMesh->GetWidth());
		float h = (float)(pTerrainMesh->GetDepth());
		CFRect rect;
		int iX = rc.right -  m_rcPVS.right + m_rcPVS.left -10 ;
		int iY = rc.top;
		int iW = rc.right - rc.left;
		int iH = m_rcPVS.bottom - m_rcPVS.top;

		rect.left = 0.0f;
		rect.right = w/1024.0f;
		rect.top = (1024-h)/1024.0f;
		rect.bottom = 1.0f;


		pRender->DrawLayer2DSolidQuad(iX, iY, m_rcPVS.right - m_rcPVS.left+10, m_rcPVS.bottom- m_rcPVS.top, 0xffffffff, m_pITexture,&rect);
		pRender->DrawLayer2DWireQuad(iX-1, iY-1, iW+11, iH+1, 0x40555555);
		pRender->DrawLayer2DWireQuad(iX-2, iY-2, iW+12, iH+2, 0x40555555);

		//渲染导航里面的小方块
		pRender->DrawLayer2DWireQuad(iX+(int)((m_ptCenter.x*MINIMAP_SCREEN_SIZE)/w)-REGION_SPACE/2,
			iY+(int)((float)((pTerrainMesh->GetDepth()-m_ptCenter.y)*MINIMAP_SCREEN_SIZE)/h)-REGION_SPACE/2,
			REGION_SPACE, REGION_SPACE, 0xFF0000FF);
	}

	void CMiniMap::RegisterAsDirty(DWORD dwMiniBlockIndex)
	{
		if ( m_clrDB[dwMiniBlockIndex].nDirtyCnt == 0 )
		{
			m_lstDirties.push_back(&m_clrDB[dwMiniBlockIndex]);
		}

		++m_clrDB[dwMiniBlockIndex].nDirtyCnt;
	}

	void CMiniMap::UpdatePixel(DWORD dwMiniBlockIndex, DWORD dwGridIndexInMiniBlock, DWORD dwNewColor)
	{
		m_clrDB[dwMiniBlockIndex].clrBranch[dwGridIndexInMiniBlock] = dwNewColor;
		RegisterAsDirty(dwMiniBlockIndex);
	}

	void CMiniMap::Finalize()
	{
		for ( list<SMiniBlock*>::iterator iter = m_lstDirties.begin(); iter != m_lstDirties.end(); ++iter)
		{
			SMiniBlock * pBlock = *iter;
			m_pITexture->FillFromMemory(pBlock->clrBranch, TF_ARGB32, MINIMAP_BLOCK_GRID_SIZE, pBlock->rcSrc, pBlock->rcDest);
			pBlock->nDirtyCnt = 0;
		}
		//RECT srcrect = { 0, 0, 1024, 1024 };
		//m_pITexture->FillFromMemory(m_pBitmap, TF_ARGB32, 1024, srcrect, srcrect);
		//
		m_lstDirties.clear();
	}

	void CMiniMap::Init()
	{
		for ( int y = 0; y < MINIMAP_BLOCK_SIZE; ++y )
		{
			for ( int x = 0; x < MINIMAP_BLOCK_SIZE; ++x )
			{
				SMiniBlock * pBlock = &m_clrDB[y*MINIMAP_BLOCK_SIZE+x];

				for ( int i = 0; i < MINIMAP_BLOCK_GRID_SIZE*MINIMAP_BLOCK_GRID_SIZE; ++i )
				{
					pBlock->clrBranch[i] = BWC_BACK;
				}
				m_pITexture->FillFromMemory(pBlock->clrBranch, TF_ARGB32, MINIMAP_BLOCK_GRID_SIZE, pBlock->rcSrc, pBlock->rcDest);
			}
		}
	}

	void CMiniMap::RefreshMiniMap( int nMode)
	{
		if ( nMode == 4)
		{
			Init();
			return;
		}

		RECT ta = {0, 0, 1, 1};
		RECT tb;
		tb.left = 4;
		tb.right = 5;
		tb.top = 1024-6;
		tb.bottom = 1024-5;

		CTerrainMesh *pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		int w			= pTerrainMesh->GetWidth();
		int nGridCount  = pTerrainMesh->GetGridCount();

		for ( int i = 0; i < nGridCount; ++i )
		{
			D3DCOLOR dwBlockColor   = 0;
			D3DCOLOR dwObjectColor  = 0xff222222;
			D3DCOLOR dwEffectColor  = 0xffffff00;
			D3DCOLOR bSceneLightColor[4]    = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

			bool bObject = false;
			bool bEffect = false;
			bool bSceneLight = false;

			SGrid & grid = pTerrainMesh->GetGrid(i);
			switch (grid.nBlockType)
			{
			case 1:
				dwBlockColor = 0xe00000ff;
				break;
			case 2:
				dwBlockColor = 0xe000ff00;
				break;
			case 3:
				dwBlockColor = 0xe0ff0000;
				break;
			}

			if (!grid.vecCenterObjects.empty())
			{
				for ( size_t j = 0; j < grid.vecCenterObjects.size(); j++)
				{
					if ( grid.vecCenterObjects[j]->GetModelGroup()->GetIsEffectBased() )
					{
						if ( grid.vecCenterObjects[j]->GetModelGroup()->m_SL.m_bIAmbScenePointLight )
							bSceneLight = true;
						else bEffect = true;
					}
					else bObject = true;
				}
			}

			int dwDeltaXStart = i % w;
			int dwDeltaZStart = i / w;

			if (bObject && ( nMode == 3 || nMode == 0) )
			{
				tb.left     = dwDeltaXStart;
				tb.right    = min( dwDeltaXStart + 1, 1024);
				tb.top      = 1024 - dwDeltaZStart;
				tb.bottom   = min( 1024 - dwDeltaZStart + 1, 1024);
				m_pITexture->FillFromMemory(&dwObjectColor, TF_ARGB32, 1, ta, tb);
			}

			if (bEffect && ( nMode == 2 || nMode == 0) )
			{

				tb.left     = min( dwDeltaXStart + 1, 1024);
				tb.right    = min( dwDeltaXStart + 2, 1024);
				tb.top      = max( 1024 - dwDeltaZStart - 1, 0);
				tb.bottom   = 1024 - dwDeltaZStart;
				m_pITexture->FillFromMemory(&dwEffectColor, TF_ARGB32, 0, ta, tb);
			}

			if (bSceneLight && ( nMode == 5 || nMode == 0) )
			{
				tb.left     = min( dwDeltaXStart + 1, 1024);
				tb.right    = min( dwDeltaXStart + 3, 1024);
				tb.top      = max( 1024 - dwDeltaZStart - 2, 0);
				tb.bottom   = 1024 - dwDeltaZStart;
				m_pITexture->FillFromMemory(bSceneLightColor, TF_ARGB32, 0, ta, tb);
			}

			if (dwBlockColor && ( nMode == 1 || nMode == 0) )
			{

				tb.left     = dwDeltaXStart;
				tb.right    = min( dwDeltaXStart + 1, 1024);
				tb.top      = max( 1024 - dwDeltaZStart - 1, 0);
				tb.bottom   = 1024 - dwDeltaZStart;
				m_pITexture->FillFromMemory(&dwBlockColor, TF_ARGB32, 0, ta, tb);
			}
		}

	}
	void CMiniMap::NavigateTo(DWORD x, DWORD y)
	{
		DWORD xOrigin = x;
		DWORD yOrigin = y;

		if ( x < MINIMAP_SCREEN_SIZE/2 )
		{
			x = MINIMAP_SCREEN_SIZE/2;
		}

		if ( x > MINIMAP_BLOCK_SIZE*MINIMAP_BLOCK_GRID_SIZE - MINIMAP_SCREEN_SIZE/2 )
		{
			x = MINIMAP_BLOCK_SIZE*MINIMAP_BLOCK_GRID_SIZE - MINIMAP_SCREEN_SIZE/2;
		}

		if ( y < MINIMAP_SCREEN_SIZE/2 )
		{
			y = MINIMAP_SCREEN_SIZE/2;
		}

		if ( y > MINIMAP_BLOCK_SIZE*MINIMAP_BLOCK_GRID_SIZE - MINIMAP_SCREEN_SIZE/2 )
		{
			y = MINIMAP_BLOCK_SIZE*MINIMAP_BLOCK_GRID_SIZE - MINIMAP_SCREEN_SIZE/2;
		}

		m_rcPVS.left = x - MINIMAP_SCREEN_SIZE/2;
		m_rcPVS.top = MINIMAP_BLOCK_SIZE*MINIMAP_BLOCK_GRID_SIZE - y - MINIMAP_SCREEN_SIZE/2;
		m_rcPVS.right = m_rcPVS.left + MINIMAP_SCREEN_SIZE - 1;
		m_rcPVS.bottom = m_rcPVS.top + MINIMAP_SCREEN_SIZE - 1;

		m_ptCenter.x = xOrigin + 3;
		m_ptCenter.y = yOrigin;
	}

	void CMiniMap::Update(const CMsg& msg)
	{
		DWORD & dwStyle = m_wndInfo.dwStyle;
		
		POINT ptMousePos;
		MSG_MOUSE_INF mouse_pos = CEventState::GetInst()->GetMousePosition();
		ptMousePos.x = mouse_pos.X;
		ptMousePos.y = mouse_pos.Y;

		if ( (dwStyle & BWS_VISIBLE) && !(dwStyle & BWS_DISABLE))
		{
			BOOL bMouseOver = ( PtInRect(&ptMousePos) > 0 );

			if ( bMouseOver )
			{
				// event.bDirty = true;

				switch (msg.msgSource)
				{
				case MGS_MOUSEMSG:
					switch(msg.msgType.mouse_type)
					{
					case MGT_MOUSE_LBNDOWN:
						{
							if ( msg.msgInfo.key_info == MK_LBUTTON && (CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN) )
							{
								int x,y;
								HitOffToCenter(x, y,&ptMousePos);

								CVector3f v;

								v.x = ((m_rcPVS.left + m_rcPVS.right)/2 + x)*GRID_SPACE;
								v.z = (1024 - (m_rcPVS.bottom + m_rcPVS.top)/2 - y)*GRID_SPACE;

								if ( CMapEditApp::GetInst()->GetEditingMesh()->IsValidVec(v) )
								{
									v.y = CPicker::GetInst()->GetHeight(v.x,v.z);
									CCameraManager::GetInst()->GetMapEditCamera()->MoveToRTSByView(v);
								}
							}
						}
						break;
					}
				}
			}
		}
	}
}
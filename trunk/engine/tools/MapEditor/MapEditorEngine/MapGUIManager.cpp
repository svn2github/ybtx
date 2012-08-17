#include "stdafx.h"
#include "MapGUIManager.h"
#include "BluelyGUI.h"
#include "MagicBall.h"
#include "CWindowTarget.h"
#include "CEventState.h"

#include "Picker.h"

#include "ExsertStructDB.h"
#include "BaseHelper.h"

namespace sqr
{
	//////////////////////////////////////////////////////////////////////////
	// manager
	//////////////////////////////////////////////////////////////////////////

	CMapGUIManager::CMapGUIManager()
	{
		m_pMainWnd								= NULL;
		m_pActiveWnd							= NULL;
		m_pMiniMap								= NULL;
		m_pTitleTextureBox						= NULL;
		m_PopupWindowState.m_ePopupWindowState	= EUPS_NONE;
		m_PopupWindowState.pTargetWnd			= NULL;
		m_PopupWindowState.pOldActive			= NULL;
	}

	CMapGUIManager::~CMapGUIManager()
	{
		SafeDelete( m_pMiniMap );
		SafeDelete( m_pTitleTextureBox );
		SafeDelete( m_pMainWnd );
	}

	void CMapGUIManager::Update(const CMsg& msg)
	{
		if ( m_PopupWindowState.m_ePopupWindowState != EUPS_NONE )
		{
			switch ( m_PopupWindowState.m_ePopupWindowState )
			{
			case EUPS_BEGIN:
				{
					m_PopupWindowState.pOldActive = m_pActiveWnd;
					ActiveWindow(m_PopupWindowState.pTargetWnd);
					m_PopupWindowState.m_ePopupWindowState = EUPS_RUNNING;
				}
				break;

			case EUPS_RUNNING:
				break;

			case EUPS_END:
				{
					ActiveWindow(m_PopupWindowState.pOldActive);
					m_PopupWindowState.m_ePopupWindowState = EUPS_NONE;
				}
				break;
			}
		}

		// default messag handle
		switch ( msg.msgSource )
		{
		case MGS_MOUSEMSG:
			{
				switch ( msg.msgType.mouse_type )
				{
				case MGT_MOUSE_LBNDOWN:
				case MGT_MOUSE_RBNDOWN:
				case MGT_MOUSE_MBNDOWN:
					{
						POINT ptMousePos;
						ptMousePos.x = msg.msgInfo.mouse_info.X;
						ptMousePos.y = msg.msgInfo.mouse_info.Y;

						if ( m_PopupWindowState.m_ePopupWindowState == EUPS_NONE )
						{
							CBlyWnd * pWndOver = NULL;
							m_pMainWnd->HitTest(&ptMousePos, &pWndOver);

							if ( pWndOver )
								ActiveWindow(pWndOver);
						}
						else
						{
							CBlyWnd * pWndOver = NULL;
							m_PopupWindowState.pTargetWnd->HitTest(&ptMousePos,&pWndOver);
							if ( pWndOver )
								ActiveWindow(pWndOver);
						}
					}
					break;
				}
			}
		case MGS_UIMSG:
			{
				switch(msg.msgType.ui_type)
				{
				case MGT_UI_SIZE:
					{
						//if(CMapEditorManage::HasInst() && CMapEditorManage::GetInst()->GetMapEditorHelper() )
						//	CMapEditorManage::GetInst()->GetMapEditorHelper()->OnSize();
					}
				}
			}

		case MGS_KEYMSG:
			{
				if ( ( CEventState::GetInst()->GetState(MGI_KEY_Menu) == MGT_KEY_DOWN ) )
				{
 					switch(msg.msgInfo.key_info)
 					{
 						///Left Alt + M 隐藏小地图
 					case MGI_KEY_M:
 						this->HideSceneMinMap();
 						return;
 					}
		}
			}

		default:
			break;
		}

		m_pMainWnd->Update(msg);
		this->UpdateFPSLimited();
	}

	void CMapGUIManager::Render()
	{
		m_pMainWnd->Render();
	}

	void CMapGUIManager::InactiveWindow(CBlyWnd * pWnd)
	{
		for ( list<CBlyWnd *>::iterator it = pWnd->m_listChild.begin(); it != pWnd->m_listChild.end(); ++it )
		{
			CBlyWnd * pWnd = *it;
			InactiveWindow(pWnd);
		}
		pWnd->m_wndInfo.bActive = FALSE;
	}

	void CMapGUIManager::BeginPopupWnd(CBlyWnd * pWnd)
	{
		// set a window as pop up
		m_PopupWindowState.m_ePopupWindowState = EUPS_BEGIN;
		m_PopupWindowState.pTargetWnd = pWnd;
	}

	void CMapGUIManager::EndPopupWnd()
	{
		m_PopupWindowState.m_ePopupWindowState = EUPS_END;
		m_PopupWindowState.pTargetWnd = NULL;
	}

	void CMapGUIManager::ActiveWindow(CBlyWnd * pWnd)
	{
		InactiveWindow(m_pMainWnd);
		m_pActiveWnd = pWnd;
		if ( pWnd )
		{
			pWnd->m_wndInfo.bActive = TRUE;
			CBlyWnd * pChild = pWnd;
			CBlyWnd * pParent = pChild->GetParent();
			CBlyWnd * pTemp = NULL;
			//modify order

			while ( pChild )
			{
				pTemp = pParent;

				if ( pParent )
				{
					pParent->RemoveChild(pChild);
					pParent->AddChild(pChild);
					pParent = pParent->GetParent();
				}
				pChild = pTemp;
			}
		}
	}

	CBlyWnd* CMapGUIManager::GetMainWnd() const
	{
		return m_pMainWnd;
	}

	void CMapGUIManager::ModifyStyle(CBlyWnd * pWnd, DWORD dwAdd, DWORD dwRemove)
	{
		pWnd->ModifyStyle(dwAdd,dwRemove);
		for ( list<CBlyWnd *>::iterator it = pWnd->m_listChild.begin(); it != pWnd->m_listChild.end(); ++it )
		{
			CBlyWnd * pWnd = *it;
			ModifyStyle(pWnd,dwAdd,dwRemove);
		}
	}

	void CMapGUIManager::CreateMapGUI()
	{
		DWORD	dwWorldWidth	= CMainWindowTarget::GetInst()->GetWidth();
		DWORD	dwWorldHeight	= CMainWindowTarget::GetInst()->GetHeight();
		RECT	rc = {0, 0, dwWorldWidth, dwWorldHeight};

		m_pMainWnd = new CBlyWnd;
		m_pMainWnd->Create("MapEditor Powered by Bluely Engine",&rc,BWS_VISIBLE,0);

		rc.left		= CMainWindowTarget::GetInst()->GetWidth() - 192;
		rc.right	= rc.left+192;
		rc.top		= 2;
		rc.bottom	= rc.top + 192;

		m_pMiniMap	= new CMiniMap;
		m_pMiniMap->Create("MiniMap",&rc,BWS_DRAG|BWS_VISIBLE,m_pMainWnd);

		// 模型纹理预览框
		rc.left		= CMainWindowTarget::GetInst()->GetWidth() - 192;
		rc.right	= rc.left+192;
		rc.top		= dwWorldHeight - 192;
		rc.bottom	= dwWorldHeight;
		m_pTitleTextureBox = new CTitleTextureBox;
		m_pTitleTextureBox->Create("TitleTextureBox",&rc,0,m_pMainWnd);

		// 显示太阳球
		rc.left		= 2;
		rc.right	= rc.left + MAGICBALL_SIZE;
		rc.top		= 2;
		rc.bottom	= rc.top + MAGICBALL_SIZE;

		this->ActiveWindow(m_pMainWnd);
	}

	void CMapGUIManager::UpdateFPSLimited()
	{
		//每帧都在更新想办法优化
		m_pMiniMap->Finalize();
		
		CVector3f v;

		if ( CPicker::GetInst()->GetIntersected(v) )
		{
			m_pMiniMap->NavigateTo(DWORD(v.x/GRID_SPACE),DWORD(v.z/GRID_SPACE));
		}
	}

	void CMapGUIManager::UpdatePixel( DWORD dwMiniBlockIndex, DWORD dwGridIndexInMiniBlock, DWORD dwNewColor )
	{
		m_pMiniMap->UpdatePixel(dwMiniBlockIndex,dwGridIndexInMiniBlock,dwNewColor);
	}

	void CMapGUIManager::Finalize()
	{
		m_pMiniMap->Finalize();
	}

	void CMapGUIManager::InitMiniMap()
	{
		m_pMiniMap->Init();
	}

	void CMapGUIManager::UpdateMiniMapPosition()
	{
		RECT rc;
		rc.left = CMainWindowTarget::GetInst()->GetWidth() - 192;
		rc.top = 2;

		m_pMiniMap->MoveWndTo(rc.left,rc.top);
	}

	void CMapGUIManager::SetTitleTextureVisibility( bool b )
	{
		m_pTitleTextureBox->ShowWnd(b);
	}

	void CMapGUIManager::SetTitleTextureName( string & name )
	{
		m_pTitleTextureBox->SetTextureName(name);
		m_pTitleTextureBox->ShowWnd(true);
	}

	void CMapGUIManager::HideSceneMinMap()
	{
		DWORD dwStyle = m_pMiniMap->GetStyle();	
		dwStyle ^= BWS_VISIBLE;
		m_pMiniMap->SetStyle(dwStyle);
	}

	void CMapGUIManager::RefreshMiniMap( int nMode )
	{
		m_pMiniMap->RefreshMiniMap(nMode);
	}
	//////////////////////////////////////////////////////////////////////////
}
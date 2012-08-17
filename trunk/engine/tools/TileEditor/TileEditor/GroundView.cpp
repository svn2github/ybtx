#include "stdafx.h"
#include "MapEditor.h"
#include "GroundView.h"
#include ".\groundview.h"
#include <gui.h>
#include "newmapdialog.h"
#include "MainFrm.h"
#include "Set.h"
#include <SharedTexture.h>

CGroundView::CGroundView()
	: CFormView(CGroundView::IDD)
{
	m_eEditState = sc::COperator::EEDIT_STATE::EES_NONE;
	m_images.Create(16,16,ILC_COLOR16|ILC_MASK,0,1);
	m_images.Add( ::AfxGetApp()->LoadIcon( IDR_SETGROUPNOR ) );
	m_images.Add( ::AfxGetApp()->LoadIcon( IDR_SETGROUPSEL ) );
	m_images.Add( ::AfxGetApp()->LoadIcon( IDR_SETUNIT2D ) );
}

CGroundView::~CGroundView()
{
}

void CGroundView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STRENGTH, m_sliderBrushStrength);
	DDX_Control(pDX, IDC_HEIGHTSTRENGTH, m_heightStrength);
	DDX_Control(pDX, IDC_TREE2, m_textureTree);
}

BEGIN_MESSAGE_MAP(CGroundView, CFormView)
	ON_BN_CLICKED(IDC_LAYERONE, OnBnClickedLayerone)
	ON_BN_CLICKED(IDC_LAYERTWO, OnBnClickedLayertwo)
	ON_BN_CLICKED(IDC_PHYSICHEIGHT, OnBnClickedPhysicheight)
	ON_BN_CLICKED(IDC_LOGICHEIGHT, OnBnClickedLogicheight)
	ON_BN_CLICKED(IDC_LINEAR, OnBnClickedLinear)
	ON_BN_CLICKED(IDC_GAUSS, OnBnClickedGauss)
	ON_BN_CLICKED(IDC_UP, OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, OnBnClickedDown)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_BN_CLICKED(IDC_X, OnBnClickedX)
	ON_BN_CLICKED(IDC_Z, OnBnClickedZ)
	ON_BN_CLICKED(IDC_BOTH, OnBnClickedBoth)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_NOTIFY(NM_CLICK, IDC_TREE2, OnNMClickTree2)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CGroundView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGroundView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CGroundView::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
//	radios.CheckRadio(this,IDC_LINEAR, IDC_GAUSS, IDC_GAUSS);
    CFormView::PreSubclassWindow();
}

BOOL CGroundView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
							  AFX_CMDHANDLERINFO* pHandlerInfo)
{
//	m_radios.CheckRadio( this, nID );
	return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}		

void CGroundView::OnBnClickedLayerone()
{
	ui::CCheckBox::SetHotID(0);
	sc::CXFile::GetInstance()->GetOperator()->SetEditState(sc::COperator::EES_SET_TILE_TEXTURE);
	m_eEditState = sc::COperator::EEDIT_STATE::EES_SET_TILE_TEXTURE;
	CheckDlgButton(IDC_PHYSICHEIGHT,false);
	CheckDlgButton(IDC_LOGICHEIGHT,false);
	CheckDlgButton(IDC_LINEAR,true);
	CheckDlgButton(IDC_GAUSS,false);
	GetDlgItem(IDC_GAUSS)->EnableWindow(false);
}

void CGroundView::OnBnClickedLayertwo()
{
	ui::CCheckBox::SetHotID(1);
	sc::CXFile::GetInstance()->GetOperator()->SetEditState(sc::COperator::EES_SET_TILE_TEXTURE);
	m_eEditState = sc::COperator::EEDIT_STATE::EES_SET_TILE_TEXTURE;
	CheckDlgButton(IDC_PHYSICHEIGHT,false);
	CheckDlgButton(IDC_LOGICHEIGHT,false);
	CheckDlgButton(IDC_LINEAR,true);
	CheckDlgButton(IDC_GAUSS,false);
	GetDlgItem(IDC_GAUSS)->EnableWindow(false);
}

LRESULT CGroundView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message ) 
	{
	case WM_SETLAYERHOT:
		{
			if ( 1 == wParam )
			{
				CheckDlgButton(IDC_LAYERONE,true);
				CheckDlgButton(IDC_LAYERTWO,false);
				CheckDlgButton(IDC_PHYSICHEIGHT,false);
				CheckDlgButton(IDC_LOGICHEIGHT,false);
				CheckDlgButton(IDC_LINEAR,true);
				CheckDlgButton(IDC_GAUSS,false);
				GetDlgItem(IDC_GAUSS)->EnableWindow(false);

			}
			
			if ( 2 == wParam )
			{
				CheckDlgButton(IDC_LAYERONE,false);
				CheckDlgButton(IDC_LAYERTWO,true);
				CheckDlgButton(IDC_PHYSICHEIGHT,false);
				CheckDlgButton(IDC_LOGICHEIGHT,false);
				CheckDlgButton(IDC_LINEAR,true);
				CheckDlgButton(IDC_GAUSS,false);
				GetDlgItem(IDC_GAUSS)->EnableWindow(false);
			}

			m_eEditState = sc::COperator::EES_SET_TILE_TEXTURE;
			sc::CXFile::GetInstance()->GetOperator()->SetEditState(m_eEditState);
		}
	break;
	
	case WM_INITGROUNDVIEW:
	{
		CheckDlgButton(IDC_GAUSS,true);
		CheckDlgButton(IDC_LINEAR,false);

		CheckDlgButton(IDC_BOTH,true);
		CheckDlgButton(IDC_X,false);
		CheckDlgButton(IDC_Z,false);

		CheckDlgButton(IDC_UP,true);
		CheckDlgButton(IDC_DOWN,false);
		CheckDlgButton(IDC_CLEAR,false);

		GetDlgItem(IDC_X)->EnableWindow(false);
		GetDlgItem(IDC_Z)->EnableWindow(false);

		m_sliderBrushStrength.SetRangeMin(0);
		m_sliderBrushStrength.SetRangeMax(32);
		m_sliderBrushStrength.SetPos(16);

		m_heightStrength.SetRangeMin(0);
		m_heightStrength.SetRangeMax(32);
		m_heightStrength.SetPos(16);
	}
	break;
	
	}
	
	return CFormView::WindowProc(message, wParam, lParam);
}

void CGroundView::OnBnClickedPhysicheight()
{
	sc::CXFile::GetInstance()->GetOperator()->SetEditState(sc::COperator::EES_SET_TILE_HEIGHT);
	m_eEditState = sc::COperator::EEDIT_STATE::EES_SET_TILE_HEIGHT;
	CheckDlgButton(IDC_LAYERONE,false);
	CheckDlgButton(IDC_LAYERTWO,false);
	CheckDlgButton(IDC_LINEAR,false);
	CheckDlgButton(IDC_GAUSS,true);
	GetDlgItem(IDC_GAUSS)->EnableWindow(true);
}

void CGroundView::OnBnClickedLogicheight()
{
	sc::CXFile::GetInstance()->GetOperator()->SetEditState(sc::COperator::EES_SET_TILE_HEIGHT);
	m_eEditState = sc::COperator::EEDIT_STATE::EES_SET_TILE_HEIGHT;
	CheckDlgButton(IDC_LAYERONE,false);
	CheckDlgButton(IDC_LAYERTWO,false);
	CheckDlgButton(IDC_LINEAR,false);
	CheckDlgButton(IDC_GAUSS,true);
	GetDlgItem(IDC_GAUSS)->EnableWindow(true);
}

void CGroundView::OnBnClickedLinear()
{
	sc::CXFile::GetInstance()->GetOperator()->SetHeightType((sc::COperator::ESET_HEIGHT_TYPE::ESHT_LINEAR));
	GetDlgItem(IDC_X)->EnableWindow(true);
	GetDlgItem(IDC_Z)->EnableWindow(true);
}

void CGroundView::OnBnClickedGauss()
{
	sc::CXFile::GetInstance()->GetOperator()->SetHeightType((sc::COperator::ESET_HEIGHT_TYPE::ESHT_GAUSS));
	sc::CXFile::GetInstance()->GetOperator()->SetAxisType((sc::COperator::EAXIS_TYPE::EAT_BOTH));
	CheckDlgButton(IDC_BOTH,true);
	CheckDlgButton(IDC_X,false);
	CheckDlgButton(IDC_Z,false);
	GetDlgItem(IDC_X)->EnableWindow(false);
	GetDlgItem(IDC_Z)->EnableWindow(false);
}

void CGroundView::OnBnClickedUp()
{
	sc::CXFile::GetInstance()->GetOperator()->SetWayType((sc::COperator::ESET_WAY_TYPE::ESWT_INC));
	GetDlgItem(IDC_GAUSS)->EnableWindow(true);
	GetDlgItem(IDC_LINEAR)->EnableWindow(true);
	GetDlgItem(IDC_X)->EnableWindow(true);
	GetDlgItem(IDC_Z)->EnableWindow(true);
	GetDlgItem(IDC_BOTH)->EnableWindow(true);
}

void CGroundView::OnBnClickedDown()
{
	sc::CXFile::GetInstance()->GetOperator()->SetWayType((sc::COperator::ESET_WAY_TYPE::ESWT_DEC));
	GetDlgItem(IDC_GAUSS)->EnableWindow(true);
	GetDlgItem(IDC_LINEAR)->EnableWindow(true);
	GetDlgItem(IDC_X)->EnableWindow(true);
	GetDlgItem(IDC_Z)->EnableWindow(true);
	GetDlgItem(IDC_BOTH)->EnableWindow(true);
}

void CGroundView::OnBnClickedClear()
{
	sc::CXFile::GetInstance()->GetOperator()->SetWayType((sc::COperator::ESET_WAY_TYPE::ESWT_CUT));
	GetDlgItem(IDC_GAUSS)->EnableWindow(false);
	GetDlgItem(IDC_LINEAR)->EnableWindow(false);
	GetDlgItem(IDC_X)->EnableWindow(false);
	GetDlgItem(IDC_Z)->EnableWindow(false);
	GetDlgItem(IDC_BOTH)->EnableWindow(false);
}

void CGroundView::OnBnClickedX()
{
	sc::CXFile::GetInstance()->GetOperator()->SetAxisType((sc::COperator::EAXIS_TYPE::EAT_X));
}

void CGroundView::OnBnClickedZ()
{
	sc::CXFile::GetInstance()->GetOperator()->SetAxisType((sc::COperator::EAXIS_TYPE::EAT_Z));
}

void CGroundView::OnBnClickedBoth()
{
	sc::CXFile::GetInstance()->GetOperator()->SetAxisType((sc::COperator::EAXIS_TYPE::EAT_BOTH));
}

void CGroundView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	static bool bFirst = false;
	
	if ( bShow && !bFirst )
	{
		m_textureTree.SetImageList( &m_images, TVSIL_NORMAL );
		bFirst = true;
	}

	//////////////////////////////////////////////////////////////////////////
	
	if ( sc::CXFile::GetInstance()->GetOperator() && bShow )
	{
		InitTextures();
		
		sc::COperator::EEDIT_STATE e = sc::CXFile::GetInstance()->GetOperator()->GetEditState();
		
		if ( e == sc::COperator::EES_NONE || e == sc::COperator::EES_SET_TILE_BRAIN || e == sc::COperator::EES_SET_TILE_FACE )
		{
			sc::CXFile::GetInstance()->GetOperator()->SetEditState(m_eEditState);
			return;
		}
		else
		{
			if ( e == sc::COperator::EES_SET_TILE_TEXTURE )
			{
				int n = sc::CXFile::GetInstance()->GetOperator()->GetLayerHot();
				
				if ( n == 0 )
				{
					m_eEditState = sc::COperator::EEDIT_STATE::EES_SET_TILE_TEXTURE;
					CheckDlgButton(IDC_LAYERONE,true);
					CheckDlgButton(IDC_LAYERTWO,false);
					CheckDlgButton(IDC_PHYSICHEIGHT,false);
					CheckDlgButton(IDC_LOGICHEIGHT,false);
					CheckDlgButton(IDC_LINEAR,true);
					CheckDlgButton(IDC_GAUSS,false);
					GetDlgItem(IDC_GAUSS)->EnableWindow(false);
				}

				if ( n == 1 )
				{
					m_eEditState = sc::COperator::EEDIT_STATE::EES_SET_TILE_TEXTURE;
					CheckDlgButton(IDC_LAYERONE,false);
					CheckDlgButton(IDC_LAYERTWO,true);
					CheckDlgButton(IDC_PHYSICHEIGHT,false);
					CheckDlgButton(IDC_LOGICHEIGHT,false);
					CheckDlgButton(IDC_LINEAR,true);
					CheckDlgButton(IDC_GAUSS,false);
					GetDlgItem(IDC_GAUSS)->EnableWindow(false);
				}

			}
		}
	}

}

void CGroundView::InitTextures()
{
	CTreeCtrl & resTree = ((CMainFrame*)AfxGetMainWnd())->GetResSetView()->GetResTree();
	
	if ( m_textureTree.GetCount() != 0 )
	{	
		m_textureTree.DeleteAllItems();
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	HTREEITEM hResTreeRoot = resTree.GetChildItem(TVI_ROOT);
	
	HTREEITEM hTextureTreeRoot = m_textureTree.GetRootItem();
	
	HTREEITEM hResTree2DRoot = resTree.GetNextItem(hResTreeRoot, TVGN_CHILD);
	
	if ( hResTree2DRoot != NULL )
	{
		HTREEITEM hCurrentGroup = resTree.GetNextItem(hResTree2DRoot, TVGN_CHILD);
		
		while ( hCurrentGroup!= NULL )
		{
			// get group
			TVITEM TItemGroup;
			char s[64];
			TItemGroup.pszText = s;
			TItemGroup.cchTextMax = 64;
			TItemGroup.hItem = hCurrentGroup;
			TItemGroup.mask = TVIF_TEXT|TVIF_PARAM;
			resTree.GetItem(&TItemGroup);

			// copy group			
			TVINSERTSTRUCT TVISGroup;
			TVISGroup.hParent = hTextureTreeRoot;
			TVISGroup.hInsertAfter = TVI_LAST;
			TVISGroup.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
			TVISGroup.item.iImage = 0;
			TVISGroup.item.iSelectedImage = 1;
			TVISGroup.item.lParam = TItemGroup.lParam;
			TVISGroup.item.pszText = TItemGroup.pszText;
			HTREEITEM hCurrentTextureTreeGroup = m_textureTree.InsertItem( &TVISGroup );
			
			HTREEITEM hCurrentUnit = resTree.GetNextItem(hCurrentGroup, TVGN_CHILD);
			
			while ( hCurrentUnit != NULL )
			{
				// get
				TVITEM TItemTexture;
				char s[64];
				TItemTexture.pszText = s;
				TItemTexture.cchTextMax = 64;
				TItemTexture.hItem = hCurrentUnit;
				TItemTexture.mask = TVIF_TEXT|TVIF_PARAM;
				resTree.GetItem( &TItemTexture );

				// copy texture
				TVINSERTSTRUCT TVISTexture;
				TVISTexture.hParent = hCurrentTextureTreeGroup;
				TVISTexture.hInsertAfter = TVI_LAST;
				TVISTexture.item.mask =  TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
				TVISTexture.item.iImage = 2;
				TVISTexture.item.iSelectedImage = 2;
				TVISTexture.item.lParam = TItemTexture.lParam;
				TVISTexture.item.pszText = TItemTexture.pszText;
				m_textureTree.InsertItem( &TVISTexture );

				hCurrentUnit = resTree.GetNextItem(hCurrentUnit, TVGN_NEXT);
			}

			hCurrentGroup = resTree.GetNextItem(hCurrentGroup,TVGN_NEXT);
		}
	}

}

void CGroundView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CSliderCtrl * pSlider = (CSliderCtrl*)pScrollBar;
	
	float f = (float)pSlider->GetPos();
	
	if ( pSlider == &m_heightStrength )
		sc::CXFile::GetInstance()->GetTerrainMesh()->SetHeightStrength(f/4.0f);
	
	if ( pSlider == &m_sliderBrushStrength )
		sc::CXFile::GetInstance()->GetTerrainMesh()->SetAlphaStrength(f);
	
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CGroundView::OnFileNew()
{
}

void CGroundView::OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint myPoint;

	::GetCursorPos(&myPoint); 
	m_textureTree.ScreenToClient(&myPoint);
	
	//// select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = m_textureTree.HitTest(myPoint, &uFlags);

	if( !hItem )
		return;
	TVITEM TItem;
	TItem.hItem = hItem;
	TItem.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	m_textureTree.GetItem( &TItem );
	
	if( TItem.iImage == 2 )
	{
		HTREEITEM Parent = m_textureTree.GetParentItem( hItem );
		CResSetGroup * pGroup = (CResSetGroup*)m_textureTree.GetItemData( Parent );
		
		CResSetUnit * pUnit;
		
		for( int i = 0; i < (int)pGroup->m_resSetUnits.size(); i++ )
		{
			if( TItem.lParam == (LPARAM)pGroup->m_resSetUnits[i] )
			{
				pUnit = pGroup->m_resSetUnits[i];
				break;
			}
		}
		
		int nTextureHandle = CSharedTexture::GetTexture(pUnit->GetName())->GetTextureHandle();
		
		sc::CXFile::GetInstance()->GetOperator()->SetTextureHandleHot(nTextureHandle);
	}
	
	*pResult = 0;
}
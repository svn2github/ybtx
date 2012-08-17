// SelectSound.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "GUIResWinData.h"
#include "CGraphic.h"
#include "CodeCvs.h"


// CSelectSound dialog
IMPLEMENT_DYNAMIC(CGUIResWinDataDlg, CDialog)

CGUIResWinDataDlg::CGUIResWinDataDlg()
: CFormView(CGUIResWinDataDlg::IDD)
, m_fOffX(0.0f)
, m_fOffY(0.0f)
, m_Width(0)
, m_Height(0)
, m_pText(NULL)
, m_bAlphaTest(TRUE)
, m_bAlphaBlend(FALSE)
, m_bStateInRender(false)
, m_bRes2Win(false)
{}

CGUIResWinDataDlg::~CGUIResWinDataDlg()
{
}

void CGUIResWinDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_RES, m_ResWinTree);
}

BOOL CGUIResWinDataDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
							   const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	UpdateData( FALSE );

	return re;
}

#ifdef _DEBUG
void CGUIResWinDataDlg::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGUIResWinDataDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CGUIResWinDataDlg, CFormView)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RES, &CGUIResWinDataDlg::OnTvnSelchangedTreeRes)
	ON_BN_CLICKED(IDC_CHECK_RES, &CGUIResWinDataDlg::OnRes2WinChanged)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CGUIResWinDataDlg::OnBnClickedBtnRefresh)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CSelectSound message handlers
void CGUIResWinDataDlg::OnTvnSelchangedTreeRes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hTreeItem = m_ResWinTree.GetSelectedItem();
	wstring temp_name = m_ResWinTree.GetItemText(hTreeItem);
	// 用这个创建，如果创建成功，就是它
	// 如果创建失败，取父再创建
	if ( temp_name == m_strCurImageName )
		return;
	
	if( temp_name.length() != 0 )
	{
		ITexture* temp_pTex = NULL;
		if (FAILED(CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str()
			, utf16_to_utf8(temp_name.c_str()).c_str(), &temp_pTex ))
			)		// 创建失败
		{
			// 寻找父ITEM
			hTreeItem = m_ResWinTree.GetParentItem(hTreeItem);
			if (hTreeItem)
			{
				temp_name = m_ResWinTree.GetItemText(hTreeItem);
				if( temp_name.length() != 0 )
				{
					ITexture* temp_pTex = NULL;
					if (FAILED(CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str()
						, utf16_to_utf8(temp_name.c_str()).c_str(), &temp_pTex ))
						)		// 创建失败
						return;
					else
					{
						// 创建成功
						m_strCurImageName = temp_name;
						if (m_pText)
							SafeRelease(m_pText);
						m_pText = temp_pTex;
						if(m_pText)
						{
							m_Width = m_pText->GetWidth();
							m_Height = m_pText->GetHeight();
						}
					}
				}
				else
					return;
			}
			else
				return;
		}
		else
		{
			// 创建成功
			m_strCurImageName = temp_name;
			if (m_pText)
				SafeRelease(m_pText);
			m_pText = temp_pTex;
			if(m_pText)
			{
				m_Width = m_pText->GetWidth();
				m_Height = m_pText->GetHeight();
			}
		}
	}
}

void CGUIResWinDataDlg::OnRes2WinChanged()
{
	if( TRUE == ((::CButton*)GetDlgItem(IDC_CHECK_RES))->GetCheck() )
		m_bRes2Win = true;
	else
		m_bRes2Win = false;

	// 刷新tree
	TV_INSERTSTRUCT treeItem;
	HTREEITEM hTreeItem, hParentTree;
	treeItem.hInsertAfter = TVI_LAST;
	treeItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	std::map<wstring, vec_wstr>* mapPtr;
	if (m_bRes2Win)
		mapPtr = &m_Res2Win;
	else
		mapPtr = &m_Win2Res;

	m_ResWinTree.DeleteAllItems();
	
	if (m_bRes2Win)
	{
		hParentTree = m_ResWinTree.InsertItem(L"资源－窗体", 1, 1);
	}
	else
	{
		hParentTree = m_ResWinTree.InsertItem(L"窗体－资源", 1, 1);
	}
	std::map<wstring, vec_wstr>::iterator itr = mapPtr->begin();
	std::map<wstring, vec_wstr>::iterator eitr = mapPtr->end();
	while (itr != eitr)
	{
		const wstring& first_str = itr->first;
		hTreeItem = m_ResWinTree.InsertItem( first_str.c_str(), 1, 1, hParentTree );		// 添加一级
		
		vec_wstr& vecStr = itr->second;
		vec_wstr::iterator sec_itr = vecStr.begin();
		vec_wstr::iterator sec_eitr = vecStr.end();
		while(sec_itr != sec_eitr)
		{
			wstring& sec_str = *sec_itr;
			m_ResWinTree.InsertItem(sec_str.c_str(), 1, 1, hTreeItem);				// 添加二级
			++ sec_itr;
		}

		m_ResWinTree.Expand( hTreeItem, TVE_EXPAND );
		++itr;
	}
	m_ResWinTree.Expand( hParentTree, TVE_EXPAND );
}

wstring CGUIResWinDataDlg::GetSelectImgName() const
{
	return m_strCurImageName;
}

void  CGUIResWinDataDlg::PushResData(const wstring& win, const wstring& res)
{
	// win 2 res
	std::map<wstring, vec_wstr>::iterator itr = m_Win2Res.find(win);
	if (itr == m_Win2Res.end())
	{
		// 创建一个添加进去
		vec_wstr vecwstr;
		m_Win2Res.insert( make_pair(win, vecwstr) );
		itr = m_Win2Res.find(win);
	}
	vec_wstr& verWs = itr->second;	// 它是个SET
	if (verWs.find(res) == verWs.end())
	{
		// 插入
		verWs.insert(res);
	}

	// res 2 win
	std::map<wstring, vec_wstr>::iterator itrwin = m_Res2Win.find(res);
	if (itrwin == m_Res2Win.end())
	{
		// 创建一个添加进去
		vec_wstr vecwstr;
		m_Res2Win.insert( make_pair(res, vecwstr) );
		itrwin = m_Res2Win.find(res);
	}
	vec_wstr& verWs2 = itrwin->second;	// 它是个SET
	if (verWs2.find(win) == verWs2.end())
	{
		// 插入
		verWs2.insert(win);
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// 渲染部分
void CGUIResWinDataDlg::BeginRenderer()
{	
	if(CMainWindowTarget::HasInst())
	{
		m_bStateInRender = true;
		CMainWindowTarget::GetInst()->InsertRenderer(this);
	}
}

void CGUIResWinDataDlg::EndRenderer()
{
	if(CMainWindowTarget::HasInst())
	{
		m_bStateInRender = false;
		CMainWindowTarget::GetInst()->DeleteRenderer(this);
	}
}

void CGUIResWinDataDlg::Render()
{
	CGraphic* pGraphic = CGraphic::GetInst();
	pGraphic->ClearStack();
	CMainWindowTarget::GetInst()->Clear();
	if(m_pText)
	{
		VerText2D* pVB;
		RenderState* pRS;
		CMatrix* Matrix = pGraphic->GetRenderStack( RS_GUI, m_pText, NULL, PT_TRIANGLESTRIP, 
			4, 2, VerText2D::Format, sizeof(VerText2D), (void**)&pVB, NULL, (void**)&pRS );
		pRS->m_Cull = 1;
		pRS->m_AlphaBlendEnable = m_bAlphaBlend;
		pRS->m_AlphaTestEnable  = m_bAlphaTest;
		pRS->m_ZTestEnable		= FALSE;
		Matrix->Identity();

		pVB[0].x = m_fOffX;  					pVB[0].y = m_fOffY;						pVB[0].z = 1.0f; 
		pVB[1].x = float(m_Width)+ m_fOffX;		pVB[1].y = m_fOffY;						pVB[1].z = 1.0f;	
		pVB[2].x = m_fOffX;						pVB[2].y = float(m_Height) + m_fOffY;	pVB[2].z = 1.0f;	
		pVB[3].x = float(m_Width)+ m_fOffX;		pVB[3].y = float(m_Height) + m_fOffY;	pVB[3].z = 1.0f;	

		pVB[0].w = 1.0;		pVB[0].tu = 0.0;	pVB[0].tv = 0.0;
		pVB[1].w = 1.0;		pVB[1].tu = 1.0;	pVB[1].tv = 0.0;
		pVB[2].w = 1.0;		pVB[2].tu = 0.0;	pVB[2].tv = 1.0; 
		pVB[3].w = 1.0;		pVB[3].tu = 1.0;	pVB[3].tv = 1.0;
	}
}

void CGUIResWinDataDlg::OnBnClickedBtnRefresh()
{
	OnRes2WinChanged();
}

void CGUIResWinDataDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);


	CFormView::OnShowWindow(bShow, nStatus);
	if(bShow)
		BeginRenderer();
	else
		EndRenderer();
}

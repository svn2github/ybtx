// TextureSetView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "TextureSetView.h"
#include <Scene.h>
#include <GUITexturedBox.h>
#include ".\texturesetview.h"
#include <Render.h>
#include <UserMsg.h>
#include "stdlib.h"]
#include "stdio.h"

// CTextureSetView

CTextureSetView::CTextureSetView()
	: CFormView(CTextureSetView::IDD),m_layer1(0),m_layer2(1)
{
	m_PathTree.SetFileList( &m_FileList );
}

CTextureSetView::~CTextureSetView()
{
	LPCTSTR m = m_PathTree.GetSelectedPath();
	FILE * pf = fopen("info","w");
	fprintf(pf,m);
	fclose(pf);
}

void CTextureSetView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GrassSet, m_layer1);
	DDX_Control(pDX, IDC_MudSet, m_layer2);
	DDX_Control(pDX, IDC_TREE1, m_PathTree);
	DDX_Control(pDX, IDC_LIST1, m_FileList);
}

BEGIN_MESSAGE_MAP(CTextureSetView, CFormView)
	ON_WM_MOUSEMOVE()
	ON_WM_DRAWITEM()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CTextureSetView 诊断

#ifdef _DEBUG
void CTextureSetView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTextureSetView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CTextureSetView::Init()
{
	m_layer1.Init(40,40,"grass.tga");
	m_layer2.Init(40,40,"mud.tga");
	m_FileList.SetFileIcon(IDR_MAINFRAME,"tga",0);
	char m[255];
	FILE * pf = fopen("info","r");
	fgets(m,255,pf);
	fclose(pf);
	m_PathTree.SetCurPath(m);
}

void CTextureSetView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_layer1.SetMove(false);
	m_layer2.SetMove(false);
	CFormView::OnMouseMove(nFlags, point);
}

void CTextureSetView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return;
	// CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
void CTextureSetView::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	LVITEM Item;
	char Buf[256];
	Item.iItem = pNMLV->iItem;
	Item.iSubItem = 0;
	Item.mask = LVIF_TEXT;
	Item.pszText = Buf;
	Item.cchTextMax = 256;
	m_FileList.GetItem(&Item);
	LPCSTR p = m_FileList.GetPath();
	*pResult = 0;
	char c[256];
	strcpy(c,p);
	strcat(c,"\\");
	strcat(c,Buf);
	if ( CTexturedBox::ms_pHot ) CTexturedBox::ms_pHot->ResetTexture(c);
}

void CTextureSetView::SetLayerHot(DWORD dwLayer)
{
	if ( dwLayer == 1 )
	{
		m_layer1.SetHot(true);
	}
	
	if ( dwLayer == 2 )
	{
		m_layer2.SetHot(true);
	}
}

LRESULT CTextureSetView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	switch ( message ) 
	{
	case WM_SETLAYERHOT:
		{
			this->SetLayerHot(wParam);
		}
	break;
    }

	return CFormView::WindowProc(message, wParam, lParam);
}

void CTextureSetView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
}

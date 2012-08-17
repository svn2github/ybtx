// ObjectView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ObjectView.h"
#include ".\objectview.h"


// CObjectView

CObjectView::CObjectView()
	: CFormView(CObjectView::IDD)
{
	m_pathTree.SetFileList( &m_fileList );
}

CObjectView::~CObjectView()
{
	LPCTSTR m = m_pathTree.GetSelectedPath();
	FILE * pf = fopen("objectPath","w");
	fprintf(pf,m);
	fclose(pf);
}

void CObjectView::Init()
{
	char m[255];
	FILE * pf = fopen("objectPath","r");
	fgets(m,255,pf);
	fclose(pf);
	m_pathTree.SetCurPath(m);
	m_fileList.SetFileIcon(IDR_MAINFRAME,"arp",0);
}

void CObjectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_pathTree);
	DDX_Control(pDX, IDC_LIST1, m_fileList);
}

BEGIN_MESSAGE_MAP(CObjectView, CFormView)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CObjectView 诊断

#ifdef _DEBUG
void CObjectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CObjectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CObjectView 消息处理程序

void CObjectView::PreSubclassWindow()
{
	CFormView::PreSubclassWindow();
}

void CObjectView::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	static int iLastItem = -1;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	LVITEM Item;
	char Buf[256];
	Item.iItem = pNMLV->iItem;
	
	if ( Item.iItem == iLastItem )
	{
		*pResult = 0;
		return;
	}	
	else
	{
		iLastItem = Item.iItem;
	}

	Item.iSubItem = 0;
	Item.mask = LVIF_TEXT;
	Item.pszText = Buf;
	Item.cchTextMax = 256;
	m_fileList.GetItem(&Item);
	LPCSTR p = m_fileList.GetPath();
	char c[256];
	strcpy(c,p);
	strcat(c,"\\");
	strcat(c,Buf);
//	((CMapEditorApp*)AfxGetApp())->BackToWorldView();
	sc::CXFile::GetInstance()->GetOperator()->SetActiveObject(c);
}

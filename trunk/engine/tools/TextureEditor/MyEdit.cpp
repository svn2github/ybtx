// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "TextureEditor.h"
#include "MyEdit.h"
#include "AutoBatDlg.h"

// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}
void CMyEdit::GetMsgNum(int msg)
{
	m_msg = msg;
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()
void CMyEdit::OnDropFiles(HDROP hDrop)
{

	TCHAR   szFilePathName[_MAX_PATH+1] = {0};
	string tmp;

	UINT  nFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); //得到文件个数

	for (UINT nIndex=0 ; nIndex< nFileCount; ++nIndex)
	{
		DragQueryFile(hDrop, nIndex, szFilePathName, _MAX_PATH);  //得到文件名
		tmp = szFilePathName;
		
		m_Path = tmp.c_str();


		if(m_msg==1)
			::SendMessage(this->GetParent()->GetSafeHwnd(),WM_DROP_MESSAGE,0,0);
		if(m_msg==2)
			::SendMessage(this->GetParent()->GetSafeHwnd(),WM_DROP_MESSAGE1,0,0);
		//abDlg->PostMessage(WM_DROP_MESSAGE,0,0);
		//GetDlgItem(IDD_AUTOBAT)->PostMessage(WM_DROP_MESSAGE,0,0);
	}
	DragFinish(hDrop);
	UpdateData(FALSE);
}


// CMyEdit message handlers



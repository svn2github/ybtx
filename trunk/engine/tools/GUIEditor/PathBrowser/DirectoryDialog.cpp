#include "stdafx.h"
#include "DirectoryDialog.h"

IMPLEMENT_DYNAMIC(CDirectoryDialog, CFileDialog)
CDirectoryDialog::CDirectoryDialog(CWnd* pParentWnd) :
CFileDialog(true, NULL, L"选择目录", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_NOVALIDATE, L"文件夹|*.||", pParentWnd)
{
}
BEGIN_MESSAGE_MAP(CDirectoryDialog, CFileDialog)
END_MESSAGE_MAP()

// CModelFileDialog message handlers

INT_PTR CDirectoryDialog::DoModal( CString& RootPath )
{
	// TODO: Add your specialized code here and/or call the base class
	if(RootPath.Trim().GetLength()>2)
		m_ofn.lpstrInitialDir = RootPath;
	INT_PTR re = CFileDialog::DoModal();
	if( re == IDOK )
		RootPath = m_FloderName;
	return re;
}

void CDirectoryDialog::OnFolderChange()
{
	// TODO: Add your specialized code here and/or call the base class
	m_FloderName = GetFolderPath();

	CFileDialog::OnFolderChange();
}

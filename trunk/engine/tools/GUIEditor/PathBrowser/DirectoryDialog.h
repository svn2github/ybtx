#pragma once
#include "afxcmn.h"
#include "CEditorConfig.h"


class CDirectoryDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CDirectoryDialog)
	CString			m_FloderName;

public:
	CDirectoryDialog(CWnd* pParentWnd = NULL);
	virtual INT_PTR DoModal( CString& RootPath );

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnFolderChange();
};

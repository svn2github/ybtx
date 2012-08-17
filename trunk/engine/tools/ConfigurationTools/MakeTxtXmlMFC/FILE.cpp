// FILE.cpp : implementation file
//

#include "stdafx.h"
#include "MakeTxtXmlMFC.h"
#include "FILE.h"


// CFILE

IMPLEMENT_DYNAMIC(CFILE, CFileDialog)

CFILE::CFILE(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{

}

CFILE::~CFILE()
{
}


BEGIN_MESSAGE_MAP(CFILE, CFileDialog)
END_MESSAGE_MAP()



// CFILE message handlers



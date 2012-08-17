#include "stdafx.h"
#include "CommonHelper.h"


const CString GetFileName( BOOL bOpenFileDialog, const TCHAR* Filter, OPENFILENAME* pOF/* = NULL*/, LPCTSTR InitName/* = NULL*/ )
{
	static TCHAR Buf[1024]	= {0};
	memset( Buf, 0, sizeof(Buf) );

	CFileDialog Dlg( bOpenFileDialog, NULL, InitName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, Filter );
	Dlg.m_ofn.lpstrInitialDir	= InitName;
	if( Dlg.DoModal() == IDCANCEL )
	{
		return  CString();
	}

	CString Filename = Dlg.GetPathName();
	string temp = Filename.GetBuffer();

	DWORD attr = GetFileAttributes(Filename);
	if(attr==-1)
	{
		MessageBox(NULL,Filename,"找不到文件",MB_OK);
		return "";
	}

	if( pOF )
		*pOF = Dlg.m_ofn;

	sprintf( Buf, "%s", (LPCTSTR)temp.c_str() );

	return Buf;
}

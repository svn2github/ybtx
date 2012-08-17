// GUIEditor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#include "GUIEditorDlg.h"
#include "GUIEditorHelper.h"

extern const wstring PATH_ALIAS_GUIEDITOR;

// CGUIEditorApp:
// 有关此类的实现，请参阅 GUIEditor.cpp
//

class CGUIEditorApp : public CWinApp
{
	CString				m_RootPath;
	CString				m_ResPath;
	CString				m_OpenPath;
	CString				m_TexPath;

	friend CString& GetRootPath();
	friend CString& GetResPath();
	friend CString& GetOpenPath();
	friend CString& GetTexPath();
public:
	CGUIEditorApp();
	virtual ~CGUIEditorApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

inline const CString GetFileName( BOOL bOpenFileDialog, const wchar_t* Filter, OPENFILENAME* pOF = NULL, LPCWSTR InitName = NULL )
{
	CString RootPath = GetTexPath();
	RootPath.MakeLower();

	if( GetOpenPath().IsEmpty() )
		GetOpenPath() = GetTexPath();

	CFileDialog Dlg( bOpenFileDialog, NULL, InitName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, Filter );
	Dlg.m_ofn.lpstrInitialDir = GetOpenPath();
	Dlg.DoModal();
	SetCurrentDirectory( RootPath );

	CString Filename = Dlg.GetPathName();
	Filename.MakeLower();
	int i = 0;
	for( ; i < RootPath.GetLength(); i++ )
	{
		if( RootPath[i] != Filename[i] )
			return CString();
	}
	if( pOF )
		*pOF = Dlg.m_ofn;

	GetOpenPath() = Dlg.GetPathName();
	return (LPCTSTR)Filename + i + 1;
}

inline const vector<CString> GetMutiFileName( const TCHAR* Filter )
{
	if( GetOpenPath().IsEmpty() )
		GetOpenPath() = GetResPath();
	CString ResPath = GetResPath();
	ResPath.MakeLower();
	CFileDialog Dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, Filter );
	Dlg.m_ofn.lpstrInitialDir = GetOpenPath();
	Dlg.DoModal();
	SetCurrentDirectory( ResPath );

	vector<CString> FileNames;

	POSITION Pos = Dlg.GetStartPosition();
	while( Pos )
	{
		CString Filename = Dlg.GetNextPathName( Pos );
		Filename.MakeLower();
		int i = 0;
		for( ; i < ResPath.GetLength(); i++ )
		{
			if( ResPath[i] != Filename[i] )
				break;
		}
		if( ResPath.GetLength() != i )
			continue;
		FileNames.push_back( (LPCTSTR)Filename + i + 1 );
	}
	GetOpenPath() = Dlg.GetPathName();
	return FileNames;
}



inline CString GetRegistKeyStr( LPCWSTR Path, LPCWSTR Name )
{
	HKEY hKey = NULL;
	DWORD Len = 1024;
	DWORD Type = REG_SZ;
	BYTE  Buf[1024];
	RegCreateKey( HKEY_LOCAL_MACHINE, Path, &hKey );
	if( ERROR_SUCCESS != RegQueryValueEx( hKey, Name, NULL, &Type, Buf, &Len ) )
		Buf[0] = 0;
	RegCloseKey(hKey);
	return (wchar_t*)Buf;
}

inline void SetRegistKeyStr( LPCTSTR Path, LPCTSTR Name, LPCTSTR str )
{
	HKEY hKey = NULL;
	RegCreateKey( HKEY_LOCAL_MACHINE, Path, &hKey );
	RegSetValueEx( hKey, Name, NULL, REG_SZ, (BYTE*)str, (DWORD)wcslen( str ) );
	RegCloseKey(hKey);
}

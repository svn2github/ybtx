#include "stdafx.h"
#include "GUIEditorHelper.h"
#include "CPkgFile.h"
#include "CPathMgr.h"
#include "GUIEditor.h"
#include "StringHelper.h"

extern CGUIEditorApp theApp;

CString& GetRootPath()
{
	return theApp.m_RootPath;
}

CString& GetResPath()
{
	return theApp.m_ResPath;
}

CString& GetOpenPath()
{
	return theApp.m_OpenPath;
}

CString& GetTexPath()
{
	return theApp.m_TexPath;
}

const vector<CString> GetMutiFileNameTex( const TCHAR* Filter)
{
	CString ResPath = GetTexPath();
	ResPath.MakeLower();

	if( GetOpenPath().IsEmpty() )
		GetOpenPath() = ResPath;

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

		CString LastFileName = (LPCTSTR)Filename + i + 1;
		FileNames.push_back( LastFileName );
	}
	GetOpenPath() = Dlg.GetPathName();

	return FileNames;
}

void MakePathLower( wstring& path )
{
	tolower(path);	
	backslash_to_slash(path);
}
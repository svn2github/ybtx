#include "stdafx.h"
#include "Package.h"
#include "CPathMgr.h"
#include "StringHelper.h"

const wstring VERIFY_ERROR = L"Verify package failed: ";

struct PkgVerifyFailedInfo
{
	PkgVerifyFailedInfo(const wstring& path, const wstring& msg)
		: full_path(path)
	{
		failed_msg =  VERIFY_ERROR + full_path + L"\n";
		failed_msg += L"    " + msg + L"\n";
	}

	wstring	full_path;
	wstring failed_msg;
};

typedef list<PkgVerifyFailedInfo> VerifyInfoList;
static VerifyInfoList	g_VerifyList;

FTW_RESULT ProcessPackageFile( const wchar_t* szCurFile, const struct _stat* pFileInfo, FTW_FLAG fileType, void* pContext )
{
	wstring szFileIn(szCurFile);
	tolower(szFileIn);
	backslash_to_slash(szFileIn);

	if ( fileType == _FTW_DIR || szFileIn.find(L".svn") != wstring::npos || szFileIn.find(L".pkg") == wstring::npos )
		return _FTW_CONTINUNE;

	wstring szFailedMsg;
	CVerifyPackage pkg(szFileIn);
	if ( !pkg.IsVerify(szFailedMsg) )
	{
		g_VerifyList.push_back(PkgVerifyFailedInfo(szFileIn, szFailedMsg));
	}
	
	return _FTW_CONTINUNE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( argc != 2 )
	{
		cout << "Usage: FixPackage.exe package_files_path\n\n";
		system("pause");
		return 1;
	}

	FileTreeWalk(argv[1], &ProcessPackageFile, NULL);
	
	wcout.imbue(locale("chs"));
	VerifyInfoList::iterator it,eit = g_VerifyList.end();
	for( it = g_VerifyList.begin(); it!=eit; ++it )
		wcout << (*it).failed_msg.c_str();
	
	system("pause");
	return 0;
}


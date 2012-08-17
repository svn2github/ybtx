#include "stdafx.h"
#include "ReadPkgCtrl\ReadPkgCtrlImp.h"
#include "CPkgFile.h"
#include "CPkgDirectory.h"
#include "GraphicBase.h"
#include "DebugHelper.h"
#include "StringHelper.h"

DEF_UI_IMP(CReadPkgCtrlImp);
CReadPkgCtrlImp::CReadPkgCtrlImp( void* param )
{

}

CReadPkgCtrlImp::~CReadPkgCtrlImp( void )
{

}

bool CReadPkgCtrlImp::GetFileInfo( const string& filepath, PkgDebugInfo* pdebugInfo )
{
	/*CPkgFile PkgFile;
	if( PkgFile.Open( PATH_ALIAS_RES.c_str(), filepath.c_str() ) != eFE_SUCCESS )
		return false;
	else
	{
		PkgDebugInfo pkgdebug = GetPkgDebugInfo(PkgFile);
		pdebugInfo = &pkgdebug;
	}*/

	return true;
}

void CReadPkgCtrlImp::ReadPkgFiles( const vector<string> pkgs )
{
	if( pkgs.empty() )
		return;

	string strPkg("");
	size_t size = pkgs.size();
	for( size_t i = 0; i < size; ++i )
	{
		strPkg = pkgs[i];

		CPkgDir pkg;
		uint32 nPkgFileSize = pkg.Open(PATH_ALIAS_RES.c_str(), strPkg.c_str());

		string msg;
		format("%s:包内文件个数是%s", strPkg.c_str(), nPkgFileSize);
		::MessageBox(NULL, msg.c_str(), "提示", MB_OK);
	}
}	
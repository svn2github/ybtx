#include "stdafx.h"
#include "ClearSrcTempCfg.h"
#include "CSplitInfo.h"
#include "ConsoleInfo.h"
#include <cstdlib>
ClearSrcCfg::ClearSrcCfg()
{	
	m_pSplitInfo = CSplitInfo::GetInst();
	m_strSrc = m_pSplitInfo->GetSrcPath();
}
ClearSrcCfg::~ClearSrcCfg()
{
}
void ClearSrcCfg::StartProcess()
{
	CConsoleInfo::GetInst()->PrintFunction("Src中间配置表删除");
	wstring wstrPath = gbk_to_utf16(m_strSrc);
	FileTreeWalk(wstrPath.c_str(), MakeSrc, NULL);
	string strSrcLanguagePath = CSplitInfo::GetInst()->GetSrcPath();
	strSrcLanguagePath = strSrcLanguagePath+"/language";
	replace_all_distinct(strSrcLanguagePath,"/","\\",0);
	string str = "rd "+strSrcLanguagePath+" /s /q";
	system(str.c_str());
	cout<<"language delete"<<endl;
	

}
FTW_RESULT ClearSrcCfg::MakeSrc( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );

	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	if(( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) )&&(_wcsicmp( szFileName + nLen - 4,L".txt")))
		return _FTW_CONTINUNE;
	ClearSrcCfg* pManager = ClearSrcCfg::GetInst();
	string strFileName = utf16_to_gbk(szFileName);
    pManager->DeleteSrc(strFileName);
	return _FTW_CONTINUNE;
	
}
void ClearSrcCfg::DeleteSrc(string & strFileName)
{
	if((m_pSplitInfo->BeAppDif(strFileName)==true)||(m_pSplitInfo->BeLangDif(strFileName)==true))
	{
		if(ForceDeleteFile(strFileName)==true)
		{
			cout<<strFileName<<endl;
		}
		else
		{
			string strMessage = strFileName + "删除失败，请确认是否有其他程序在使用中";
			MessageBox(0,strMessage.c_str(),"错误",NULL);
		}
	}
}
bool ClearSrcCfg::ForceDeleteFile(const string& strFile)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strFile.c_str(),&fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	string strFullPath = strFile;
	strFullPath.resize(strFullPath.length()+1);
	SHFILEOPSTRUCT op;
	memset(&op,0,sizeof(op));
	op.pFrom = strFullPath.c_str();
	op.pTo = NULL;
	op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	op.wFunc = FO_DELETE;
	op.hwnd = NULL;
	op.hNameMappings = NULL;
	if (SHFileOperation(&op))
	{
		return false;
	}
	return true;
}
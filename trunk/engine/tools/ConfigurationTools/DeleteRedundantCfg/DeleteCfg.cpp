#include "stdafx.h"
#include "DeleteCfg.h"
#include "CSplitInfo.h"
#include "ConsoleInfo.h"
#include <iostream>
using namespace std;
CDeleteCfg::CDeleteCfg()
{
	m_pSplitInfo=CSplitInfo::GetInst();
	strTgt = m_pSplitInfo->GetTgtPath();
	strSrc = m_pSplitInfo->GetSrcPath();
	strTemp = m_pSplitInfo->GetCfgTempPath();
}
CDeleteCfg::~CDeleteCfg()
{

}
void CDeleteCfg::StartProcess()
{
	CConsoleInfo::GetInst()->PrintFunction("多余配置表检测");
	CConsoleInfo::GetInst()->PrintFunction("temp和src同步检测");
	uint64 time = GetCurrentTime();
	wstring wstrPath = gbk_to_utf16(strSrc);
	FileTreeWalk(wstrPath.c_str(), MakeFilePath, NULL);
	wstring wstrTemp = gbk_to_utf16(strTemp);
	FileTreeWalk(wstrTemp.c_str(), CheckTemp, NULL);
	if(!m_vFile.empty())
	{
		CConsoleInfo::GetInst()->PrintFunction("开始删除temp冗余文件");
		DeleteCfg(m_vFile);
	}
	time = GetCurrentTime()-time;
	printf("temp和src同步检测完成，共计耗时%d毫秒\n",time);
	time = GetCurrentTime();
	CConsoleInfo::GetInst()->PrintFunction("tgt和src同步检测");
	wstring wstrTgt = gbk_to_utf16(strTgt);
	FileTreeWalk(wstrPath.c_str(), MakeAllFilePath, NULL);
	FileTreeWalk(wstrTgt.c_str(), CheckTgt, NULL);
	if(!m_vAllFile.empty())
	{
		CConsoleInfo::GetInst()->PrintFunction("开始删除tgt冗余文件");
		DeleteCfg(m_vAllFile);
	}
	time = GetCurrentTime()-time;
	printf("tgt和src同步检测完成，共计耗时%d毫秒\n",time);
	time = GetCurrentTime();
	CConsoleInfo::GetInst()->PrintFunction("Chinese_Simple和src同步检测");
	strLangFile = m_pSplitInfo->GetLangTablePath();
	wstring wstrLangFile = gbk_to_utf16(strLangFile);
	FileTreeWalk(wstrLangFile.c_str(), CheckLanguage, NULL);
	if(!m_vLangFile.empty())
	{
		CConsoleInfo::GetInst()->PrintFunction("开始删除Chinese_Simple冗余文件");
		DeleteCfg(m_vLangFile);
	}
    time = GetCurrentTime()-time;
	printf("Chinese_Simple和src同步检测完成，共计耗时%d毫秒\n",time);
}
FTW_RESULT CDeleteCfg::MakeFilePath( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	wstring strNdfPostfix = CSplitInfo::GetInst()->WGetCppCfgExtend();

	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) 
		&& _wcsicmp( szFileName + nLen - strNdfPostfix.length(),strNdfPostfix.c_str() ) )
		return _FTW_CONTINUNE;

	CDeleteCfg* pManager = CDeleteCfg::GetInst();
	string strFileName = utf16_to_gbk(szFileName);
    pManager->Add2Map(strFileName);
	
	return _FTW_CONTINUNE;
	
}
void CDeleteCfg::Add2Map(string &strFileName)
{
	string str=strFileName;
	size_t pos = str.rfind("/");
	if (pos != -1)
	{
		str = str.substr(pos+1,str.length()-pos-1);
	}
	if(m_pSplitInfo->BeInSplitList(strFileName))
	{
		m_mFilePath.insert(make_pair(strFileName,m_pSplitInfo->GetAppName(str)));
	}
  
}
FTW_RESULT CDeleteCfg::CheckTemp( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	wstring strTxtPostfix = gbk_to_utf16(".txt");

	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) &&_wcsicmp(szFileName+nLen-strTxtPostfix.length(),strTxtPostfix.c_str()))
		return _FTW_CONTINUNE;

	CDeleteCfg* pManager = CDeleteCfg::GetInst();
	string strFileName = utf16_to_gbk(szFileName);
    pManager->CheckTempFile(strFileName);
	
	return _FTW_CONTINUNE;
	
}
void CDeleteCfg::CheckTempFile(string &strFileName)
{
	bool Mark=false;
	//获取当前文件完整存储路径
	size_t g_pos = strFileName.rfind("/");
	string strFilePath = strFileName;
	strFilePath = strFilePath.substr(0,g_pos);
	map<string,string>::iterator iter = m_mFilePath.begin();
	for(;iter!=m_mFilePath.end();iter++)
	{
		string ss = iter->first;
		size_t pos = ss.rfind("/");
		if (pos != -1)
		{
			ss = ss.substr(pos+1,ss.length()-pos-1);
		}
		//获取语言表真实名
		string strLangName = m_pSplitInfo->GetLangName(ss);
		string strTxtLangName = strLangName;
		strLangName = strLangName.substr(0,strLangName.size()-4);
		string strPostfix = m_pSplitInfo->GetLuaCfgExtend();
		strLangName.append(strPostfix);
		//获取配置表完整的语言表存储路径	
		string strLangSavePath = "";
		string strLang = strTemp;
		strLangSavePath = m_pSplitInfo->GetLangSavePath(ss);
		replace_all_distinct(strLangSavePath,"chinese_simple","langsrc",0);
		strLangSavePath = strLangSavePath.substr(1,strLangSavePath.size()-1);
		strLangSavePath = strLang.append(strLangSavePath);
		//获取配置表完整的非语言表存储路径
		string strAppSavePath = "";
		string strApp = strTemp;
		strAppSavePath = m_pSplitInfo->GetAppSavePath(ss);
		if(strAppSavePath!="")
		{
			strAppSavePath = strAppSavePath.substr(1,strAppSavePath.size()-1);
		}
		strAppSavePath = strApp.append(strAppSavePath);
		//获取非语言表真实名
		string strAppName = m_pSplitInfo->GetAppName(ss);
		strAppName = strAppName.substr(0,strAppName.size()-4);
		strAppName.append(strPostfix);
		if(strFileName.find((m_pSplitInfo->GetLangSrcPath()).c_str())!=string::npos)
		{
			if((strFileName.find(strLangName.c_str())!=string::npos)||(strFileName.find(strTxtLangName.c_str())!=string::npos))
			{
				if(strFilePath==strLangSavePath)
				{
					Mark=true;
					break;
				}
			}
		}
		else if((strFileName.find(strAppName.c_str())!=string::npos))
		{
			if((strAppName!=strPostfix)&&(strFilePath==strAppSavePath))
			{
				Mark=true;
				break;
			}
		}
					
	}
	if(Mark==false)
	{
		m_vFile.push_back(strFileName);
	}
}
FTW_RESULT CDeleteCfg::MakeAllFilePath( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	wstring strNdfPostfix = CSplitInfo::GetInst()->WGetCppCfgExtend();

	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) 
		&& _wcsicmp( szFileName + nLen - strNdfPostfix.length(),strNdfPostfix.c_str() ) )
		return _FTW_CONTINUNE;

	CDeleteCfg* pManager = CDeleteCfg::GetInst();
	string strFileName = utf16_to_gbk(szFileName);
    pManager->AddAll2Map(strFileName);
	
	return _FTW_CONTINUNE;
	
}
void CDeleteCfg::AddAll2Map(string &strFileName)
{
	string str=strFileName;
	size_t pos = str.rfind("/");
	if (pos != -1)
	{
		str = str.substr(pos+1,str.length()-pos-1);
	}
	m_mAllFilePath.insert(make_pair(strFileName,m_pSplitInfo->GetAppName(str)));
}
FTW_RESULT CDeleteCfg::CheckTgt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = gbk_to_utf16(".txt");
	wstring strXmlPostfix = gbk_to_utf16(".xml");

	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) && _wcsicmp( szFileName + nLen - strXmlPostfix.length(),strXmlPostfix.c_str() ) )
		return _FTW_CONTINUNE;

	CDeleteCfg* pManager = CDeleteCfg::GetInst();
	string strFileName = utf16_to_gbk(szFileName);
    pManager->CheckTgtFile(strFileName);
	
	return _FTW_CONTINUNE;
	
}
void CDeleteCfg::CheckTgtFile(string &strFileName)
{
	string str=strFileName;
	size_t pos = str.rfind("/");
	if (pos != -1)
	{
		str = str.substr(pos+1,str.length()-pos-1);
	}
	bool mark =false;
	str = str.substr(0,str.size()-4);
	map<string,string>::iterator iter = m_mAllFilePath.begin();
	for(;iter!=m_mAllFilePath.end();iter++)
	{
		string strtemp=iter->first;
		size_t pos = strtemp.rfind("/");
		if (pos != -1)
		{
			strtemp = strtemp.substr(pos+1,strtemp.length()-pos-1);
		}
		string ss  = m_pSplitInfo->GetAppName(strtemp);
		strtemp = strtemp.substr(0,strtemp.size()-4);
		ss = ss.substr(0,ss.size()-4);
		//当tgt下文件名和src目录下文件名相同或者和src目录下拆分出的非语言表名相同跳出循环。
		if((strtemp==str)||(ss == str))
		{
			mark = true;
			break;
		}
	}
	if(mark == false)
	{
		m_vAllFile.push_back(strFileName);
	}
}
FTW_RESULT CDeleteCfg::CheckLanguage( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	size_t nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = gbk_to_utf16(".txt");
	wstring strXmlPostfix = gbk_to_utf16(".xml");


	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) 
		&& _wcsicmp( szFileName + nLen - strXmlPostfix.length(),strXmlPostfix.c_str() ) )
		return _FTW_CONTINUNE;

	CDeleteCfg* pManager = CDeleteCfg::GetInst();
	string strFileName = utf16_to_gbk(szFileName);
    pManager->CheckLanguageFile(strFileName);
	
	return _FTW_CONTINUNE;
	
}
void CDeleteCfg::CheckLanguageFile(string &strFileName)
{
	string str=strFileName;
	size_t pos = str.rfind("/");
	if (pos != -1)
	{
		str = str.substr(pos+1,str.length()-pos-1);
	}
	bool mark=false;
	str = str.substr(0,str.size()-4);
	map<string,string>::iterator iter = m_mFilePath.begin();
	for(;iter!=m_mFilePath.end();iter++)
	{
		string strtemp=iter->first;
		size_t pos = strtemp.rfind("/");
		if (pos != -1)
		{
			strtemp = strtemp.substr(pos+1,strtemp.length()-pos-1);
		}
		string ss  = m_pSplitInfo->GetLangName(strtemp);
		ss = ss.substr(0,ss.size()-4);
		if(ss == str)
		{
			mark = true;
			break;
		}
	}
	if(mark == false)
	{
		m_vLangFile.push_back(strFileName);
	}
}
bool CDeleteCfg::ForceDeleteFile(const string& strFile)
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
void CDeleteCfg::DeleteCfg(vector<string> vec)
{
	CConsoleInfo::GetInst()->PrintFunction("冗余文件列表");
	vector<string>::iterator iter = vec.begin();
	for(;iter!=vec.end();iter++)
	{
		cout<<*iter<<endl;
	}
	CConsoleInfo::GetInst()->PrintFunction("开始删除");
	iter = vec.begin();
	string input("");
	while(iter!=vec.end())
	{
		input="";
		string str = *iter;
		cout<<str<<endl;
		CConsoleInfo::GetInst()->PrintFunction("是否删除?是请按y");
		cin>>input;
		if(input=="y")
		{
			if(ForceDeleteFile(*iter))
			{
				CConsoleInfo::GetInst()->PrintFunction("删除成功");
			}
			else
			{
				string strMessage = *iter + "删除失败，请确认是否有其他程序在使用中";
				GenErr(strMessage);
				//MessageBox(0,strMessage.c_str(),"错误",NULL);
			}
		}
		iter++;
	}
}
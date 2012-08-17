#include "stdafx.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "CatHelper.h"
#include "ErrLogHelper.h"
#include "CPathMgr.h"
#include "CodeCvs.h"


#ifdef _WIN32
#else
#include <libgen.h>
#endif

const char* szHelp = "\n"					
"errlog文件查看工具，用于查看info和comm文件\n"
"在命令行下处理格式为\n"
"\n"
"	CatLog gas[c]_err_(sz_guid)_num[_username_]v(version).info[comm] [-path MapFileDir] [-o OutFile]\n"
"\n"
"		sz_guid 为生成log文件的时候的guid信息\n"
"		num 为第几次生成的log"
"		username 用来表明客户端上传的ip\n"
"		version 表明当前log文件的版本号\n"
"		MapFileDir 指明对应版本version的mapfile所在目录，并且目录名字必须已该版本号命名\n"
"		OutFile 指明输出到某一个文件\n"
"		程序会将正确解析之后的文件显示到控制台\n"
"\n"
"	CatLog input_path [-path MapFileDir] -o output_path\n"
"		input_path 为指定处理的log路径\n"
"		MapFileDir 指明对应版本version的mapfile所在目录\n"
"		output_path 指明输出log的路径\n"
"		程序将把处理的log输出到指定的地方\n"
"\n";

void PrintUsage()
{
	printf("%s\n", szHelp);	
}

void PrintMessage(const char* msg)
{
	printf("%s\n", msg);
	PrintUsage();
}

bool IsDir(const char* szFilePath)
{
	struct stat buf;

	int ret = stat(szFilePath, &buf);
	if(ret == -1)
		return false;

	return ((buf.st_mode & S_IFDIR) == S_IFDIR);
}

bool IsFile(const char* szFileName)
{
	struct stat buf;

	stat(szFileName, &buf);

	return ((buf.st_mode & S_IFREG) == S_IFREG);
}


void GetBaseName(const char* szFileName, string& strBaseName)
{
#ifdef _WIN32
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( szFileName, drive, dir, fname, ext ); 

	strBaseName.append(fname).append(ext);
#else
	strBaseName = basename((char*)szFileName);
#endif
}

int CheckFileName(CatFileNameData& data)
{
	string& strParam = data.m_strBaseName;

	GetBaseName(data.m_strFileName.c_str(), strParam);

	bool bGac = false;
	bool bInfo = false;

	if(strParam.find("gas_err") == 0)
	{
		bGac = false;
	}
	else if(strParam.find("gac_err") == 0)
	{
		bGac = true;
	}
	else
	{
		return -1;
	}

	if(strParam.find(".info") != string::npos)
	{
		bInfo = true;
	}
	else if(strParam.find(".comm") != string::npos)
	{
		bInfo = false;
	}
	else
	{
		return -1;
	}

	uint32 uVersion = GetRevisionFromLogFile(strParam.c_str());

	data.m_uVersion = uVersion;
	data.m_bGac = bGac;
	data.m_bInfo = bInfo;


	return 0;
}

int HandleFile(const char* szInput, const char* szOutput)
{
	CatFileNameData data;
	data.m_strFileName = szInput;

	if(szOutput)
	{
		data.m_strOutFile = szOutput;
	}

	int ret = CheckFileName(data);
	if(ret != 0)
	{
		PrintMessage("Error 输入的文件名格式不正确");
		return -1;
	}
	CatFile(data);

	return 0;
}

namespace sqr
{
	static FTW_RESULT FetchErrLog( const char* szFileName, const struct _stat * pStat, FTW_FLAG eFlag, void *pContext )
	{
		if( eFlag == _FTW_DIR )
			return _FTW_IGNORE;

		if( eFlag == _FTW_FILE) // 处理文件
		{
			string strOutFile((char*)pContext);
			string szBaseName;
			GetBaseName(szFileName, szBaseName);
			strOutFile.append("/").append(szBaseName).append(".log");
			HandleFile(szFileName, strOutFile.c_str());
		}

		return _FTW_CONTINUNE;
	}
}

static FTW_RESULT FetchErrLogUtf16( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void *pContext )
{
#ifdef _WIN32
	string mbs_tmp = utf16_to_gbk(szFileName);
#else
	string mbs_tmp = utf16_to_utf8(szFileName);
#endif
	return FetchErrLog(mbs_tmp.c_str(), 0, eFlag, pContext);
}

int HandleDir(const char* szInput, const char* szOutput)
{
	FileTreeWalk(gbk_to_utf16(szInput).c_str(), FetchErrLogUtf16, (void*)szOutput);
	return 0;	
}

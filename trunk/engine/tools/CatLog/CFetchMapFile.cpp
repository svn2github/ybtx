#include "stdafx.h"
#include "CFetchMapFile.h"
#include <sys/stat.h>
#include <errno.h>
#include "CMapFileMgr.h"
#include "CodeCvs.h"
#include "CatHelper.h"


#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define F_OK 0
#else
#include <unistd.h>
#define _access access
#endif

CFetchMapFile::CFetchMapFile()
{
}

CFetchMapFile::~CFetchMapFile()
{

}

CFetchMapFile& CFetchMapFile::Inst()
{
	static CFetchMapFile ls_Inst;
	return ls_Inst;
}

void CFetchMapFile::SetPath(const char* szPath)
{
	if(!szPath)
		return;

	m_strMapFilePath.assign(szPath);
}

void CFetchMapFile::Fetch(uint32 uVersion, bool bGac)
{
	list<string> lstMapFile;

	if(m_strMapFilePath.empty())
		return ;

	
	string strRunModule;
	string strModule;
	string strGame;
	string strVersion;

	if(uVersion != 0)
	{
		char buf[24];
		sprintf(buf, "_%u", uVersion);
		strVersion.append(buf);
	}

	//查看版本目录是否存在
	string strFilePath;
	strFilePath.assign(m_strMapFilePath);
	
	char szVersion[24];
	sprintf(szVersion, "/%u", uVersion);
	strFilePath.append(szVersion);

	if(!IsDir(strFilePath.c_str()))
	{
		return;		
	}

	strRunModule.append(strFilePath).append("/RunModule.map");
	strModule.append(strFilePath).append("/Module.map");
	
	if(bGac)
		strGame.append(strFilePath).append("/GameGac.map");
	else
		strGame.append(strFilePath).append("/GameGas.map");

	
	lstMapFile.push_back(strGame);
	lstMapFile.push_back(strModule);
	lstMapFile.push_back(strRunModule);


	for(list<string>::iterator it = lstMapFile.begin(); it != lstMapFile.end(); ++it)
	{
		CMapFileMgr::Inst().HandleMapFile(gbk_to_utf16(*it).c_str(), uVersion);
	}
}


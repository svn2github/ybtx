#include "stdafx.h"
#include "CMapFileMgr.h"
#include "CMapFile.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "TimeHelper.h"

CMapFileMgr::CMapFileMgr()
{

}

CMapFileMgr::~CMapFileMgr()
{
	ClearMapFile();
}

CMapFileMgr& CMapFileMgr::Inst()
{
	static CMapFileMgr ls_Inst;
	return ls_Inst;
}

void CMapFileMgr::ClearMapFile()
{
	SafeDelCtn2(m_mapVersionToMapFile);
}

CMapFile* CMapFileMgr::GetMapFile(uint32 uVersion)
{
	MapVersionToMapFile_t::iterator iter = m_mapVersionToMapFile.find(uVersion);
	if(iter != m_mapVersionToMapFile.end())
		return iter->second;

	return NULL;
}

void CMapFileMgr::HandleMapFile(const wchar_t* szFileName, uint32 uVersion)
{
	if(uVersion == 0)
		uVersion = GetCurrentRevision();

	CMapFile* pMapFile = GetMapFile(uVersion);
	if(!pMapFile)
	{
		pMapFile = new CMapFile;
		m_mapVersionToMapFile.insert(pair<uint32, CMapFile*>(uVersion, pMapFile));
	}
	pMapFile->HandleMapFile(szFileName);
}

const char* CMapFileMgr::FindFunName(uint32 uFunAddr, uint32 uVersion)
{
	if(uVersion == 0)
		uVersion = GetCurrentRevision();

	CMapFile* pMapFile = GetMapFile(uVersion);
	if(!pMapFile)
		return "";

	return pMapFile->FindFunName(uFunAddr);
}


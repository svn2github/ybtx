
#include "stdafx.h"
#include "VersionManager.h"
#include "CodeCvs.h"

using namespace std;

#define  INI_FILE_NAME  L"../../etc/gac/Update.ini"
#define  VERSION_FLAG   "Version="
#define  LINE_MAX_SIZE  4096




VersionManager::VersionManager(IAsyncUpdateHandler* pUpadteHandler)
	: m_UpadteHandler(pUpadteHandler)
{
}

VersionManager::~VersionManager()
{
	map<string, char*>::iterator iterMap;
	for (iterMap = m_mapPatchList.begin(); iterMap != m_mapPatchList.end(); ++iterMap)
	{
		delete[] iterMap->second;
		iterMap->second = NULL;
	}
}

static bool CheckVersion(ReleaseVersion* NewVer, ReleaseVersion* OldVer)
{
	if ( NewVer->uSequelVersion > OldVer->uSequelVersion )
	{
		return true;
	}
	else if ( NewVer->uSequelVersion == OldVer->uSequelVersion  )
	{
		if ( NewVer->uBranchVersion > OldVer->uBranchVersion )
		{
			return true;
		}
		else if ( NewVer->uBranchVersion == OldVer->uBranchVersion )
		{
			if ( NewVer->uRevision > OldVer->uRevision )
			{
				return true;
			}
			else if (NewVer->uRevision == OldVer->uRevision)
			{
				if (NewVer->uSVersion > OldVer->uSVersion)
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return false;
}

static void ClearLineEnd(char* str)
{
	while ( *str != 10 && *str != 13 )
		++str;
	*str = '\0';
}

static void ExtractVersion(char* str)
{
	while ( (*str >= 48 && *str <= 57) || *str == '.' )
		++str;
	*str = '\0';
}

static void ParseVersion(const char* szVersion, int32& nVersionPos, int32& nBaseVersionPos)
{
	int32 nCurPos = -1;
	while ((*szVersion >= 48 && *szVersion <= 57) || *szVersion == '.')
	{
		++nCurPos;
		++szVersion;
	}
	nVersionPos = nCurPos;
	while (!((*szVersion >= 48 && *szVersion <= 57) || *szVersion == '.'))
	{
		++nCurPos;
		++szVersion;
	}
	nBaseVersionPos = nCurPos + 1;
}

static void MyStrcpy(const char* szSrc, char* szDest, int32 nStartPos, int32 nCount)
{
	for (int32 i = 0; i < nCount; i++)
	{
		szDest[i] = szSrc[i + nStartPos];
	}
	szDest[nCount] = '\0';
}

void VersionManager::GetVersionString(const wchar_t* wszFile, char* szVersion)
{
	assert(szVersion);

	m_PatchList.clear();
	FILE *fp = _wfopen(INI_FILE_NAME, L"rb");
	if (fp == NULL)
	{
		m_UpadteHandler->AlertExit(L"无法打开Update.ini配置文件");
	}

	fseek(fp, 0, SEEK_END);
	uint32 nFileLen = ftell(fp);
	char* szBuf = new char[nFileLen + 1];
	memset(szBuf, 0, nFileLen + 1);

	fseek(fp, 0, SEEK_SET);
	fread(szBuf, sizeof(char), nFileLen, fp);
	fclose(fp);

	char* szTemp = strstr(szBuf, VERSION_FLAG);
	szTemp += strlen(VERSION_FLAG);
	ExtractVersion(szTemp);

	strcpy(szVersion, szTemp);

	delete[] szBuf;
	szBuf = NULL;
	fclose(fp);
}

void VersionManager::GetPatchListInfo(const wchar_t* wszFile)
{
	assert(wszFile);
	ifstream ifsPatchList(wszFile);
	if (!ifsPatchList)
	{
		m_UpadteHandler->AlertExit(L"无法打开已经下载完成的补丁列表文件");
	}

	getline(ifsPatchList, m_strPatchPath);

	char* szTgtVersion = NULL;
	char* szBaseVersion = NULL;
	string strVersionLine;
	int32 nVersionPos, nBaseVersionPos;

	while (getline(ifsPatchList, strVersionLine))
	{
		if (strcmp(strVersionLine.c_str(), "") == 0)
		{
			break;
		}
		szTgtVersion = new char[strlen(strVersionLine.c_str()) + 1];
		szBaseVersion = new char[strlen(strVersionLine.c_str()) + 1];
		ParseVersion(strVersionLine.c_str(), nVersionPos, nBaseVersionPos);
		MyStrcpy(strVersionLine.c_str(), szTgtVersion , 0, nVersionPos + 1);
		MyStrcpy(strVersionLine.c_str(), szBaseVersion, nBaseVersionPos, strlen(strVersionLine.c_str()) - nBaseVersionPos);
		m_mapPatchList[szTgtVersion] = new char[strlen(szBaseVersion) + 1];
		strcpy(m_mapPatchList[szTgtVersion], szBaseVersion);
		delete[] szTgtVersion;
		szTgtVersion = NULL;
		delete[] szBaseVersion;
		szBaseVersion = NULL;
	}
}

void VersionManager::GetPatchType(const char* szSrc, const char* szTgt)
{
	assert(szSrc && szTgt);
	uint32 uSrcSequelVersion = 0;
	uint32 uSrcBranchVersion = 0;
	uint32 uSrcRevision = 0;
	uint32 uSrcSVersion = 0;
	uint32 uRet = sscanf(szSrc , "%d.%d.%d.%d", &uSrcSequelVersion, &uSrcBranchVersion, &uSrcRevision, &uSrcSVersion);
	if (uRet != 4)
	{
		return;	
	}
	uint32 uTgtSequelVersion = 0;
	uint32 uTgtBranchVersion = 0;
	uint32 uTgtRevision = 0;
	uint32 uTgtSVersion = 0;
	uRet = sscanf(szTgt , "%d.%d.%d.%d", &uTgtSequelVersion, &uTgtBranchVersion, &uTgtRevision, &uTgtSVersion);
	if (uRet != 4)
	{
		return;
	}
	if (uSrcSequelVersion != uTgtSequelVersion)
	{
		return;
	}

	if (uSrcBranchVersion != uTgtBranchVersion)
	{
		m_pMemFun = &VersionManager::PatchBranchVersion;
		return;
	}

	if (uSrcRevision != uTgtRevision)
	{
		m_pMemFun = &VersionManager::PatchReVersion;
		return;
	}

	if (uSrcSVersion != uTgtSVersion)
	{
		m_pMemFun = &VersionManager::PatchSmallVersion;
		return;
	}
}

void VersionManager::PatchSmallVersion(const char* szSrc, const char* szTgt)
{
	assert(szSrc && szTgt);
	uint32 uSrcSequelVersion = 0;
	uint32 uSrcBranchVersion = 0;
	uint32 uSrcRevision = 0;
	uint32 uSrcSVersion = 0;
	uint32 uRet = sscanf(szSrc , "%d.%d.%d.%d", &uSrcSequelVersion, &uSrcBranchVersion, &uSrcRevision, &uSrcSVersion);
	if (uRet != 4)
	{
		return;	
	}
	uint32 uTgtSequelVersion = 0;
	uint32 uTgtBranchVersion = 0;
	uint32 uTgtRevision = 0;
	uint32 uTgtSVersion = 0;
	uRet = sscanf(szTgt , "%d.%d.%d.%d", &uTgtSequelVersion, &uTgtBranchVersion, &uTgtRevision, &uTgtSVersion);
	if (uRet != 4)
	{
		return;
	}
	
	if (uSrcSequelVersion != uTgtSequelVersion || uSrcBranchVersion != uTgtBranchVersion || uSrcRevision != uTgtRevision)
	{
		return;
	}

	UpdatePacket packet;
	char szVersion[20];
	if (uSrcSVersion < uTgtSVersion) // 第四位版本号升级
	{
		uSrcSVersion++;
		while (uSrcSVersion <= uTgtSVersion)
		{
			packet.ver.uReserve = 0;
			packet.ver.uSequelVersion = uSrcSequelVersion;
			packet.ver.uBranchVersion = uSrcBranchVersion;
			packet.ver.uRevision = uSrcRevision;
			packet.ver.uSVersion = uSrcSVersion;
			sprintf(szVersion, "%d.%d.%d.%d", uSrcSequelVersion, uSrcBranchVersion, uSrcRevision, uSrcSVersion);
			packet.url = m_strPatchPath+ "upgrade_" + szVersion + ".ybtx";
			m_PatchList.push_back(packet);
			uSrcSVersion++;
		}
	}
	else if (uSrcSVersion > uTgtSVersion) // 第四位版本号降级
	{
		while (uSrcSVersion > uTgtSVersion)
		{
			packet.ver.uReserve = 0;
			packet.ver.uSequelVersion = uSrcSequelVersion;
			packet.ver.uBranchVersion = uSrcBranchVersion;
			packet.ver.uRevision = uSrcRevision;
			packet.ver.uSVersion = uSrcSVersion;
			sprintf(szVersion, "%d.%d.%d.%d", uSrcSequelVersion, uSrcBranchVersion, uSrcRevision, uSrcSVersion);
			packet.url = m_strPatchPath+ "degrade_" + szVersion + ".ybtx";
			m_PatchList.push_back(packet);
			uSrcSVersion--;
		}
	}
}

void VersionManager::PatchRevision(const char* szSrc, const char* szTgt)
{
	assert(szSrc && szTgt);
	uint32 uSrcSequelVersion = 0;
	uint32 uSrcBranchVersion = 0;
	uint32 uSrcRevision = 0;
	uint32 uSrcSVersion = 0;
	uint32 uRet = sscanf(szSrc , "%d.%d.%d.%d", &uSrcSequelVersion, &uSrcBranchVersion, &uSrcRevision, &uSrcSVersion);
	if (uRet != 4)
	{
		return;	
	}
	uint32 uTgtSequelVersion = 0;
	uint32 uTgtBranchVersion = 0;
	uint32 uTgtRevision = 0;
	uint32 uTgtSVersion = 0;
	uRet = sscanf(szTgt , "%d.%d.%d.%d", &uTgtSequelVersion, &uTgtBranchVersion, &uTgtRevision, &uTgtSVersion);
	if (uRet != 4)
	{
		return;
	}

	if (uSrcSequelVersion != uTgtSequelVersion || uSrcBranchVersion != uTgtBranchVersion)
	{
		return;
	}
	if (uSrcSVersion != 0 || uTgtSVersion != 0)
	{
		return;
	}
	
	UpdatePacket packet;
	char szVersion[20];
	if (uSrcRevision < uTgtRevision) // 升级第三位版本号
	{
		uSrcRevision++;
		while (uSrcRevision <= uTgtRevision)
		{
			packet.ver.uReserve = 0;
			packet.ver.uSequelVersion = uSrcSequelVersion;
			packet.ver.uBranchVersion = uSrcBranchVersion;
			packet.ver.uRevision = uSrcRevision;
			packet.ver.uSVersion = uSrcSVersion;
			sprintf(szVersion, "%d.%d.%d.%d", uSrcSequelVersion, uSrcBranchVersion, uSrcRevision, uSrcSVersion);
			packet.url = m_strPatchPath+ "upgrade_" + szVersion + ".ybtx";
			m_PatchList.push_back(packet);
			uSrcRevision++;
		}
	}
	else if (uSrcRevision > uTgtRevision) // 降级第三位版本号
	{
		while (uSrcRevision > uTgtRevision)
		{
			packet.ver.uReserve = 0;
			packet.ver.uSequelVersion = uSrcSequelVersion;
			packet.ver.uBranchVersion = uSrcBranchVersion;
			packet.ver.uRevision = uSrcRevision;
			packet.ver.uSVersion = uSrcSVersion;
			sprintf(szVersion, "%d.%d.%d.%d", uSrcSequelVersion, uSrcBranchVersion, uSrcRevision, uSrcSVersion);
			packet.url = m_strPatchPath+ "degrade_" + szVersion + ".ybtx";
			m_PatchList.push_back(packet);
			uSrcRevision--;
		}
	}
}

void VersionManager::PatchReVersion(const char* szSrc, const char* szTgt)
{
	assert(szSrc && szTgt);
	uint32 uSrcSequelVersion = 0;
	uint32 uSrcBranchVersion = 0;
	uint32 uSrcRevision = 0;
	uint32 uSrcSVersion = 0;
	uint32 uRet = sscanf(szSrc , "%d.%d.%d.%d", &uSrcSequelVersion, &uSrcBranchVersion, &uSrcRevision, &uSrcSVersion);
	if (uRet != 4)
	{
		return;	
	}
	uint32 uTgtSequelVersion = 0;
	uint32 uTgtBranchVersion = 0;
	uint32 uTgtRevision = 0;
	uint32 uTgtSVersion = 0;
	uRet = sscanf(szTgt , "%d.%d.%d.%d", &uTgtSequelVersion, &uTgtBranchVersion, &uTgtRevision, &uTgtSVersion);
	if (uRet != 4)
	{
		return;
	}

	if (uSrcSequelVersion != uTgtSequelVersion || uSrcBranchVersion != uTgtBranchVersion)
	{
		return;
	}


	char szSrcRevision[20];
	sprintf(szSrcRevision, "%d.%d.%d.0", uSrcSequelVersion, uSrcBranchVersion, uSrcRevision);
	PatchSmallVersion(szSrc, szSrcRevision);
	char szTgtRevision[20];
	sprintf(szTgtRevision, "%d.%d.%d.0", uTgtSequelVersion, uTgtBranchVersion, uTgtRevision);
	PatchRevision(szSrcRevision, szTgtRevision);
	PatchSmallVersion(szTgtRevision, szTgt);
}

char* VersionManager::GetBranchPatchVersion(uint32 uSequelVersion, uint32 uBranchVersion)
{
	char szVersion[20];
	sprintf(szVersion, "%d.%d.0.0", uSequelVersion, uBranchVersion);
	
	map<string, char*>::iterator iterMap;
	if ((iterMap = m_mapPatchList.find(szVersion)) != m_mapPatchList.end())
	{
		return iterMap->second;
	}
	else
	{
		return NULL;
	}
}

void VersionManager::PatchBranchVersion(const char* szSrc, const char* szTgt)
{
	assert(szSrc && szTgt);
	char szSVersionTgt[20];
	uint32 uSrcSequelVersion = 0;
	uint32 uSrcBranchVersion = 0;
	uint32 uSrcRevision = 0;
	uint32 uSrcSVersion = 0;
	uint32 uRet = sscanf(szSrc , "%d.%d.%d.%d", &uSrcSequelVersion, &uSrcBranchVersion, &uSrcRevision, &uSrcSVersion);
	if (uRet != 4)
	{
		return;	
	}
	sprintf(szSVersionTgt, "%d.%d.%d.0", uSrcSequelVersion, uSrcBranchVersion, uSrcRevision);
	PatchSmallVersion(szSrc, szSVersionTgt);
	uint32 uTgtSequelVersion = 0;
	uint32 uTgtBranchVersion = 0;
	uint32 uTgtRevision = 0;
	uint32 uTgtSVersion = 0;
	uRet = sscanf(szTgt , "%d.%d.%d.%d", &uTgtSequelVersion, &uTgtBranchVersion, &uTgtRevision, &uTgtSVersion);
	if (uRet != 4)
	{
		return;
	}
	if (uSrcSequelVersion != uTgtSequelVersion)
	{
		return;
	}

	char* szReTgtVersion = NULL;
	char szBranchVersion[20];
	char szReSrcVersion[20];
	uint32 uReSrcSequelVersion = uSrcSequelVersion;
	uint32 uReSrcBranchVersion = uSrcBranchVersion;
	uint32 uReSrcRevision = uSrcRevision;
	uint32 uReSrcSVersion = 0;
	UpdatePacket packet;
	if (uSrcBranchVersion < uTgtBranchVersion)
	{
		while (uReSrcBranchVersion < uTgtBranchVersion)
		{
			sprintf(szReSrcVersion, "%d.%d.%d.%d", uSrcSequelVersion, uReSrcBranchVersion, uReSrcRevision, uReSrcSVersion);
			uReSrcBranchVersion++;
			szReTgtVersion = GetBranchPatchVersion(uSrcSequelVersion, uReSrcBranchVersion);
			PatchRevision(szReSrcVersion, szReTgtVersion);
			packet.ver.uReserve = 0;
			packet.ver.uSequelVersion = uSrcSequelVersion;
			packet.ver.uBranchVersion = uReSrcBranchVersion;
			packet.ver.uRevision = 0;
			packet.ver.uSVersion = 0;
			sprintf(szBranchVersion, "%d.%d.0.0", uSrcSequelVersion, uReSrcBranchVersion);
			packet.url = m_strPatchPath+ "upgrade_" + szBranchVersion + ".ybtx";
			m_PatchList.push_back(packet);
			uReSrcRevision = 0;
			uReSrcSVersion = 0;
		}
	}
	else if (uSrcBranchVersion > uTgtBranchVersion)
	{
		while (uReSrcBranchVersion > uTgtBranchVersion)
		{
			sprintf(szReSrcVersion, "%d.%d.%d.%d", uSrcSequelVersion, uReSrcBranchVersion, uReSrcRevision, uReSrcSVersion);
			sprintf(szBranchVersion, "%d.%d.0.0", uSrcSequelVersion, uReSrcBranchVersion);
			PatchRevision(szReSrcVersion, szBranchVersion);
			packet.ver.uReserve = 0;
			packet.ver.uSequelVersion = uSrcSequelVersion;
			packet.ver.uBranchVersion = uReSrcBranchVersion;
			packet.ver.uRevision = 0;
			packet.ver.uSVersion = 0;
			packet.url = m_strPatchPath+ "degrade_" + szBranchVersion + ".ybtx";
			m_PatchList.push_back(packet);
			szReTgtVersion = GetBranchPatchVersion(uSrcSequelVersion, uReSrcBranchVersion);
			sscanf(szReTgtVersion, "%d.%d.%d.%d", &uReSrcSequelVersion, &uReSrcBranchVersion, &uReSrcRevision, &uReSrcSVersion);
		}
	}

	sprintf(szReSrcVersion, "%d.%d.%d.%d", uSrcSequelVersion, uReSrcBranchVersion, uReSrcRevision, uReSrcSVersion);
	sprintf(szReTgtVersion, "%d.%d.%d.0", uTgtSequelVersion, uTgtBranchVersion, uTgtRevision);
	PatchRevision(szReSrcVersion, szReTgtVersion);
	PatchSmallVersion(szReTgtVersion, szTgt);
}

bool VersionManager::GetVersionList(const wchar_t* szFile, const char* szTargetVersion)
{
	char* szLocalVersion = new char[50];
	GetVersionString(szFile, szLocalVersion);

	GetPatchListInfo(szFile);
	
	uint32 uSequelVersion = 0;
	uint32 uBranchVersion = 0;
	uint32 uRevision = 0;
	uint32 uSVersion = 0;
	uint32 ret = sscanf(szLocalVersion , "%d.%d.%d.%d", &uSequelVersion, &uBranchVersion, &uRevision, &uSVersion);
	if (ret != 4)
	{
		m_UpadteHandler->AlertExit(L"本地版本号格式不正确，请检查版本配置文件");
	}
	m_LocalVersion.uReserve = 0;
	m_LocalVersion.uSequelVersion = uSequelVersion;
	m_LocalVersion.uBranchVersion = uBranchVersion;
	m_LocalVersion.uRevision = uRevision;
	m_LocalVersion.uSVersion = uSVersion;

	ret = sscanf(szTargetVersion, "%d.%d.%d.%d", &uSequelVersion, &uBranchVersion, &uRevision, &uSVersion);
	if (ret != 4)
	{
		m_UpadteHandler->AlertExit(L"服务器版本号不正确，请耐心等待维护人员修复");
	}
	m_TgtVersion.uReserve = 0;
	m_TgtVersion.uSequelVersion = uSequelVersion;
	m_TgtVersion.uBranchVersion = uBranchVersion;
	m_TgtVersion.uRevision = uRevision;
	m_TgtVersion.uSVersion = uSVersion;

	// 如果当前客户端与服务器端版本一致，则无需上补丁
	if (m_LocalVersion.uSequelVersion == m_TgtVersion.uSequelVersion
		&& m_LocalVersion.uBranchVersion == m_TgtVersion.uBranchVersion
		&& m_LocalVersion.uRevision == m_TgtVersion.uRevision
		&& m_LocalVersion.uSVersion == m_TgtVersion.uSVersion)
	{
		return false;
	}

	GetPatchType(szLocalVersion, szTargetVersion);
	(this->*m_pMemFun)(szLocalVersion, szTargetVersion);
	return true;
}

uint32 VersionManager::GetLocalSequel()
{
	return m_LocalVersion.uSequelVersion;
}

uint32 VersionManager::GetLocalBranch()
{
	return m_LocalVersion.uBranchVersion;
}

uint32 VersionManager::GetLocalRevision()
{
	return m_LocalVersion.uRevision;
}

uint32 VersionManager::GetLocalSmallVersion()
{
	return m_LocalVersion.uSVersion;
}

uint32 VersionManager::GetOfficialSequel()
{
	return m_PatchList.back().ver.uSequelVersion;
}

uint32 VersionManager::GetOfficialBranch()
{
	return m_PatchList.back().ver.uBranchVersion;
}

uint32 VersionManager::GetOfficialRevision()
{
	return m_PatchList.back().ver.uRevision;
}

uint32 VersionManager::GetOfficialSmallVersion()
{
	return m_PatchList.back().ver.uSVersion;
}

string VersionManager::PopUrl()
{
	string strPatchUrl = m_PatchList.front().url;
	m_LocalVersion = m_PatchList.front().ver;
	m_PatchList.pop_front();
	return strPatchUrl;
}

VersionManager::PatchList_t& VersionManager::GetPatchList()
{
	return m_PatchList;
}


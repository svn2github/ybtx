#pragma once


#include "IAsyncUpdateHandler.h"
#include "BaseTypes.h"
#include <string>
#include <list>
#include <set>
#include <map>
#include <fstream>
#include <cassert>

namespace sqr
{
	struct ReleaseVersion
	{
		uint16 uReserve;       // 未使用
		uint16 uSequelVersion; // 资料片版本号(不可进行Patch更新)
		uint16 uBranchVersion; // 分支版本号
		uint16 uRevision;      // 分支内修订版本号
		uint16 uSVersion;	   // 小版本号
	};

	struct UpdatePacket
	{
		ReleaseVersion  ver;
		std::string     url;
	};

	// 该类数据操作没加锁，不能被分线程调用
	class VersionManager
	{
	public:
		typedef  std::list<UpdatePacket>  PatchList_t;

	public:
		VersionManager(IAsyncUpdateHandler* pUpadteHandler);
		~VersionManager();
		bool GetVersionList(const wchar_t* szFile, const char* szTargetVersion);
		uint32 GetLocalSequel();
		uint32 GetLocalBranch();
		uint32 GetLocalRevision();
		uint32 GetLocalSmallVersion();
		uint32 GetOfficialSequel();
		uint32 GetOfficialBranch();
		uint32 GetOfficialRevision();
		uint32 GetOfficialSmallVersion();
		std::string PopUrl();
		PatchList_t& GetPatchList();
		void GetPatchListInfo(const wchar_t* wszFile);
		void GetVersionString(const wchar_t* wszFile, char* szVersion);
		void PatchSmallVersion(const char* szSrc, const char* szTgt);
		void PatchRevision(const char* szSrc, const char* szTgt);
		char* GetBranchPatchVersion(uint32 uSequelVersion, uint32 uBranchVersion);
		void PatchBranchVersion(const char* szSrc, const char* szTgt);
		void PatchReVersion(const char* szSrc, const char* szTgt);
		void GetPatchType(const char* szSrc, const char* szTgt);

	private:
		IAsyncUpdateHandler*  m_UpadteHandler;
		PatchList_t           m_PatchList; // 需要上补丁的列表，最后一个为官方最新版本
		ReleaseVersion        m_LocalVersion;
		ReleaseVersion		  m_TgtVersion;
		string				  m_strPatchPath;
		map<string, char*> m_mapPatchList; // PatchList记录第二位版本号升级线路
		typedef void (VersionManager::* pVersionManagerMemfun)(const char*, const char*);
		pVersionManagerMemfun m_pMemFun;
	};

}


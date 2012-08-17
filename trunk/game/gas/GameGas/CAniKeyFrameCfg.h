#pragma once
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"
#include "CIniFile.h"

#ifdef _WIN32
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

class CAniKeyFrameCfg : public CConfigMallocObject
{
public:
	static bool LoadAniKeyFrameConfig(const string& szFileName);
	static void UnloadAniKeyFrameConfig();

	static sqr::CIniFile* GetAniKeyFrameCfg(const TCHAR* szName);
	static uint32 GetNPCAniKeyFrame(const TCHAR* szNPCName, const TCHAR* szAniName, const TCHAR* szKeyName);

	static void SetNpcName2AinMap(string sNpcName, string sAniFileName);
	static bool CheckNpcMapAniKeyFrameCfg();

private:
#ifndef _WIN32
	struct HashFunc
	{
		size_t operator()(const string& strName)const
		{
			return __stl_hash_string(strName.c_str());
		}
	};
	typedef hash_map<string, sqr::CIniFile*, HashFunc,equal_to<string>,  TConfigAllocator<pair<string, sqr::CIniFile* > > >					MapAniKeyFrameCfg;
#else
	typedef hash_map<string, sqr::CIniFile*, hash_compare<string, less<string> >,  TConfigAllocator<pair<string, sqr::CIniFile* > > >					MapAniKeyFrameCfg;
#endif

	typedef map<string, string, less<string>, TConfigAllocator<pair<string, string > > >	MapNpcName2AniFileName;
	typedef map<string, uint8, less<string>, TConfigAllocator<pair<string, uint8 > > >		MapAniKeyFrame;

	static MapAniKeyFrameCfg				ms_mapAniKeyFrameCfg;		// 全部动作关键帧MAP
	static MapNpcName2AniFileName			m_mapNpcName2AniFileName;	// 索引NPC名字对应的动作名称
	MapAniKeyFrame							m_mapAniKeyFrame;			// 一个动作包中动作名对应的关键帧
};

#pragma once
#include "TSingleton.h"
#include "CVector3.h"

namespace sqr
{
class IProgressCallback;

class CLoadSavedMapMgr: public Singleton<CLoadSavedMapMgr>
{
	IProgressCallback		 *progress;
public:
	map<string, sqr::uint16> sceneRgnIndexs;
	uint32					 uModelProVersion;
	uint32					 uGridProVersion;
	DWORD					 dwLocalVersion;
	string					 strMapName;
public:
	CLoadSavedMapMgr(void);
	~CLoadSavedMapMgr(void);

	void LoadMapRegionSize(string strMapName, int& nRegionInWidth, int& nRegionInDepth );
	void SaveMapRegionSize();

	bool LoadBlockInfo();
	bool SaveBlockInfo();
	
	void LoadMapResAmpInfo();
	void SaveMapResAmpInfo();

	void LoadRegionInfo(const int nodeIndex, const int i, const int j);
	void SaveRegionInfo(const int nodeIndex, const int i, const int j);

	void LoadTerrainInfo(const int nodeIndex, const int i, const int j);
	void SaveTerrainInfo(const int nodeIndex, const int i, const int j);

	void LoadTerrainObjcetInfo(const int nodeIndex, const int i, const int j);
	void SaveTerrainObjcetInfo(const int nodeIndex, const int i, const int j);

	void Save();
	void Load(const string& strFileName);

	void DealRegionSceneName();

	bool FileIsReadOnly(const string& strFileName);

	void SetProcessCallback( IProgressCallback* p )
	{
		Ast(NULL != p);
		this->progress = p;
	}
};

};

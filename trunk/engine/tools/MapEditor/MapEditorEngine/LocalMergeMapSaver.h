#pragma once
/**
	@class CLocalMergeMapSaver

	Save .local File

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
SmartPointer(CLocalMergeMapSaver);

class CLocalMergeMapSaver : public CRefObject
{
public:
	CLocalMergeMapSaver();
	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);
	void SetMapPathName(const string& name);

	/// Save merge .project File
	void SaveAsMergeProject();
	void LoadAsMapProject(const string& path);

	/// Save merge .operaterinfo File
	void SaveAsMergeRgnLightInfo();
	void LoadAsRgnLightInfo(const string& path);

	void CreateReadMapLightInfo(const string& path);

	/// Save merge .Local File
	void SaveAsMergeLocal(const string& path, const int xSort, const int zSort);

	/// Save .audio File
	void SaveAsAudio(const string& path);
	
	/// Save .audio File
	void SaveAsLogin(const string& path, set<int> nGridIndexs);

	/// Save .rgn File
	void SaveAsRgn(const string& path);

private:
	// 保存地图阻挡信息
	void SaveMergeMapBlockInfo(const string& path, const int nSaveMapGridCnt, const int nReadMapGridCnt);
	// 保存水信息
	void SaveWaterInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt);
	// 保存地图的点光源信息
	void SavePointLightInfo(FILE *pRead, FILE *pSave);
	// 保存地图的聚光灯信息
	void SaveSpotLightInfo(FILE *pRead, FILE *pSave);
	/// 保存精确的阻挡信息
	void SaveExactBlockInfo(FILE *pRead, FILE *pSave);
	/// 保存每个格子有没有浪花
	void SaveWaveSideInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt);
	/// 只保存带骨骼但是不播放骨骼的信息
	void SaveOffAnimateModelIndex(FILE *pRead, FILE *pSave);	
	/// 保存格子材质索引值
	void SaveTerGridMatIndexInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt);
	void SaveRecordInfo(FILE *pRead, FILE *pSave);
	void MakeTerrainTexture(set<string> terrainTexIndexsSet);

private:
	CTerrainMesh* terrain;
	IProgressCallback* progress;
	set<string>	  setTitleResourceName;
	set<string>	  setEffecteResourceName;
	string		  m_strMapPathName;
	///合并后旧地图格子索引在新地图中的新索引映射 first old vertex index second new vertex index
	map<int, int> mapOldMapVertexIndex;
	map<int, int> mapReadMapVertexIndex;
	///合并后旧地图格子索引在新地图中的新索引映射 first old grid index second new grid index
	map<int, int> mapOldMapGridIndex;
	map<int, int> mapReadMapGridIndex; //first read grid index second new save index
	///合并后旧地图细化区域索引在新地图中的新索引映射 first read region divide index second save region divide index
	map<int, int> mapReadMapDivideRegionIndex;
	set<string> setHasRegNames;

	uint32 uModelVersion;
	uint32 uGridProVersion;	

	string m_strDefaultTerrainTexName;
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
inline void
CLocalMergeMapSaver::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

//------------------------------------------------------------------------------
inline void
CLocalMergeMapSaver::SetMapPathName(const string& name)
{
	m_strMapPathName = name;
}

}// namespace sqr
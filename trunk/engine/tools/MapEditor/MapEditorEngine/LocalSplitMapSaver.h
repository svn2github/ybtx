#pragma once
/**
	@class CLocalSplitMapSaver

	Save .local File

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class IProgressCallback;
SmartPointer(CLocalSplitMapSaver);

///分割地图
class CLocalSplitMapSaver : public CRefObject
{
public:
	CLocalSplitMapSaver();
	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);
	
	void SetSplitMapPathName(const string& name);
	void SetSplitMapSizeInfo(const DWORD dwStartX, const DWORD dwStartZ, const DWORD dwSpiltMapWidth, const DWORD dwSplitMapDepth);

	/// Save merge .project File
	void SaveAsSplitProject();
	void LoadAsMapProject(const string& path);

	/// Save merge .operaterinfo File
	void SaveAsSplitRgnLightInfo();

	/// Save merge .Local File
	void SaveAsSplitLocal();

	/// Save .audio File
	void SaveAsAudio();

private:
	// 保存地图阻挡信息
	void SaveSplitMapBlockInfo(const int nSaveMapGridCnt);
	// 保存地图的点光源信息
	void SavePointLightInfo(FILE * fp, const int SceneLightCount);
	// 保存地图的聚光灯信息
	void SaveSpotLightInfo(FILE * fp, const int nSpotLightCnt);
	/// 保存每个格子有没有浪花
	void SaveWaveSideInfo(FILE *pRead, FILE *pSave, const int nSaveMapGridCnt, const int nReadMapGridCnt);
	/// 只保存带骨骼但是不播放骨骼的信息
	void SaveOffAnimateModelIndex(FILE * fp);	
	/// 保存格子材质索引值
	void SaveSplitMapGridMatIndexInfo(FILE * fp, DWORD newGridCnt);
	/// 保存精确的阻挡信息
	void SaveSplitMapExactBlockInfo(FILE * fp, DWORD newGridCnt);
	/// 保存每个格子有没有浪花
	void SaveWaveSideInfo(FILE* file, DWORD newGridCnt);

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

	///分割后的地图格子索引与被分割地图的格子索引间的映射 first split map grid index; second splited map grid index
	map<int, int> mapSplitMapGridIndex;
	map<int, int> mapRevertSplitMapGridIndex;
	///分割后的地图顶点索引与被分割地图的顶点索引间的映射 first split map vertex index; second splited map vertex index
	map<int, int> mapSplitMapVertexIndex;

	DWORD m_dwSplitStartX;
	DWORD m_dwSplitStartZ;
	DWORD m_dwSplitMapWidth;
	DWORD m_dwSplitMapDepth;

	string m_strDefaultTerrainTexName;
};

//------------------------------------------------------------------------------
inline void
CLocalSplitMapSaver::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

//------------------------------------------------------------------------------
inline void
CLocalSplitMapSaver::SetSplitMapPathName(const string& name)
{
	m_strMapPathName = name;
}

//------------------------------------------------------------------------------
inline void
CLocalSplitMapSaver::SetSplitMapSizeInfo(const DWORD dwStartX, const DWORD dwStartZ, 
										 const DWORD dwSpiltMapWidth, const DWORD dwSplitMapDepth)
{
	m_dwSplitStartX		= dwStartX;
	m_dwSplitStartZ		= dwStartZ;
	m_dwSplitMapWidth   = dwSpiltMapWidth;
	m_dwSplitMapDepth   = dwSplitMapDepth;
}


}// namespace sqr
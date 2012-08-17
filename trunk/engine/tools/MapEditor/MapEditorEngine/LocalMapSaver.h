#pragma once
/**
	@class CLocalMapSaver

	Save .local File

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"
#include "CVector3.h"

namespace sqr
{
class CTerrainMesh;
class COperator;
class SGrid;
class IProgressCallback;
SmartPointer(CLocalMapSaver);

class CLocalMapSaver : public CRefObject
{
public:
	CLocalMapSaver();
	/// 设置terrain mesh
	void SetTerrainMesh(CTerrainMesh* terrain);
	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// Save .Local File
	void SaveAsLocal(const string& path);

	///resize .local
	bool ResizeAndSaveAsLocal(string strTitle, DWORD dwWidth, DWORD dwDepth);

	/// Save .audio File
	void SaveAsAudio(const string& path, bool bResize);
	
	/// Save .audio File
	void SaveAsLogin(const string& path, set<int> nGridIndexs);

	/// Save .rgn File
	void SaveAsRgn(const string& path);
private:
	// 保存地图的点光源灯信息
	void SavePointLightInfo(FILE * fp, const int SceneLightCount, bool& bSceneLightCntEqual);
	// 保存地图的聚光灯信息
	void SaveSpotLightInfo(FILE * fp, const int nSpotLightCnt, bool& bSaveLocalFile);
	/// 保存精确的阻挡信息
	void SaveExactBlockInfo(FILE * fp);
	/// 保存精确的阻挡信息
	void SaveReSizeExactBlockInfo(FILE * fp, DWORD dwWidth, DWORD dwDepth);
	/// 保存每个格子有没有浪花
	void SaveWaveSideInfo(FILE* file);
	/// 只保存带骨骼但是不播放骨骼的信息
	void SaveOffAnimateModelIndex(FILE* fp);	
	/// 保存格子材质索引值
	void SaveTerGridMatIndexInfo(FILE* fp);
	/// 保存精确的阻挡信息
	void SaveReSizeTerGridMatIndexInfo(FILE * fp, DWORD dwWidth, DWORD dwDepth);
	///保存旧地图的起点坐标在新地图中的坐标值
	void SaveOldMapStartCoordinateInNewMap(const DWORD dwNewWidth, const DWORD dwNewDepth, const int nOldSatrtGridInNewIndex);
	//// 保存区域特效信息
	//void SaveRgnAmbientFxInfo(FILE *fp, bool bResize, bool bSplit);
private:
	CTerrainMesh*	   terrain;
	COperator*		   pOperator;
	IProgressCallback* progress;
	string			   m_strDefaultTerrainTexName;
};

//------------------------------------------------------------------------------
inline void
CLocalMapSaver::SetTerrainMesh( CTerrainMesh* t )
{
	Ast(NULL != t);
	this->terrain = t;
}

//------------------------------------------------------------------------------
inline void
CLocalMapSaver::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

}// namespace sqr
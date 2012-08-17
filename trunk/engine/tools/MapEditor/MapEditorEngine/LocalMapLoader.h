#pragma once
/**
	@class CLocalMapLoader

	Load .local file

	(C) 2009 ThreeOGCMan
 */
#include "CRefObject.h"

namespace sqr
{
class CTerrainMesh;
class COperator;
class SVertex;
class SGrid;
class IProgressCallback;
SmartPointer(CLocalMapLoader);

class CLocalMapLoader : public CRefObject
{
public:
	CLocalMapLoader();

	/// 设置进度条回调
	void SetProcessCallback(IProgressCallback* progress);

	/// Load .Local File
	void LoadAsLocal(const string& path);

	void NewLoadAsLocal(const string& path);
	
private:
	void LoadAsOOldLocal(FILE * fp, DWORD& version, string strFileName);
	void LoadAsOldLocal(FILE * fp, DWORD& version, string strFileName);
	void LoadAsNewLocal(FILE * fp, DWORD& version, string strFileName);

	// 读取格子纹理索引
	void LoadTerrainGridTexIndex(FILE * fp, map<int16, string>& terrainTextureNames);
	// 读取每个顶点信息
	void LoadEachGridInfo(FILE * fp, CTerrainMesh*pTerrain, SGrid* pGrid);

	/// 读取地图的点光源信息
	void LoadPointLightInfo(FILE * fp);
	/// 读取地图的聚光灯信息
	void LoadSpotLightInfo(FILE * fp);
	/// 读取精确的阻挡信息
	void LoadExactBlockInfo(FILE * fp);
	/// 读取每个格子有没有浪花
	void LoadWaveSideInfo(FILE* file);
	/// 读取带骨骼但是不播放骨骼的信息
	void LoadOffAnimateModelIndex(FILE* fp);	
	/// 读取水的信息
	void LoadWaterInfo(FILE* fp);	
	/// 拼接地表纹理
	void MakeTerrainTexture(set<string> terrainTexIndexsSet);

	/// 保存格子材质索引值
	void LoadTerGridMatIndex(FILE* fp);

// 	/// 读取区域特效信息
// 	void LoadRgnAmbientFxInfoInfo(FILE * fp);
private:
	COperator*		      pOperator;
	IProgressCallback*    progress;
	map<int, vector<int>> LoadFaildModelMap;
	vector<string>		  ErrObject;
	locale				  loc;
	uint32				  uModelProVersion;
	uint32				  uGridProVersion;
	string				  m_strDefaultTerrainTexName;
	string				  m_strDefaultWaterTexName;
};

//------------------------------------------------------------------------------
inline void
CLocalMapLoader::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

}// namespace sqr
#pragma once
#include "TSingleton.h"
#include "CVector3.h"
namespace sqr
{
	class CTerrainMesh;
	class COperator;
	class SGrid;

	class CMapCommonDataSaver : public TSingleton<CMapCommonDataSaver>
	{
		COperator    *pOperator;
		CTerrainMesh *terrain;
	public:
		friend class TSingleton<CMapCommonDataSaver>;
		CMapCommonDataSaver();
		virtual ~CMapCommonDataSaver();

		void SetOperator(COperator *pOper);
		void SetTerrainMesh(CTerrainMesh *pTerrain);

		// 保存env 环境信息
		void SaveEnvInfo(FILE * fp);

		// 保存背景图信息
		void SaveBackPictureInfo(FILE * fp);

		// 保存wave信息
		void SaveWaveInfo(FILE * fp);

		// 保存地表纹理权重信息，用于阻挡优先级
		void SaveMainRoadWeightInfo(FILE * fp);

		/// 保存模型属性以及格子属性版本号
		void SaveGridModeProVersion(FILE * fp);

		// 保存每个顶点信息
		void SaveEachVertexInfo(FILE * fp, const float fPositionY, const CVector3f& vNormal, const DWORD oColor, const DWORD sColor, const short sLogicHeight);

		///记录格子纹理索引
		void SaveTerrainGridTexIndex(FILE * fp, const int nGridCnt, map<string, int16>& terrainTextureNames);

		// 保存每个顶点信息
		void SaveEachGridInfo(FILE * fp, SGrid* pGrid, int& SceneLightCount, int& nSpotLightCount, bool bResize, bool bSplit);

	public:
		set<int> nLoginGridIndexs;
		DWORD    dwNewMapWidth;
	};
}

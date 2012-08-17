#pragma once
#include "TSingleton.h"
#include "CVector3.h"
#include "ExsertStructDB.h"

namespace sqr
{
	class CTerrainMesh;
	class SVertex;

	struct  ClientGridRgn
	{
		DWORD gAmbientColor;
		DWORD gPlayerAmbientColor;
		DWORD gDirLightDiffuse;
		DWORD gPlayerDiffuse;
		DWORD gFogColor;
	};

	class CTerrainAlgorithm : public TSingleton<CTerrainAlgorithm>
	{
	public:

		CTerrainAlgorithm();
		virtual ~CTerrainAlgorithm();

		// 计算所有顶点法线法线
		void	CalTerrainNormal();
		// 具体计算每个顶点法线
		void	CalculateVertexNormal(DWORD dwVertexIndex);
		//计算grid中心y值
		void ReCalculateGridsHeight(DWORD dwVertexIndex);
		///判断顶点高度是否有效
		void AssertHeight(SVertex* pVertex);

		///顶点色调和
		///do
		void ModulateVertexDiffuseRGB(int nVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, BYTE byRed, BYTE byGreen, BYTE byBlue, int nVertexIndexHot);
		void ModulateVertexSpecularRGB(int nVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, BYTE byRed, BYTE byGreen, BYTE byBlue, int nVertexIndexHot);
		///undo
		void SetVertexDiffuseRGB(int nVertexIndex, DWORD dwColor);
		void SetVertexSpecularRGB(int nVertexIndex, DWORD dwColor);

		///alpha调和
		void CutVertexAlpha(DWORD dwVertexIndex);

		void IncVertexAlphaX(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void IncVertexAlphaZ(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void IncVertexAlphaB(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);

		void DecVertexAlphaX(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void DecVertexAlphaZ(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void DecVertexAlphaB(sqr::EALPHA_TYPE eAlphaType, DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);

		void SetVertexAlpha(DWORD dwVertexIndex, D3DCOLOR cColor);

		///height manipulation
		void CutVertexHeight(DWORD dwVertexIndex, float fMinY);
		void SmoothVertexHeight(int nVertexIndex);

		void IncLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void IncLinearVertexHeightZ(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void IncLinearVertexHeightB(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);
		void IncGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);

		void DecLinearVertexHeightX(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void DecLinearVertexHeightZ(DWORD dwVertexIndex, DWORD dwBrushSize, int nVertexIndexHot);
		void DecLinearVertexHeightB(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);

		void DecGaussVertexHeight(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth,  int nVertexIndexHot);

		void TemplateIncCutVertexHeightByGauss(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);
		void TemplateIncCutVertexHeightByLinear(DWORD dwVertexIndex, DWORD dwBrushWidth, DWORD dwBrushDepth, int nVertexIndexHot);

		// undo
		void SetVertexHeight(DWORD dwVertexIndex, float fHeight);

		///阻挡 block manipulation
		// do and undo
		void SetTileBlock(DWORD dwGridIndex, int nBlockType);

		//////////////////////////////////////////////////////////////////////////
		///悬崖计算
		CVector2f GetProjectCoord(CVector3f& pos, CVector3f& normal);
		void ReCalculateGridsLayerUV(const int nGridIndex, const int nLayer);
		void ReBackGridLayerUV(const int nGridIndex, const int nLayer);
		//////////////////////////////////////////////////////////////////////////

		///跟生成小地图图片相关的计算
		float GetApex();

		//及时映射以及小地图更新
		void SetMappedRegion( set<int>& regionunits );

		void GetMapCoordinate(const int nGridIndex, int& nX, int& nZ);
	

	};
}

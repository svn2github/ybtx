#pragma once
#include "TRefPointer.h"
#include "TSingleton.h"
#include "CColor4.h"
#include "CVector3.h"

#ifdef _DEBUG
#define VERTEX_COLOR_EDIT_WHEN_BAKED	0
#else
#define VERTEX_COLOR_EDIT_WHEN_BAKED	1
#endif
namespace sqr
{
	class SGrid;
	class CMapEditObject;
	SmartPointer(CTObject);

	class OfflineRender : public TSingleton<OfflineRender>
	{
	public:
		OfflineRender();
		virtual ~OfflineRender();

		///计算每个格子所在的环境光和区域光
		void CalGridColorParam();

		void UseBakingResult(bool use, bool bShowTerrainBaking, bool bCleanRS = true);
		void PreviewTerrainBakingResult(bool use);

		///add new spot or point light
		void UpdateGridLights( int nGirdIndex );

		///取消烘培效果的时候，将vertex.bColor换成vertex.oColor
		void RevertVertexBakingToVertexColor(bool use);
	private:
		///静态光源影响地表的计算
		void UpdateTerrainColor(bool shadow);

		void AddLightsToIlluminator(bool terrainOnly);
	private:
		friend class CObjectVertexProcessor;
		friend class CLightMapReceiver;
		friend class CObjectVertexBaker;
		friend class CTerrainBaker;

		struct LightColor
		{
			LightColor()
			{
				ZeroMemory(this, sizeof(LightColor));
			}
			CColor4 Ambient;
			CColor4 Diffuse;
			CColor4 Shadow;
		};
		vector<LightColor>			m_GridColor;	///根据区域光，计算每个格子对应的太阳光和环境光参
		map< int32, vector< CTObjectPtr> >  mPointLight;	//记录点光源
		map< int32, vector< CTObjectPtr> >  mSpotLight;		//记录聚光灯
		CVector3f							mSunlightDir;
	};
}
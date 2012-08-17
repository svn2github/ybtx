#pragma once

#include "BakerBase.h"
#include "LightMapWriter.h"
#include "CRenderStyle.h"
#include "CEditTerrainMgr.h"

//#include "../Graphic/CShadowMatrixBuilder.h"
//#include "../Graphic/CShadowClipper.h"

namespace sqr
{
	class CLightMapFilter;
	SmartPointer(CLightMapFilter);
	SmartPointer(CRenderTexture);
	SmartPointer(CLightMapReceiver);
	SmartPointer(CLightMapWriter);

	class CLightMapBaker :public CBakerBase
	{
	public:
		enum
		{
			LIGHT_MAP_SIZE = 512,
			REGION_SIZE_IN_PIXEL = 1024,
			TILE_WIDTH_IN_REGION = 4,
			LIGHT_MAP_SIZE_IN_PIXEL = REGION_SIZE_IN_PIXEL * TILE_WIDTH_IN_REGION,
		};

	public:
		CLightMapBaker(void);
		~CLightMapBaker(void);
		// 初始化
		void Setup();
		// 销毁
		void Discard();
		//方法准备
		void OnPrepare();
		//tile准备
		void TilePrepare(index_t x,index_t z);
		//过程
		void OnProcess(index_t x, index_t z);

	protected:

		void RenderTile(index_t x,index_t z);
		virtual void RenderReceive() = 0;

		CAxisAlignedBox GetTileBox(index_t x,index_t z);
		void ChangeCamera(index_t x,index_t z);

	protected:
		CLightMapFilter* m_pShadowFilter;
		CRenderTexturePtr  m_LightMapTarget;
		CLightMapWriterPtr m_LightMapWriter;

		EShadowType oldShadowType;
		CCamera* LightMapCamera;
		
		CEditTerrainMgr* terrain;
		int tileWidth;
		int tileDepth;
	};

//!namespace
}

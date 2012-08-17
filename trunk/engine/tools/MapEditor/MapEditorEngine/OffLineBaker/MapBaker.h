#pragma once
//------------------------------------------------------------------------------
/**
	@class CMapBaker

	LightMap生成器.
	遍历每个4x4区域, 每次把地形和物件的阴影渲染渲染到RTT

	(C) 2009 ThreeOGCMan
*/
#include "CRefObject.h"

#include "BakerMgr.h"

#define OLD_BAKER false

namespace sqr
{
	class CMapEditCamera;
	class CRenderGroup;
	class CTerrainMesh;
	class IProgressCallback;

	SmartPointer(CMapBaker);

	class CMapBaker : public sqr::CRefObject
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
		CMapBaker();
		virtual ~CMapBaker();

		/// 设置接收者
		void SetProcessor();
		/// 进度信息回调
		void SetProgressCallback(IProgressCallback* callback);
		/// 设置Z偏移
		void SetZOffset(float f);

		/// 初始化
		void Setup();
		/// 生成
		void Generate();
		/// 分区域块生成
		void GenerateByTileRegions(set<int> regions);
		/// 释放资源
		void Discard();

		//
		void SetBakerMgr(CBakerMgrPtr bakermgr);

	private:
		CTerrainMesh* terrainMesh;
		CMapEditCamera* camera;
		CRenderGroup* renderGroup;

		float zOffset;
		float terrainOffset;
		IProgressCallback* progress;

		CBakerMgrPtr BakerMgr;
	};

	//------------------------------------------------------------------------------
	inline void
	CMapBaker::SetProgressCallback( IProgressCallback* callback )
	{
		this->progress = callback;
	}

	//------------------------------------------------------------------------------
	inline void
	CMapBaker::SetZOffset( float f )
	{
		this->zOffset = f;
	}
}
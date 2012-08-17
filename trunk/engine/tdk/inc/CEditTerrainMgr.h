#pragma once
#include "CTerrainMgr.h"

namespace sqr_tools
{
	class CEditLogicDataMgr;
	class CEditTerrainMgr : public CTerrainMgr
	{
	public:
		CEditTerrainMgr(CRenderGroup* pGroup,CEditLogicDataMgr* pLogicDataMgr);
		~CEditTerrainMgr(void);

		bool			SetRenderFlag(TerrainFlag::Mask mask);
		bool			isVisable(URegionID ID);

		void			SetLightMap(const URegionID ID, const char* filename);
		void			SetLightMap(const URegionID ID, ITexture* pText);

		virtual void	Render();
		
		bool			GenerateTerrain(int tileWidth,int tileHeight);
		void			WriteMapBuffer(const set<URegionID> regions);
	
	protected:
		CElementNode*	_CreateNode( URegionID ID );
		CEditLogicDataMgr* m_pLogicDataMgr;
		//存放一个Tile的顶点数据(标准 目前只有位置信息)
		static CVector3f	OffPos[64*64];
	public:
		static const string st_DefaultMapName;
	};
}
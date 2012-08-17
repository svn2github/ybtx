#include "stdafx.h"
#include "CEditWaterMgr.h"
#include "CEditLogicDataMgr.h"
#include "CEditLogicData.h"

#include "CWaterRegion.h"


CEditWaterMgr::CEditWaterMgr(CRenderGroup* pGroup,CEditLogicDataMgr* pLogicDataMgr)
: CWaterMgr(pGroup)
{
	m_pLogicDataMgr=pLogicDataMgr;
}

CEditWaterMgr::~CEditWaterMgr(void)
{

}

CElementNode* CEditWaterMgr::_CreateNode( URegionID ID )
{
	return new CWaterRegion(this,ID);
}

bool CEditWaterMgr::GenerateWater(int tileWidth,int tileHeight)
{
	//vector<string> textureNames(2);
	//textureNames[0] = "tile\\地编专用\\tex\\big.dds";
	//textureNames[1] = "tile\\地编专用\\tex\\small.dds";
	//SVector<ITexture*>& textures;
	//CWaterMgr::WaterEffectType type = (textures.size() > 1) ? CWaterMgr::TextureAnimation : CWaterMgr::EnvironmentMapping;
	//CWaterSurface* m_pWaterSurface = s_pWaterMgr->CreateSurface(type, textures);
	set<URegionID> regions;
	int tileNum=tileWidth*tileHeight;
	for(int i=0;i<tileNum;i++)
	{
		int	off_x = i % tileWidth;
		int	off_z = i / tileHeight;
		URegionID regionID = URegionID(off_x,off_z);
		regions.insert(regionID);
		CWaterRegion* pWaterNode=(CWaterRegion*)this->CreateNode(regionID);	//水面节点		
	}
	return true;
}

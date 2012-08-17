#include "stdafx.h"
#include "CWaterCoder.h"
#include "CMetaGridMgr.h"
#include "CMetaStaticData.h"
#include "CGraphic.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

BEGIN_VERSION_MAP(CWaterCoder)
	ON_VERSION( 0 , DeCode_Ver_0 )
END_VERSION_MAP()


const float WATER_UV_SCALE = 1.0f / (eGridSpan * 16.0f);

CWaterCoder::CWaterCoder(CSceneManagerClient* pScene)
: m_pScene(pScene)
{

}

bool CWaterCoder::_CElementDeCode( const DeCodeParams& Params )
{
	CDataChunk tempChunk;
	uint16		ver = 0;

	tempChunk.SetBuffer(NULL, 0);
	tempChunk.SetVersion(ver);
	return DeCode(tempChunk,Params.m_pNode);
}

bool CWaterCoder::DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CWaterRegion* pWaterRegion = (CWaterRegion*)handle;
	CWaterMgr* pWaterMgr = (CWaterMgr*)pWaterRegion->GetParentManager();
	CMetaGridMgr& GridMgr = *m_pScene->m_pGridMgr;
	uint32 regionId;

	int32 w = pWaterRegion->GetID().X;// % m_pScene->GetWidth();
	int32 h = pWaterRegion->GetID().Z;// / m_pScene->GetWidth();

	w *= 4;
	h *= 4;
	for( int x = 0; x<4 ; ++x )
	{
		for( int y = 0; y<4; ++y)
		{
			uint32 deltaWidth = w + x;
			uint32 deltaHeight = y + h;
			regionId = deltaWidth + deltaHeight*m_pScene->GetWidthInRegion();
			
			CMetaRegion* region = GridMgr.GetRegion(regionId);
			if (NULL == region)
				continue;
			RegionWater* waterInfo = region->GetWaterInfo();
			if (NULL == waterInfo || 0 == waterInfo->m_Waters.size())
				continue;
			
			deltaWidth *= eRegionSpan;
			deltaHeight *= eRegionSpan;

			for (uint32 i = 0; i < waterInfo->m_Waters.size(); ++i)
			{
				SVector<VerColorTex> waterVertices;
				CWater& water = waterInfo->m_Waters[i];
				waterVertices.reserve(water.GetWaterGridNum() * 4);
				for (uint32 n = 0; n < water.GetWaterGridNum(); ++n)
				{
					int xGrid = water.GetWaterGrid(n) % eRegionSpan;
					int yGrid = water.GetWaterGrid(n) / eRegionSpan;
					float xPixel = (float)(xGrid + deltaWidth) * eGridSpan;
					float zPixel = (float)(yGrid + deltaHeight) * eGridSpan;
					const CMetaStaticData::StaticGrid& grid = CMetaStaticData::GetGrid( xGrid, yGrid );
					for(int x = xGrid; x < xGrid + 2; x++, xPixel += float(eGridSpan), zPixel -= float(eGridSpan*2))
					{
						for(int y = yGrid; y < yGrid + 2; y++, zPixel += float(eGridSpan))
						{
							int index = y*( eRegionSpan + 1 ) + x;
							VerColorTex vertex;
							vertex.p = CVector3f(xPixel, waterInfo->m_nWaterHeight[index], zPixel);
							vertex.diffuse = waterInfo->m_cWaterColor[index];
							vertex.tu = xPixel * WATER_UV_SCALE;
							vertex.tv = zPixel * WATER_UV_SCALE;
							waterVertices.push_back(vertex);
						}
						SyncCoder_BreakPoint();
					}// grid vertices
				}// river grid
				if (!waterVertices.empty())
				{
					SVector<ITexture*> textures;
					uint texBegin, texCount;
					water.GetTextureRange(texBegin, texCount);
					for (uint32 t = 0; t < texCount; ++t)
					{
						textures.push_back(pWaterMgr->m_TextUnit[texBegin + t].GetTexture());
					}
					CWaterMgr::WaterEffectType type = (textures.size() > 1) ? CWaterMgr::TextureAnimation : CWaterMgr::EnvironmentMapping;
					CWaterSurface* surface = pWaterMgr->CreateSurface(type, textures);
					CWaterRegionMesh* pWaterMesh = pWaterRegion->GetMesh(surface,regionId);
					
					pWaterMesh->SetBoundingBox(waterInfo->aabb);
					SyncCoder_BreakPoint();

					CVertexHardwareBuffer* pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerColorTex),
						waterVertices.size(), HBU_WRITE_ONLY); 
					VerColorTex* buffer = (VerColorTex*)pBuffer->lock(HBL_DISCARD);
					if ( buffer == 0 )
						return true;

					memcpy(buffer, &waterVertices.front(), sizeof(VerColorTex) * waterVertices.size());
					pBuffer->unlock();	
					pWaterMesh->InitRegion(pBuffer,water.GetWaterGridNum());
					pWaterMesh->m_vecWater.push_back(&water);
				}
				SyncCoder_BreakPoint();
			}// river
		}
		SyncCoder_BreakPoint();
	}
	pWaterRegion->SetValid(true);
	return true;
}

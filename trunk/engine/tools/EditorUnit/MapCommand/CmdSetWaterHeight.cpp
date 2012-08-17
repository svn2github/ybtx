#include "StdAfx.h"
#include <algorithm>
#include "CmdSetWaterHeight.h"
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CEditWaterMgr.h"
#include "CTerrainOperate.h"
#include "CWaterRegion.h"
#include "CWaterRegionMesh.h"
#include "CWaterSurface.h"
#include "CMetaGridMgr.h"

CCmdSetWaterHeight::CCmdSetWaterHeight(CEditContext* pContext)
:CCmdSetVertexHeight(pContext)
{
	m_pWaterSurface=NULL;
}

CCmdSetWaterHeight::~CCmdSetWaterHeight(void)
{
}

void CCmdSetWaterHeight::SetWaterSurface(CWaterSurface* surface)
{
	m_pWaterSurface=surface;
}

void CCmdSetWaterHeight::_Execute()
{
	CreateWaterMesh();
}

void CCmdSetWaterHeight::_UnExecute()
{
}

void CCmdSetWaterHeight::CreateWaterMesh()
{
	if(!m_pWaterSurface)
		return ;
	//vector<VerColorTex> vertices;
	//CEditDataScene* ds=m_pEditContext->GetEditDataScene();
	//CEditWaterMgr* pEditWaterMgr = ds->GetWaterMgr();
	//CEditLogicDataMgr* pEditLogicDataMgr = ds->GetLogicDataMgr();
	//int ReginWidth=ds->GetWidthInRegion();
	//int GridWidth=ds->GetWidthInGrid();
	//int GridHeight=ds->GetHeightInGrid();
	////区域索引整理
	//m_RegionVec.clear();
	//unsigned int num = m_vecDiffs.size();
	//for ( unsigned int m = 0; m < num; ++m )
	//{	
	//	int GridX=m_vecDiffs[m].vertexCoord.x;
	//	int GridZ=m_vecDiffs[m].vertexCoord.y;
	//	//画刷选中点扩展成Mesh
	//	if(GridX>=GridWidth || GridZ>=GridHeight)
	//		break;
	//	else
	//	{
	//		//标准点
	//		for(unsigned int k=0;k<4;k++)
	//		{
	//			VerColorTex ver;
	//			//一个点扩充到四个点 一个MESH
	//			unsigned int tmpGridX=GridX+k%2;
	//			unsigned int tmpGridZ=GridX+k/2;
	//			ver.p=pEditLogicDataMgr->GetVertexPosition(tmpGridX,tmpGridZ);
	//			ver.p.y=100.0f;
	//			ver.diffuse=0xff00ff00;//pEditLogicDataMgr->GetVertexDiffuse(tmpGridX,tmpGridZ);
	//			ver.tu=float(k%2);
	//			ver.tv=float(k/2);
	//			vertices.push_back(ver);
	//		}
	//	}
	//	
	//	//计算一共选这得区域
	//	bool isExist=false;
	//	int tmpRegionIndex=(GridX%eRegionSpan)+(GridZ/eRegionSpan)*ReginWidth;
	//	for(vector<int>::iterator it=m_RegionVec.begin();it!=m_RegionVec.end();it++)
	//	{
	//		if((*it)==tmpRegionIndex)
	//		{
	//			isExist=true;
	//			break;
	//		}
	//	}
	//	if(!isExist)
	//		m_RegionVec.push_back(tmpRegionIndex);
	//}
	////创建
	//for(vector<int>::iterator it=m_RegionVec.begin();it!=m_RegionVec.end();it++)
	//{
	//	int tmpRegionIndex=(*it);
	//	CWaterRegion* pWaterRegion = (CWaterRegion*)pEditWaterMgr->CreateNode(0);
	//	CWaterRegionMesh* regionMesh = pWaterRegion->GetMesh(m_pWaterSurface,tmpRegionIndex);
	//	CAxisAlignedBox aabb;
	//	//根据点计算AABB盒
	//	for(unsigned int i=0;i<vertices.size();i++)
	//		aabb.mergeBox(CVector3f(vertices[i].p.x,vertices[i].p.y,vertices[i].p.z));
	//	regionMesh->SetBoundingBox(aabb);
	//	CVertexHardwareBuffer* pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerColorTex),vertices.size(), HBU_WRITE_ONLY); 
	//	VerColorTex* buffer = (VerColorTex*)pBuffer->lock(HBL_DISCARD);
	//	memcpy(buffer, &vertices.front(), sizeof(VerColorTex) * vertices.size());
	//	pBuffer->unlock();
	//	regionMesh->InitRegion(pBuffer,vertices.size()/4);
	//}
}

void CCmdSetWaterHeight::DestroyWaterMesh()
{
	if(!m_pWaterSurface)
		return ;
	//销毁并重新生成
}

void CCmdSetWaterHeight::ModifyWaterRegionInfo(CWaterRegion* pWaterRegion,int regionIndex)
{
	//CMetaRegion* region = m_pEditContext->GetEditDataScene()->GetRegion(regionIndex);
}

//bool CWaterCoder::DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle )
//{
//	CWaterRegion* pWaterRegion = (CWaterRegion*)handle;
//	CWaterMgr* pWaterMgr = (CWaterMgr*)pWaterRegion->GetParentManager();
//	CMetaGridMgr& GridMgr = *m_pScene->m_pGridMgr;
//	uint32 regionId;
//
//	int32 w = pWaterRegion->GetID().X;// % m_pScene->GetWidth();
//	int32 h = pWaterRegion->GetID().Z;// / m_pScene->GetWidth();
//
//	w *= 4;
//	h *= 4;
//	for( int x = 0; x<4 ; ++x )
//	{
//		for( int y = 0; y<4; ++y)
//		{
//			uint32 deltaWidth = w + x;
//			uint32 deltaHeight = y + h;
//			regionId = deltaWidth + deltaHeight*m_pScene->GetWidthInRegion();
//			
//			CMetaRegion* region = GridMgr.GetRegion(regionId);
//			if (NULL == region)
//				continue;
//			RegionWater* waterInfo = region->GetWaterInfo();
//			if (NULL == waterInfo || 0 == waterInfo->m_Waters.size())
//				continue;
//			
//			deltaWidth *= eRegionSpan;
//			deltaHeight *= eRegionSpan;
//
//			for (uint32 i = 0; i < waterInfo->m_Waters.size(); ++i)
//			{
//				SVector<VerColorTex> waterVertices;
//				CWater& water = waterInfo->m_Waters[i];
//				waterVertices.reserve(water.GetWaterGridNum() * 4);
//				for (uint32 n = 0; n < water.GetWaterGridNum(); ++n)
//				{
//					int xGrid = water.GetWaterGrid(n) % eRegionSpan;
//					int yGrid = water.GetWaterGrid(n) / eRegionSpan;
//					float xPixel = (float)(xGrid + deltaWidth) * eGridSpan;
//					float zPixel = (float)(yGrid + deltaHeight) * eGridSpan;
//					const CMetaStaticData::StaticGrid& grid = CMetaStaticData::GetGrid( xGrid, yGrid );
//					for(int x = xGrid; x < xGrid + 2; x++, xPixel += float(eGridSpan), zPixel -= float(eGridSpan*2))
//					{
//						for(int y = yGrid; y < yGrid + 2; y++, zPixel += float(eGridSpan))
//						{
//							int index = y*( eRegionSpan + 1 ) + x;
//							VerColorTex vertex;
//							vertex.p = CVector3f(xPixel, waterInfo->m_nWaterHeight[index], zPixel);
//							vertex.diffuse = waterInfo->m_cWaterColor[index];
//							vertex.tu = xPixel * WATER_UV_SCALE;
//							vertex.tv = zPixel * WATER_UV_SCALE;
//							waterVertices.push_back(vertex);
//						}
//						SyncCoder_BreakPoint();
//					}// grid vertices
//				}// river grid
//				if (!waterVertices.empty())
//				{
//					SVector<ITexture*> textures;
//					uint texBegin, texCount;
//					water.GetTextureRange(texBegin, texCount);
//					for (uint32 t = 0; t < texCount; ++t)
//					{
//						textures.push_back(pWaterMgr->m_TextUnit[texBegin + t].GetTexture());
//					}
//					CWaterMgr::WaterEffectType type = (textures.size() > 1) ? CWaterMgr::TextureAnimation : CWaterMgr::EnvironmentMapping;
//					CWaterSurface* surface = pWaterMgr->CreateSurface(type, textures);
//					CWaterRegionMesh* pWaterMesh = pWaterRegion->GetMesh(surface,regionId);
//					
//					pWaterMesh->SetBoundingBox(waterInfo->aabb);
//					SyncCoder_BreakPoint();
//
//					CVertexHardwareBuffer* pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerColorTex),
//						waterVertices.size(), HBU_WRITE_ONLY); 
//					VerColorTex* buffer = (VerColorTex*)pBuffer->lock(HBL_DISCARD);
//					if ( buffer == 0 )
//						return true;
//
//					memcpy(buffer, &waterVertices.front(), sizeof(VerColorTex) * waterVertices.size());
//					pBuffer->unlock();	
//					pWaterMesh->InitRegion(pBuffer,water.GetWaterGridNum());
//					pWaterMesh->m_vecWater.push_back(&water);
//				}
//				SyncCoder_BreakPoint();
//			}// river
//		}
//		SyncCoder_BreakPoint();
//	}
//	pWaterRegion->SetValid(true);
//	return true;
//}
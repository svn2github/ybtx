#include "StdAfx.h"
#include "RegionShadowMapBaker.h"
#include "CGraphic.h"
//
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CTerrain.h"
#include "TerrainMeshRegion.h"

#include "CScreenAlignedQuad.h"
#include "CVertexDeclaration.h"
#include "CRenderPipeline.h"
#include "CEditRenderPipeline.h"
//
#include "TerrainMeshRegion.h"
#include "SceneRegionMgr.h"
#include "SceneRegion.h"


namespace sqr
{


RegionShadowMapBaker::RegionShadowMapBaker(void)
{
	m_TexWidth=m_TexHeight=64;

	m_RegionTex = CGraphic::GetInst()->CreateRenderTarget();
	m_RegionTex->Release(); //临时补丁，强制计数器减1
	m_RegionTex->SetWidth(m_TexWidth);
	m_RegionTex->SetHeight(m_TexHeight);
	m_RegionTex->SetFormat(TFMT_A8R8G8B8);
	m_RegionTex->SetUseDepthBuffer(false);
	m_RegionTex->Create();

	Init();
	ColorList.resize(64);
}

RegionShadowMapBaker::~RegionShadowMapBaker(void)
{
	m_RegionTex=NULL;
	m_Op.Destroy();
	
	ColorList.clear();
}

void RegionShadowMapBaker::Init()
{
	quad[0]=VerColor2D(0, 1, 0, 0xffff00ff);
	quad[1]=VerColor2D(0, 0, 0, 0xff00ffff);
	quad[2]=VerColor2D(1, 1, 0, 0xffffff00);
	quad[3]=VerColor2D(1, 0, 0, 0xffffffff);
	//构建数据
	m_Op.Create();
	m_Op.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerColor2D::Format);
	m_Op.useIndexes = false;
	m_Op.vertexData->vertexStart = 0;
	m_Op.vertexData->vertexCount = 4;
	m_Op.operationType = PT_TRIANGLESTRIP;
	pBuf = CHardwareBufferMgr::GetInst()->createVertexBuffer( sizeof(VerColor2D),4,HBU_DYNAMIC_WRITE_ONLY,quad);
	m_Op.vertexData->vertexBufferBinding->SetBinding(0,pBuf);
}

void RegionShadowMapBaker::RenderTile(int x,int z)
{	
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetTerrain();
	if ( pTerrainMesh == NULL )	
		return;
	int tmpRegionMapWidth=pTerrainMesh->GetWidth()/BAKE_REGION_SIZE;
	int tmpRegionMapDepth=pTerrainMesh->GetDepth()/BAKE_REGION_SIZE;

	//制作当前Title区域索引
	int tmpRegionIndexList[BAKE_REGION_SIZE*BAKE_REGION_SIZE];
	for(int i=0;i<BAKE_REGION_SIZE;i++)     //行
	{
		for(int j=0;j<BAKE_REGION_SIZE;j++) //列
		{
			tmpRegionIndexList[i*BAKE_REGION_SIZE+j]=(z*BAKE_REGION_SIZE)*tmpRegionMapWidth+(x*BAKE_REGION_SIZE)+i*tmpRegionMapWidth+j;
		}
	}
	//填充当前Title每个区域的阴影颜色
	for(int k=0;k<BAKE_REGION_SIZE*BAKE_REGION_SIZE;k++)
	{
		SDivideRegion *pDivideRegion = pTerrainMesh->GetDivideRegion(tmpRegionIndexList[k]);
		if(pDivideRegion)
		{
			SceneRegion *pSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion(pDivideRegion->m_strRegionName);
			if(pSceneRgn)
			{
				CColor tmpColor=pSceneRgn->lightParam.globalShadow;
				tmpColor.A=255;
				ColorList[k]=tmpColor.ToARGB();
			}
		}
	}

	int ColUnit=m_TexWidth/BAKE_REGION_SIZE;
	int RowUnit=m_TexHeight/BAKE_REGION_SIZE;
	//采用后处理的方式进行
	//设置RS
	STATIC_RS tmpRS;
	tmpRS.m_AlphaBlendEnable=false;
	CEditRenderPipeline::GetInst()->SetRenderStyle(tmpRS);

	m_RegionTex->Begin();

	for(int row=0;row<BAKE_REGION_SIZE;row++)
	{
		for(int col=0;col<BAKE_REGION_SIZE;col++)
		{
			//Y轴反转
			quad[0]=VerColor2D(1.0f*col*ColUnit         , 1.0f*m_TexHeight-1.0f*row*RowUnit-RowUnit, 0.0f, ColorList[row*BAKE_REGION_SIZE+col]);  //(0,1)
			quad[1]=VerColor2D(1.0f*col*ColUnit+ColUnit , 1.0f*m_TexHeight-1.0f*row*RowUnit-RowUnit, 0.0f, ColorList[row*BAKE_REGION_SIZE+col]);  //(1,1)
			quad[2]=VerColor2D(1.0f*col*ColUnit         , 1.0f*m_TexHeight-1.0f*row*RowUnit,         0.0f, ColorList[row*BAKE_REGION_SIZE+col]);  //(0,0)
			quad[3]=VerColor2D(1.0f*col*ColUnit+ColUnit , 1.0f*m_TexHeight-1.0f*row*RowUnit,         0.0f, ColorList[row*BAKE_REGION_SIZE+col]);  //(1,0)
			
			CVertexHardwareBuffer* pTmpBuf;
			pTmpBuf=m_Op.vertexData->vertexBufferBinding->GetBuffer(0);
			pTmpBuf->write(0,pTmpBuf->getSize(),&quad[0],true);
			CRenderPipeline::GetInst()->RenderEx(&m_Op);
		}
	}
	m_RegionTex->End();
	//CString regionName;
	//regionName.Format("e:/%d_%d_region.dds",x,z);

	//m_RegionTex->SaveToFile(regionName.GetBuffer());
}


//!namespace
}
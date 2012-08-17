#include "stdafx.h"
#include "CTerrainCoder.h"
#include "CSceneClientBase.h"
#include "CSceneManagerClient.h"
#include "CIndexHardwareBuffer.h"
#include "CVertexHardwareBuffer.h"
#include "CVertexDeclaration.h"
#include "StringHelper.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "CTerrain.h"
#include "CMetaGridMgr.h"
#include "CMetaStaticData.h"
#include "TSafeBuffer.h"
#include "TSqrAllocator.inl"

BEGIN_VERSION_MAP(CNormalTerrainCoder)
	ON_VERSION( 0 , DeCode_Ver_0 )
END_VERSION_MAP()

BEGIN_VERSION_MAP(CShaderTerrainCoder)
	ON_VERSION( 0 , DeCode_Ver_0 )
END_VERSION_MAP()

const float MagicL = 128.5f/2304.0f;
const float Magic = 0.0f;//.5f/2304.0f;
const float MagicA = Magic/2;//*2;//0.00038f;

CTerrainCoder::CTerrainCoder(CSceneManagerClient* pScene)
: m_pDataScene(pScene)
{
	m_eSpri = SPRI_High;
}

bool CTerrainCoder::_CElementDeCode( const DeCodeParams& Params )
{
	CDataChunk tempChunk;
	uint16		ver = 0;

	tempChunk.SetBuffer(NULL,0);
	tempChunk.SetVersion(ver);
	return DeCode(tempChunk,Params.m_pNode);
}

bool CNormalTerrainCoder::DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle )
{
	TerrainNode* pNode = (TerrainNode*)handle;
	URegionID    id		  = pNode->GetID();
	CTerrainMgr* pTerrain = (CTerrainMgr*)pNode->GetParentManager();


	CMetaGridMgr& GridMgr = *m_pDataScene->m_pGridMgr;
	bool bVisible = GridMgr.GetRenderTileVisibleMark(id);
	if( !bVisible )
		return true;

	CVertexDeclaration* pVD = CTerrainMgr::st_TerrainDecl[CTerrainMgr::GetVtxDeclIndex(pTerrain->GetRenderFlag())];

	uint32 SceneWidth	= m_pDataScene->GetWidth();
	URegionID NodeId	= pNode->GetID();
	bool	tf;
	float	wi,hi,wp,hp;
	uint	idx = 0, x = 0, y = 0;
	int		xd = 0, yd = 0;
	UINT	first = 0, second = 0;
	uint	PreNum = eSRegionSpan / CTerrainMgr::MAP_SIZE;
	uint	PerSqr = PreNum*PreNum;

	pNode->m_TerrainTiles.resize(PerSqr);
	for( uint i =0;i<PerSqr; ++i )
	{
		pNode->m_TerrainTiles[i].op.Create();
		pNode->m_TerrainTiles[i].op.vertexData->vertexDeclaration = pVD;

		size_t VerSize;
		pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->DestroyAllBuffer();
		for ( int j = 0; j < 5; ++j )
		{
			VerSize = pVD->GetVertexSize(j);
			if (VerSize!=0)
			{
				pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->SetBinding(j,
					CHardwareBufferMgr::GetInst()->createVertexBuffer(
					VerSize,
					CTerrainMgr::MAP_SIZE_SQR*4,
					HBU_WRITE_ONLY));
			}
		}
		SyncCoder_BreakPoint();
	}

	string strLightMapPath= m_pDataScene->GetSceneName();
	strLightMapPath += "/lightmap/";
	format(strLightMapPath,"%s%d_%d.dds",strLightMapPath.c_str(),NodeId.X,NodeId.Z);
	SafeRelease(pNode->text);
	CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(),strLightMapPath.c_str(),&pNode->text);

	SyncCoder_BreakPoint();
	pNode->m_TAAB.setNull();
	for( uint i =0;i<PerSqr; ++i )
	{
		CVertexBufferBinding*	pVBB = pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding;
		CVertexHardwareBuffer*	pVerBuf0 = pVBB->GetBuffer(0);///buf0: pos, normal, diffuse
		
	// 	///buf1 specular color
	// 	CVertexHardwareBuffer*	pVerBuf1 = NULL;
	// 	pVerBuf1 = pVBB->GetBuffer(1);

		///layer0 texture uv address coordinate
		CVertexHardwareBuffer*	pVerBuf3 = pVBB->GetBuffer(3);
		///layer1 texture uv address coordinate
		CVertexHardwareBuffer*	pVerBuf4 = pVBB->GetBuffer(4);

		pVBB->SetBinding(7,pTerrain->st_LightMapBuffer[i]);
		
		TSafeBuffer<vTerrain::Stream0>   Buf0 ( pVerBuf0->lock(HBL_NO_OVERWRITE), pVerBuf0->getSize() );
		TSafeBuffer<vTerrain::StreamUV>	 Buf3 ( pVerBuf3->lock(HBL_NO_OVERWRITE), pVerBuf3->getSize() );
		TSafeBuffer<vTerrain::StreamUV>  Buf4 ( pVerBuf4->lock(HBL_NO_OVERWRITE), pVerBuf4->getSize() );
		
		CVector2f gridUV0, gridUV1, gridUV2, gridUV3;

		wi = NodeId.X*64.0f + (i % PreNum) * CTerrainMgr::MAP_SIZE;
		hi = NodeId.Z*64.0f + (i / PreNum) * CTerrainMgr::MAP_SIZE;

		wp = 0.0f;
		hp = 0.0f;
		float minH = 9999.0f;
		float maxH = -9999.0f;

		for(uint m=0;m<CTerrainMgr::MAP_SIZE;++m)
		{
			for(uint n=0;n<CTerrainMgr::MAP_SIZE;++n)
			{
				idx = (m*CTerrainMgr::MAP_SIZE+n)*4;
				x = (int)(wi+wp);
				y = (int)(hi+hp);

				xd = x == 0 ? 0 : -1;
				yd = y == 0 ? 0 : -1;


				uint32 index = GridMgr.GetTextureIndex( x, y );
				const CMetaStaticData::StaticGrid& Grid = CMetaStaticData::GetGrid( x+xd, y+yd );
				first = index>>24;
				second = (index>>16) & 0xff;

				CVector2f& uv0 = pTerrain->getUV(first);
				CVector2f& uv1 = pTerrain->getUV(second);

				if(GridMgr.GetTerrainGridIsHollow(x,y))
				{
					Buf0[idx].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx].color  = 0x00000000;

					Buf3[idx].tu = 0.0f;                 
					Buf3[idx].tv = 0.0f;
					Buf4[idx].tu = 0.0f;                 
					Buf4[idx].tv = 0.0f;
					//------------------------------------------------------------------------------------------


					//------------------------------------------------------------------------------------------
					Buf0[idx+1].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx+1].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx+1].color  = 0x00000000;

					Buf3[idx+1].tu = 0.0f;                 
					Buf3[idx+1].tv = 0.0f;
					Buf4[idx+1].tu = 0.0f;                 
					Buf4[idx+1].tv = 0.0f;
					//------------------------------------------------------------------------------------------


					//------------------------------------------------------------------------------------------
					Buf0[idx+2].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx+2].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx+2].color  = 0x00000000;

					Buf3[idx+2].tu = 0.0f;                 
					Buf3[idx+2].tv = 0.0f;
					Buf4[idx+2].tu = 0.0f;                 
					Buf4[idx+2].tv = 0.0f;

					//------------------------------------------------------------------------------------------


					//------------------------------------------------------------------------------------------
					Buf0[idx+3].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx+3].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx+3].color  = 0x00000000;

					Buf3[idx+3].tu = 0.0f;                 
					Buf3[idx+3].tv = 0.0f;
					Buf4[idx+3].tu = 0.0f;                 
					Buf4[idx+3].tv = 0.0f;

				}
				else
				{
					gridUV0 = GridMgr.GetTerranLayer1UV(x, y);
					gridUV1 = GridMgr.GetTerranLayer2UV(x, y);
					gridUV2 = GridMgr.GetTerranLayer3UV(x, y);
					gridUV3 = GridMgr.GetTerranLayer4UV(x, y);

					tf = fabs(gridUV0.x - gridUV1.x) > fabs(gridUV0.x - gridUV3.x);

					Buf0[idx].p = CVector3f((wi+wp)*64.0f,GridMgr.GetTerranHeight(x + xd,y + yd),(hi+hp)*64.0f);
					Buf0[idx].np = CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x + xd, y + yd ) );
					Buf0[idx].color  = (GridMgr.GetTerranDiffuseColor( x + xd ,y + yd ) & (second==0xff?0x00000000:0xFF000000)) | (GridMgr.GetTerranBakingColor( x + xd, y + yd )&0x00FFFFFF);

					Buf3[idx].tu = uv0.x + MagicL * (tf ? gridUV0.x : gridUV0.y);
					Buf3[idx].tv = uv0.y + MagicL * (tf ? gridUV0.y : gridUV0.x);
					
					Buf4[idx].tu = uv1.x + MagicL * gridUV0.x;
					Buf4[idx].tv = uv1.y + MagicL * gridUV0.y;

					maxH = max(maxH,Buf0[idx].p.y);
					minH = min(minH,Buf0[idx].p.y);


					Buf0[idx+1].p = CVector3f((wi+wp)*64.0f,GridMgr.GetTerranHeight(x + xd,y),(hi+hp+1.0f)*64.0f);
					Buf0[idx+1].np = CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x + xd, y ) );
					Buf0[idx+1].color  = (GridMgr.GetTerranDiffuseColor( x + xd ,y ) & (second==0xff?0x00000000:0xFF000000)) | (GridMgr.GetTerranBakingColor( x + xd, y )&0x00FFFFFF);
					
	// 				Buf2[idx+1].tu = uv0.x + MagicL * (tf ? gridUV3.x : gridUV3.y);
	// 				Buf2[idx+1].tv = uv0.y + MagicL * (tf ? gridUV3.y : gridUV3.x);

					Buf3[idx+1].tu = uv0.x + MagicL * (tf ? gridUV3.x : gridUV3.y);
					Buf3[idx+1].tv = uv0.y + MagicL * (tf ? gridUV3.y : gridUV3.x);
				
					Buf4[idx+1].tu = uv1.x + MagicL * gridUV3.x;
					Buf4[idx+1].tv = uv1.y + MagicL * gridUV3.y;


					Buf0[idx+2].p = CVector3f((wi+wp+1.0f)*64.0f,GridMgr.GetTerranHeight(x ,y+yd),(hi+hp)*64.0f);
					Buf0[idx+2].np = CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x, y+yd ) );
					Buf0[idx+2].color  = (GridMgr.GetTerranDiffuseColor( x ,y+yd ) & (second==0xff?0x00000000:0xFF000000))|(GridMgr.GetTerranBakingColor( x, y+yd )&0x00FFFFFF);
					
	// 				Buf2[idx+2].tu = uv0.x + MagicL * (tf ? gridUV1.x : gridUV1.y);
	// 				Buf2[idx+2].tv = uv0.y + MagicL * (tf ? gridUV1.y : gridUV1.x);

					Buf3[idx+2].tu = uv0.x + MagicL * (tf ? gridUV1.x : gridUV1.y);
					Buf3[idx+2].tv = uv0.y + MagicL * (tf ? gridUV1.y : gridUV1.x);
					
					Buf4[idx+2].tu = uv1.x + MagicL * gridUV1.x;
					Buf4[idx+2].tv = uv1.y + MagicL * gridUV1.y;


					Buf0[idx+3].p = CVector3f((wi+wp+1.0f)*64.0f,GridMgr.GetTerranHeight(x,y),(hi+hp+1.0f)*64.0f);
					Buf0[idx+3].np =  CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x, y) );
					Buf0[idx+3].color  = (GridMgr.GetTerranDiffuseColor( x ,y ) & (second==0xff?0x00000000:0xFF000000)) | (GridMgr.GetTerranBakingColor( x, y )&0x00FFFFFF);
				
	// 				Buf2[idx+3].tu = uv0.x + MagicL * (tf ? gridUV2.x : gridUV2.y);
	// 				Buf2[idx+3].tv = uv0.y + MagicL * (tf ? gridUV2.y : gridUV2.x);

					Buf3[idx+3].tu = uv0.x + MagicL * (tf ? gridUV2.x : gridUV2.y);
					Buf3[idx+3].tv = uv0.y + MagicL * (tf ? gridUV2.y : gridUV2.x);
					
					Buf4[idx+3].tu = uv1.x + MagicL * gridUV2.x;
					Buf4[idx+3].tv = uv1.y + MagicL * gridUV2.y;
				}
				wp += 1.0f;
			}		

			hp +=1.0f;
			wp = 0.0f;
			SyncCoder_BreakPoint();
		}

		if(	minH == 9999.0f&&maxH == -9999.0f)
		{
			minH = 0.0f;
			maxH = 0.0f;
		}

		pNode->m_TerrainTiles[i].aab.setMaximum( (wi+CTerrainMgr::MAP_SIZE)*64.0f, maxH,	(hi+CTerrainMgr::MAP_SIZE)*64.0f );
		pNode->m_TerrainTiles[i].aab.setMinimum(		wi*64.0f, minH,		 hi*64.0f );
		pNode->m_TAAB.mergeBox(pNode->m_TerrainTiles[i].aab);
		pVerBuf0->unlock();
		SyncCoder_BreakPoint();
		pVerBuf3->unlock();
		SyncCoder_BreakPoint();
		pVerBuf4->unlock();

		pNode->m_TerrainTiles[i].op.indexData->indexStart = 0;
		pNode->m_TerrainTiles[i].op.indexData->indexCount = CTerrainMgr::st_pIndex->getNumIndexes();
		pNode->m_TerrainTiles[i].op.indexData->indexBuffer = CTerrainMgr::st_pIndex;
		pNode->m_TerrainTiles[i].op.useIndexes = true;
		pNode->m_TerrainTiles[i].op.vertexData->vertexStart = 0;
		pNode->m_TerrainTiles[i].op.vertexData->vertexCount = CTerrainMgr::MAP_SIZE_SQR*4;
	}

	pNode->SetValid(true);
	return true;

}

bool CShaderTerrainCoder::DeCode_Ver_0( const CDataChunk& inData, OBJ_HANDLE handle )
{
	TerrainNode* pNode = (TerrainNode*)handle;
	CTerrainMgr* pTerrain = (CTerrainMgr*)pNode->GetParentManager();
	URegionID    id		  = pNode->GetID();

	CMetaGridMgr& GridMgr = *m_pDataScene->m_pGridMgr;
	bool bVisible = GridMgr.GetRenderTileVisibleMark(id);
	if( !bVisible )
		return true;

	CVertexDeclaration* pVD = CTerrainMgr::st_TerrainDecl[CTerrainMgr::GetVtxDeclIndex(pTerrain->GetRenderFlag())];

	uint32 SceneWidth	= m_pDataScene->GetWidth();
	URegionID NodeId	= pNode->GetID();
	bool	tf;
	float	wi,hi,wp,hp;
	uint	idx = 0, x = 0, y = 0;
	int		xd = 0, yd = 0;
	UINT	first = 0, second = 0, third = 0;
	uint	PreNum = eSRegionSpan / CTerrainMgr::MAP_SIZE;
	uint	PerSqr = PreNum*PreNum;

	pNode->m_TerrainTiles.resize(PerSqr);
	for( uint i =0;i<PerSqr; ++i )
	{
		pNode->m_TerrainTiles[i].op.Create();
		pNode->m_TerrainTiles[i].op.vertexData->vertexDeclaration = pVD;

		size_t VerSize;
		pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->DestroyAllBuffer();
		for ( int j = 0; j < 6; ++j )
		{
			VerSize = pVD->GetVertexSize(j);
			if (VerSize!=0)
			{
				pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->SetBinding(j,
					CHardwareBufferMgr::GetInst()->createVertexBuffer(
					VerSize,
					CTerrainMgr::MAP_SIZE_SQR*4,
					HBU_WRITE_ONLY));
			}
		}
		SyncCoder_BreakPoint();
	}

	string strLightMapPath= m_pDataScene->GetSceneName();
	strLightMapPath += "/lightmap/";
	format(strLightMapPath,"%s%d_%d.dds",strLightMapPath.c_str(),NodeId.X,NodeId.Z);
	SafeRelease(pNode->text);
	CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(),strLightMapPath.c_str(),&pNode->text);

	SyncCoder_BreakPoint();
	
	pNode->m_TAAB.setNull();
	for( uint i =0;i<PerSqr; ++i )
	{
		CVertexBufferBinding*	pVBB = pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding;
		CVertexHardwareBuffer*	pVerBuf0 = pVBB->GetBuffer(0);
	//	CVertexHardwareBuffer*	pVerBuf1 = NULL; pVerBuf1 = pVBB->GetBuffer(1);
		CVertexHardwareBuffer*	pVerBuf2 = pVBB->GetBuffer(2);
		CVertexHardwareBuffer*	pVerBuf3 = pVBB->GetBuffer(3);
		CVertexHardwareBuffer*	pVerBuf4 = pVBB->GetBuffer(4);
		CVertexHardwareBuffer*	pVerBuf5 = pVBB->GetBuffer(5);
		
		pVBB->SetBinding(7,pTerrain->st_LightMapBuffer[i]);

		///buf0: position, normal, baking vertex color
		TSafeBuffer<vTerrain::Stream0>  Buf0(pVerBuf0->lock(HBL_NO_OVERWRITE),pVerBuf0->getSize());

	// 	///buf1: vertex specular color
	// 	vTerrain::Color    *Buf1 = (vTerrain::Color*)pVerBuf1->lock(HBL_NO_OVERWRITE);

		///buf2: vertex original color
		TSafeBuffer<vTerrain::Color>    Buf2(pVerBuf2->lock(HBL_NO_OVERWRITE),pVerBuf2->getSize());

		///buf3: layer0 texture uv address
		TSafeBuffer<vTerrain::StreamUV> Buf3(pVerBuf3->lock(HBL_NO_OVERWRITE),pVerBuf3->getSize());

		///buf4: layer1 texture uv address
		TSafeBuffer<vTerrain::StreamUV> Buf4(pVerBuf4->lock(HBL_NO_OVERWRITE),pVerBuf4->getSize());

		///buf5: cliff texture coordinate
		TSafeBuffer<vTerrain::StreamUV> Buf5(pVerBuf5->lock(HBL_NO_OVERWRITE),pVerBuf5->getSize());

		CVector2f gridUV0, gridUV1, gridUV2, gridUV3;

		wi = NodeId.X*64.0f + (i % PreNum) * CTerrainMgr::MAP_SIZE;
		hi = NodeId.Z*64.0f + (i / PreNum) * CTerrainMgr::MAP_SIZE;

		wp = 0.0f;
		hp = 0.0f;
		float minH = 9999.0f;
		float maxH = -9999.0f;

		DWORD Diffuse = 0,Specular = 0;
		for(uint m=0;m<CTerrainMgr::MAP_SIZE;++m)
		{
			for(uint n=0;n<CTerrainMgr::MAP_SIZE;++n)
			{
				idx = (m*CTerrainMgr::MAP_SIZE+n)*4;
				x = (int)(wi+wp);
				y = (int)(hi+hp);

				xd = x ==0 ? 0 : -1;
				yd = y ==0 ? 0 : -1;

				uint32 index = GridMgr.GetTextureIndex( x, y );
				const CMetaStaticData::StaticGrid& Grid = CMetaStaticData::GetGrid( x+xd, y+yd );
				first = index>>24;
				second = (index>>16) & 0xff;
				third = (index>>8) & 0xff;


				CVector2f& uv0 = pTerrain->getUV(first);
				CVector2f& uv1 = pTerrain->getUV(second);
				CVector2f& uv2 = pTerrain->getUV(third);

				if(GridMgr.GetTerrainGridIsHollow(x,y))
				{
					Buf0[idx].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx].color  = 0x00000000;

	// 				Buf1[idx].specular  = 0x00000000;
	// 				Buf1[idx].tu0 = 0.0f;
	// 				Buf1[idx].tv0 = 0.0f;
	// 				Buf1[idx].tu1 = 0.0f;                 
	// 				Buf1[idx].tv1 = 0.0f;

					Buf2[idx].color = 0x00000000;

					Buf3[idx].tu = 0.0f;                 
					Buf3[idx].tv = 0.0f;
				
					Buf4[idx].tu = 0.0f;                 
					Buf4[idx].tv = 0.0f;

					Buf5[idx].tu = 0.0f;                 
					Buf5[idx].tv = 0.0f;

					//------------------------------------------------------------------------------------------


					//------------------------------------------------------------------------------------------
					Buf0[idx+1].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx+1].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx+1].color = 0x00000000;

	// 				Buf1[idx+1].specular  = 0x00000000;
	// 				Buf1[idx+1].tu0 = 0.0f;
	// 				Buf1[idx+1].tv0 = 0.0f;
	// 				Buf1[idx+1].tu1 = 0.0f;                 
	// 				Buf1[idx+1].tv1 = 0.0f;

					Buf2[idx+1].color = 0x00000000;

					Buf3[idx+1].tu = 0.0f;                 
					Buf3[idx+1].tv = 0.0f;
					
					Buf4[idx+1].tu = 0.0f;                 
					Buf4[idx+1].tv = 0.0f;

					Buf5[idx+1].tu = 0.0f;                 
					Buf5[idx+1].tv = 0.0f;
					//------------------------------------------------------------------------------------------


					//------------------------------------------------------------------------------------------
					Buf0[idx+2].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx+2].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx+2].color  = 0x00000000;

	// 				Buf1[idx+2].specular  = 0x00000000;
	// 				Buf1[idx+2].tu0 = 0.0f;
	// 				Buf1[idx+2].tv0 = 0.0f;
	// 				Buf1[idx+2].tu1 = 0.0f;                 
	// 				Buf1[idx+2].tv1 = 0.0f;

					Buf2[idx+2].color  = 0x00000000;

					Buf3[idx+2].tu = 0.0f;                 
					Buf3[idx+2].tv = 0.0f;

					Buf4[idx+2].tu = 0.0f;                 
					Buf4[idx+2].tv = 0.0f;

					Buf5[idx+2].tu = 0.0f;                 
					Buf5[idx+2].tv = 0.0f;
					//------------------------------------------------------------------------------------------


					//------------------------------------------------------------------------------------------
					Buf0[idx+3].p = CVector3f(0.0f,0.0f,0.0f);
					Buf0[idx+3].np = CVector3f(0.0f,1.0f,0.0f);
					Buf0[idx+3].color  = 0x00000000;

	// 				Buf1[idx+3].specular  = 0x00000000;
	// 				Buf1[idx+3].tu0 = 0.0f;
	// 				Buf1[idx+3].tv0 = 0.0f;
	// 				Buf1[idx+3].tu1 = 0.0f;                 
	// 				Buf1[idx+3].tv1 = 0.0f;

					Buf2[idx+3].color  = 0x00000000;

					Buf3[idx+3].tu = 0.0f;                 
					Buf3[idx+3].tv = 0.0f;
				
					Buf4[idx+3].tu = 0.0f;                 
					Buf4[idx+3].tv = 0.0f;

					Buf5[idx+3].tu = 0.0f;                 
					Buf5[idx+3].tv = 0.0f;
				}
				else
				{
					gridUV0 = GridMgr.GetTerranLayer1UV(x, y);
					gridUV1 = GridMgr.GetTerranLayer2UV(x, y);
					gridUV2 = GridMgr.GetTerranLayer3UV(x, y);
					gridUV3 = GridMgr.GetTerranLayer4UV(x, y);

					tf = fabs(gridUV0.x - gridUV1.x) > fabs(gridUV0.x - gridUV3.x);

					Buf0[idx].p = CVector3f((wi+wp)*64.0f,GridMgr.GetTerranHeight(x + xd,y + yd),(hi+hp)*64.0f);
					Buf0[idx].np = CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x + xd, y + yd ) );
					Diffuse		= GridMgr.GetTerranDiffuseColor( x + xd ,y + yd ) & (second==0xff?0x00FFFFFF:0xFFFFFFFF);
					Specular	=  GridMgr.GetTerranBakingColor( x + xd, y + yd ) & (third==0xff?0x00FFFFFF:0xFFFFFFFF);
					Buf0[idx].color  = (Diffuse & 0xFF000000) + (Specular & 0x00FFFFFF);
					
	// 				Buf1[idx].specular = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);
	// 				Buf1[idx].tu0 = uv0.x;
	// 				Buf1[idx].tv0 = uv0.y;
	// 				Buf1[idx].tu1 = gridUV0.x;
	// 				Buf1[idx].tv1 = gridUV0.y;

					Buf2[idx].color = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);

 					Buf3[idx].tu = uv0.x;
 					Buf3[idx].tv = uv0.y;
					
					Buf4[idx].tu = uv1.x + MagicL * gridUV0.x;
					Buf4[idx].tv = uv1.y + MagicL * gridUV0.y;

					Buf5[idx].tu = gridUV0.x;
					Buf5[idx].tv = gridUV0.y;

					maxH = max(maxH,Buf0[idx].p.y);
					minH = min(minH,Buf0[idx].p.y);

					//////////////////////////////////////////////////////////////////////////

					Buf0[idx+1].p = CVector3f((wi+wp)*64.0f,GridMgr.GetTerranHeight(x + xd,y),(hi+hp+1.0f)*64.0f);
					Buf0[idx+1].np = CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x + xd, y ) );

					Diffuse		= GridMgr.GetTerranDiffuseColor( x + xd ,y ) & (second==0xff?0x00FFFFFF:0xFFFFFFFF);
					Specular	= GridMgr.GetTerranBakingColor( x + xd, y ) & (third==0xff?0x00FFFFFF:0xFFFFFFFF);
					Buf0[idx+1].color  = (Diffuse & 0xFF000000) + (Specular & 0x00FFFFFF);
				
	// 				Buf1[idx+1].specular = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);
	// 				Buf1[idx+1].tu0 = uv0.x;
	// 				Buf1[idx+1].tv0 = uv0.y;
	// 				Buf1[idx+1].tu1 = gridUV3.x;
	// 				Buf1[idx+1].tv1 = gridUV3.y;

					Buf2[idx+1].color = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);

					Buf3[idx+1].tu = uv0.x;
					Buf3[idx+1].tv = uv0.y;
					
					Buf4[idx+1].tu = uv1.x + MagicL * gridUV3.x;
					Buf4[idx+1].tv = uv1.y + MagicL * gridUV3.y;

					Buf5[idx+1].tu = gridUV3.x;
					Buf5[idx+1].tv = gridUV3.y;

					//////////////////////////////////////////////////////////////////////////

					Buf0[idx+2].p = CVector3f((wi+wp+1.0f)*64.0f,GridMgr.GetTerranHeight(x ,y+yd),(hi+hp)*64.0f);
					Buf0[idx+2].np = CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x, y+yd ) );

					Diffuse		= GridMgr.GetTerranDiffuseColor( x ,y+yd ) & (second==0xff?0x00FFFFFF:0xFFFFFFFF);
					Specular	= GridMgr.GetTerranBakingColor( x, y+yd ) & (third==0xff?0x00FFFFFF:0xFFFFFFFF);
					Buf0[idx+2].color  = (Diffuse & 0xFF000000) + (Specular & 0x00FFFFFF);
					
	// 				Buf1[idx+2].specular = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);
	// 				Buf1[idx+2].tu0 = uv0.x;
	// 				Buf1[idx+2].tv0 = uv0.y;
	// 				Buf1[idx+2].tu1 = gridUV1.x;
	// 				Buf1[idx+2].tv1 = gridUV1.y;

					Buf2[idx+2].color = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);

					Buf3[idx+2].tu = uv0.x;
					Buf3[idx+2].tv = uv0.y;
					
					Buf4[idx+2].tu = uv1.x + MagicL * gridUV1.x;
					Buf4[idx+2].tv = uv1.y + MagicL * gridUV1.y;

					Buf5[idx+2].tu = gridUV1.x;
					Buf5[idx+2].tv = gridUV1.y;

					//////////////////////////////////////////////////////////////////////////

					Buf0[idx+3].p = CVector3f((wi+wp+1.0f)*64.0f,GridMgr.GetTerranHeight(x,y),(hi+hp+1.0f)*64.0f);
					Buf0[idx+3].np =  CMetaStaticData::GetNormal( GridMgr.GetTerranNomal( x, y) );

					Diffuse		= GridMgr.GetTerranDiffuseColor( x ,y ) & (second==0xff?0x00FFFFFF:0xFFFFFFFF);
					Specular	= GridMgr.GetTerranBakingColor( x, y ) & (third==0xff?0x00FFFFFF:0xFFFFFFFF);

					Buf0[idx+3].color  = (Diffuse & 0xFF000000) + (Specular & 0x00FFFFFF);
					
	// 				Buf1[idx+3].specular = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);
	// 				Buf1[idx+3].tu0 = uv0.x;
	// 				Buf1[idx+3].tv0 = uv0.y;
	// 				Buf1[idx+3].tu1 = gridUV2.x;
	// 				Buf1[idx+3].tv1 = gridUV2.y;

					Buf2[idx+3].color = (Diffuse & 0x00FFFFFF) + (Specular & 0xFF000000);

					Buf3[idx+3].tu = uv0.x;
					Buf3[idx+3].tv = uv0.y;
					
					Buf4[idx+3].tu = uv1.x + MagicL * gridUV2.x;
					Buf4[idx+3].tv = uv1.y + MagicL * gridUV2.y;

					Buf5[idx+3].tu = gridUV2.x;
					Buf5[idx+3].tv = gridUV2.y;

				}
				wp += 1.0f;
			}				
			hp +=1.0f;
			wp = 0.0f;
			SyncCoder_BreakPoint();
		}
		if(	minH == 9999.0f&&maxH == -9999.0f)
		{
			minH = 0.0f;
			maxH = 0.0f;
		}

		pNode->m_TerrainTiles[i].aab.setMaximum( (wi+CTerrainMgr::MAP_SIZE)*64.0f	, maxH,	(hi+CTerrainMgr::MAP_SIZE)*64.0f );
		pNode->m_TerrainTiles[i].aab.setMinimum(	wi*64.0f						, minH,	hi*64.0f );
		pNode->m_TAAB.mergeBox(pNode->m_TerrainTiles[i].aab);

		pVerBuf0->unlock();
		SyncCoder_BreakPoint();
		pVerBuf2->unlock();
		SyncCoder_BreakPoint();
		pVerBuf3->unlock();
		SyncCoder_BreakPoint();
		pVerBuf4->unlock();
		SyncCoder_BreakPoint();
		pVerBuf5->unlock();

		pNode->m_TerrainTiles[i].op.indexData->indexStart = 0;
		pNode->m_TerrainTiles[i].op.indexData->indexCount = CTerrainMgr::st_pIndex->getNumIndexes();
		pNode->m_TerrainTiles[i].op.indexData->indexBuffer = CTerrainMgr::st_pIndex;
		pNode->m_TerrainTiles[i].op.useIndexes = true;
		pNode->m_TerrainTiles[i].op.vertexData->vertexStart = 0;
		pNode->m_TerrainTiles[i].op.vertexData->vertexCount = CTerrainMgr::MAP_SIZE_SQR*4;
	}
	pNode->SetValid(true);
	return true;
}
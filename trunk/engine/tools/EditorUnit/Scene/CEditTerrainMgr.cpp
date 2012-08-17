#include "stdafx.h"
#include "CEditTerrainMgr.h"
#include "CEditLogicDataMgr.h"
#include "CEditLogicData.h"
#include "CEditTerrainNode.h"

#include "CVertexDeclaration.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"
#include "CShadowFilter.h"
#include "BaseHelper.h"

const string CEditTerrainMgr::st_DefaultMapName = "新建立的世界";

CVector3f CEditTerrainMgr::OffPos[64*64];

CEditTerrainMgr::CEditTerrainMgr(CRenderGroup* pGroup,CEditLogicDataMgr* pLogicDataMgr)
: CTerrainMgr(pGroup)
{
	m_pLogicDataMgr=pLogicDataMgr;
}

CEditTerrainMgr::~CEditTerrainMgr(void)
{

}

bool CEditTerrainMgr::SetRenderFlag(TerrainFlag::Mask mask)
{
	CTerrainMgr::SetRenderFlag(mask);
	CEditTerrainNode* pNode;
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	size_t size = m_ElePool.size();

	if (HasRenderFlag(TerrainFlag::LightMap))
	{			
		for( ; it != ite; ++it )
		{
			pNode = (CEditTerrainNode*)it->second;
			for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
			{		
				pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->SetBinding(7,st_LightMapBuffer[i]);
				pNode->m_TerrainTiles[i].op.vertexData->vertexDeclaration = st_TerrainDecl[GetVtxDeclIndex(m_Mask)];
			}
		}

		m_Param.m_RS.m_TexOP_S2   = TOP_MODULATE;
		m_Param.m_RS.m_TexArg1_S2 = TA_TEXTURE;
		m_Param.m_RS.m_TexArg2_S2 = TA_CURRENT;
		m_Param.m_RS.m_TexOP_S3	  = TOP_MODULATE2X;
	}
	else
	{
		for( ; it != ite; ++it )
		{
			pNode = (CEditTerrainNode*)it->second;
			for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
			{
				pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->UnsetBinding(7);
				pNode->m_TerrainTiles[i].op.vertexData->vertexDeclaration = st_TerrainDecl[GetVtxDeclIndex(m_Mask)];
			}
		}
		m_Param.m_RS.m_TexOP_S2   = TOP_MODULATE2X;
		m_Param.m_RS.m_TexArg1_S2 = TA_DIFFUSE;
		m_Param.m_RS.m_TexArg2_S2 = TA_CURRENT;
		m_Param.m_RS.m_TexOP_S3	  = TOP_DISABLE;
	}

	if (HasRenderFlag(TerrainFlag::Baking))
	{
		st_Terrain_VS_Shader_ID = MakeSysVS_ID(MakeGroupID(1) + 5);
	}
	else
	{
		st_Terrain_VS_Shader_ID = MakeSysVS_ID(MakeGroupID(1));
	}
	return true;
}

CElementNode* CEditTerrainMgr::_CreateNode( URegionID ID )
{
	CEditTerrainNode* pNode=new CEditTerrainNode(this,ID);
	 return pNode;
}

bool sqr_tools::CEditTerrainMgr::isVisable( URegionID ID )
{
	TerrainNode* pNode = (TerrainNode*)GetNode(ID);
	if( pNode )
	{
		return ClipStatus::Outside != m_pGroup->GetCamera()->cull(pNode->m_TAAB);
	}
	return false;
}

void sqr_tools::CEditTerrainMgr::SetLightMap( const URegionID ID, const char* filename )
{
	TerrainNode* pNode = (TerrainNode*)GetNode(ID);
	CGraphic::GetInst()->ReloadTexture(filename);
	CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(),filename,&pNode->text);
}

void sqr_tools::CEditTerrainMgr::SetLightMap( const URegionID ID, ITexture* pText )
{
	//AstMsg(pText!=NULL, "空值进来哆色啥！");
	TerrainNode* pNode = (TerrainNode*)GetNode(ID);
	if(pText != pNode->text)
	{
		SafeRelease(pNode->text);
		pNode->text = pText;
		pText->AddRef();	
	}
}

void sqr_tools::CEditTerrainMgr::Render()
{
	if( m_ElePool.empty() )
		return;

	if ( !m_Param.m_RS.m_ShaderType.IsNull() || st_isEditor ||	HasRenderFlag(TerrainFlag::Cliff) )
	{
		DWORD handle = (st_Terrain_PS_Shader_ID + (GetVtxDeclIndex(m_Mask) & ~(TerrainFlag::NormalSpecular)) );
		m_Param.m_RS.m_ShaderType |= st_Terrain_VS_Shader_ID;
		m_Param.m_RS.m_ShaderType |= handle;
	}

	CRenderPipeline::GetInst()->SetRenderStyle(m_Param.m_RS);
	CRenderPipeline::GetInst()->SetWorld(m_Param.m_Mat);
	m_ShadowFilter->UpdateShadowParams();
	RenderEx();
}

//void sqr_tools::CEditTerrainMgr::RenderEx( void )
//{
//	ElementPool::iterator it = m_ElePool.begin();
//	ElementPool::iterator ite = m_ElePool.end();
//	CRenderPipeline* rp = CRenderPipeline::GetInst();
//	CCamera* pCamera = m_pGroup->GetCamera();
//	TerrainNode* pNode = NULL;
//	CEditTerrainNode*pEditNode = NULL;
//
//	CRenderPipeline::GetInst()->_SetSamplerState(2,SAMP_ADDRESSU,TADDRESS_CLAMP );
//	CRenderPipeline::GetInst()->_SetSamplerState(2,SAMP_ADDRESSV,TADDRESS_CLAMP );
//	CRenderPipeline::GetInst()->_SetSamplerState(2, SAMP_MINFILTER, TEXF_LINEAR);
//	CRenderPipeline::GetInst()->_SetSamplerState(2, SAMP_MAGFILTER, TEXF_LINEAR);
//
//	for (;it!=ite;++it)
//	{
//		pNode = (TerrainNode*)it->second;
//		pEditNode = (CEditTerrainNode*)it->second;
//
//		if( !pNode->IsValid() || pCamera->cull(pNode->m_TAAB) == ClipStatus::Outside )
//			continue;
//
//		for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
//		{
//			if ( pEditNode->m_TerrainTileVisibles[i] && pCamera->cull(pNode->m_TerrainTiles[i].aab) > 0 )
//			{
//				rp->_SetTexture(2,pNode->text);
//				rp->RenderEx(&(pNode->m_TerrainTiles[i].op));
//			}
//		}
//	}
//}

bool CEditTerrainMgr::GenerateTerrain(int tileWidth,int tileHeight)
{
	//生成一个Tile的标准数据
	CVector3f tmpPos;
	float tmpUnit=64.0f;
	int row=0;
	int col=0;
	for(int index=0;index<eSGridSpan*eSGridSpan;index++)
	{
		row = index/eSGridSpan;
		col = index%eSGridSpan;
		tmpPos.x = col*tmpUnit;
		tmpPos.y = 0;
		tmpPos.z = row*tmpUnit;
		OffPos[index]=tmpPos;
	}

	CVector3f basicPos;
	CVector3f basicNormal(0,1,0); 
	DWORD basicColor=VERTEX_COLOR;//0x0000ff;
	string texname;
	set<URegionID> regions;
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pLogicDataMgr;
	CVertexDeclaration* pVD = CTerrainMgr::st_TerrainDecl[TerrainFlag::NormalSpecular];

	int tileNum=tileWidth*tileHeight;
	for(int i=0;i<tileNum;i++)
	{
		int	off_x = i % tileWidth;
		int	off_z = i / tileHeight;
		URegionID regionID = URegionID(off_x,off_z);
		regions.insert(regionID);
		CEditLogicData* dataNode=(CEditLogicData*)(m_pLogicDataMgr->CreateNode(regionID) );
		CEditTerrainNode* pTerrainNode=(CEditTerrainNode*)this->CreateNode(regionID);	//地形节点		
		
		basicPos.x=off_x*tmpUnit*(eSGridSpan);
		basicPos.y=0;
		basicPos.z=off_z*tmpUnit*(eSGridSpan);
		
		if(dataNode && pTerrainNode)
		{
			for(int index=0;index<eSGridSpan*eSGridSpan;index++)
			{
				tmpPos=basicPos+OffPos[index];
				dataNode->SetVertexPosition(index,tmpPos);
				dataNode->SetLogicNormal(index,basicNormal);
				dataNode->SetVertexDiffuse(index,basicColor);
				dataNode->SetWaterHeight(index, 0.0f);
				dataNode->SetBlockType(index, 0);
				dataNode->SetMaterialIndex(index, -1);
				dataNode->SetGridHideMark(index, false);
				dataNode->SetGridTextureName(index, 0, "res\\default.bmp");
			}

			pTerrainNode->m_TerrainTileVisibles.resize(4);
			pTerrainNode->m_TerrainTiles.resize(4);
			for ( uint m = 0; m < 4; ++m )
			{
				pTerrainNode->m_TerrainTileVisibles[m] = true;
				pTerrainNode->m_TerrainTiles[m].op.Create();
				pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexDeclaration = pVD;
				size_t VerSize = 0;
				pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding->DestroyAllBuffer();
				for ( int jj = 0; jj < 6; ++jj )
				{
					VerSize = pVD->GetVertexSize(jj);
					if (VerSize!=0)
					{
						pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding->SetBinding(jj,
							CHardwareBufferMgr::GetInst()->createVertexBuffer(
							VerSize,
							CTerrainMgr::MAP_SIZE_SQR*4,
							HBU_DYNAMIC,NULL,false,true));
					}
				}
			}
		}
	}

	WriteMapBuffer(regions);
	LoadTexture("map.jpg");
	SetRenderFlag(TerrainFlag::NormalSpecular);

	return true;
}

void CEditTerrainMgr::WriteMapBuffer(const set<URegionID> regions)
{
	if( regions.empty() )
		return;

	vector<sqr::vTerrain::Stream0>  Buf0;// buf0: pos, normal, diffuse(rgb为烘培顶点色，a为1，2层纹理混合alpha值)
	vector<sqr::vTerrain::Color>    Buf1;// buf1: vertex specular color顶点高光色
	vector<sqr::vTerrain::Color>    Buf2;// buf2: vertex original color顶点原始色（包括刷的顶点色）
	vector<sqr::vTerrain::StreamUV> Buf3;// buf3: layer0 texture uv address一层纹理在大纹理中寻址的坐标
	vector<sqr::vTerrain::StreamUV> Buf4;// buf3: layer1 texture uv address二层纹理在大纹理中寻址的坐标
	vector<sqr::vTerrain::StreamUV> Buf5;// buf4: cliff texture coordinate悬崖贴图坐标
	int dpTileVertexCount = CTerrainMgr::MAP_SIZE_SQR * 4;   //32*32*4
	Buf0.resize(dpTileVertexCount);
	Buf1.resize(dpTileVertexCount);
	Buf2.resize(dpTileVertexCount);
	Buf3.resize(dpTileVertexCount);
	Buf4.resize(dpTileVertexCount);
	Buf5.resize(dpTileVertexCount);
	
	string texname;
	int index = -1;
	CVector2f uv0, uv1;
	const float MagicL	= 128.5f/2304.0f;
	const float Magic	= 0.0f;
	const float MagicA	= Magic/2;
	float delta = 0.001f;
	float U[2][4] =
	{
		{delta, 0.5f, 0.5f, delta},
		{0.5f, 1.0f - delta, 1.0f - delta, 0.5f}
	};
	float V[2][4] =
	{
		{1.0f - delta, 1.0f - delta, 0.5f, 0.5f},
		{0.5f, 0.5f, delta, delta}
	};

	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pLogicDataMgr;

	set<URegionID>::const_iterator beg = regions.begin();
	set<URegionID>::const_iterator end = regions.end();
	for( int i = 0; beg != end; ++beg )
	{
		URegionID regionID = *beg;
		int	off_x = regionID.X;
		int	off_z = regionID.Z;
		
		CEditLogicData* logicData=pEditLogicDataMgr->GetEditLogicData(regionID);
		CEditTerrainNode* pTerrainNode=(CEditTerrainNode*)GetNode(regionID);
		if(logicData && pTerrainNode)
		{
			pTerrainNode->m_TerrainTileVisibles.resize(4);
			//填充Tile数据 一个Tile=64*64格子 相当于现在概念的一个区域 下分4个SubTile 每个SubTile 一个DP
			pTerrainNode->m_TerrainTiles.resize(4);
			for ( uint m = 0; m < 4; ++m )
			{
				float wi = off_x*64.0f + (m % 2) * CTerrainMgr::MAP_SIZE;
				float hi = off_z*64.0f + (m / 2) * CTerrainMgr::MAP_SIZE;
				float minH = 9999.0f;
				float maxH = -9999.0f;	// 顶点高度范围

				int GridBasicOffX=(m%2)*CTerrainMgr::MAP_SIZE;
				int GridBasicOffZ=(m/2)*CTerrainMgr::MAP_SIZE;

				for(unsigned int subTileGIndex=0;subTileGIndex<CTerrainMgr::MAP_SIZE*CTerrainMgr::MAP_SIZE;subTileGIndex++)
				{
					int GridTmpOffX=subTileGIndex%CTerrainMgr::MAP_SIZE;
					int GridTmpOffZ=subTileGIndex/CTerrainMgr::MAP_SIZE;

					int subGridIndex0,subGridIndex1,subGridIndex2,subGridIndex3;
					subGridIndex0=(GridBasicOffX+GridTmpOffX)+(GridBasicOffZ+GridTmpOffZ)*eSGridSpan;
					subGridIndex1=subGridIndex0+eSGridSpan;
					subGridIndex2=subGridIndex0+1;
					subGridIndex3=subGridIndex0+1+eSGridSpan;

					sqr::vTerrain::Stream0  tmpSteam0;
					sqr::vTerrain::Stream0  tmpSteam1;
					sqr::vTerrain::Stream0  tmpSteam2;
					sqr::vTerrain::Stream0  tmpSteam3;

					sqr::vTerrain::Color	tmpColor;

					int x = subGridIndex0 % eSGridSpan;
					int z = subGridIndex0 / eSGridSpan;
					bool bXBorder = (GridBasicOffX+GridTmpOffX)%eSGridSpan == 63;
					bool bZBorder = (GridBasicOffZ+GridTmpOffZ)%eSGridSpan == 63;
					if( bXBorder || bZBorder )
					{
						int x = subGridIndex0 % eSGridSpan;
						int z = subGridIndex0 / eSGridSpan;

						//DebugOut("grid0 %d, %d, %d, %d, %d, %d\n", subGridIndex0, subGridIndex1, subGridIndex2, subGridIndex3, x, z);

						tmpSteam0.p=logicData->GetVertexPosition(subGridIndex0);
						tmpSteam0.np=logicData->GetLogicNormal(subGridIndex0);
						tmpSteam0.color=logicData->GetVertexDiffuse(subGridIndex0);

						x = subGridIndex1 % eSGridSpan;
						z = bZBorder ? (subGridIndex1 / eSGridSpan + off_z * eSGridSpan): subGridIndex1 / eSGridSpan;
						tmpSteam1.p=pEditLogicDataMgr->GetVertexPosition(x, z);
						tmpSteam1.p.x = tmpSteam0.p.x; tmpSteam1.p.z = tmpSteam0.p.z + 64.0f;
						tmpSteam1.np=pEditLogicDataMgr->GetLogicNormal(x, z);
						tmpSteam1.color=pEditLogicDataMgr->GetVertexDiffuse(x, z);

						x = bXBorder ? subGridIndex2 : subGridIndex2 % eSGridSpan;
						z = subGridIndex2 / eSGridSpan;
						tmpSteam2.p=pEditLogicDataMgr->GetVertexPosition(x, z);
						tmpSteam2.p.x = tmpSteam0.p.x + 64.0f; tmpSteam2.p.z = tmpSteam0.p.z; 
						tmpSteam2.np=pEditLogicDataMgr->GetLogicNormal(x, z);
						tmpSteam2.color=pEditLogicDataMgr->GetVertexDiffuse(x, z);

						x = bXBorder ? subGridIndex3 : subGridIndex3 % eSGridSpan;
						z = subGridIndex3 / eSGridSpan;
						tmpSteam3.p=pEditLogicDataMgr->GetVertexPosition(x, z);
						tmpSteam3.p.x = tmpSteam0.p.x + 64.0f;	tmpSteam3.p.z = tmpSteam0.p.z + 64.0f;
						tmpSteam3.np=pEditLogicDataMgr->GetLogicNormal(x, z);
						tmpSteam3.color=pEditLogicDataMgr->GetVertexDiffuse(x, z);
					}
					else
					{
						tmpSteam0.p=logicData->GetVertexPosition(subGridIndex0);
						tmpSteam0.np=logicData->GetLogicNormal(subGridIndex0);
						tmpSteam0.color=logicData->GetVertexDiffuse(subGridIndex0);

						tmpSteam1.p=logicData->GetVertexPosition(subGridIndex1);
						tmpSteam1.np=logicData->GetLogicNormal(subGridIndex1);
						tmpSteam1.color=logicData->GetVertexDiffuse(subGridIndex1);

						tmpSteam2.p=logicData->GetVertexPosition(subGridIndex2);
						tmpSteam2.np=logicData->GetLogicNormal(subGridIndex2);
						tmpSteam2.color=logicData->GetVertexDiffuse(subGridIndex2);

						tmpSteam3.p=logicData->GetVertexPosition(subGridIndex3);
						tmpSteam3.np=logicData->GetLogicNormal(subGridIndex3);
						tmpSteam3.color=logicData->GetVertexDiffuse(subGridIndex3);

						//DebugOut("%d, %d, %d, %d, %d, %d\n", subGridIndex0, subGridIndex1, subGridIndex2, subGridIndex3, x, z);
					}

					int gridTypeX=subGridIndex0%eSGridSpan;
					int gridTypeZ=subGridIndex0/eSGridSpan;

					Buf0[subTileGIndex*4+0]=tmpSteam0;
					Buf0[subTileGIndex*4+1]=tmpSteam1;
					Buf0[subTileGIndex*4+2]=tmpSteam2;
					Buf0[subTileGIndex*4+3]=tmpSteam3;

					maxH = max(maxH,Buf0[subTileGIndex*4+0].p.y);
					minH = min(minH,Buf0[subTileGIndex*4+0].p.y);

					tmpColor.color=logicData->GetVertexDiffuse(subGridIndex0);

					texname = logicData->GetGridTextureName(subGridIndex0, 0);
					if ( texname == "" )
						uv0 = CVector2f(0.0f, 0.0f);
					else
					{
						index = pEditLogicDataMgr->GetTextureIndex(texname);
						if( index == -1 )
							uv0 = CVector2f(0.0f, 0.0f);
						else
							uv0 = getUV(pEditLogicDataMgr->GetTextureIndex(texname));
					}

					texname = logicData->GetGridTextureName(subGridIndex0, 1);
					if( texname != "" )
						int test = 0;

					if ( texname == "" )
						uv1 = CVector2f(0.0f, 0.0f);
					else
					{
						index = pEditLogicDataMgr->GetTextureIndex(texname);
						if( index == -1 )
							uv1 = CVector2f(0.0f, 0.0f);
						else
							uv1 = getUV(pEditLogicDataMgr->GetTextureIndex(texname));
					}

					int nUX = x % 2;
					int nUZ = z % 2;
					CVector2f vertexuv0 = CVector2f(U[nUX][0], V[nUZ][0]);
					CVector2f vertexuv1 = CVector2f(U[nUX][3], V[nUZ][3]);
					CVector2f vertexuv2 = CVector2f(U[nUX][1], V[nUZ][1]);
					CVector2f vertexuv3 = CVector2f(U[nUX][2], V[nUZ][2]);

					Buf1[subTileGIndex*4+0]=tmpColor;
					Buf2[subTileGIndex*4+0]=tmpColor;
					Buf3[subTileGIndex*4+0].tu=uv0.x + MagicL * vertexuv0.x;
					Buf3[subTileGIndex*4+0].tv=uv0.y + MagicL * vertexuv0.y;
					Buf4[subTileGIndex*4+0].tu=uv1.x + MagicL * vertexuv0.x;
					Buf4[subTileGIndex*4+0].tv=uv1.y + MagicL * vertexuv0.y;
					Buf5[subTileGIndex*4+0].tu=vertexuv0.x;
					Buf5[subTileGIndex*4+0].tv=vertexuv0.y;

					Buf1[subTileGIndex*4+1]=tmpColor;
					Buf2[subTileGIndex*4+1]=tmpColor;
					Buf3[subTileGIndex*4+1].tu=uv0.x + MagicL * vertexuv1.x;
					Buf3[subTileGIndex*4+1].tv=uv0.y + MagicL * vertexuv1.y;
					Buf4[subTileGIndex*4+1].tu=uv1.x + MagicL * vertexuv1.x;
					Buf4[subTileGIndex*4+1].tv=uv1.y + MagicL * vertexuv1.y;
					Buf5[subTileGIndex*4+1].tu=vertexuv1.x;
					Buf5[subTileGIndex*4+1].tv=vertexuv1.y;

					Buf1[subTileGIndex*4+2]=tmpColor;
					Buf2[subTileGIndex*4+2]=tmpColor;
					Buf3[subTileGIndex*4+2].tu=uv0.x + MagicL * vertexuv2.x;
					Buf3[subTileGIndex*4+2].tv=uv0.y + MagicL * vertexuv2.y;
					Buf4[subTileGIndex*4+2].tu=uv1.x + MagicL * vertexuv2.x;
					Buf4[subTileGIndex*4+2].tv=uv1.y + MagicL * vertexuv2.y;
					Buf5[subTileGIndex*4+2].tu=vertexuv2.x;
					Buf5[subTileGIndex*4+2].tv=vertexuv2.y;

					Buf1[subTileGIndex*4+3]=tmpColor;
					Buf2[subTileGIndex*4+3]=tmpColor;
					Buf3[subTileGIndex*4+3].tu=uv0.x + MagicL * vertexuv3.x;
					Buf3[subTileGIndex*4+3].tv=uv0.y + MagicL * vertexuv3.y;
					Buf4[subTileGIndex*4+3].tu=uv1.x + MagicL * vertexuv3.x;
					Buf4[subTileGIndex*4+3].tv=uv1.y + MagicL * vertexuv3.y;
					Buf5[subTileGIndex*4+3].tu=vertexuv3.x;
					Buf5[subTileGIndex*4+3].tv=vertexuv3.y;
				}
				pTerrainNode->m_TerrainTiles[m].op.indexData->indexStart = 0;
				pTerrainNode->m_TerrainTiles[m].op.indexData->indexCount = CTerrainMgr::st_pIndex->getNumIndexes();
				pTerrainNode->m_TerrainTiles[m].op.indexData->indexBuffer = CTerrainMgr::st_pIndex;
				pTerrainNode->m_TerrainTiles[m].op.useIndexes = true;
				pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexStart = 0;
				pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexCount = CTerrainMgr::MAP_SIZE_SQR*4;

				pTerrainNode->m_TerrainTiles[m].aab.setMaximum( (wi+CTerrainMgr::MAP_SIZE)*64.0f, maxH,	(hi+CTerrainMgr::MAP_SIZE)*64.0f );
				pTerrainNode->m_TerrainTiles[m].aab.setMinimum(  wi*64.0f,						   minH,	 hi*64.0f  );
				pTerrainNode->m_TAAB.mergeBox(pTerrainNode->m_TerrainTiles[m].aab);

				CVertexBufferBinding*	pVBB = pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding;
				CVertexHardwareBuffer* pBuf0 = pVBB->GetBuffer(0);
				pBuf0->write(0, pBuf0->getSize(), &(Buf0[0]), true);
				CVertexHardwareBuffer* pBuf1 = pVBB->GetBuffer(1);
				pBuf1->write(0, pBuf1->getSize(), &(Buf1[0]), true);
				CVertexHardwareBuffer* pBuf2 = pVBB->GetBuffer(2);
				pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);
				CVertexHardwareBuffer* pBuf3 = pVBB->GetBuffer(3);
				pBuf3->write(0, pBuf3->getSize(), &(Buf3[0]), true);
				CVertexHardwareBuffer* pBuf4 = pVBB->GetBuffer(4);
				pBuf4->write(0, pBuf4->getSize(), &(Buf4[0]), true);
				CVertexHardwareBuffer* pBuf5 = pVBB->GetBuffer(5);
				pBuf5->write(0, pBuf5->getSize(), &(Buf5[0]), true);
			}		
		}
		pTerrainNode->SetValid(true);
	}
	SetRenderFlag(TerrainFlag::NormalSpecular);
	Buf0.clear();
	Buf1.clear();
	Buf2.clear();
	Buf3.clear();
	Buf4.clear();
	Buf5.clear();
}

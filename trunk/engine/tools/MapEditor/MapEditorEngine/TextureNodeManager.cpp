#include "StdAfx.h"
#include "TextureNodeManager.h"
#include "Render.h"
#include "CMapEditApp.h"
#include "River.h"
#include "TerrainMeshVertex.h"
#include "TerrainMeshGrid.h"
#include "TerrainMesh.h"
#include "Operator.h"

//const DWORD Layer_One_Shader =0;
//const DWORD Layer_Two_Shader =0;
//const DWORD Const_Layer_Tree_Shader = 0;
//DWORD Layer_Tree_Shader = 0;

const DWORD Layer_One_Shader =0xFFFF07D1;
const DWORD Layer_Two_Shader =0xFFFF07D2;
const DWORD Const_Layer_Tree_Shader = 0xFFFF07D3;
DWORD Layer_Tree_Shader = 0xFFFF07D3;

namespace bly
{
	DWORD GetAdjustedColor(BYTE * pARGB, float fTimeRatio)
	{	
		if ( CMapEditApp::GetInstance()->GetOperator()->GetDegree()  == 0 )
		{
			return *(DWORD*)(pARGB);
		}

		if ( CMapEditApp::GetInstance()->GetOperator()->GetDegree()  == 1 )
		{
			BYTE b = BYTE(pARGB[0] * (1.0f + (1.0f - (pARGB[0]/255.0f))*fTimeRatio)); 
			if ( b < pARGB[0] )
				b = 255;

			BYTE g = BYTE(pARGB[1] * (1.0f + (1.0f - (pARGB[1]/255.0f))*fTimeRatio)); 
			if ( g < pARGB[1] ) 
				g = 255;

			BYTE r = BYTE(pARGB[2] * (1.0f + (1.0f - (pARGB[2]/255.0f))*fTimeRatio));

			if ( r < pARGB[2] ) 
				r = 255;

			return (pARGB[3]<<24)|(r<<16)|(g<<8)|b;
		}
		else
			return 0;
	}

	void CTextureNode::Render(SGrid * pGrids, SVertex * pVertices, bool bWater, float fTimeRatio)
	{
		static CRenderDevice * pRender = CRenderDevice::GetInstance();
		static CTerrainMesh * pTerrain = CMapEditApp::GetInstance()->GetTerrainMesh();
		static COperator * pOperator = CMapEditApp::GetInstance()->GetOperator();

		static D3DXVECTOR3 vertex[VERTEX_BATCH_SIZE];
		static D3DXVECTOR2 uv0[VERTEX_BATCH_SIZE];
		static D3DXVECTOR2 uv1[VERTEX_BATCH_SIZE];
		static DWORD color[VERTEX_BATCH_SIZE];
		static DWORD scolor[VERTEX_BATCH_SIZE];
		static D3DXVECTOR3 nml[VERTEX_BATCH_SIZE];

		static D3DXVECTOR3 vertexBlock[VERTEX_BATCH_SIZE];
		static DWORD colorBlock[VERTEX_BATCH_SIZE];

		static VerNCT  rvertex[VERTEX_BATCH_SIZE];
		static VerNCST1 tvertex[VERTEX_BATCH_SIZE];
		static VerColorTex rvertexBlock[VERTEX_BATCH_SIZE];

		bool bWireFrame = pOperator->GetWireFrame();
		bool bHideVertexColor = pOperator->GetHideVertexColor();
		bool bHideLayerOne = pOperator->GetHideLayerOneTexture();
		bool bHideLayerTwo = pOperator->GetHideLayerTwoTexture();
		bool bHideLayerThree = pOperator->GetHideLayerThreeTexture();
		bool bSetRegionTool = pOperator->GetIsEditRegionTool();
	
		DWORD c[4];
		const float mipMapBias = -0.5f;

		if ( !bWater )
		{
			DWORD dwTextureLayer = this->GetTextureLayer();

			if ( dwTextureLayer != INVALID )
			{
				if( dwTextureLayer == 0 )
				{
					std::string layer1 = this->GetTextureName(0);

					m_pLayerOneTexture = pRender->GetTexture(layer1);
				}
				else if( dwTextureLayer == 1)
				{
					std::string layer1 = this->GetTextureName(0);
					std::string layer2 = this->GetTextureName(1);

					m_pLayerOneTexture = pRender->GetTexture(layer1);
					m_pLayerSecondTexture = pRender->GetTexture(layer2);

				}
				else if( dwTextureLayer == 2)
				{
					std::string layer1 = this->GetTextureName(0);
					std::string layer2 = this->GetTextureName(1);
					std::string layer3 = this->GetTextureName(2);

					m_pLayerOneTexture = pRender->GetTexture(layer1);
					m_pLayerSecondTexture = pRender->GetTexture(layer2);
					m_pLayerThreeTexture = pRender->GetTexture(layer3);
				}
				else if( dwTextureLayer == 3)
				{
					std::string layer1 = this->GetTextureName(0);
					std::string layer2 = this->GetTextureName(1);
					std::string layer3 = this->GetTextureName(2);
					std::string layer4 = this->GetTextureName(3);

					m_pLayerOneTexture = pRender->GetTexture(layer1);
					m_pLayerThreeTexture = pRender->GetTexture(layer3);
					m_pLayerSecondTexture = pRender->GetTexture(layer4);

				}
				//////////////////////////////////////////////////////////////////////////

				if( bHideLayerOne )
					m_pLayerOneTexture = NULL;
				if( bHideLayerTwo )
					m_pLayerSecondTexture = NULL;
				if( bHideLayerThree )
				{
					m_pLayerThreeTexture = NULL;
					Layer_Tree_Shader = Layer_Two_Shader;
				}
				else
					Layer_Tree_Shader = Const_Layer_Tree_Shader;

				DWORD cnt = this->GetGridCnt();


				CMapEditApp::GetInstance()->GetOperator()->AddToPrimitiveCnt(cnt*2);

				DWORD drawnCnt = 0, n = 0;
				for ( n = 0, drawnCnt = 0; n < cnt; n++ )
				{	
					if ( cnt && drawnCnt && !((drawnCnt) % GRID_BATCH_SIZE) )
					{	
						if ( dwTextureLayer == 0 )
						{
							pRender->DrawLayer3DSolidQuad(&m_pRS, tvertex, m_pLayerOneTexture, NULL, NULL, NULL, GRID_BATCH_SIZE, Layer_One_Shader); 

							m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
							m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);
							m_pRS->m_TexOP_S0 = MODULATETYPE ;
							m_pRS->m_TexArg1_S0 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S0 = D3DTA_DIFFUSE;
							m_pRS->m_AlphaBlendEnable = false;
							m_pRS->m_AlphaTestEnable = false;

							m_pRS->m_LightEnable = TRUE;
							m_pRS->m_ZTestEnable = true;
							m_pRS->m_ZWR = true;

							if( bWireFrame )
								m_pRS->m_FillMode = D3DFILL_WIREFRAME;
						}
						else if ( dwTextureLayer == 1 || dwTextureLayer == 3 )
						{
							pRender->DrawLayer3DSolidQuad(&m_pRS, tvertex, m_pLayerOneTexture, m_pLayerSecondTexture, NULL, NULL, GRID_BATCH_SIZE, Layer_Two_Shader);

							m_pRS->m_MipMapBias_S0 = *(DWORD*)&(mipMapBias);
							m_pRS->m_MipMapBias_S1 = *(DWORD*)&(mipMapBias);

							m_pRS->m_TexOP_S0 = MODULATETYPE;
							m_pRS->m_TexArg1_S0 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S0 = D3DTA_DIFFUSE;

							m_pRS->m_TexOP_S1 = D3DTOP_BLENDDIFFUSEALPHA;
							m_pRS->m_TexArg1_S1 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S1 = D3DTA_CURRENT;

							m_pRS->m_AlphaBlendEnable = false;
							m_pRS->m_AlphaTestEnable = false;

							m_pRS->m_LightEnable = TRUE;
							m_pRS->m_ZTestEnable = true;
							m_pRS->m_ZWR = true;

							if( bWireFrame )
								m_pRS->m_FillMode = D3DFILL_WIREFRAME;
						}
						else if ( dwTextureLayer == 2 )
						{
							pRender->DrawLayer3DSolidQuad(&m_pRS, tvertex, m_pLayerOneTexture, m_pLayerSecondTexture, m_pLayerThreeTexture, NULL, GRID_BATCH_SIZE, Layer_Tree_Shader);

							m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
							m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);

							m_pRS->m_TexOP_S0 = MODULATETYPE;
							m_pRS->m_TexArg1_S0 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S0 = D3DTA_DIFFUSE;

							m_pRS->m_TexOP_S1 = D3DTOP_BLENDDIFFUSEALPHA;
							m_pRS->m_TexArg1_S1 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S1 = D3DTA_CURRENT;

							m_pRS->m_TexOP_S2 = D3DTOP_BLENDDIFFUSEALPHA;
							m_pRS->m_TexArg1_S2 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S2 = D3DTA_CURRENT;

							m_pRS->m_TexOP_S3 = D3DTOP_BLENDDIFFUSEALPHA;
							m_pRS->m_TexArg1_S3 = D3DTA_TEXTURE;
							m_pRS->m_TexArg2_S3 = D3DTA_CURRENT;

							m_pRS->m_AlphaBlendEnable = false;
							m_pRS->m_AlphaTestEnable = false;

							m_pRS->m_LightEnable = TRUE;
							m_pRS->m_ZTestEnable = true;
							m_pRS->m_ZWR = true;

							if( bWireFrame )
								m_pRS->m_FillMode = D3DFILL_WIREFRAME;
						}
						drawnCnt = 0;
					}

					DWORD index = m_vGridIndices[n];

					SGrid & grid = pGrids[index];

					if( grid.bGridHide )
						continue;

					if ( grid.eLodLevel == ELL_0 )
					{
						uint Index[] = {0, 3, 2, 2, 1, 0};
						uint Rindex[] = {0, 3, 1, 3, 2, 1};
						uint* index = grid.bReverse ? Rindex : Index;
						c[0] =  GetAdjustedColor((BYTE*)&pVertices[grid.dwVertexIndex[0]].cColor,fTimeRatio);
						c[1] =  GetAdjustedColor((BYTE*)&pVertices[grid.dwVertexIndex[1]].cColor,fTimeRatio);
						c[2] =  GetAdjustedColor((BYTE*)&pVertices[grid.dwVertexIndex[2]].cColor,fTimeRatio);
						c[3] =  GetAdjustedColor((BYTE*)&pVertices[grid.dwVertexIndex[3]].cColor,fTimeRatio);

						//仅仅只是隐藏rgb3个分量值，但仍保留顶点alpha值
						if( bHideVertexColor )
						{
							c[0]  = ( c[0] & 0xff000000 ) | 0x00808080;
							c[1]  = ( c[1] & 0xff000000 ) | 0x00808080;
							c[2]  = ( c[2] & 0xff000000 ) | 0x00808080;
							c[3]  = ( c[3] & 0xff000000 ) | 0x00808080;
						}

						for (int i = 0; i < 6; i++)
						{
							uint curIndex = 6 * drawnCnt + i;
							vertex[curIndex] = pVertices[grid.dwVertexIndex[index[i]]].vPosition;
							nml[curIndex] = pVertices[grid.dwVertexIndex[index[i]]].vNormal;

							if ( grid.bInHideRegion && bSetRegionTool )
								color[curIndex] = pVertices[grid.dwVertexIndex[index[i]]].cColor & 0xffff0000;
							else
								color[curIndex] = c[index[i]];

							uv0[curIndex] = grid.uv[0][index[i]];
							uv1[curIndex] = grid.uv[1][index[i]];

							scolor[curIndex] = pVertices[grid.dwVertexIndex[index[i]]].sColor;

							tvertex[curIndex] = VerNCST1(vertex[curIndex].x, vertex[curIndex].y, vertex[curIndex].z
								, nml[curIndex].x, nml[curIndex].y, nml[curIndex].z
								, color[curIndex], scolor[curIndex]
							, uv0[curIndex].x, uv0[curIndex].y, uv1[curIndex].x, uv1[curIndex].y);
						}

						drawnCnt++;
					}
				}

				if( drawnCnt > 0)
				{
					if ( dwTextureLayer == 0 )
					{
						pRender->DrawLayer3DSolidQuad(&m_pRS, tvertex, m_pLayerOneTexture, m_pLayerSecondTexture, NULL, NULL, drawnCnt, Layer_One_Shader);

						m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
						m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);

						m_pRS->m_TexOP_S0 = MODULATETYPE ;

						m_pRS->m_TexArg1_S0 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S0 = D3DTA_DIFFUSE;

						m_pRS->m_AlphaBlendEnable = false;
						m_pRS->m_AlphaTestEnable = false;
						m_pRS->m_LightEnable = TRUE;
						m_pRS->m_ZTestEnable = true;
						m_pRS->m_ZWR = true;

						if( bWireFrame )
							m_pRS->m_FillMode = D3DFILL_WIREFRAME;
					}
					else if ( dwTextureLayer == 1 )
					{
						pRender->DrawLayer3DSolidQuad(&m_pRS, tvertex, m_pLayerOneTexture, m_pLayerSecondTexture, NULL, NULL, drawnCnt, Layer_Two_Shader);

						m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
						m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);

						m_pRS->m_TexOP_S0 = MODULATETYPE;
						m_pRS->m_TexArg1_S0 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S0 = D3DTA_DIFFUSE;

						m_pRS->m_TexOP_S1 = D3DTOP_BLENDDIFFUSEALPHA;
						m_pRS->m_TexArg1_S1 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S1 = D3DTA_CURRENT;

						m_pRS->m_AlphaBlendEnable = false;
						m_pRS->m_AlphaTestEnable = false;

						m_pRS->m_LightEnable = TRUE;
						m_pRS->m_ZTestEnable = true;
						m_pRS->m_ZWR = true;

						if( bWireFrame )
							m_pRS->m_FillMode = D3DFILL_WIREFRAME;
					}
					else if ( dwTextureLayer == 2 )
					{
						pRender->DrawLayer3DSolidQuad(&m_pRS, tvertex, m_pLayerOneTexture, m_pLayerSecondTexture, m_pLayerThreeTexture, NULL, drawnCnt, Layer_Tree_Shader);

						m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
						m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);
						m_pRS->m_UVSrc_S0 = 0;

						m_pRS->m_TexOP_S0 = MODULATETYPE;
						m_pRS->m_TexArg1_S0 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S0 = D3DTA_DIFFUSE;

						m_pRS->m_TexOP_S1 = D3DTOP_BLENDDIFFUSEALPHA;
						m_pRS->m_UVSrc_S1 = 0;
						m_pRS->m_TexArg1_S1 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S1 = D3DTA_CURRENT;

						m_pRS->m_TexOP_S2 = D3DTOP_BLENDDIFFUSEALPHA;
						m_pRS->m_TexArg1_S2 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S2 = D3DTA_CURRENT;

						m_pRS->m_AlpOP_S2 = D3DTOP_SELECTARG1;
						m_pRS->m_AlpArg1_S2 = D3DTA_TEXTURE;
						m_pRS->m_UVSrc_S2 = 0;

						m_pRS->m_TexOP_S3 = D3DTOP_BLENDDIFFUSEALPHA;
						m_pRS->m_TexArg1_S3 = D3DTA_TEXTURE;
						m_pRS->m_TexArg2_S3 = D3DTA_CURRENT;
						m_pRS->m_UVSrc_S3 = 0;

						m_pRS->m_AlphaBlendEnable = false;
						m_pRS->m_AlphaTestEnable = false;

						m_pRS->m_LightEnable = TRUE;
						m_pRS->m_ZTestEnable = true;
						m_pRS->m_ZWR = true;

						if( bWireFrame )
							m_pRS->m_FillMode = D3DFILL_WIREFRAME;
					}
				}

				if ( CMapEditApp::GetInstance()->GetOperator()->GetDoesShowBlock() )
				{
					// use depth bias
					pRender->GetICoreGraphic()->SetZDelta( -0.1f );
					DWORD drawnCntBlock = 0, n = 0;
					for (n, drawnCntBlock; n < cnt; n++ )
					{	
						if ( drawnCntBlock && !((drawnCntBlock) % GRID_BATCH_SIZE) )
						{	
							// use depth bias
							pRender->DrawLayer3DWireQuad(&m_pRS, rvertexBlock, GRID_BATCH_SIZE);

// 							m_pRS->m_TexOP_S0 = D3DTOP_SELECTARG1;
// 							m_pRS->m_TexArg1_S0 = D3DTA_DIFFUSE;
// 							m_pRS->m_LightEnable = false;
// 							m_pRS->m_ZTestEnable = true;
// 							m_pRS->m_ZWR = true;
// 							m_pRS->m_Cull = D3DCULL_CCW;
// 							CMapEditApp::GetInstance()->GetOperator()->AddToPrimitiveCnt(drawnCntBlock*2);
							m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
							m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);
							m_pRS->m_TexOP_S0 = D3DTOP_SELECTARG1;
							m_pRS->m_TexArg1_S0 = D3DTA_DIFFUSE;

							m_pRS->m_AlpOP_S0 = D3DTOP_SELECTARG1;
							m_pRS->m_AlpArg1_S0 = D3DTA_TEXTURE;

							m_pRS->m_AlphaTestEnable = true;
							m_pRS->m_Alpharef = 0x40;
							m_pRS->m_AlphaTestFun = D3DCMP_GREATER; 
							m_pRS->m_AlphaBlendEnable = false;
							m_pRS->m_LightEnable = false;
							m_pRS->m_ZTestEnable = false;
							m_pRS->m_ZWR = true;
							m_pRS->m_Cull = D3DCULL_CCW;

							drawnCntBlock = 0;
						}

						DWORD index = m_vGridIndices[n];

						SGrid & grid = pGrids[index];

						if ( !grid.vecBlockTypes.empty() || grid.nBlockType )
						{
							vertexBlock[6*drawnCntBlock+0] = pVertices[grid.dwVertexIndex[0]].vPosition;
							vertexBlock[6*drawnCntBlock+1] = pVertices[grid.dwVertexIndex[3]].vPosition;
							vertexBlock[6*drawnCntBlock+2] = pVertices[grid.dwVertexIndex[1]].vPosition;

							vertexBlock[6*drawnCntBlock+3] = pVertices[grid.dwVertexIndex[3]].vPosition;
							vertexBlock[6*drawnCntBlock+4] = pVertices[grid.dwVertexIndex[2]].vPosition;
							vertexBlock[6*drawnCntBlock+5] = pVertices[grid.dwVertexIndex[1]].vPosition;

							int nBlockType;

							if ( !grid.vecBlockTypes.empty() )
								nBlockType = grid.vecBlockTypes[0].first;
							else
								nBlockType = grid.nBlockType; 

							if ( nBlockType == 3 ) // high
							{
								colorBlock[6*drawnCntBlock+0] = 0xffff0000;
								colorBlock[6*drawnCntBlock+1] = 0xffff0000;
								colorBlock[6*drawnCntBlock+2] = 0xffff0000;

								colorBlock[6*drawnCntBlock+3] = 0xffff0000;
								colorBlock[6*drawnCntBlock+4] = 0xffff0000;
								colorBlock[6*drawnCntBlock+5] = 0xffff0000;
							}
							else if ( nBlockType == 2 ) // mid
							{
								colorBlock[6*drawnCntBlock+0] = 0xff00ff00;
								colorBlock[6*drawnCntBlock+1] = 0xff00ff00;
								colorBlock[6*drawnCntBlock+2] = 0xff00ff00;

								colorBlock[6*drawnCntBlock+3] = 0xff00ff00;
								colorBlock[6*drawnCntBlock+4] = 0xff00ff00;
								colorBlock[6*drawnCntBlock+5] = 0xff00ff00;
							}
							else if ( nBlockType == 1 ) // low
							{
								colorBlock[6*drawnCntBlock+0] = 0xff0000ff;
								colorBlock[6*drawnCntBlock+1] = 0xff0000ff;
								colorBlock[6*drawnCntBlock+2] = 0xff0000ff;

								colorBlock[6*drawnCntBlock+3] = 0xff0000ff;
								colorBlock[6*drawnCntBlock+4] = 0xff0000ff;
								colorBlock[6*drawnCntBlock+5] = 0xff0000ff;
							}

							rvertexBlock[6*drawnCntBlock+0] = VerColorTex(vertexBlock[6*drawnCntBlock+0].x, vertexBlock[6*drawnCntBlock+0].y, vertexBlock[6*drawnCntBlock+0].z, 
								colorBlock[6*drawnCntBlock+0], 0.0f,1.0f);

							rvertexBlock[6*drawnCntBlock+1] = VerColorTex(vertexBlock[6*drawnCntBlock+1].x, vertexBlock[6*drawnCntBlock+1].y, vertexBlock[6*drawnCntBlock+1].z, 
								colorBlock[6*drawnCntBlock+1], 0.0f,0.0f);

							rvertexBlock[6*drawnCntBlock+2] = VerColorTex(vertexBlock[6*drawnCntBlock+2].x, vertexBlock[6*drawnCntBlock+2].y, vertexBlock[6*drawnCntBlock+2].z, 
								colorBlock[6*drawnCntBlock+2], 1.0f,1.0f);

							rvertexBlock[6*drawnCntBlock+3] = VerColorTex(vertexBlock[6*drawnCntBlock+3].x, vertexBlock[6*drawnCntBlock+3].y, vertexBlock[6*drawnCntBlock+3].z, 
								colorBlock[6*drawnCntBlock+3], 0.0f,0.0f);

							rvertexBlock[6*drawnCntBlock+4] = VerColorTex(vertexBlock[6*drawnCntBlock+4].x, vertexBlock[6*drawnCntBlock+4].y, vertexBlock[6*drawnCntBlock+4].z, 
								colorBlock[6*drawnCntBlock+4], 1.0f,0.0f);

							rvertexBlock[6*drawnCntBlock+5] = VerColorTex(vertexBlock[6*drawnCntBlock+5].x, vertexBlock[6*drawnCntBlock+5].y, vertexBlock[6*drawnCntBlock+5].z, 
								colorBlock[6*drawnCntBlock+5], 1.0f,1.0f);

							drawnCntBlock++;
						}
					}

					if( drawnCntBlock > 0 )
					{
						pRender->DrawLayer3DWireQuad(&m_pRS, rvertexBlock, drawnCntBlock);
						m_pRS->m_MipMapBias_S0 =  *(DWORD*)&(mipMapBias);
						m_pRS->m_MipMapBias_S1 =  *(DWORD*)&(mipMapBias);
						m_pRS->m_TexOP_S0 = D3DTOP_SELECTARG1;
						m_pRS->m_TexArg1_S0 = D3DTA_DIFFUSE;

						m_pRS->m_AlpOP_S0 = D3DTOP_SELECTARG1;
						m_pRS->m_AlpArg1_S0 = D3DTA_TEXTURE;

						m_pRS->m_AlphaTestEnable = true;
						m_pRS->m_Alpharef = 0x40;
						m_pRS->m_AlphaTestFun = D3DCMP_GREATER; 
						m_pRS->m_AlphaBlendEnable = false;
						m_pRS->m_LightEnable = false;
						m_pRS->m_ZTestEnable = false;
						m_pRS->m_ZWR = true;
						m_pRS->m_Cull = D3DCULL_CCW;
					}

					CMapEditApp::GetInstance()->GetOperator()->AddToPrimitiveCnt(drawnCntBlock*2);

					//// turn off depth bias
					pRender->GetICoreGraphic()->SetZDelta( 0.0f );
				}

			}
		}
	}

	void CTextureNode::AddGrid( int dw )
	{
		m_vGridIndices.push_back(dw);
		m_dwGridCnt++;
	}

	CTextureNode::CTextureNode() : m_dwGridCnt(0), m_dwTextureLayer(INVALID), m_pLayerOneTexture(NULL), m_pLayerSecondTexture(NULL), 
		m_pLayerThreeTexture(NULL), m_pLayerFourTexture(NULL), m_pRS(NULL)
	{
		m_strTextureNames[0] = "";
		m_strTextureNames[1] = "";
		m_strTextureNames[2] = "";
		m_strTextureNames[3] = "";
	}

	CTextureNode::~CTextureNode()
	{
		m_pLayerOneTexture = NULL;
		m_pLayerSecondTexture = NULL;
		m_pLayerThreeTexture = NULL;
		m_pLayerFourTexture = NULL;
		m_pRS = NULL;
	}

	int CTextureNode::GetGridIndex( int dw )
	{
		return m_vGridIndices[dw];
	}

	int CTextureNode::GetGridCnt()
	{
		return m_dwGridCnt;
	}

	void CTextureNode::SetTextureLayer( DWORD dw )
	{
		m_dwTextureLayer = dw;
	}

	DWORD CTextureNode::GetTextureLayer()
	{
		return m_dwTextureLayer;
	}

	std::string CTextureNode::GetTextureName( int layer )
	{
		return m_strTextureNames[layer];
	}

	void CTextureNode::SetTextureName( std::string name, int layer )
	{
		m_strTextureNames[layer] = name;
	}

	void CTextureNodeManager::Insert(const bly::STextureNodeInfo &nodeInfo)
	{
		std::string str1 = nodeInfo.strTextureNames[0];
		std::string str2 = nodeInfo.strTextureNames[1];
		std::string str3 = nodeInfo.strTextureNames[2];
		std::string str4 = nodeInfo.strTextureNames[3];

		std::map<std::string, int>::iterator iter;

		//layer 1
		if( str1 != "" && str2 == "" && str3 == "" && str4 == "" )
		{
			iter = m_LayerOneIndex.find(str1);

			if( iter == m_LayerOneIndex.end() )
			{
				CTextureNode * p = NULL;
				p = new CTextureNode;
				if( p == NULL )
					return;

				p->AddGrid(nodeInfo.dwGridIndex);
				p->SetTextureName(str1, 0);
				p->SetTextureLayer(0);

				m_TextureNodeOneList.push_back(p);
				m_LayerOneIndex.insert(make_pair(str1, layerone++));

				CMapEditApp::GetInstance()->GetTerrainMesh()->m_TerrainTextureLists.insert(str1);
			}
			else
			{
				CTextureNode * p = NULL;
				p = m_TextureNodeOneList[iter->second];
				if( p == NULL )
					return;

				string layer1 = p->GetTextureName(0);

				if( layer1 != str1 )
				{
					CTextureNode * node = new CTextureNode;
					node->AddGrid(nodeInfo.dwGridIndex);

					node->SetTextureName(str1, 0);
					node->SetTextureLayer(0);
					m_TextureNodeOneList.push_back(node);
				}
				else
				{
					m_TextureNodeOneList[iter->second]->AddGrid(nodeInfo.dwGridIndex);
				}
			}
		}
		//layer 2
		if( str2 != "" && str3 == "" && str4 == "" )
		{
			iter = m_LayerTwoIndex.find(str2);

			if( iter == m_LayerTwoIndex.end() )
			{
				CTextureNode * p = NULL;
				p = new CTextureNode;
				if( p == NULL )
					return;

				p->AddGrid(nodeInfo.dwGridIndex);
				p->SetTextureName(str1, 0);
				p->SetTextureName(str2, 1);
				p->SetTextureName("", 2);
				p->SetTextureName("", 3);
				p->SetTextureLayer(1);

				m_TextureNodeTwoList.push_back(p);
				m_LayerTwoIndex.insert(make_pair(str2, layertwo++));
				CMapEditApp::GetInstance()->GetTerrainMesh()->m_TerrainTextureLists.insert(str2);
			}
			else 
			{
				CTextureNode * p = NULL;
				p = m_TextureNodeTwoList[iter->second];
				if( p == NULL )
					return;

				string layer1 = p->GetTextureName(0);
				string layer2 = p->GetTextureName(1);

				if( layer1 != str1 || layer2 != str2 ) 
				{
					CTextureNode * node = NULL;
					node = new CTextureNode;
					if( node == NULL )
						return;

					node->AddGrid(nodeInfo.dwGridIndex);
					node->SetTextureName(str1, 0);
					node->SetTextureName(str2, 1);
					p->SetTextureName("", 2);
					p->SetTextureName("", 3);
					node->SetTextureLayer(1);
					m_TextureNodeTwoList.push_back(node);
				}
				else
				{
					m_TextureNodeTwoList[iter->second]->AddGrid(nodeInfo.dwGridIndex);
				}
			}
		}
		else
		{
			//layer 3
			if( str3 != "" && str4 == "" )
			{
				iter = m_LayerThreeIndex.find(str3);

				if( iter == m_LayerThreeIndex.end() )
				{
					CTextureNode * p = NULL;
					p = new CTextureNode;
					if( p == NULL )
						return;

					p->AddGrid(nodeInfo.dwGridIndex);
					p->SetTextureName(str1, 0);
					p->SetTextureName(str2, 1);
					p->SetTextureName(str3, 2);
					p->SetTextureName("", 3);
					p->SetTextureLayer(2);

					m_TextureNodeThreeList.push_back(p);
					m_LayerThreeIndex.insert(make_pair(str3, layerthree++));

					CMapEditApp::GetInstance()->GetTerrainMesh()->m_TerrainTextureLists.insert(str3);
				}
				else
				{
					CTextureNode * p = NULL;
					p = m_TextureNodeThreeList[iter->second];
					if( p == NULL )
						return;

					string layer1 = p->GetTextureName(0);
					string layer2 = p->GetTextureName(1);
					string layer3 = p->GetTextureName(2);

					if( layer1 != str1 || layer2 != str2 || layer3 != str3 ) 
					{
						CTextureNode * node = NULL;
						node = new CTextureNode;
						if( node == NULL )
							return;

						node->AddGrid(nodeInfo.dwGridIndex);
						node->SetTextureName(str1, 0);
						node->SetTextureName(str2, 1);
						node->SetTextureName(str3, 2);
						p->SetTextureName("", 3);

						node->SetTextureLayer(2);
						m_TextureNodeThreeList.push_back(node);

					}
					else
					{
						m_TextureNodeThreeList[iter->second]->AddGrid(nodeInfo.dwGridIndex);
					}
				}
			}
			else
			{
				//layer 4
				if( str4 != "" )
				{
					iter = m_LayerFourIndex.find(str4);

					if( iter == m_LayerFourIndex.end() )
					{
						CTextureNode * p = NULL;
						p = new CTextureNode;
						if( p == NULL )
							return;

						p->AddGrid(nodeInfo.dwGridIndex);
						p->SetTextureName(nodeInfo.strTextureNames[0], 0);
						p->SetTextureName(nodeInfo.strTextureNames[1], 1);
						p->SetTextureName(nodeInfo.strTextureNames[2], 2);
						p->SetTextureName(nodeInfo.strTextureNames[3], 3);
						p->SetTextureLayer(3);

						m_TextureNodeFourList.push_back(p);
						m_LayerFourIndex.insert(make_pair(str4, layerfour++));

						CMapEditApp::GetInstance()->GetTerrainMesh()->m_TerrainTextureLists.insert(str4);
					}
					else
					{
						CTextureNode * p = NULL;
						p = m_TextureNodeFourList[iter->second];
						if( p == NULL )
							return;

						string layer1 = p->GetTextureName(0);
						string layer2 = p->GetTextureName(1);
						string layer3 = p->GetTextureName(2);
						string layer4 = p->GetTextureName(3);

						if( layer1 != str1 || layer2 != str2 || layer3 != str3 || layer4 != str4 ) 
						{
							CTextureNode * node = NULL;
							node = new CTextureNode;
							if( node == NULL )
								return;

							node->AddGrid(nodeInfo.dwGridIndex);
							node->SetTextureName(str1, 0);
							node->SetTextureName(str2, 1);
							node->SetTextureName(str3, 2);
							node->SetTextureName(str4, 3);

							node->SetTextureLayer(3);
							m_TextureNodeFourList.push_back(node);
						}
						else
						{
							m_TextureNodeFourList[iter->second]->AddGrid(nodeInfo.dwGridIndex);
						}
					}
				}
			}
		}
	}

	void CTextureNodeManager::Render(SGrid * pGrids, SVertex * pVertices, bool bWater, float fTimeRatio)
	{
		std::vector<CTextureNode*>::iterator iter; 

		//第一层
		iter =  m_TextureNodeOneList.begin();
		for( iter; iter !=  m_TextureNodeOneList.end(); ++iter )
		{
			(*iter)->Render(pGrids, pVertices, bWater, fTimeRatio);
		}

		//第二层
		iter =  m_TextureNodeTwoList.begin();
		for( iter; iter !=  m_TextureNodeTwoList.end(); ++iter )
		{
			(*iter)->Render(pGrids, pVertices, bWater, fTimeRatio);
		}

		//第三层
		iter =  m_TextureNodeThreeList.begin();
		for( iter; iter !=  m_TextureNodeThreeList.end(); ++iter )
		{
			(*iter)->Render(pGrids, pVertices, bWater, fTimeRatio);
		}

		//第四层
		iter =  m_TextureNodeFourList.begin();
		for( iter; iter !=  m_TextureNodeFourList.end(); ++iter )
			(*iter)->Render(pGrids, pVertices, bWater, fTimeRatio);
	}

	void CTextureNodeManager::Clear()
	{
		std::vector<CTextureNode*>::iterator iter;
		CTextureNode *pNode = NULL;

		if ( !m_TextureNodeOneList.empty() )
		{
			iter = m_TextureNodeOneList.begin();
			for(iter; iter != m_TextureNodeOneList.end(); ++iter)
			{
				pNode = *iter;

				if( pNode )
				{
					pNode->m_pLayerOneTexture = NULL;
					SafeDelete(pNode);
				}
			}

			m_TextureNodeOneList.clear();
			m_LayerOneIndex.clear();
		}
		if ( !m_TextureNodeTwoList.empty() )
		{
			iter = m_TextureNodeTwoList.begin();
			for(iter; iter != m_TextureNodeTwoList.end(); ++iter)
			{
				pNode = *iter;
				if( pNode )
				{
					pNode->m_pLayerOneTexture = NULL;
					pNode->m_pLayerSecondTexture = NULL;
					SafeDelete(pNode);
				}
			}

			m_TextureNodeTwoList.clear();
			m_LayerTwoIndex.clear();
		}
		if ( !m_TextureNodeThreeList.empty() )
		{
			iter = m_TextureNodeThreeList.begin();
			for(iter; iter != m_TextureNodeThreeList.end(); ++iter)
			{
				pNode = *iter;
				if( pNode )
				{
					pNode->m_pLayerOneTexture = NULL;
					pNode->m_pLayerSecondTexture = NULL;
					pNode->m_pLayerThreeTexture = NULL;
					SafeDelete(pNode);
				}
			}

			m_TextureNodeThreeList.clear();
			m_LayerThreeIndex.clear();
		}
		if ( !m_TextureNodeFourList.empty() )
		{
			iter = m_TextureNodeFourList.begin();
			for(iter; iter != m_TextureNodeFourList.end(); ++iter)
			{
				pNode = *iter;
				if( pNode )
				{
					pNode->m_pLayerOneTexture = NULL;
					pNode->m_pLayerSecondTexture = NULL;
					pNode->m_pLayerThreeTexture = NULL;
					pNode->m_pLayerFourTexture = NULL;
					SafeDelete(pNode);
				}
			}

			m_TextureNodeFourList.clear();
			m_LayerFourIndex.clear();
		}

		layerone = layertwo = layerthree = layerfour = 0;
	}
}
#include "stdafx.h"
#include "ObjectVertexBaker.h"
#include "VertexShadowJudge.h"
#include "MeshColorSmoother.h"
#include "ShadowMapBakerPool.h"
#include "TerrainMesh.h"
#include "CMatrix.h"
#include "OfflineRender.h"
#include "CEditModel.h"
#include "CRenderPiece.h"
#include "StaticIlluminator.h"
#include "CMapEditApp.h"
#include "ModelGroup.h"
#include "Algorithm.h"
#include "CMapEditObject.h"
#include "CColor4.inl"

namespace sqr
{
	const float MIN_EDGE_LEN = 64 / 4.0f;	// 递归结束条件: 1/n米
	const float MIN_EDGE_LEN_SQ = MIN_EDGE_LEN * MIN_EDGE_LEN;

	CObjectVertexBaker::CObjectVertexBaker()
		: m_shadowJudge(new CVertexShadowJudge())
		, m_meshColorSmoother(new CMeshColorSmoother())
		, m_shadowColor(CColor4::Black)
	{
	}

	CObjectVertexBaker::~CObjectVertexBaker()
	{
	}

	void CObjectVertexBaker::Setup()
	{
		CLightMapBaker::Setup();
		
	}

	void CObjectVertexBaker::Discard()
	{
		CLightMapBaker::Discard();
	}

	void CObjectVertexBaker::TilePrepare( index_t x,index_t z )
	{
		CLightMapBaker::TilePrepare(x,z);
		OfflineRender::GetInst()->AddLightsToIlluminator(false);
	}

	void CObjectVertexBaker::OnProcess( index_t x,index_t z )
	{
		uint32 nTileIndex = x + z*tileWidth;

		ShadowMapBuf shadowMapData;
		CShadowMapBakingPool::GetInst()->GetShadowBuf(nTileIndex, shadowMapData);

		// begin to judge
		CRenderTexture* depthTarget = shadowMapData.ShadowTexture;
		const CMatrix& shadowMatrix = shadowMapData.MatrixBuild->GetShadowMatrix();
		m_shadowJudge->Begin(depthTarget, shadowMatrix);

		// for each region
		CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
		size_t numRegionsPerTileRow = terrainMesh->GetWidthInRegion() * TILE_WIDTH_IN_REGION;
		index_t baseRegionIndex = z * numRegionsPerTileRow + x * TILE_WIDTH_IN_REGION;
		for (index_t j = 0; j < TILE_WIDTH_IN_REGION; ++j)
		{
			for (index_t i = 0; i < TILE_WIDTH_IN_REGION; ++i)
			{
				int regionIndex = baseRegionIndex + i;
				SRegion& region = terrainMesh->GetRegion(regionIndex);
				
				// for each grid
				for (vector<int>::iterator it = region.vGridIndices.begin();
					it != region.vGridIndices.end(); ++it)
				{
					int gridIndex = *it;
					SGrid& grid = terrainMesh->GetGrid(gridIndex);
					ProcessGrid(grid);
				}
			}
			
			// next row
			baseRegionIndex += terrainMesh->GetWidthInRegion();	
		}

		// end judge
		m_shadowJudge->End();

		CLightMapBaker::OnProcess(x, z);
	}

	void CObjectVertexBaker::RenderReceive()
	{
	}

	void CObjectVertexBaker::ProcessGrid( SGrid& grid )
	{
		OfflineRender* offlineRender = OfflineRender::GetInst();
		CColor4 ambient = offlineRender->m_GridColor[grid.dwGridIndex].Ambient;
		CColor4 diffuse = offlineRender->m_GridColor[grid.dwGridIndex].Diffuse;
		CStaticIlluminator* illuminator = CStaticIlluminator::GetInst();
		illuminator->SetAmbient(ambient);

		// global light
		CRenderGroup* renderGroup = CMainWindowTarget::GetInst()->GetGroup();
		CLightSystem* lightSystem = renderGroup->GetLightSystem();
		CLight light = *lightSystem->GetGlobalLight();
		light.SetColor(diffuse);
		illuminator->SetGlobalLight(light);
		m_shadowColor = offlineRender->m_GridColor[grid.dwGridIndex].Shadow;

		// clear old colors
		grid.ClearModelColorMap();

		DWORD			dwGridIdx		= grid.dwGridIndex;
		CTerrainMesh*	pTerrainMesh	= CMapEditApp::GetInst()->GetEditingMesh();
		int				x				= (dwGridIdx % pTerrainMesh->GetWidth()) / REGION_SPACE / 4;
		int				z				= (dwGridIdx / pTerrainMesh->GetWidth()) / REGION_SPACE / 4;
		TileIndex		ti(x, z);

		TileModelDiffuseMap&		tmdm			= CVertexDiffuseColor::GetInst()->GetAllObjectColor();
		GridModelDiffuseMap&		gridModColor	= tmdm[ti];
		Idx2ModelGroupDiffuseMap&	i2mgdm			= gridModColor[grid.dwGridIndex].i2mgdm;

		// for each model
		for (ModelGroupIndex modelGroupIndex = 0; modelGroupIndex < grid.vecCenterObjects.size(); ++modelGroupIndex)
		{
			CEditModelGroup* modelGroup = grid.vecCenterObjects[modelGroupIndex]->m_pModelGroup;
			if ( modelGroup->m_SL.m_bIAmbScenePointLight || modelGroup->m_SpotLight.m_bIAmbSceneSpotLight || modelGroup->m_pSceneAudio )
				continue;

			ModelGroupColorMap* modelColorMap = NULL;
			modelColorMap = grid.GetModelColorMap(modelGroupIndex);
			if (NULL == modelColorMap)
				continue;

			ModelGroupDiffuseMap& mgdm = i2mgdm[modelGroupIndex];

			for (vector<CMapEditObject*>::iterator itm = modelGroup->m_vecSelfPresentation.begin();
				itm!= modelGroup->m_vecSelfPresentation.end(); ++itm)
			{
				CMapEditObject* meModel = *itm;
				CEditModel& model = *meModel;
				bool bBakingColorMark = meModel->GetBakingColorIsNotGreaterShadow();

				PieceClassDiffuseVector& pcdv = mgdm[meModel->GetName()];

				CColor4 matdiffuse;
				matdiffuse.a = 1.0f;

				if ( meModel->sModelCommonPro.bHideModelVertexColor )
				{
					matdiffuse.r = MATAMBIENT;
					matdiffuse.g = MATAMBIENT;
					matdiffuse.b = MATAMBIENT;
				}
				else
				{
					matdiffuse.r = meModel->sModelCommonPro.fRedOverallAmbientRatio;
					matdiffuse.g = meModel->sModelCommonPro.fGreenOverallAmbientRatio;
					matdiffuse.b = meModel->sModelCommonPro.fBlueOverallAmbientRatio;
				}

				ProcessModel(model, (*modelColorMap)[meModel->GetName().c_str()], bBakingColorMark, matdiffuse, pcdv);
			}
		}
	}

	void CObjectVertexBaker::ProcessModel( CEditModel& model, vector<PieceClassColor>& pieceColors, 
		bool bBakingColorMark, const CColor4& matdiffuse, const PieceClassDiffuseVector& pcdv )
	{
		// update world matrix
		CMatrix world;
		model.GetMatrix(world, 0, -1);

		// material
		//sMaterial& material = *model.GetMaterial();
		sMaterial material =
		{
			matdiffuse,
			matdiffuse,
			CColor4((uint32)0),
			CColor4((uint32)0xffffffff),
			0,
		};

		// for each piece
		pieceColors.resize(model.GetRenderPieces().size());
		index_t renderPieceIndex = 0;
		for (CEditModel::RenderPieceMap::iterator it = model.GetRenderPieces().begin();
			it != model.GetRenderPieces().end(); ++it, ++renderPieceIndex)
		{
			PieceClassColor& classColor = pieceColors[renderPieceIndex];
			classColor.className = it->first.c_str();
			CRenderPiece& renderPiece = it->second;

			// for each mesh
			int pieceIndex = 0;
			for (GVector<CRenderPiece::PieceData>::iterator itp = renderPiece.m_Pieces.begin();
				itp != renderPiece.m_Pieces.end(); ++itp, ++pieceIndex)
			{
				CMesh& mesh = *itp->m_pPiece->GetMesh();

				const PieceDiffuse* pd = 0;
				if ( renderPieceIndex < pcdv.size() )
				{
					if ( (uint32)pieceIndex < pcdv[renderPieceIndex].pieceColorVec.size() )
					{
						pd = &pcdv[renderPieceIndex].pieceColorVec[pieceIndex];
					}
				}

				// for each triangle
				ProcessMesh(mesh, material, world, bBakingColorMark, pd);
				// average adjacent vertices
				SmoothColorBuffer(mesh);
				// save to grid
				CVertexHardwareBuffer* pBuffer;
				pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(
					sizeof(DWORD),
					m_colorBuffer.size(),
					HBU_WRITE_ONLY , &m_colorBuffer[0]);
				VBRef* vbRef = new VBRef();
				vbRef->SetVB(pBuffer);
				classColor.pieceColorMap[pieceIndex] = vbRef;
			}
		}
	}

	void CObjectVertexBaker::ProcessMesh( CMesh& mesh, sMaterial& material, 
		const CMatrix& world, bool bBakingColorMark, const PieceDiffuse* pd )
	{
		IndexData* indexData = mesh.GetIndexBuf(0);
		vector<uint16> indices(indexData->indexBuffer->getNumIndexes());
		indexData->indexBuffer->read(0, indices.size() * sizeof(uint16), &indices.front());
		float shadowRatio[3];
		CVector3f worldPos[3];
		CStaticIlluminator* staticIlluminator = CStaticIlluminator::GetInst();

		for (int i = 0; i < mesh.GetFaceNum(0); ++i)
		{
			int offset = i * 3;
			for (int j = 0; j < 3; ++j)
			{
				index_t index = indices[offset + j];
				CVector3f position = mesh.GetVertex(index);
				worldPos[j] = position * world;
			}
			ComputeTriangleShadow(shadowRatio, worldPos[0], worldPos[1], worldPos[2] );
			//bool isAllInShadow = gIsZero(shadowRatio[0]) &&	gIsZero(shadowRatio[1]) && gIsZero(shadowRatio[2]);
			
			// for each vertex
			for (int j = 0; j < 3; ++j)
			{
				index_t index = indices[offset + j];
				CVector3f normal = mesh.GetNormal(index);
				normal = normal.MultiplyNormal(world);
				//if (isAllInShadow)
				//	normal.y = -1.0f;
				normal.normalize();

				// lighting 光照计算
				CColor4 matdiffuse;
				if ( (pd != 0) && (index < (*pd).size()) )
				{
					matdiffuse = ((*pd)[index]);
				}
				else
				{
					matdiffuse = CColor4::Black;
				}
				
				sMaterial new_material =
				{
					matdiffuse,
					matdiffuse,
					CColor4((uint32)0),
					CColor4((uint32)0xffffffff),
					0,
				};

				//CColor4 lighted = staticIlluminator->Illuminate(new_material,
				//	worldPos[j], normal);
				CColor4 lighted = staticIlluminator->Illuminate(material,
					worldPos[j], normal);

				// shadow 当前格子阴影插值lerp(x,y,s) = x + s(y-x);
				CColor4 shadow = lerp(m_shadowColor, CColor4::White,
					CColor4(shadowRatio[j], shadowRatio[j], shadowRatio[j], 0.0f));
				lighted *= shadow;

				//模型烘培颜色不比阴影色更暗,周自强希望烘焙后的草跟地表的lightmap颜色一致, 就是color = max(bakedColor, globalShadowColor);
				if( bBakingColorMark )
				{
					uint uLighted = lighted.MakeDWord();
					uint uShadow  = m_shadowColor.MakeDWord();
					uint uMax	  = max(uLighted, uShadow);
					lighted = CColor4(uMax);
				}

				// add to buffer
				m_meshColorSmoother->AddVertex(mesh.GetVertex(index),
					mesh.GetNormal(index), lighted);
			}
		}
	}

	void CObjectVertexBaker::ComputeTriangleShadow( float* outRatios, 
		const CVector3f& p1, const CVector3f& p2, const CVector3f& p3 )
	{
		// mid-points
		CVector3f p11, p22, p33;
		p11 = (p2 + p3)*0.5f;
		p22 = (p1 + p3)*0.5f;
		p33 = (p1 + p2)*0.5f;

		float area1 = ComputeShadowArea(p1, p33, p22);
		float area2 = ComputeShadowArea(p2, p11, p33);
		float area3 = ComputeShadowArea(p3, p22, p11);
		float area4 = ComputeShadowArea(p11, p22, p33);

		outRatios[0] = (area1 * 3.0f + area4) * 0.25f;
		outRatios[1] = (area2 * 3.0f + area4) * 0.25f;
		outRatios[2] = (area3 * 3.0f + area4) * 0.25f;
	}	

	float CObjectVertexBaker::ComputeShadowArea( const CVector3f& p1, const CVector3f& p2, const CVector3f& p3 )
	{
		if (MIN_EDGE_LEN_SQ < (p1-p2).MagSqr() ||
			MIN_EDGE_LEN_SQ < (p2-p3).MagSqr() ||
			MIN_EDGE_LEN_SQ < (p3-p1).MagSqr() )
		{
			// mid-points
			CVector3f p11, p22, p33;
			p11 = (p2 + p3)*0.5f;
			p22 = (p1 + p3)*0.5f;
			p33 = (p1 + p2)*0.5f;

			float result = 0.0f;
			result += ComputeShadowArea(p1, p33, p22);
			result += ComputeShadowArea(p2, p11, p33);
			result += ComputeShadowArea(p3, p22, p11);
			result += ComputeShadowArea(p11, p22, p33);

			result *= 0.25f;
			return result;
		}
		else // if all the edges are too short, end recursion
		{
			float result = 0.0f;
			bool isInShadow = false;

			isInShadow = m_shadowJudge->IsInShadow(p1);
			result += isInShadow ? 0.0f : 1.0f;
			isInShadow = m_shadowJudge->IsInShadow(p2);
			result += isInShadow ? 0.0f : 1.0f;
			isInShadow = m_shadowJudge->IsInShadow(p3);
			result += isInShadow ? 0.0f : 1.0f;

			result /= 3.0f;
			return result;
		}
	}

	void CObjectVertexBaker::SmoothColorBuffer( CMesh& mesh )
	{
		int numVertices = mesh.GetVerNum();
		m_colorBuffer.resize(numVertices);
		for (int i = 0; i < numVertices; ++i)
		{
			m_colorBuffer[i] = m_meshColorSmoother->GetSmoothedColor(
				mesh.GetVertex(i), mesh.GetNormal(i));
		}
		m_meshColorSmoother->Clear();
	}
}

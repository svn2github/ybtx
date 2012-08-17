//------------------------------------------------------------------------------
//  LogicalHeightMeshBuilder.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LogicalHeightMeshBuilder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CEditTerrainMgr.h"
#include "CTerrain.h"
#include "CVertexDeclaration.h"
#include "CHardwareBufferMgr.h"
#include "CGraphic.h"
#include "CDataSources.h"
#include "CRenderOperation.h"
#include "GraphicBase.h"

namespace sqr
{
void
CLogicalHeightMeshBuilder::Build( vector<LogicalMeshPtr>& outMeshes )
{
	Ast(outMeshes.empty());

	CTerrainMesh* terrainMesh = CMapEditApp::GetInst()->GetTerrain();
	CEditTerrainMgr* terrain = terrainMesh->m_pTerrain;
	CVertexDeclaration* vtxDecl = CVertexDeclaration::GET_VERDECL(VerColor3D::Format);

	uint numTiles		= terrainMesh->GetRegionCount() / 16;
	uint numRenderTiles = numTiles * CTerrainMesh::Tile_PerSqr;
	outMeshes.resize(numRenderTiles);
	uint widthInLightMap = terrainMesh->GetWidthInRegion() / TILE_WIDTH_IN_REGION;
	int nWidthInTileRegion = terrainMesh->GetWidthInTileRegion();

	vector<LogicalMeshPtr>::iterator it = outMeshes.begin();
	index_t tileIndex = 0;
	for (tileIndex; tileIndex < numTiles; ++it, ++tileIndex)
	{
		LogicalMeshPtr& mesh = *it;
		mesh = new LogicalMesh();
		
		unsigned int i = tileIndex % nWidthInTileRegion;
		unsigned int z = tileIndex / nWidthInTileRegion;

		const TerrainNode* pTerrainNode = (TerrainNode*)terrain->GetNode(URegionID(i,z));
		
		for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
		{
			STileRenderRegionUnit *pRenderRegionUnit = new STileRenderRegionUnit;

			// AABB
			mesh->aabb = pTerrainNode->m_TerrainTiles[m].aab;

			// render operation
			mesh->rp.Create();
			// index buffer
			mesh->rp.indexData->indexStart			= 0;
			mesh->rp.indexData->indexCount			= pTerrainNode->m_TerrainTiles[m].op.indexData->indexCount;
			mesh->rp.indexData->indexBuffer			= pTerrainNode->m_TerrainTiles[m].op.indexData->indexBuffer;
			mesh->rp.useIndexes = true;
			// vertex buffer
			mesh->rp.vertexData->vertexStart		= 0;
			mesh->rp.vertexData->vertexCount		= pTerrainNode->m_TerrainTiles[m].op.vertexData->vertexCount;
			mesh->rp.vertexData->vertexDeclaration	= vtxDecl;
			CVertexHardwareBuffer* vertexBuffer		= CHardwareBufferMgr::GetInst()->createVertexBuffer(
				sizeof(VerColor3D), mesh->rp.vertexData->vertexCount, HBU_WRITE_ONLY);
			
			mesh->rp.vertexData->vertexBufferBinding->SetBinding(0, vertexBuffer);
			// write vertex buffer
			VerColor3D* buffer = (VerColor3D*)vertexBuffer->lock(HBL_DISCARD);
			index_t bufferIndex = 0;
			index_t index = (tileIndex % widthInLightMap) * TILE_WIDTH_IN_REGION
				+ uint(tileIndex / widthInLightMap) * (TILE_WIDTH_IN_REGION * terrainMesh->GetWidthInRegion());
			for (index_t z = 0; z < TILE_WIDTH_IN_REGION; ++z)
			{
				for (index_t x = 0; x < TILE_WIDTH_IN_REGION; ++x)
				{
					const SRegion& region = terrainMesh->GetRegion(index + x);
					for (vector<int>::const_iterator it = region.vGridIndices.begin();
						it != region.vGridIndices.end(); ++it)
					{
						const SGrid& grid = terrainMesh->GetGrid(*it);
						buffer[bufferIndex].p = terrainMesh->GetVertex(grid.dwVertexIndex[0]).vPosition;
						buffer[bufferIndex].diffuse = terrainMesh->GetVertex(grid.dwVertexIndex[0]).bColor;
						buffer[bufferIndex++].p.y = buffer[bufferIndex].p.y + terrainMesh->GetVertex(grid.dwVertexIndex[0]).fLogicHeight;
						buffer[bufferIndex].p = terrainMesh->GetVertex(grid.dwVertexIndex[3]).vPosition;
						buffer[bufferIndex].diffuse = terrainMesh->GetVertex(grid.dwVertexIndex[3]).bColor;
						buffer[bufferIndex++].p.y = buffer[bufferIndex].p.y + terrainMesh->GetVertex(grid.dwVertexIndex[3]).fLogicHeight;
						buffer[bufferIndex].p = terrainMesh->GetVertex(grid.dwVertexIndex[1]).vPosition;
						buffer[bufferIndex].diffuse = terrainMesh->GetVertex(grid.dwVertexIndex[1]).bColor;
						buffer[bufferIndex++].p.y = buffer[bufferIndex].p.y + terrainMesh->GetVertex(grid.dwVertexIndex[1]).fLogicHeight;
						buffer[bufferIndex].p = terrainMesh->GetVertex(grid.dwVertexIndex[2]).vPosition;
						buffer[bufferIndex].diffuse = terrainMesh->GetVertex(grid.dwVertexIndex[2]).bColor;
						buffer[bufferIndex++].p.y = buffer[bufferIndex].p.y + terrainMesh->GetVertex(grid.dwVertexIndex[2]).fLogicHeight;
					}
				}
				// next row
				index += terrainMesh->GetWidthInRegion();
			}
			vertexBuffer->unlock();
		}
	}
}

//------------------------------------------------------------------------------
LogicalMesh::~LogicalMesh()
{
	if (NULL != this->rp.vertexData)
	{
		CHardwareBufferMgr::GetInst()->DestroyBuffer(this->rp.vertexData->vertexBufferBinding->GetBuffer(0));
		rp.Destroy();
	}
}
}
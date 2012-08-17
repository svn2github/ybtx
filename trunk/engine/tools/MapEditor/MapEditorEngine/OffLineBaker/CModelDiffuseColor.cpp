#include "stdafx.h"
#include "CModelDiffuseColor.h"
#include "CMapEditObject.h"
#include "CEditModel.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "BaseHelper.h"
#include "ExsertStructDB.h"
#include "CEditTerrainMgr.h"

namespace sqr
{
	typedef CEditModel::RenderPieceMap			RenderPieceMap;
	typedef RenderPieceMap::iterator			RenderPieceIter;
	typedef GVector<CRenderPiece::PieceData>	PieceDataVector;
	typedef PieceDataVector::iterator			PieceDataIter;


	void CalulateMeshDiffuse( PieceDiffuse& pd, CMesh& mesh, const CMatrix& world );


	CVertexDiffuseColor::CVertexDiffuseColor()
		: m_pModelProcesser(new CDiffuseModelProcesser())
	{
	}

	CVertexDiffuseColor::~CVertexDiffuseColor()
	{
		SafeDelete(m_pModelProcesser);
	}

	TileModelDiffuseMap& 
		CVertexDiffuseColor::GetAllObjectColor()
	{
		return m_TileModelDiffuseMap;
	}

	void CVertexDiffuseColor::ClearAll()
	{
		m_TileModelDiffuseMap.clear();
	}

	void CVertexDiffuseColor::ClearTile( uint32 x, uint32 z )
	{
		TileIndex ti(x, z);
		m_TileModelDiffuseMap.erase(ti);
	}

	void CVertexDiffuseColor::CreateModelColorVB( PieceClassDiffuseVector& pcdv )
	{
		PieceClassDiffuseIter it  = pcdv.begin();
		PieceClassDiffuseIter eit = pcdv.end();

		for ( ; it != eit; ++ it )
		{
			PieceClassDiffuse& pcd = *it;
			pcd.pieceColorVBVec.clear();

			for ( uint32 idxPiece = 0; idxPiece < pcd.pieceColorVec.size(); ++idxPiece )
			{
				PieceDiffuse& pd = pcd.pieceColorVec[idxPiece];
				if ( !pd.empty() )
				{
					CVertexHardwareBuffer* pBuf = CHardwareBufferMgr::GetInst()->createVertexBuffer(
						sizeof(uint32),
						pd.size(),
						HBU_WRITE_ONLY,
						&pd[0]
					);

					VBRef* vb = new VBRef();
					vb->SetVB(pBuf);

					pcd.pieceColorVBVec.push_back(vb);
				}
			}
		}
	}

	PieceClassDiffuseVector& 
		CVertexDiffuseColor::GetModelDiffuse( uint32 nGridIdx, uint32 nModelGroup, const char* name )
	{
		CTerrainMesh*	pTerrainMesh		= CMapEditApp::GetInst()->GetEditingMesh();
		uint32			xTile				= (nGridIdx % pTerrainMesh->GetWidth()) / REGION_SPACE / 4;
		uint32			zTile				= (nGridIdx / pTerrainMesh->GetWidth()) / REGION_SPACE / 4;
		TileIndex		ti(xTile, zTile);

		TileModelDiffuseMap&		tmdm			= CVertexDiffuseColor::GetInst()->GetAllObjectColor();
		GridModelDiffuseMap&		gridModColor	= tmdm[ti];
		Idx2ModelGroupDiffuseMap&	i2mgdm			= gridModColor[nGridIdx].i2mgdm;
		ModelGroupDiffuseMap&		mgdm			= i2mgdm[nModelGroup];
		PieceClassDiffuseVector&	pcdv			= mgdm[name];

		return pcdv;
	}

	void CVertexDiffuseColor::BakeModelDiffuse( uint32 iModelGroup, CMapEditObject& meObject )
	{
		return;

		string strModelName = meObject.GetName();
		if( strModelName.find("tile\\µØ±à×¨ÓÃ\\model\\effectdummy.mod") != -1 )
			return;

		PieceClassDiffuseVector& pcdv = CVertexDiffuseColor::GetInst()
			->GetModelDiffuse(meObject.GetCenterGridIndex(), iModelGroup, strModelName.c_str());

		index_t		idxRenderPiece	= 0;
		CEditModel& model			= meObject;

		// calculate model diffuse color
		CMatrix	world;
		model.GetMatrix(world, 0, -1);
		pcdv.resize(model.GetRenderPieces().size());
		for ( RenderPieceIter it = model.GetRenderPieces().begin();
			it != model.GetRenderPieces().end(); ++it, ++idxRenderPiece )
		{
			PieceClassDiffuse&	pcd	= pcdv[idxRenderPiece];
			pcd.className = it->first.c_str();
			CRenderPiece& renderPiece = it->second;

			// for each mesh
			for ( PieceDataIter itp = renderPiece.m_Pieces.begin();
				itp != renderPiece.m_Pieces.end(); ++itp )
			{
				pcd.pieceColorVec.push_back( PieceDiffuse() );

				CMesh& mesh = *itp->m_pPiece->GetMesh();
				CalulateMeshDiffuse(pcd.pieceColorVec.back(), mesh, world);
			}
		}

		// create model diffuse color
		CreateModelColorVB(pcdv);
		
		// write model diffuse color
		ApplyModelDiffuse( iModelGroup, meObject );
	}

	void CVertexDiffuseColor::ApplyModelDiffuse( uint32 iModelGroup, CMapEditObject& meObject )
	{
		PieceClassDiffuseVector& pcdv = CVertexDiffuseColor::GetInst()
			->GetModelDiffuse(meObject.GetCenterGridIndex(), iModelGroup, meObject.GetName().c_str());
		index_t		idxRenderPiece	= 0;
		CEditModel& model			= meObject;

		if ( pcdv.size() ==0 )
			return;

		for ( RenderPieceIter it = model.GetRenderPieces().begin();
			it != model.GetRenderPieces().end(); ++it, ++idxRenderPiece )
		{
			PieceClassDiffuse&	pcd	= pcdv[idxRenderPiece];
			PieceVBVector&		pvv = pcd.pieceColorVBVec;

			for ( UINT32 idxPiece = 0; idxPiece < it->second.m_Pieces.size(); ++idxPiece )
			{
				/*
				model.ModifyMesh( it->first.c_str(), 0, 0, MO_DELETE, idxPiece );
				model.ModifyMesh( it->first.c_str(), pvv[idxPiece]->GetVB(), 0, MO_ADD, idxPiece, 
					pcd.pieceColorVec[idxPiece].size(), VES_DIFFUSE, VET_COLOUR, 0 ); 
				*/		
				model.ModifyMesh( it->first.c_str(), 0, 0, MO_DELETE, idxPiece );
			}
		}
	}

	void CVertexDiffuseColor::ClearModelDiffuse( uint32 iModelGroup, CMapEditObject& meObject )
	{
		return;

		PieceClassDiffuseVector& pcdv = CVertexDiffuseColor::GetInst()
			->GetModelDiffuse(meObject.GetCenterGridIndex(), iModelGroup, meObject.GetName().c_str());
		index_t		idxRenderPiece	= 0;
		CEditModel& model			= meObject;

		if ( pcdv.size() ==0 )
			return;

		for ( RenderPieceIter it = model.GetRenderPieces().begin();
			it != model.GetRenderPieces().end(); ++it, ++idxRenderPiece )
		{
			PieceClassDiffuse&	pcd	= pcdv[idxRenderPiece];
			PieceVBVector&		pvv = pcd.pieceColorVBVec;

			for ( UINT32 idxPiece = 0; idxPiece < it->second.m_Pieces.size(); ++idxPiece )
			{
				model.ModifyMesh( it->first.c_str(), 0, 0, MO_DELETE, idxPiece );
			}
		}
	}

	void CVertexDiffuseColor::BakeMapDiffuse()
	{
		CTerrainMesh*		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		CEditTerrainMgr*	terrain		 = pTerrainMesh->m_pTerrain;
		int width = pTerrainMesh->GetWidthInRegion() / TILE_WIDTH_IN_REGION;
		int depth = pTerrainMesh->GetDepthInRegion() / TILE_WIDTH_IN_REGION;

		for (int j = 0; j < depth; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				BakeTileDiffuse(i, j);
			}
		}
	}

	void CVertexDiffuseColor::BakeTileDiffuse( uint32 x, uint32 z )
	{
		ClearTile(x, z);
		m_pModelProcesser->ProcessTile(x, z);
	}

	// --------------------------------------------------------------------------------------------

	void CalulateMeshDiffuse( PieceDiffuse& pd, CMesh& mesh, const CMatrix& world )
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		CVector3f worldPos;

		for ( uint32 i = 0; i < mesh.GetVerNum(); ++i )
		{
			CVector3f position = mesh.GetVertex(i);
			worldPos = position * world;

			int nDeltaX = int(worldPos.x / GRID_SPACE);
			int nDeltaZ = int(worldPos.z / GRID_SPACE);
			int nCenterGridIndex = nDeltaZ * CMapEditApp::GetInst()->GetEditingMesh()->GetWidth() + nDeltaX;

			DWORD Red	= 0;
			DWORD Green = 0;
			DWORD Blue	= 0;
			if ( pTerrain->IsValid(nCenterGridIndex) )
			{
				SGrid& grid = pTerrain->GetGrid(nCenterGridIndex);

				for ( int k = 0; k < 4; ++k )
				{
					SVertex & vertex = pTerrain->GetVertex(grid.dwVertexIndex[k]);
					Red += ((BYTE*)(&vertex.oColor))[2];
					Green += ((BYTE*)(&vertex.oColor))[1];
					Blue += ((BYTE*)(&vertex.oColor))[0];
				}
			}

			Red		/= 4; 
			Green	/= 4; 
			Blue	/= 4;
			unsigned long iR = Red	 << 16;
			unsigned long iG = Green << 8;
			unsigned long iB = Blue;
			DWORD color = 0xff000000 | iR | iG | iB;
			pd.push_back(color);
		}
	}
}

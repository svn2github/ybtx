#include "stdafx.h"
#include "CTerrain.h"
#include "CHardwareBufferMgr.h"
#include "CVertexDeclaration.h"
#include "CTexture.h"
#include "CTerrainMgr.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

CTerrainTile::CTerrainTile()

{

}

CTerrainTile::~CTerrainTile()
{
	if(NULL!=op.vertexData && NULL!=op.vertexData->vertexBufferBinding)
	{
		for ( int i = 0; i < MIN_SHARE_BUF; ++i )
			CHardwareBufferMgr::GetInst()->DestroyBuffer( op.vertexData->vertexBufferBinding->GetBuffer(i) );
		op.Destroy();
	}
}

TerrainNode::TerrainNode(CElementManager* pParent , URegionID id)
	: CElementNode(pParent,id) 
	, text( NULL )
{
}

TerrainNode::~TerrainNode()
{
	SafeRelease(text);
}

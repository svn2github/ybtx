#include "stdafx.h"
#include "CPluginDataSources.h"
#include "CPluginVertexBuffer.h"
#include "CPluginIndexBuffer.h"
#include "Model.h"

namespace sqr
{
	CVertexHardwareBuffer* CPluginDataSources::NewVertexBuffer( size_t vertexSize, size_t numVerts, Usage usage, void* pInitData, bool useSysMem, bool useShadowBuffer )
	{
		return new CPluginVertexBuffer(vertexSize, numVerts, usage);
	}

	CIndexHardwareBuffer* CPluginDataSources::NewIndexBuffer( size_t numIndexes, Usage usage, void* pInitData, bool useSysMem, bool useShadowBuffer )
	{
		return new CPluginIndexBuffer(numIndexes, usage);
	}

	CPieceGroup* CPluginDataSources::NewPieceGroup( const string& szName , const wstring& Alias )
	{
		return new CPluginPieceGroup();
	}

	CPiece* CPluginDataSources::NewPiece( CPieceGroup* pGrp )
	{
		return new CPluginPiece((CPluginPieceGroup*)pGrp);

	}

	CPieceRenderStyle* CPluginDataSources::NewPieceRenderStyle()
	{
		return new CPluginRenderStyle;
	}
}

#pragma once
#include "CDataSources.h"

namespace sqr
{
	class CPluginDataSources : public CDataSources
	{
	public:
		CVertexHardwareBuffer*	
				NewVertexBuffer( 
					size_t vertexSize, 
					size_t numVerts, 
					Usage usage,
					void* pInitData, 
					bool useSysMem, 
					bool useShadowBuffer 
					);
		
		CIndexHardwareBuffer*	
				NewIndexBuffer( 
					size_t numIndexes, 
					Usage usage,
					void* pInitData, 
					bool useSysMem, 
					bool useShadowBuffer 
					);

		CPieceGroup*			
				NewPieceGroup( const string& szName , const wstring& Alias );
		
		CPiece*	NewPiece( CPieceGroup* pGrp );

		CPieceRenderStyle*	
				NewPieceRenderStyle();
	};
}
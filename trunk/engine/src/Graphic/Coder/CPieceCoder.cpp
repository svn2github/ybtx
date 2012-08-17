#include "stdafx.h"
#include "CPieceCoder.h"
#include "CPieceGroup.h"
#include "CDataSources.h"
#include "CVertexAniMesh.h"
#include "CStaticMesh.h"
#include "CSkeletalMesh.h"
#include "CMeshCoder.h"
#include "CGpuProgramMgr.h"
#include "CPieceRenderStyleCoder.h"
#include "CCoderNameHelper.h"
#include "TSqrAllocator.inl"
#include <string>

namespace
{
	class CPieceCoderPieceGroup : public CPieceGroup
	{
		friend class CPieceCoder;
	};

	class CPieceCoderPiece : public CPiece
	{
		friend class CPieceCoder;
	};
}

namespace sqr
{
	class CPieceNameCoder : public CCoder
	{
		class CPNCoderPiece : public CPiece
		{
			friend class CPieceNameCoder;
		};

		DECLARE_VERSION_MAP();
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::NAME_CODER; }
		uint16	GetNewVer(void)		{ return 1; }
		
		bool Encode(WriteDataInf& wdi , OBJ_HANDLE handle )
		{
			CPNCoderPiece* ps = static_cast<CPNCoderPiece*>(handle);
			return _Encode(wdi, &ps->m_sPieceName);
		}
		
		bool DeCode01( const CDataChunk& inData , OBJ_HANDLE handle )
		{
			CPNCoderPiece* ps = static_cast<CPNCoderPiece*>(handle);
			CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
			string str;
			int32 size = bufFile.GetBufSize();
			vector<char> temp( size + 1, 0);
			bufFile.read(&temp[0], size);
			str = &temp[0];

			ps->_SetPieceName(str);

			return true;
		}

	private:
		bool _EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
		{
			GString* str = static_cast<GString*>(handle);
			wdi.Write(str->c_str(), str->size());

			return true;
		}
	};
	BEGIN_VERSION_MAP(CPieceNameCoder)
		ON_VERSION( 1 , DeCode01 )
	END_VERSION_MAP()

	// ---- CPieceCoder ---------------------------------------------------------------------------

	CPieceCoder::CPieceCoder()
	{
		RegisterCoder(new CPieceNameCoder);
		RegisterCoder(new CPieceRenderStyleCoder);
		RegisterCoder(new CVertexAniMeshCoder);
		RegisterCoder(new CStaticMeshCoder);
		RegisterCoder(new CSkeletalMeshCoder);
	}

	FourCC CPieceCoder::GetChunkName()
	{
		return CCoderNameHelper::PIECE_CODER;
	}

	bool CPieceCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CPieceCoderPieceGroup* pPieceGroup = static_cast<CPieceCoderPieceGroup*>(handle);
		for( size_t i = 0; i < pPieceGroup->m_Piece.size(); ++i )
		{
			for( size_t j = 0; j < pPieceGroup->m_Piece[i].size(); ++j)
			{
				_Encode(wdi,pPieceGroup->m_Piece[i][j]);
			}
		}

		return true;
	}

	bool CPieceCoder::DeCode_Piece( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CPieceCoderPieceGroup* pPieceGroup = (CPieceCoderPieceGroup*)handle;
		CPieceCoderPiece* pPiece = (CPieceCoderPiece*)CDataSources::GetInst()->NewPiece(pPieceGroup);
		_AutoDecode(inData, pPiece);

		CVector3f Scale = pPiece->m_Mesh->GetScale()*0.5f;
		const CVector3f& Off = pPiece->m_Mesh->GetOff();

		Scale.x = CMath::Abs(Scale.x);
		Scale.y = CMath::Abs(Scale.y);
		Scale.z = CMath::Abs(Scale.z);

		pPiece->m_AABox.setExtents( Off - Scale , Off + Scale );

		if ( pPieceGroup->m_PieceClassIndex.find( pPiece->GetPieceClassName() ) == pPieceGroup->m_PieceClassIndex.end() )
		{
			int nIndex = pPieceGroup->m_PieceClassIndex.size();
			pPieceGroup->m_PieceClassIndex[ pPiece->GetPieceClassName() ] = nIndex;
			pPieceGroup->m_Piece.push_back( CPieceClass() );
		}
		pPieceGroup->m_Piece[ pPieceGroup->m_PieceClassIndex[ pPiece->GetPieceClassName() ] ].push_back( pPiece );
		
		return true;
	}

	bool CPieceCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	BEGIN_VERSION_MAP(CPieceCoder)
		ON_VERSION( 1 , DeCode_Piece )
	END_VERSION_MAP()
}

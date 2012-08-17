#include "stdafx.h"
#include "CPieceRenderStyleCoder.h"
#include "CPieceGroup.h"
#include "Algorithm.h"
#include "CRsRefCoder.h"
#include "CTexRefCoder.h"
#include "CDataSources.h"
#include "CCoderNameHelper.h"

namespace sqr
{
	class CPieceRenderStylePrivate : public CPieceRenderStyle
	{
		friend class CPrsRsRefCoder;
		friend class CPrsTexRefCoder;
		friend class CRsIndexCoder;
		friend class CPieceRenderStyleCoder;
	};

	class CPrsCoderPiece : public CPiece
	{
		friend class CPieceRenderStyleCoder;
	};

	class CPrsRsRefCoder : public CRsRefCoder
	{
	public:
		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CPieceRenderStylePrivate* prs = static_cast<CPieceRenderStylePrivate*>(handle);
			string tmp = prs->m_RenderStyleName.c_str();
			_Encode(wdi, &tmp);

			return true;
		}

	private:
		bool _DecodeImpl( const string& refName, PRIVATE_HANDLE handle )
		{
			CPieceRenderStylePrivate* prs = static_cast<CPieceRenderStylePrivate*>(handle);
			prs->m_RenderStyleName = refName.c_str();
			prs->_CreateRenderStyle(refName.c_str(), &prs->m_RenderStyle);
			
			return true;
		}
	};

	// --------------------------------------------------------------------------------------------

	class CPrsTexRefCoder : public CTexRefCoder
	{
	public:
		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CPieceRenderStylePrivate* prs = static_cast<CPieceRenderStylePrivate*>(handle);
			
			for( size_t i = 0; i < prs->m_TextureName.size(); ++i )
			{
				string texName = prs->m_TextureName[i].c_str();
				_Encode(wdi, &texName);
			}

			return true;
		}

	private:
		bool _DecodeImpl( const string& inData, OBJ_HANDLE handle )
		{
			CPieceRenderStylePrivate* prs = static_cast<CPieceRenderStylePrivate*>(handle);
			prs->_CreateStyleTexture(inData.c_str());
			
			return true;
		}
	};

	// --------------------------------------------------------------------------------------------

	class CRsIndexCoder : public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::RS_INDEX_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			CPieceRenderStylePrivate* prs = static_cast<CPieceRenderStylePrivate*>(handle);
			_Encode(wdi, &prs->m_RSIndexName);

			return true;
		}

		bool Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
		{
			CPieceRenderStylePrivate* prs = static_cast<CPieceRenderStylePrivate*>(handle);
			CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
			int32 size = bufFile.GetBufSize();
			vector<char> temp( size + 1, 0);
			bufFile.read(&temp[0], size);
			prs->m_RSIndexName = &temp[0];

			return true;
		}

	private:
		bool _EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
		{
			GString* str = static_cast<GString*>(handle);

			wdi.Write( (const char*)str->c_str(), str->size() );

			return true;
		}
	};
	BEGIN_VERSION_MAP(CRsIndexCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	// --------------------------------------------------------------------------------------------

	BEGIN_VERSION_MAP(CPieceRenderStyleCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	CPieceRenderStyleCoder::CPieceRenderStyleCoder()
	{
		RegisterCoder(new CPrsRsRefCoder);
		RegisterCoder(new CPrsTexRefCoder);
		RegisterCoder(new CRsIndexCoder);
	}

	FourCC CPieceRenderStyleCoder::GetChunkName( void )
	{
		return CCoderNameHelper::PIECE_RENDER_STYLE_CODER;
	}

	bool CPieceRenderStyleCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{	
		CPrsCoderPiece* piece = static_cast<CPrsCoderPiece*>(handle);

		for ( uint32 i=0; i<piece->GetRSNum(); ++i )
		{
			CPieceRenderStyle* prs = piece->GetRS(i);
			_Encode(wdi,prs);
		}

		return true;
	}

	bool CPieceRenderStyleCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CPieceRenderStyleCoder::Decode1( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		CPrsCoderPiece* piece = static_cast<CPrsCoderPiece*>(handle);
		CPieceRenderStyle* pNewPRS = CDataSources::GetInst()->NewPieceRenderStyle();
		piece->m_RenderStyle.push_back(pNewPRS);
		_AutoDecode(inData, pNewPRS);

		GString szRsIndex = pNewPRS->GetRSIndexName();
		piece->m_RSIndxToID[szRsIndex] = piece->m_RenderStyle.size() - 1;

		return true;
	}
}
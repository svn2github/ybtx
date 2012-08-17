#include "stdafx.h"
#include "CModCoder.h"
#include "CPieceCoder.h"
#include "CPieceGroup.h"
#include "CDataSources.h"
#include "CCoderNameHelper.h"
#include <string>

namespace sqr
{
	class CPortraitCoder : public CCoder
	{
		class CPortraitCoderPieceGroup : public CPieceGroup
		{
			friend class CPortraitCoder;
		};

		DECLARE_VERSION_MAP()
	public:
		FourCC	GetChunkName(void)	{ return CCoderNameHelper::PORTRAIT_CODER; }
		uint16	GetNewVer(void)		{ return 1; }

		bool Encode( WriteDataInf& wdi, OBJ_HANDLE handle ) 
		{
			CPortraitCoderPieceGroup* pPieceGroup = static_cast<CPortraitCoderPieceGroup*>(handle);
			if( pPieceGroup->m_pPortraitInfo )
			{
				_Encode(wdi, pPieceGroup->m_pPortraitInfo);
				return true;
			}

			return false; 
		};

		bool Decode1( const CDataChunk& inData, OBJ_HANDLE handle )
		{
			CPortraitCoderPieceGroup* pPieceGroup = static_cast<CPortraitCoderPieceGroup*>(handle);
			CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
			pPieceGroup->m_pPortraitInfo = new PortraitInfo();
			bufFile.SafeRead(pPieceGroup->m_pPortraitInfo);

			return true;
		}

	private:
		bool _EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
		{
			PortraitInfo* pPortInfo = static_cast<PortraitInfo*>(handle);
			wdi.Write( (char*)(pPortInfo), sizeof(PortraitInfo));

			return true;
		}
	};

	BEGIN_VERSION_MAP(CPortraitCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()



	// ---- CPieceGroupCoder ----------------------------------------------------------------------

	CPieceGroupCoder::CPieceGroupCoder()
	{
		RegisterCoder(new CPieceCoder);
		RegisterCoder(new CPortraitCoder);
	}

	FourCC CPieceGroupCoder::GetChunkName()
	{
		return CCoderNameHelper::PIECE_GROUP_CODER;
	}

	bool CPieceGroupCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CPieceGroup* pPieceGroup = static_cast<CPieceGroup*>(handle);
		_Encode(wdi, pPieceGroup);

		return true;
	}

	bool CPieceGroupCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CPieceGroupCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);
		return true;
	}

	BEGIN_VERSION_MAP(CPieceGroupCoder)
		ON_VERSION( 1 , Decode1 )
	END_VERSION_MAP()

	// ---- CModFormat ----------------------------------------------------------------------------

	CModFormat::CModFormat(void)
		: CFileFormatMgr(CCoderNameHelper::MOD_CODER, 0)
	{
		RegisterCoder(new CPieceGroupCoder);
	}

	CModFormat::~CModFormat(void)
	{
	}
}

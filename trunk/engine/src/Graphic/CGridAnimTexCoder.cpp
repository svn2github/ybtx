#include "stdafx.h"
#include "CTexture.h"
#include "CGridAnimTex.h"
#include "CGridAnimTexCoder.h"
#include "CCoderNameHelper.h"

namespace sqr
{
	class CCoderTexGridAnim
		: public CGridAnimTex
	{
		friend class CGridAnimTexCoder;
	};

	// ---- CGridAnimTexCoder ---------------------------------------------------------------------

	CGridAnimTexCoder::CGridAnimTexCoder()
	{
	}

	FourCC CGridAnimTexCoder::GetChunkName(void)
	{
		return CCoderNameHelper::GRID_ANIM_TEX_CODER;
	}

	uint16 CGridAnimTexCoder::GetNewVer(void) 
	{ 
		return 1;
	}

	uint16 CGridAnimTexCoder::GetChunkType()
	{
		return CDataChunk::DCT_AUTO;
	}

	bool CGridAnimTexCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle ) 
	{
		IResTexture* pTex = static_cast<IResTexture*>(handle);

		if ( pTex->GetType() != CGridAnimTex::Type() )
			return false;

		_Encode(wdi, handle);

		return true; 
	};

	bool CGridAnimTexCoder::DeCode1( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		CCoderTexGridAnim* pGridAnimTex = static_cast<CCoderTexGridAnim*>(handle);

		return true;
	}

	bool CGridAnimTexCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		_AutoEncode(wdi, handle);

		CCoderTexGridAnim* pGridAnimTex = static_cast<CCoderTexGridAnim*>(handle);

		return true;
	}

	BEGIN_VERSION_MAP(CGridAnimTexCoder)
		ON_VERSION(1, DeCode1)
	END_VERSION_MAP()

}

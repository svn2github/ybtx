#include "stdafx.h"
#include "CTextureCubeCoder.h"
#include "CCoderNameHelper.h"
#include "CTextureCube.h"

namespace sqr
{
	class CCoderTexCube
		: public CTextureCube
	{
		friend class CTextureCubeCoder;
	};

	// ---- CTextureCubeCoder ---------------------------------------------------------------------

	CTextureCubeCoder::CTextureCubeCoder()
	{
	}

	FourCC CTextureCubeCoder::GetChunkName(void)
	{
		return CCoderNameHelper::TEXTURE_CUBE_CODER;
	}

	uint16 CTextureCubeCoder::GetNewVer(void) 
	{ 
		return 1;
	}

	uint16 CTextureCubeCoder::GetChunkType()
	{
		return CDataChunk::DCT_LEAF;
	}

	bool CTextureCubeCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle ) 
	{
		ITexture* pTex = static_cast<ITexture*>(handle);

		if ( pTex->GetType() != CTextureCube::Type() )
			return false;

		return _Encode(wdi, handle);
	};

	bool CTextureCubeCoder::DeCode1( const CDataChunk& inData ,OBJ_HANDLE handle )
	{
		CCoderTexCube* pTexCube = static_cast<CCoderTexCube*>(handle);

		return true;
	}

	bool CTextureCubeCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		_AutoEncode(wdi, handle);

		CCoderTexCube* pTexCube = static_cast<CCoderTexCube*>(handle);

		return true;
	}

	BEGIN_VERSION_MAP(CTextureCubeCoder)
		ON_VERSION(1, DeCode1)
	END_VERSION_MAP()

}

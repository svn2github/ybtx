#include "stdafx.h"
#include "CTexture3DCoder.h"
#include "CCoderNameHelper.h"
#include "CTexture3D.h"

namespace sqr
{
	class CCoderTex3D
		: public CTexture3D
	{
	public:
		friend class CTexture3DCoder;
	};

	// ---- CTexture3DCoder -----------------------------------------------------------------------

	CTexture3DCoder::CTexture3DCoder()
	{
	}

	FourCC CTexture3DCoder::GetChunkName(void)
	{
		return CCoderNameHelper::TEXTURE_3D_CODER;
	}

	uint16 CTexture3DCoder::GetNewVer(void) 
	{ 
		return 1; 
	}

	uint16 CTexture3DCoder::GetChunkType()
	{
		return CDataChunk::DCT_AUTO;
	}

	bool CTexture3DCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle ) 
	{	
		ITexture* pTex = static_cast<ITexture*>(handle);

		if ( pTex->GetType() != CTexture3D::Type() )
			return false;

		return _Encode(wdi, static_cast<PRIVATE_HANDLE>(handle));
	}

	bool CTexture3DCoder::DeCode1( const CDataChunk& inData ,OBJ_HANDLE handle )
	{
		CCoderTex3D* pTex3D = static_cast<CCoderTex3D*>(handle);

		return true;
	}

	bool CTexture3DCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		_AutoEncode(wdi, handle);

		CCoderTex3D* pTex3D = static_cast<CCoderTex3D*>(handle);

		return true;
	};

	BEGIN_VERSION_MAP(CTexture3DCoder)
		ON_VERSION(1, DeCode1)
	END_VERSION_MAP()

}

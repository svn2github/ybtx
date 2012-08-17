#include "stdafx.h"
#include "CFrameAnimTex.h"
#include "CFrameAnimTexCoder.h"
#include "CCoderNameHelper.h"

namespace sqr
{
	class CCoderTexFrameAnim
		: public CFrameAnimTex
	{
		friend class CFrameAnimTexCoder;
	};

	// ---- CFrameAnimTexCoder --------------------------------------------------------------------

	CFrameAnimTexCoder::CFrameAnimTexCoder()
	{

	}

	FourCC CFrameAnimTexCoder::GetChunkName(void)
	{
		return CCoderNameHelper::FRAME_ANIM_TEX_CODER;
	}

	uint16 CFrameAnimTexCoder::GetNewVer(void) 
	{ 
		return 1; 
	}

	uint16 CFrameAnimTexCoder::GetChunkType()
	{
		return CDataChunk::DCT_AUTO;
	}

	bool CFrameAnimTexCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle ) 
	{
		ITexture* pTex = static_cast<ITexture*>(handle);

		if ( pTex->GetType() != CFrameAnimTex::Type() )
			return false;

		return _Encode(wdi, handle);
	};

	bool CFrameAnimTexCoder::DeCode1( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		CCoderTexFrameAnim* pFrameAnimTex = static_cast<CCoderTexFrameAnim*>(handle);

		return true;
	}

	bool CFrameAnimTexCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		_AutoEncode(wdi, handle);

		CCoderTexFrameAnim* pFrameAnimTex = static_cast<CCoderTexFrameAnim*>(handle);

		return true;
	}

	BEGIN_VERSION_MAP(CFrameAnimTexCoder)
		ON_VERSION(1, DeCode1)
	END_VERSION_MAP()
}

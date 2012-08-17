#include "stdafx.h"
#include "CTexture.h"
#include "CTextureImp.h"
#include "CTextureCoder.h"
#include "CCoderNameHelper.h"

#include "CTexture2DCoder.h"
#include "CTexture3DCoder.h"
#include "CTextureCubeCoder.h"
#include "CGridAnimTexCoder.h"
#include "CFrameAnimTexCoder.h"


namespace sqr
{
	class CCoderTex
		: public IResTexture
	{
	public:
		friend class CTextureCommonCoder;
	};

	class CCoderTexImp
		: public CTextureImp
	{
	public:
		friend class CTextureCommonCoder;
	};

	// ---- CTextureCommonCoder -------------------------------------------------------------------

	class CTextureCommonCoder
		: public CCoder
		, public CCoderHelper
	{
		DECLARE_VERSION_MAP()
	public:
		CTextureCommonCoder()
		{
		}

		FourCC	GetChunkName(void)
		{
			return CCoderNameHelper::TEXTURE_COMMON_CODER;
		}

		uint16	GetNewVer(void)		
		{ 
			return 1; 
		}

		uint16	GetChunkType()		
		{ 
			return CDataChunk::DCT_LEAF; 
		}

		bool	Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
		{
			return _Encode(wdi, handle);
		}
		
		bool	Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
		{
			CCoderTexImp* pTex = (CCoderTexImp*)(((CCoderTex*)(handle))->m_pImp);
			CBufFile bufFile(inData.GetBuffer(), inData.GetSize());

			bufFile >> pTex->m_eFormat;
			bufFile >> pTex->m_uWidth;
			bufFile >> pTex->m_uHeight;
			bufFile >> pTex->m_uMipMap;
			bufFile >> pTex->m_uOrgWidth;
			bufFile >> pTex->m_uOrgHeight;
			bufFile >> pTex->m_uOrgMipMap;

			return true;
		}

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
		{
			CCoderTexImp* pTex = (CCoderTexImp*)(((CCoderTex*)(handle))->m_pImp);

			wdi << pTex->m_eFormat;
			wdi << pTex->m_uWidth;
			wdi << pTex->m_uHeight;
			wdi << pTex->m_uMipMap;
			wdi << pTex->m_uOrgWidth;
			wdi << pTex->m_uOrgHeight;
			wdi << pTex->m_uOrgMipMap;

			return true;
		}
	};

	BEGIN_VERSION_MAP(CTextureCommonCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()


	// ---- CTextureCoder --------------------------------------------------------------------------

	CTextureCoder::CTextureCoder( ITexture *pTexture, CRefBufFile* pBufFile )
		: m_pTexture(pTexture)
		, m_pBufFile(pBufFile)
	{
		RegisterCoder(new CTextureCommonCoder());
		RegisterCoder(new CTexture2DCoder());
		RegisterCoder(new CTexture3DCoder());
		RegisterCoder(new CTextureCubeCoder());
		m_pBufFile->AddRef();
	}

	CTextureCoder::~CTextureCoder()
	{
		m_pBufFile->Release();
	}

	bool CTextureCoder::AutoDeCode( void )
	{
		if ( 0 == m_pBufFile )
			return true;

		CDataChunk dataChunk;
		m_pBufFile->Seek(0);
		m_pBufFile->SafeRead(&dataChunk.GetHdr());
		ReadChunk(*m_pBufFile, dataChunk);

		return DeCode(dataChunk, (OBJ_HANDLE)(m_pTexture));
	}

	FourCC CTextureCoder::GetChunkName( void )
	{
		return CCoderNameHelper::TEXTURE_CODER;
	}

	bool CTextureCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		ITexture* pTex = static_cast<ITexture*>(handle);
		return _Encode(wdi, static_cast<PRIVATE_HANDLE>(pTex));
	}

	bool CTextureCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		return _AutoEncode(wdi, handle);
	}

	bool CTextureCoder::Decode1( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		_AutoDecode(inData, handle);

		return true;
	}

	BEGIN_VERSION_MAP(CTextureCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

}

#include "stdafx.h"
#include "CTexture2DCoder.h"
#include "CCoderNameHelper.h"
#include "CTexture2D.h"
#include "CTexture2DImp.h"
#include "CBufFile.h"
#include "CTexFmtMapper.h"
#include "CGraphic.h"
#include "CGraphicExp.h"
#include "ExpHelper.h"

namespace sqr
{
	class CCoderTex2D
		: public CTexture2D
	{
	public:
		friend class CTexture2DCoder;
		friend class CDDS2DFmtCoder;
	};

	// ---- CNew2DFmtCoder ------------------------------------------------------------------------

	

	// ---- CDDS2DFmtCoder -----------------------------------------------------------------------

	class CDDS2DFmtCoder
		: public CCoder
	{
		DECLARE_VERSION_MAP()
	public:
		CDDS2DFmtCoder()
			: m_nLeftLevel(0)
			, m_uLastReadPos(0)
			, m_bFirstRun(true)
		{
		}

		FourCC	GetChunkName(void)
		{
			CCoderNameHelper::TEXTURE_2D_DDS_CODER;
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
			return true;
		}

		bool	Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
		{
			CCoderTex2D* pTex2D = static_cast<CCoderTex2D*>(handle);
			CTexture2DImp* pTex2DImp = pTex2D->m_pActiveTex;

			if ( pTex2DImp == 0 )
				return true;

			// 纹理创建只能在第一次解码（也就是解码最低的mip map）时进行
			if ( m_bFirstRun )
			{
				SQR_TRY
				{
					bool ret = pTex2DImp->CreateTex(pTex2D->GetWidth(), pTex2D->GetHeight(), pTex2D->GetLevelNum(), pTex2D->GetTexFormat());
					if ( false == ret )
					{
						CGraphic::GetInst()->ClearRecycle();
						ret = pTex2DImp->CreateTex(pTex2D->GetWidth(), pTex2D->GetHeight(), pTex2D->GetLevelNum(), pTex2D->GetTexFormat());
						if ( false == ret )
						{
							GenErr(GraphicErr::GetErrTypeStr(TextureCreate_Err));
							return true;
						}
					}
				}
				SQR_CATCH(exp)
				{
					GenErr(exp.ErrorTitle(), exp.ErrorMsg());
					return true;
				}
				SQR_TRY_END
				
				m_nLeftLevel = pTex2D->GetLevelNum();
				m_uLastReadPos = inData.GetSize();
				m_bFirstRun = false;
			}

			--m_nLeftLevel;
			uint32 numBytes = 0;
			GetLevelInfo(pTex2D->GetWidth(), pTex2D->GetHeight(), pTex2D->GetTexFormat(), m_nLeftLevel, &numBytes, 0, 0);
			
			// 从dds文件的末尾开始加载
			TexBuffer texBuf(numBytes);
			m_uLastReadPos -= numBytes;
			CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
			bufFile.Seek(m_uLastReadPos);
			bufFile.SafeRead(&texBuf[0], texBuf.size());
			pTex2DImp->SetLevelData(texBuf, m_nLeftLevel);

			// 所有level都解码完毕
			if ( m_nLeftLevel == 0 )
				return true;

			// 还有剩余level没有解码
			return false;
		}

	private:
		bool	_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
		{
			return true;
		}

	private:
		int32	m_nLeftLevel;
		uint32	m_uLastReadPos;
		bool	m_bFirstRun;
	};

	BEGIN_VERSION_MAP(CDDS2DFmtCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	// ---- CTexture2DCoder -----------------------------------------------------------------------

	CTexture2DCoder::CTexture2DCoder()
	{
	}

	FourCC CTexture2DCoder::GetChunkName(void)
	{
		return CCoderNameHelper::TEXTURE_2D_CODER;
	}

	uint16 CTexture2DCoder::GetNewVer(void)		
	{ 
		return 1; 
	}

	uint16 CTexture2DCoder::GetChunkType()
	{ 
		return CDataChunk::DCT_AUTO;
	}

	bool CTexture2DCoder::Encode( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		ITexture* pTex = static_cast<ITexture*>(handle);

		if ( pTex->GetType() != CTexture2D::Type() )
			return false;

		return _Encode(wdi, static_cast<PRIVATE_HANDLE>(pTex));
	}

	bool CTexture2DCoder::Decode1( const CDataChunk& inData , OBJ_HANDLE handle )
	{
		CCoderTex2D* pTex = static_cast<CCoderTex2D*>(handle);

		return true;
	}

	bool CTexture2DCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		_AutoEncode(wdi, handle);

		CCoderTex2D* pTex2D = static_cast<CCoderTex2D*>(handle);

		return true;
	}

	BEGIN_VERSION_MAP(CTexture2DCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()
}

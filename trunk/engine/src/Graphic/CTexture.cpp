#include "stdafx.h"
#include "CTexture.h"
#include "BaseHelper.h"
#include "CTextureImp.h"
#include "CTextureQualityMgr.h"

// old texture
#include "Graphic_dx9/CDx9Texture.h"
#include "CCoderNameHelper.h"
#include "CDataChunk.h"
#include "CBufFile.h"
#include "CGraphicExp.h"

namespace sqr
{
	ITexture::ITexture()
		: m_nTexRef(0)
	{
	}

	ITexture::~ITexture()
	{
	}

	const char* ITexture::GetName() const
	{
		return 0;
	}

	const char* ITexture::GetType() const
	{
		return "tex";
	}

	HDC	ITexture::GetHDC(void) 
	{
		return 0;
	}

	bool ITexture::FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes )
	{
		return true;
	}

	uint32 ITexture::GetLevelNum() const
	{
		return 0;
	}

	void ITexture::UpdateAnim( float fRadio )
	{
	}

	uint32 ITexture::FrameCount()
	{
		return 0;
	}

	eTextureFormat ITexture::GetTexFormat() const
	{
		return TF_ARGB32;
	}

	void ITexture::AddTexRef()
	{
		++m_nTexRef;
	}

	void ITexture::ReleaseTexRef()
	{
		--m_nTexRef;

		if ( m_nTexRef == 0 && GetRefCount() == 0 )
			Free();
	}

	void ITexture::AutoGenMip()
	{
	}

	void ITexture::SetLod( uint32 lod )
	{
	}

	// ---- IResTexture ---------------------------------------------------------------------------

	IResTexture::IResTexture( const string& szName, const wstring& szAlias )
		: CResUnit(szName, szAlias, eRT_eTex )
		, m_pImp(0)
		, m_bCustom(false)
	{
		AddRef();
	}

	IResTexture::~IResTexture()
	{
		SafeDelete(m_pImp);
		if( CTextureQualityMgr::HasInst() )
		{
			CTextureQualityMgr::GetInst()->_DelTexture(this);
		}	
	}
	
	const char* IResTexture::GetType() const
	{
		return m_pImp != 0 ? m_pImp->GetType() : 0;
	}

	bool IResTexture::FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes )
	{
		return m_pImp != 0 ? m_pImp->FillFromTexture(pText, rtSrc, rtDes) : false;
	}

	HRESULT IResTexture::Lock( uint32 level, TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num /*= 0*/, uint32 Flags /*= 0 */ )
	{
		return m_pImp != 0 ? m_pImp->Lock(level, pTexLocked, Rect, Num, Flags) : E_FAIL;
	}

	HRESULT IResTexture::Unlock( uint32 level, uint32 Num /*= 0 */ )
	{
		return m_pImp != 0 ? m_pImp->Unlock(level, Num) : E_FAIL;
	}

	eTextureFormat IResTexture::GetTexFormat() const
	{
		return m_pImp != 0 ? m_pImp->GetTexFormat() : TF_UNKNOW;
	}

	uint32 IResTexture::GetWidth() const
	{
		return m_pImp != 0 ? m_pImp->GetWidth() : 0;
	}

	uint32 IResTexture::GetHeight() const
	{
		return m_pImp != 0 ? m_pImp->GetHeight() : 0;
	}

	uint32 IResTexture::GetLevelNum() const
	{
		return m_pImp != 0 ? m_pImp->GetLevelNum() : 0;
	}

	int32 IResTexture::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
	{
		return m_pImp != 0 ? m_pImp->FillFromMemory(pMemory, Format, nWidth, rtSrc, rtDes) : 0;
	}

	int32 IResTexture::FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask /*= NULL */ )
	{
		return m_pImp != 0 ? m_pImp->FillFromHBitmap(hBitmap, hMask) : 0;
	}

	void IResTexture::UpdateAnim( float fRadio )
	{
		if ( m_pImp != 0 )
			m_pImp->UpdateAnim(fRadio);
	}

	uint32 IResTexture::FrameCount()
	{
		return m_pImp != 0 ? m_pImp->FrameCount() : 0;
	}

	bool IResTexture::IsTransparent( float x, float y ) const
	{
		return m_pImp != 0 ? m_pImp->IsTransparent(x, y) : false;
	}

	HRESULT IResTexture::SaveToFile( const string& filename, IMAGE_FILEFORMAT Format /*= IFF_DDS */ )
	{
		return m_pImp != 0 ? m_pImp->SaveToFile(filename, Format) : E_FAIL;
	}

	int32 IResTexture::AddRef()
	{
		return ITexture::AddRef();
	}

	const char* IResTexture::GetName() const
	{
		return CResUnit::GetName();
	}

	bool IResTexture::IsValid()
	{
		return CResUnit::IsValid() && m_pImp != 0 && m_pImp->IsValid();
	}

	void IResTexture::OnRead( CRefBufFile& GraphicFile )
	{
		m_IsRecycle = true;

		CDataChunk checkChunk;
		GraphicFile.SafeRead(&checkChunk.GetHdr());
		GraphicFile.Seek(0);
		if ( checkChunk.GetName() != CCoderNameHelper::TEXTURE_CODER )
		{
			// 使用老的纹理代码
			if ( m_pImp )
				SafeDelete(m_pImp);

			m_pImp = new CDx9Texture(this);
			if( m_bCustom )
			{
				m_pImp->SetCustomParams(m_uCustomWidth, m_uCustomHeight, m_uCustomMinMap, m_eCustomFormat, m_uCustomFilter, m_uCustomMipFilter);
				CTextureQualityMgr::GetInst()->_AddTexture(this);
				((CDx9Texture*)m_pImp)->LoadCOMEx( &GraphicFile );
			}
			else
			{
				if ( m_szName.substr(m_szName.length() - 4, 4) == ".tex" )
				{
					m_IsRecycle = false;
					((CDx9Texture*)m_pImp)->LoadTEX( &GraphicFile );
				}
				else
				{
					CTextureQualityMgr::GetInst()->_AddTexture(this);
					((CDx9Texture*)m_pImp)->LoadCOM( &GraphicFile );
				}
			}
		}
		else
		{
			// 使用新版纹理代码
			m_pImp->ReadDataFromMem(GraphicFile);
		}
	}

	void IResTexture::Free()
	{
		if ( m_nTexRef > 0 )
		{
			stringstream ss;
			ss << "m_nTexRef:" << m_nTexRef << " tex name:" << GetName();
			GfkLogErrOnce("纹理释放时仍被Command引用，放弃进入回收站", ss.str());
		}
		else
			FreeRes();
	}

	int IResTexture::CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, EmptyTexType type )
	{
		switch( type )
		{
		case ETT_OLD:
			m_pImp = new CDx9Texture(this);
			break;

		case ETT_2D:
			break;

		case ETT_3D:
			break;

		case ETT_CUBE:
			break;
		}

		int ret = m_pImp->CreateEmpty(nWidth, nHeight, nLevel, eFormat);
		if ( ret != GERROR )
			SetValid();

		return ret;
	}

	int IResTexture::LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, DWORD Filter, DWORD MipFilter )
	{
		if ( m_szName.size() < 5 )
			return GERROR;

		m_bCustom	= true;
		m_uCustomWidth		= Width;
		m_uCustomHeight	= Height;
		m_uCustomMinMap	= MinMap;
		m_eCustomFormat	= eFormat;
		m_uCustomFilter	= Filter;
		m_uCustomMipFilter	= MipFilter;
		if ( FAILED( CreateResFile(  ) ) )
			return -1;

		return 0;
	}

	int IResTexture::LoadPic()
	{
		if ( m_szName.size() < 5 )
			return GERROR;
		
		if ( FAILED( CreateResFile(  ) ) )
			return GERROR;

		return 0;
	}

	int IResTexture::ReLoadPic()
	{
		m_ppMyself  = new CBaseGraphicRead*;
		if ( FAILED( CreateResFile() ) )
			return -1;

		m_pImp->ReLoadPic();

		return 0;
	}

	void IResTexture::SetBufSize( uint32 uSize )
	{
		if ( m_pImp )
		{
			uSize = uint32(uSize * 1.334f);
			int32 ChangeSize = int32(int64(uSize)-int64(GetBufSize()));
			if( CTextureQualityMgr::HasInst() )
				CTextureQualityMgr::GetInst()->_ChangeTextureSize( ChangeSize, this );
			if(CResMgr::HasInst())
				CResMgr::GetInst()->VideoMemRecycle()->ChangeSize(ChangeSize);
			m_uSize = uSize;
		}
	}

	void* IResTexture::GetBuffer() const
	{
		return m_pImp != 0 ? m_pImp->GetBuffer() : 0;
	}

	void IResTexture::_Reclaim()
	{
		if(m_pImp)
			m_pImp->_Reclaim();
	}

	HDC	IResTexture::GetHDC(void)
	{
		HDC dc=NULL;
		if(m_pImp)
			dc=m_pImp->GetHDC();
		return dc;
	}

	void IResTexture::AutoGenMip()
	{
		if ( m_pImp )
			m_pImp->AutoGenMip();	
	}

	void IResTexture::SetLod( uint32 lod )
	{
		if ( m_pImp )
			m_pImp->SetLod(lod);
	}
}

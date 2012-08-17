#include "stdafx.h"
#include "CTextureImp.h"

namespace sqr
{
	CTextureImp::CTextureImp( IResTexture* pParent )
		: m_eFormat(TF_UNKNOW)
		, m_uWidth(0)
		, m_uHeight(0)
		, m_uMipMap(0)
		, m_uOrgWidth(0)
		, m_uOrgHeight(0)
		, m_uOrgMipMap(0)
		, m_pParent(pParent)
	{
	}

	CTextureImp::~CTextureImp()
	{
		m_pParent = 0;
	}

	uint32 CTextureImp::FrameCount()
	{
		return 0;
	}

	eTextureFormat CTextureImp::GetTexFormat() const
	{
		return m_eFormat;	
	}

	uint32 CTextureImp::GetWidth() const
	{
		return m_uWidth;
	}

	uint32 CTextureImp::GetHeight() const
	{
		return m_uHeight;
	}

	uint32 CTextureImp::GetLevelNum() const
	{
		return m_uMipMap;
	}

	bool CTextureImp::FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes )
	{
		return true;
	}

	bool CTextureImp::IsValid()
	{
		return true;
	}

	void CTextureImp::UpdateAnim( float fRadio )
	{
	}

	void CTextureImp::SetCustomParams( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, uint32 Filter, uint32 MipFilter )
	{

	}

	int CTextureImp::ReLoadPic()
	{
		return 0;
	}
}

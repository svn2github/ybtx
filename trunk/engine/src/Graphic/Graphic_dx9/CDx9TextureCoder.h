#pragma once
#include "SyncSystem.h"
#include "Dx9Base.h"
#include "CBufFile.h"
#include "CDx9Texture.h"

namespace sqr
{
	class IResTexture;

	class CDx9TexCoder 
		: public SyncCoder
		, public CGraphicMallocObject
	{
	public:
		CDx9TexCoder(CDx9Texture* pText, CRefBufFile* pBufFile);
		virtual~CDx9TexCoder();

	protected:
		CRefBufFile* m_pBufFile;
		CDx9Texture* m_pTexture;
	};

	class CDx9DefaultTexCoder : public CDx9TexCoder
	{
	public:
		CDx9DefaultTexCoder(CDx9Texture* pText, CRefBufFile* pBufFile,UINT width, UINT height, UINT mipmap );
		bool AutoDeCode(void);
	protected:
		UINT		 m_uWidth;
		UINT         m_uHeight;
		UINT		 m_uMipMap;
	};

}
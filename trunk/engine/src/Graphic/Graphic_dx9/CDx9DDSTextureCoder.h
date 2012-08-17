#pragma once
#include "CDx9TextureCoder.h"

namespace sqr
{
	class CDx9Texture;

	enum ECUBEMAP_FACES 
	{
		ECMF_POSITIVE_X = 0,
		ECMF_NEGATIVE_X = 1,
		ECMF_POSITIVE_Y = 2,
		ECMF_NEGATIVE_Y = 3,
		ECMF_POSITIVE_Z = 4,
		ECMF_NEGATIVE_Z = 5,
		ECMF_COUNT,
		ECMF_FORCE_DWORD = 0xffffffff,
	};

	typedef struct _DDSCreateParams
	{
		UINT		m_uWidth;
		UINT        m_uHeight;
		UINT		m_uMipMap;
		D3DFORMAT	m_eTexFmt;

		UINT		m_uOrgWidth;
		UINT		m_uOrgHeight;
		UINT		m_uOrgMipMap;
		D3DFORMAT	m_eOrgTexFmt;	
	} DDSCREATEPARAMS;

	class CDx9DDSCoder : public CDx9TexCoder
	{
	public:
		CDx9DDSCoder(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams );
		~CDx9DDSCoder();
		bool CopyDataToSurface( const string& strTexName, BYTE *pSuf, UINT uWidth, UINT uHeight, UINT uMipMap, D3DFORMAT fmt,  
			UINT pitch );
		bool AutoDeCode(void);
		//void	Release();
		virtual		HRESULT _CreateTexture(void)	= 0;
		virtual		void _UpdateParams(void)		= 0;
		virtual     bool _LoadTextureLod(void)	= 0;

	private:
		virtual void checkTextureSize() = 0;
		virtual void calculateSkipBytes();

	protected:
		DDSCREATEPARAMS	m_stCreateParams;
		uint32			m_uSkipBytes;
		//string			m_TextureName;
	};

	class CDx9DDSTexture : public CDx9DDSCoder
	{
	public:
		CDx9DDSTexture(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams  );
	protected:
		HRESULT _CreateTexture(void);
		void _UpdateParams(void);
		bool _LoadTextureLod(void);

	private:
		void	checkTextureSize();
		virtual void calculateSkipBytes();
	};

	class CDx9DDSVolumeTexture : public CDx9DDSCoder
	{
	public:
		CDx9DDSVolumeTexture(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams ,UINT Depth );
	protected:
		HRESULT _CreateTexture(void);
		void _UpdateParams(void);
		bool _LoadTextureLod(void);

	private:
		void	checkTextureSize();
		virtual void calculateSkipBytes();

	protected:
		UINT		 m_uDepth;
	};
	
	class CDx9DDSCubeTexture : public CDx9DDSCoder
	{
	public:
		CDx9DDSCubeTexture(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams  );
	protected:
		HRESULT _CreateTexture(void);
		void _UpdateParams(void);
		bool _LoadTextureLod(void);

	private:
		void	checkTextureSize();

	protected:
		UINT		m_eCubeFace;
		int32		m_sCubeOffset[ECMF_COUNT];		
	};
}
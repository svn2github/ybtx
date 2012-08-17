#pragma once
#include "CTexture.h"
#include "CWindowTarget.h"
#include "RTTFormat.h"

namespace sqr
{
	SmartPointer(CRenderTexture);
	class CRenderTexture
		: public CWindowTarget
		, public ITexture
	{
	public:
		CRenderTexture();
		virtual	~CRenderTexture();
		virtual void	Begin(bool needClear = true) = 0;
		virtual void	End()			= 0;
		virtual void	Render(void);

	public:
		inline bool Create()
		{
			return m_bCreate = _Create();
		}

		inline	_texFORMAT	GetFormat() const
		{
			return m_Format;
		}
		inline	void	SetFormat(_texFORMAT fmt)
		{
			m_Format = fmt;
		}
		virtual	bool	IsAntiAlias() const
		{
			return m_isAntiAlias;
		}
		virtual void	SetAntiAlias(bool isUseAntiAlias)
		{
			m_isAntiAlias = isUseAntiAlias;
		}

		inline uint32	GetWidth(void) const
		{
			return CWindowTarget::GetWidth();
		}
		inline uint32	GetHeight(void) const
		{
			return CWindowTarget::GetHeight();
		}
		inline bool	IsUseDepthBuffer()
		{
			return m_bDepthBuffer;
		}

		virtual void	SetUseDepthBuffer(bool b);

		inline int32	AddRef();
		inline void		Release();
		inline int32	GetRefCount() const;
		void	Free();

		void	SetWidth( uint32 width );
		void	SetHeight( uint32 height );
		void	SetRect( const CIRect& rect );


		virtual uint32		FrameCount()							{ return 0; }
		virtual const char*	Name()	const							{ return "RenderTarget"; };
		virtual bool		IsTransparent(float x,float y) const	{ return false; };
		virtual void		RestoreSize( bool useClientRect = true ){};
		virtual int			FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL ) { return 0; };
		virtual int			FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes ) { return 0; };

		virtual CRenderTexture*		GetDepthBuffer(void)
		{ return NULL; };

		virtual bool		UpdateDefaultTex(void) { return false; };

	protected:
		virtual bool		_Create() 			= 0;
		bool				m_bDepthBuffer;
		_texFORMAT			m_Format;
		bool				m_isAntiAlias;	///< 反锯齿, 要Graphic启用之后才有效
		bool				m_bCreate;
	};

	inline int32 CRenderTexture::AddRef()
	{
		return CWindowTarget::AddRef();
	}

	inline void	CRenderTexture::Release()
	{
		CWindowTarget::Release();
	}

	inline int32 CRenderTexture::GetRefCount() const
	{
		return CWindowTarget::GetRefCount();
	}

}
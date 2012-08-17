#pragma once
#include "FilterDefine.h"
#include "GraphicBase.h"
#include "CPostFilter.h"
#include "CBloomTarget.h"

namespace sqr
{

#define	MAX_FILTER	5

	class CCamera;

	//‰÷»æ≥–‘ÿÃÂ
	class CRenderTarget 
		: public CRefObject
		, public CGraphicMallocObject
	{
	public:
		CRenderTarget(void);
		virtual ~CRenderTarget();

	public:
		virtual CRenderGroup*	GetGroup(void);
		virtual CCamera*		GetCamera(void);
				CBloomTarget*	GetTargetPipeline(void);
		virtual void	SetRenderGroup(CRenderGroup* pRenderGroup);
		virtual void	InsertRenderer( CRendererBase* pRenderSys );
		virtual void	DeleteRenderer( CRendererBase* pRenderSys );
		virtual wstring	DoScreenShot(const wchar_t* szAliasPath, const wchar_t * szFilename) {return L"";};

		virtual	void	Clear();
		virtual void	SetClearColor(const CColor& color);
				CColor	GetClearColor(void);
		virtual void	AutoClearColor( bool bAuto );

		virtual void	UpdateViewport();
		virtual void	Render(void);
				void	UpdateAspect(void);

		virtual uint32	GetFrameCount(void)	{ return 0; };

	public:
		virtual void	SetAutoRender( bool isAutoRender );
		virtual bool	IsAutoRender(void);
		virtual void	LockAspectRatio(float ratio);
		virtual bool	isLockAspectRatio(void);
		virtual void	SetWidth( uint32 width );
		virtual void	SetHeight( uint32 height );
		virtual void	SetRect( const CIRect& rect );
		virtual void	GetRect( CIRect& rect );
		virtual uint32	GetWidth(void) const;
		virtual uint32	GetHeight(void) const;

		struct Filter
		{
			ITexture*			pText;
			uint32				dwColor;
			bool				bMulti;
			bool				bRatio;
			_TEXTUREADDRESS		eSamp;
			CFRect				cTexRt;
		};

		int						m_nNearCount;
		Filter					m_NearFilter[MAX_FILTER];
		int						m_nFarCount;
		Filter					m_FarFilter[MAX_FILTER];
		int						m_nPostCount;
		CPostFilter*			m_PostFilter[MAX_FILTER];

		void	Flip();
		void	AddFilter( ITexture* pTexture, uint32 Color, bool bNear, bool bMulti, bool bratio = false, _TEXTUREADDRESS samp = TADDRESS_WRAP, CFRect* rt = NULL );
		void	AddPostFilter( CPostFilter* pPostFilter );
		void	AddFilterOff( float xDelta, float yDelta, float zDelta );

		inline  void SetClearModeType( ClearModeType cType )
		{
			m_cmtClear = cType;
		}

		inline	ClearModeType	GetClearModeType()
		{
			return m_cmtClear;
		}

	protected:
		typedef GVector<CRendererBase*>	RendererList;
		bool			Update();

		CIRect					m_rcViewPort;
		bool					m_bIsAutoRender;
		float					m_fLockAsp;
		CRenderGroup*			m_pRenderGroup;
		CBloomTarget*			m_pBloomTarget;
		ClearModeType			m_cmtClear;
		RendererList			m_lRenderSystems;
		bool					m_isAutoClearColor;
		CColor					m_ClearColor;

		//---------------…„œÒª˙’∂Ø--------------
		CMatrix					m_matPreDeltaView;
		CMatrix					m_matCurDeltaView;
		//  
		/*	CMatrix					m_matPreDeltaProj;*/
		/*	CMatrix					m_matCurDeltaProj;*/
	};

	inline void CRenderTarget::SetAutoRender( bool isAutoRender )
	{
		m_bIsAutoRender = isAutoRender;
	}

	inline bool CRenderTarget::IsAutoRender(void)
	{
		return m_bIsAutoRender;
	}


	inline void CRenderTarget::LockAspectRatio(float ratio)
	{
		m_fLockAsp = ratio;
		Update();
	}

	inline bool CRenderTarget::isLockAspectRatio(void)
	{
		return !gIsZero(m_fLockAsp);
	}

	inline void CRenderTarget::GetRect( CIRect& rect )
	{
		rect = m_rcViewPort;
	}

	inline CRenderGroup* CRenderTarget::GetGroup(void)
	{
		return m_pRenderGroup;
	}

	inline uint32 CRenderTarget::GetWidth(void) const 
	{
		return m_rcViewPort.Width();
	}

	inline uint32 CRenderTarget::GetHeight(void) const
	{
		return m_rcViewPort.Height();
	}

}
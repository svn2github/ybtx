#pragma once
#include "CRendererBase.h"
#include "CTexture.h"
#include "CRectangle.h"
#include "TSingleton.h"


class CFlashEditor;
namespace sqr
{
	class CFlashRender : public CRendererBase
	{
		friend class CFlashEditor;
	public:
		CFlashRender();
		virtual ~CFlashRender();

		void SetBackTexture(ITexture* pText);
		void SetDrawRect(const CFRect& Rect);
		void SetOffSet( float X, float Y );

		inline void SetAlphaTest(bool bAT)		{ m_bAlphaTest = bAT;	}
		inline void SetAlphaBlend(bool bAB)		{ m_bAlphaBlend	= bAB;	}
		inline bool	GetAlphaTest(void)			{ return m_bAlphaTest;	}
		inline bool GetAlphaBlend(void)			{ return m_bAlphaBlend;	}

		inline void SetShowRect(bool bShow)		{ m_bShowRect = bShow;	}
		inline bool	GetShowRect(void)			{ return m_bShowRect;	}

		inline const CFRect& GetDrawRect(void)	{ return m_fRect; }

		void Render();

		void	BeginRenderer(void);
		void	EndRenderer(void);
		inline bool	IsStateInRender(void)		{ return m_bStateInRender; }


	protected:
		ITexture*	m_pText;
		CFRect		m_fRect;

		float		m_fOffX;
		float		m_fOffY;

		uint32		m_Width;
		uint32		m_Height;

		bool		m_bAlphaTest;
		bool		m_bAlphaBlend;

		bool		m_bShowRect;

		bool		m_bStateInRender;

		// ±à¼­¹¦ÄÜ
	};
}
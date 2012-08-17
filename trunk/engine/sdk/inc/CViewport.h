#pragma once
#include "CRenderTarget.h"
namespace sqr
{
	class CViewport : public CRenderTarget
	{
	public:
		CViewport(CWindowTarget* pParentWindow);
		virtual ~CViewport();
		void	Render(void);
		void	SetRect( const CIRect& rect );
	protected:
		CWindowTarget* m_pParentWindow;
	};
}
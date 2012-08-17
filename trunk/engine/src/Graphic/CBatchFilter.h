#pragma once
#include "CGeometryBatch.h"
#include "CShadowFilter.h"
namespace sqr
{
	class CBatchFilter : public CGeometryBatch
	{
	public:
		CBatchFilter( CGeometryParams* pParams, CShadowFilter* pShadowFilter = NULL);
		virtual ~CBatchFilter( );

		void	Render(void);
		void	ModifyRS(STATIC_RS&	RS);
		void	ChangeShadowFilter(CShadowFilter* pShadowFilter);
	protected:
		CShadowFilter*	m_pShadowFilter;
	};
}
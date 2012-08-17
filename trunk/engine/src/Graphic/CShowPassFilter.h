#pragma once
#include "CRenderFilter.h"
#include "CFilterNode.h"
#include "CRenderList.h"

namespace sqr
{
	class CShowPassFilter : public CRenderFilter
	{
	public:
		CShowPassFilter(CRenderGroup* pGroup);
		virtual ~CShowPassFilter();

		FilterNode* Filter(FilterNode* node);
		void Render();

	protected:
		STATIC_RS		m_ShowRs;
		StencilParam	m_ShadowStencil;
		CRenderList		m_RenderList;
		CRenderGroup*	m_pParentGroup;
	};
}
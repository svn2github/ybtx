#pragma once
#include "CDefaultFilter.h"

/*用于明确Alpha混合的物体*/

namespace sqr
{
	class CAlphaFilter : public CDefaultFilter
	{
	public:
		CAlphaFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup);
		virtual ~CAlphaFilter();
	public:
		FilterNode* Filter(FilterNode* fn);
	};
}

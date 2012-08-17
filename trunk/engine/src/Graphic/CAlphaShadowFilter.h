#pragma once
#include "CDefaultFilter.h"
namespace sqr
{
	//Ó°×Ó·ÖÀëÆ÷
	class CAlphaShadowFilter : public CDefaultFilter
	{
	public:
		CAlphaShadowFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup)
			:CDefaultFilter(eRos,pRenderGroup) {};
		FilterNode* Filter(FilterNode* fn);
	};
}
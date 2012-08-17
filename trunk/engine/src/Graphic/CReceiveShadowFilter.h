#pragma once
#include "CDefaultFilter.h"

namespace sqr
{
class FilterNode;
class CShadowFilter;

class CReceiveShadowFilter : public CDefaultFilter
{
public:
	CReceiveShadowFilter(ERenderObjStyle ros, CRenderGroup* pRenderGroup , CShadowFilter* pShadowFilter);
	virtual ~CReceiveShadowFilter();

	FilterNode* Filter( FilterNode* fn );
	void		Render();
private:
	CShadowFilter *m_pShadowFilter;
};


class CUnReceiveShadowFilter : public CDefaultFilter
{
public:
	CUnReceiveShadowFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup);
	FilterNode* Filter(FilterNode* fn);
};

}// namespace sqr

#ifndef _CPlane_Shadow_Filter_H_
#define _CPlane_Shadow_Filter_H_

#include "CShadowFilter.h"

//阴影排序器
namespace sqr
{
//平面阴影
class CPlaneShadowFilter : public CShadowFilter
{
public:
	CPlaneShadowFilter(CRenderGroup* pRenderGroup);
	FilterNode* Filter(FilterNode* fn);
	void Update(FilterNode* fn);
	void Begin(void);
	void End(void);
	EShadowType GetShadowType() const
	{
		//return ST_PLANE;
		return ST_NONE;
	}
	void ResetFilterQueue(RenderQueueFlag& fQueueFlag);
protected:
	StencilParam	m_ShadowStencil;
};
}
#endif
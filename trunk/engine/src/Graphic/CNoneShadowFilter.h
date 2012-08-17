#pragma once
#include "CShadowFilter.h"
//“ı”∞≈≈–Ú∆˜
namespace sqr
{
	class CNoneShadowFilter : public CShadowFilter
	{
	public:
		CNoneShadowFilter(CRenderGroup* pRenderGroup);
		void Render(void) {};
		EShadowType GetShadowType() const
		{
			return ST_NONE;
		}
		virtual void ResetFilterQueue(RenderQueueFlag& fQueueFlag);
		void SetShadowParams(STATIC_RS& RS);
	};

}
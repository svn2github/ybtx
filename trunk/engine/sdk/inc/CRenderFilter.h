#pragma once
#include "CRenderBatch.h"
#include "CFilterNode.h"
namespace sqr
{
	class CRenderFilter : public CRenderBatch //äÖÈ¾ÈÝÆ÷
	{
	public:
		CRenderFilter();
		virtual ~CRenderFilter();
		virtual FilterNode* Filter(FilterNode* fn);
		virtual void Render(void);
		virtual void RenderEx(void);
		virtual	void Refresh(CRenderFilter* newFilter);
		virtual void SetLightIndex(uint index);
		virtual void UpdateRenderStyle(const RenderParam& RS );
	};
}
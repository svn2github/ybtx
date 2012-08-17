#pragma once
#include "GraphicRes.h"
#include "GraphicBase.h"
#include "FilterDefine.h"

namespace sqr
{
	class CRenderBatch  
		: public CRefObject
		, public CGraphicMallocObject
	{
	public:
		CRenderBatch(){}
		virtual ~CRenderBatch(){};
		virtual void	Render(void){};
		virtual void	Update(void){};
	};
}
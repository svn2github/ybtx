#pragma once
#include "CRefObject.h"
namespace sqr
{
	class CPostFilter : public CRefObject
	{
	public:
		virtual void RenderFilter(void) = 0;
	};
}
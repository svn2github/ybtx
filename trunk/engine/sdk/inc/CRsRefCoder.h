#pragma once
#include "CRefCoder.h"

namespace sqr
{
	class CRsRefCoder : public CRefCoder
	{
	public:
		FourCC	GetChunkName();
	};
}
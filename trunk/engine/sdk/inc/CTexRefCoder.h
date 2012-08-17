#pragma once
#include "CRefCoder.h"

namespace sqr
{
	class CTexRefCoder : public CRefCoder
	{
	public:
		FourCC	GetChunkName(void);
	};
}
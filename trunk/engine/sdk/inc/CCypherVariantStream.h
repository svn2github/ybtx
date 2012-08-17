#pragma once
#include "TVariantStream.h"

namespace sqr
{
	class CCypherVariantStream
		: public TVariantStream<CCypherVariantMallocObject>
	{
	};
}


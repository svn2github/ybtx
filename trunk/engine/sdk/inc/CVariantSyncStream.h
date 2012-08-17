#pragma once
#include "TVariantStream.h"

namespace sqr
{
	class CVariantSyncStream
		: public TVariantStream<CTrMsgMallocObject>
	{
	};
}


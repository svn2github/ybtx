#pragma once
#include "CTrMsgBufferSwapper.h"

namespace sqr
{
	
	class CAsynServerTrMsgBufferSwapper
		: public CTrMsgBufferSwapper
	{
	public:
		CAsynServerTrMsgBufferSwapper();
		~CAsynServerTrMsgBufferSwapper();
	
		static CAsynServerTrMsgBufferSwapper*& Inst();

		void OnWakeLeft();
	
	};

}

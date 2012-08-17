#pragma once
#include "CTrMsgBufferSwapper.h"

namespace sqr
{
	
	class CSyncServerTrMsgBufferMgrSwapper
		: public CTrMsgBufferSwapper
	{
	public:
		CSyncServerTrMsgBufferMgrSwapper();
		~CSyncServerTrMsgBufferMgrSwapper();
	
		static CSyncServerTrMsgBufferMgrSwapper*& Inst();

		void OnWakeLeft();
	
	};

}

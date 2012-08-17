#pragma once
#include "CCoreThreadMsg.h"

namespace sqr
{
	class CCoreMsgSignal
		:public CCoreThreadMsg
	{
	public:
		CCoreMsgSignal(int32 nSignal);
		~CCoreMsgSignal(void);

	private:
		int32	m_nSignal;
	};
}


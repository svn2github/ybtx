#pragma once
#include "CPos.h"
#include "CTimeCheckPoint.h"


namespace sqr
{
	class CDirObjActiveState
	{
	public:
		CDirObjActiveState(void);
		~CDirObjActiveState(void);

		uint32	m_uTellStepCounter;		//计数器，用于实现每固定次数step之后，再发送一次step通知给服务端。

		float	m_fLastSentMovedDist;		//最后一个发给仲裁对象的已移动距离
		float	m_fUnsentMovedDist;			//目前未发送给仲裁对象的已移动距离

		//移动开始的时间点，用于计算timestamp
		CTimeCheckPoint		m_tcpBegin;
		CTimeCheckPoint		m_tcpLastTellStep;
	};
}
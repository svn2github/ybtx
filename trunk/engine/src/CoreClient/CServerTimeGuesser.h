#pragma once
#include "CTimeCheckPoint.h"

namespace sqr
{
		class CServerTimeGuesser
		{ 
		public:
			CServerTimeGuesser();
			uint64 Guess()const;
			uint64 GetElapse()const;
			uint64 GetElapsedFrameTime()const;

			void SetBaseServerTime(uint64 uBaseServerTime);

		private:
			uint64			m_uBaseServerTime;
			uint64			m_uBaseFrameTime;
			CTimeCheckPoint	m_tcpElapsedTime;

		};

}

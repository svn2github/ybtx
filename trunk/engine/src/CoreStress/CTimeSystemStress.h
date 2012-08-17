#pragma once

#include "TTimeSystem.h"

namespace sqr
{

	class CTimeSystemStress
		:public TTimeSystem<CTimeSystemStress>
	{
	private:
		friend class CConnStress;

		typedef TTimeSystem<CTimeSystemStress>	Parent_t;
		friend class TTimeSystem<CTimeSystemStress>;
		CTimeSystemStress(uint32 uBaseCyc);

		int32	m_nClientServerTimeDiff;	//client和server的绝对时间的秒数差异

		void SetGlobalTime(uint32 uGlobalTime);
	public:
		uint32 GetGlobalTime()const;
	};

}


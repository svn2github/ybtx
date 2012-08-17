#pragma once

#include "TTimeSystem.h"

namespace sqr
{

	class CTimeSystemClient
		:public TTimeSystem<CTimeSystemClient>
	{
	private:
		friend class CConnClient;

		typedef TTimeSystem<CTimeSystemClient>	Parent_t;
		friend class TTimeSystem<CTimeSystemClient>;
		CTimeSystemClient(uint32 uBaseCyc);
	public:
	};

}
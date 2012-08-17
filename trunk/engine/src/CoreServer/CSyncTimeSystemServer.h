#pragma once

#include "TTimeSystem.h"

namespace sqr
{

	class CSyncTimeSystemServer
		:public TTimeSystem<CSyncTimeSystemServer>
	{
	private:
		typedef TTimeSystem<CSyncTimeSystemServer>	Parent_t;
		friend class TTimeSystem<CSyncTimeSystemServer>;
		CSyncTimeSystemServer(uint32 uBaseCyc);		
	};

}

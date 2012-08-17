#pragma once

#include "TTimeSystem.h"

namespace sqr
{

	class CAsynTimeSystemServer
		:public TTimeSystem<CAsynTimeSystemServer>
	{
	private:
		typedef TTimeSystem<CAsynTimeSystemServer>	Parent_t;
		friend class TTimeSystem<CAsynTimeSystemServer>;
		CAsynTimeSystemServer(uint32 uBaseCyc);		
	};

}

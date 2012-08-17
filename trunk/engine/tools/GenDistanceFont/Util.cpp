#include "stdafx.h"
#include "Util.hpp"

#include <boost/date_time.hpp>
#include <boost/thread/thread.hpp>

namespace sqr
{
	void Sleep( uint32_t ms )
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
	}
}
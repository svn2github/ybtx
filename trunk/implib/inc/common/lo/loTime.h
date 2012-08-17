#ifndef __LOTIME_H__
#define __LOTIME_H__

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "common/lo/loctypes.h"

#ifdef POSIX
#include <sys/time.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

namespace locom
{
#ifdef POSIX
	inline uint32 Time()
	{
		struct timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
#endif

#ifdef WIN32
	inline uint32 Time() 
	{
		return GetTickCount();
	}
#endif

	inline uint32 StartTime()
	{
		// Close to program execution time
		static const uint32 g_start = Time();
		return g_start;
	}



	inline bool TimeIsBetween(uint32 later, uint32 middle, uint32 earlier) 
	{
		if (earlier <= later) {
			return ((earlier <= middle) && (middle <= later));
		} else {
			return !((later < middle) && (middle < earlier));
		}
	}

	inline int32 TimeDiff(uint32 later, uint32 earlier)
	{
		uint32 LAST = 0xFFFFFFFF;
		uint32 HALF = 0x80000000;
		if (TimeIsBetween(earlier + HALF, later, earlier)) {
			if (earlier <= later) {
				return static_cast<long>(later - earlier);
			} else {
				return static_cast<long>(later + (LAST - earlier) + 1);
			}
		} else {
			if (later <= earlier) {
				return -static_cast<long>(earlier - later);
			} else {
				return -static_cast<long>(earlier + (LAST - later) + 1);
			}
		}
	}
	inline uint32 ElapsedTime()
	{
		return TimeDiff(Time(), StartTime());
	}

	// TODO: Delete this old version.
    #define GetMillisecondCount Time
} // namespace locom


#endif /* __LOTIME_H__ */
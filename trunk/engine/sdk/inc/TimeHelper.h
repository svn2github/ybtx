#pragma once
#include "ModuleDefs.h"
#include "BaseTypes.h"

namespace sqr
{
	MODULE_API uint64 GetHDProcessTime();
	MODULE_API uint64 GetProcessTime();
	MODULE_API uint64 GetProcessBeginTime();
	MODULE_API void SetProcessBeginTime(uint64 uTime);


#define PROFILE_BEGIN					\
	vector<uint64> __uProfileTimes;int __ProfileIndex=0;__uProfileTimes[__ProfileIndex++]= GetHDProcessTime();

#define PROFILE_CHECK					\
	__uProfileTimes[__ProfileIndex++]= GetHDProcessTime();

#define PROFILE_END						\
	__uProfileTimes[ __ProfileIndex++ ] = GetHDProcessTime();	\
	for( int i=1;i < __ProfileIndex;++i ){cout<< __uProfileTimes[i]-__uProfileTimes[i-1] << "\t";}\
	cout << __uProfileTimes[ __ProfileIndex -1 ] - __uProfileTimes[ 0 ] << endl;

}


#pragma once
#include "TMallocAllocator.h"


namespace sqr
{
	class CStackString
		:public basic_string<char,char_traits<char>,TMallocAllocator<char> >
	{
	};
}


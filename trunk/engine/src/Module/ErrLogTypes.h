#pragma once
#include "TMallocAllocator.h"
#include "CMallocObject.h"
#include <fstream>

namespace sqr
{
	class CLogString
		:public basic_string<wchar_t,std::char_traits<wchar_t>,
		TMallocAllocator<wchar_t> >
	{};

	class CLogStringStream 
		: public basic_ostringstream<char,std::char_traits<char>,
		TMallocAllocator<char> >
	{};
}

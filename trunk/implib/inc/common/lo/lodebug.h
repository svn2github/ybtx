#ifndef __lo_DEBUG_H__
#define __lo_DEBUG_H__

#if defined(_MSC_VER)
// warning C4355: 'this' : used in base member initializer list
#pragma warning(disable:4355)
#endif

#if defined(WIN32) || defined(WINCE)
#include <windows.h>
#include <cstdio>
#endif
/////////////////////////////////////////////////////////////////////////////
// Assertions
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

// Break causes the debugger to stop executing, or the program to abort
inline void _Break()
{
#ifdef WIN32
	::DebugBreak();
#else // !WIN32
	abort();
#endif // !WIN32
}

// LogAssert writes information about an assertion to the log

inline void LogAssert(const char * function, const char * file, int line, const char * expression)
{
  // TODO - if we put hooks in here, we can do a lot fancier logging
  fprintf(stderr, "%s(%d): %s @ %s\n", file, line, expression, function);
}

inline void Assert(bool result, const char * function, const char * file, int line, const char * expression)
{
	if (!result)
	{
		LogAssert(function, file, line, expression);
		_Break();
	}
}

#if defined(_MSC_VER) && _MSC_VER < 1300
#define __FUNCTION__ ""
#endif

#ifndef ASSERT
#define ASSERT(x) Assert((x),__FUNCTION__,__FILE__,__LINE__,#x)
#endif

#ifndef VERIFY
#define VERIFY(x) Assert((x),__FUNCTION__,__FILE__,__LINE__,#x)
#endif

#else // !_DEBUG

#ifndef ASSERT
#define ASSERT(x) (void)0
#endif

#ifndef VERIFY
#define VERIFY(x) (void)(x)
#endif

#endif // !_DEBUG

#endif // __lo_DEBUG_H__

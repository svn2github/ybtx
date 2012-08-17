#pragma once

namespace sqr
{
#ifdef _WIN32
	typedef int (* SQRENTRYPROC)(int, wchar_t*[]);
#else
	typedef int (* SQRENTRYPROC)(int, char*[]);
#endif

#ifdef _WIN32
#define SQRENTRY	extern "C" __declspec(dllexport)
#else
#define SQRENTRY	extern "C"
#endif
}


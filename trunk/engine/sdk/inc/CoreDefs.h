#ifndef __ARKCOREDEFS_H__
#define __ARKCOREDEFS_H__



#if defined(WIN32)

#if defined(ENGINEBUILD)

#define _API __declspec(dllexport)
#define _CAPI extern "C" __declspec(dllexport)

#else

#define _API __declspec(dllimport)
#define _CAPI extern "C" __declspec(dllimport)

#endif

#else

#define _API
#define _CAPI

#endif



#endif


#ifndef __lo_CTYPES_H__
#define __lo_CTYPES_H__

#ifdef COMPILER_MSVC
typedef __int64 int64;
#else
typedef long long int64;
#endif /* COMPILER_MSVC */
typedef long int32;
typedef short int16;
typedef char int8;

#ifdef COMPILER_MSVC
typedef unsigned __int64 uint64;
typedef __int64 int64;
#else
typedef unsigned long long uint64;
typedef long long int64;
#endif /* COMPILER_MSVC */

typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

//
#define MIN_(a, b) ((a) > (b)) ? (b) : (a)
#define MAX_(a,b)  ((a) < (b)) ? (b) : (a)

#if defined(UNICODE) || defined(_UNICODE)
typedef wchar_t  loTCHAR;
#define _loT(s)  L##s
#define _TCSNCPY wcsncpy
#define _TCSCHR  wcschr
#else
typedef char     loTCHAR;
#define _loT(s)  s
#define _TCSNCPY strncpy
#define _TCSCHR  strchr
#endif

#endif //__lo_CTYPES_H__

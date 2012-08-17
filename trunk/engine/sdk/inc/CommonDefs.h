#pragma once

//#if defined(_WIN32) && defined(_USRDLL)
//
//#if defined(COMMON_EXPORTS)
//
//#define COMMON_API __declspec(dllexport)
//#define COMMON_CAPI extern "C" __declspec(dllexport)
//
//#else
//
//#define COMMON_API __declspec(dllimport)
//#define COMMON_CAPI extern "C" __declspec(dllimport)
//
//#endif
//
//
//#else

#define COMMON_API
#define COMMON_CAPI

//#endif


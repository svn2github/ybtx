#pragma once
//本文件同时用于引擎内和引擎外

#if defined(_WIN32) && defined(_USRDLL)

#if defined(NETWORK_EXPORTS)

#define NETWORK_API __declspec(dllexport)
#define NETWORK_CAPI extern "C" __declspec(dllexport)

#else

#define NETWORK_API __declspec(dllimport)
#define NETWORK_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define NETWORK_API
#define NETWORK_CAPI

#endif


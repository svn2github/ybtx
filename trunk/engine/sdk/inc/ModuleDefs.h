#pragma once

#if defined(_WIN32) && defined(_USRDLL)

#if defined(MODULE_EXPORTS)

#define MODULE_API __declspec(dllexport)
#define MODULE_CAPI extern "C" __declspec(dllexport)

#else

#define MODULE_API __declspec(dllimport)
#define MODULE_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define MODULE_API
#define MODULE_CAPI

#endif


#pragma once

#if defined(_WIN32)

#if defined(EFFECTCONTAINER_EXPORTS)

#define EFFECTCONTAINER_API __declspec(dllexport)
#define EFFECTCONTAINER_CAPI extern "C" __declspec(dllexport)

#else

#define EFFECTCONTAINER_API __declspec(dllimport)
#define EFFECTCONTAINER_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define EFFECTCONTAINER_API
#define EFFECTCONTAINER_CAPI

#endif
#pragma once

#if defined(_WIN32) && defined(_USRDLL)

#if defined(EFFECT_EXPORTS)

#define EFFECT_API __declspec(dllexport)
#define EFFECT_CAPI extern "C" __declspec(dllexport)

#else

#define EFFECT_API __declspec(dllimport)
#define EFFECT_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define EFFECT_API
#define EFFECT_CAPI

#endif
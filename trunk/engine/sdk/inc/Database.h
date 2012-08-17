#pragma once

#if defined(_WIN32) && defined(_USRDLL)

#if defined(DATABASE_EXPORTS)

#define DATABASE_API __declspec(dllexport)
#define DATABASE_CAPI extern "C" __declspec(dllexport)

#else

#define DATABASE_API __declspec(dllimport)
#define DATABASE_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define DATABASE_API
#define DATABASE_CAPI

#endif


#pragma once

#if defined(_WIN32)

#if defined(CLIENT_EXPORTS)

#define CLIENT_API __declspec(dllexport)
#define CLIENT_CAPI extern "C" __declspec(dllexport)

#else

#define CLIENT_API __declspec(dllimport)
#define CLIENT_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define CLIENT_API
#define CLIENT_CAPI

#endif
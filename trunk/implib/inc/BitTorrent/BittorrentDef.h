
#pragma once

#if defined(_WIN32) && defined(_USRDLL)

#if defined(BITTORRENT_EXPORTS)

#define BITTORRENT_API __declspec(dllexport)
#define BITTORRENT_CAPI extern "C" __declspec(dllexport)

#else

#define BITTORRENT_API __declspec(dllimport)
#define BITTORRENT_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define BITTORRENT_API
#define BITTORRENT_CAPI

#endif

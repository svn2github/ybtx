#pragma once

#define _USRDLL
#define GAMEGAS_EXPORTS

#if defined(_WIN32) && defined(_USRDLL)

#if defined(GAMEGAS_EXPORTS)

#define GAMEGAS_API __declspec(dllexport)				//for C++
#define GAMEGAS_CAPI extern "C" __declspec(dllexport)			//for C

#else

#define GAMEGAS_API __declspec(dllimport)
#define GAMEGAS_CAPI extern "C" __declspec(dllimport)

#endif


#else

#define GAMEGAS_API 
#define GAMEGAS_CAPI

#endif





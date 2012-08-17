#pragma once

	#if defined(_USRDLL)

	#if defined(GRAPHIC_EXPORTS)

	#define GRAPHIC_API __declspec(dllexport)
	#define GRAPHIC_CAPI extern "C" __declspec(dllexport)

	#else

	#define GRAPHIC_API __declspec(dllimport)
	#define GRAPHIC_CAPI extern "C" __declspec(dllimport)

	#endif


#else

#define GRAPHIC_API
#define GRAPHIC_CAPI
#endif


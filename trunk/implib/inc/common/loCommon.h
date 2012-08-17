#ifdef COMMON_DLL

#if defined(WIN32) || defined(_WIN32) || defined(WINCE)

#ifdef COMMON_EXPORTS
#define loCOMMON_API __declspec(dllexport)
#else
#define loCOMMON_API __declspec(dllimport)
#endif

#else

#define loCOMMON_API

#endif //!defined(WIN32) || defined(_WIN32) || defined(WINCE)

#else

#define loCOMMON_API

#endif

#ifndef __INLINE_lo__
#define __INLINE_lo__
#endif

#ifndef INLINE_lo
#define INLINE_lo inline
#endif

#if defined(WIN32) || defined(_WIN32) || defined(WINCE)

#include <windows.h>
typedef HMODULE HMODULE_lo;

#else

typedef void* HMODULE_lo;

#endif //defined(WIN32) || defined(_WIN32)

#include <common/lo/lodefine.h>

DEFINE_NAMESPACE(locom)

BEGIN_EXTERN_C

/** 初始化 */
loCOMMON_API void gloInitializeCommon(void);

/** 反初始 */
loCOMMON_API void gloUnInitializeCommon(void);

END_EXTERN_C

END_NAMESPACE(locom)


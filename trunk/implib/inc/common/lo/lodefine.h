#ifndef  __lo_DEFINE_H__
#define  __lo_DEFINE_H__

#include <stddef.h>
#include <common/lo/loctypes.h>
/**
* @author loach
*
* @date 2009-06-08
* 
* 属于 loach建议的库所需要的一些宏定义
*/

#ifndef ReleaseM_lo
#define ReleaseM_lo(p) if(p) { (p)->Release(); (p) = NULL; }
#endif //ReleaseM_lo

#ifndef AddRefM_lo
#define AddRefM_lo(p) if(p) (p)->AddRef();
#endif //AddRefM_lo

#ifndef DeleteM_lo
#define DeleteM_lo(h) if ( h ) { delete (h); (h)= NULL;}
#define DeleteAM_lo(h) if ( h ) { delete[] (h); (h)= NULL;}
#endif //DeleteM_lo


#ifndef SafeFreeA_lo
#define SafeFreeA_lo(x) if(x){ ::free((void *)x); x = NULL; }
#endif

#ifndef DelHandleM_lo

#ifdef WIN32
#define DelHandleM_lo(h) 	if ( h ) { ::DeleteObject(h); h = NULL ; }
#else
#define DelHandleM_lo(h) 	
#endif

#endif //DelHandleM_lo

#ifndef DEFINE_NAMESPACE
#define DEFINE_NAMESPACE(name) namespace name{
#define END_NAMESPACE(name) }
#define USE_NAMESPACE(name) using namespace name;
#endif

/// 虚函数表宏定义
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#ifndef loNovtableM
#define loNovtableM __declspec(novtable)
#endif
#else
#define loNovtableM
#endif

/** 主要用在查询，不显露出真实的对象 */
struct t_loPOSITION {};
typedef t_loPOSITION* POSITION_l;

#define REINTERPRET_CAST(cls) reinterpret_cast<cls>
#define LPVOID_CAST(p) ( (void*)p )

/** dump 函数 */
#if defined(_DEBUG) && defined(USE_VL_DUMP)
#define DEBUG_DUMP() void Dump()
#define CALL_DEBUG_DUMP() Dump()
#else
#define DEBUG_DUMP()
#define CALL_DEBUG_DUMP()
#endif

// extern "C" 
#ifdef __cplusplus

#ifndef BEGIN_EXTERN_C

#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }

#endif /* BEGIN_EXTERN_C */

#else

#define BEGIN_EXTERN_C 
#define END_EXTERN_C

#endif /* !__cplusplus */

#ifndef EXTERN
#define EXTERN extern
#endif

#if (defined(WIN32) )|| ( defined(_WIN32) )

#define EXPORTM_LO(__type)     extern __declspec(dllexport) __type
#define EXPORTDATAM_LO(__type) extern __declspec(dllexport) __type
#define IMPORTM_LO(__type)            __declspec(dllimport) __type
#define IMPORTDATAM_LO(__type)        __declspec(dllimport) __type

#define EXTERN_EXPORTM_LO(__type)        extern __declspec(dllexport) __type
#define EXTERN_EXPORTDATAM_LO(__type)    extern __declspec(dllexport) __type
#define IMPLEMENT_EXPORTM_LO(__type)            __declspec(dllexport) __type
#define IMPLEMENT_EXPORTDATAM_LO(__type)        __declspec(dllexport) __type

#else

// add by welbon ,in UNIX platform define

#define far
#define near

#ifndef VOID
#define VOID void

#undef FAR
#undef  NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif


#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef MAX_PATH
#define MAX_PATH            255
#endif

#ifndef _T(x)

#ifdef UNICODE
#define _T(x)        Lx
#else
#define _T(x)         x
#endif 

#endif

typedef unsigned long		DWORD;
typedef int                	BOOL;
typedef unsigned char		BYTE;
typedef unsigned short	WORD;
typedef float			FLOAT;
typedef FLOAT               	*PFLOAT;
typedef BOOL near           	*PBOOL;
typedef BOOL far            	*LPBOOL;
typedef BYTE near           	*PBYTE;
typedef BYTE far            	*LPBYTE;
typedef int near            	*PINT;
typedef int far             	*LPINT;
typedef WORD near           	*PWORD;
typedef WORD far            	*LPWORD;
typedef long far            	*LPLONG;
typedef DWORD near          	*PDWORD;
typedef DWORD far           	*LPDWORD;
typedef void far            	*LPVOID;
typedef CONST void far      	*LPCVOID;

typedef int                  	INT;
typedef unsigned int        	UINT;
typedef unsigned int        	*PUINT;

typedef long			__int32;
typedef long long		__int64;


#endif //VOID

typedef char   CHAR;
typedef short SHORT;
typedef long   LONG;
typedef int     BOOL;

// tchar 
typedef CHAR   TCHAR, *PTCHAR, *LPSTR;
typedef CONST  TCHAR  *LPCTSTR,*LPCSTR;

typedef wchar_t WCHAR;
typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef CONST  WCHAR *LPCWSTR, *PCWSTR;

//
typedef double LONGLONG;
typedef double ULONGLONG;


// handle
typedef void *HANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
typedef HANDLE *PHANDLE;
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;      /* HMODULEs can be used in place of HINSTANCE */
typedef long HRESULT;
typedef unsigned long ULONG;
#define STDMETHODCALLTYPE       __stdcall


//--------------------------------------------------------------------------------------//
// error code 
#define _HRESULT_TYPEDEF_(sc) sc

#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                            ((HRESULT)0x00000001L)

//
// MessageId: E_POINTER
//
// MessageText:
//
//  Invalid pointer
//
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)

//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
//  No such interface supported
//
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)
//--------------------------------------------------------------------------------------//

#define STDMETHODCALLTYPE 

#endif // (defined(WIN32) )|| ( defined(_WIN32) )


#endif /* __lo_DEFINE_H__*/
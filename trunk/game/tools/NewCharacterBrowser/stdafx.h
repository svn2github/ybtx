// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

// TODO: reference additional headers your program requires here

#define  GTR_UP		1
#define  GTR_LEFT	2
#define  GTR_DOWN	3
#define  GTR_RIGHT	4
#define  GTR_HIGH	5
#define  GTR_LOW	6

#define _WINSOCKAPI_
#include "Base.h"
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "BaseTypes.h"
namespace sqr_tools{}
using namespace std;
using namespace sqr_tools;
using namespace sqr;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
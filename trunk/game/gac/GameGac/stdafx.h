// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "Base.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <stack>
#include <set>

#if defined(_WIN32) || defined(_USE_STLPORT)
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

using namespace std;
using namespace sqr;

// TODO: reference additional headers your program requires here

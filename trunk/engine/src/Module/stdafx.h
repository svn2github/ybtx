#if !defined _STDAFX_H
#define _STDAFX_H

#include "Base.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <deque>
#include <vector>
#include <list>

#include "BaseTypes.h"
using namespace std;
using namespace sqr;

#ifdef _WIN32

#include <hash_set>
using namespace stdext;

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0403
#include <windows.h>
#include <dbghelp.h>

#else

#include <ext/hash_set>
#include <execinfo.h>
#include <dirent.h>

#endif



#endif


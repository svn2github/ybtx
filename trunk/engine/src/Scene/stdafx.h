// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include "Base.h"

//#pragma warning(disable:4996)


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include <cmath>
#include <sstream>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <string>

#ifdef _WIN32
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
#endif

#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>


using namespace std;
using namespace CPPUNIT_NS;
using namespace sqr;

// TODO: reference additional headers your program requires here

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <tchar.h>

#endif

// TODO: reference additional headers your program requires here

#include <string>
#include <iostream>
#include <set>
#include <map>
#include <list>
#include <vector>

using namespace std;

#include <stdio.h>

#include "BaseTypes.h"
#include "BaseHelper.h"

using namespace sqr;

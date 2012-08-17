// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include "Base.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <dbghelp.h>
#include "ExpHelper.h"
#include "CPathMgr.h"
using namespace std;
using namespace sqr;

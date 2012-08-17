// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						
#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <direct.h>
#include "BaseDefs.h"
#include "BaseTypes.h"
#include "ExpHelper.h"
#include "CodeCvs.h"
#include "CPathMgr.h"
#include <windows.h>
#include <dbghelp.h>
#include "tinyxml.h"
using namespace std;
using namespace sqr;


// TODO: reference additional headers your program requires here

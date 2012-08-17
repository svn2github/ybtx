// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <stdio.h>
#include <string.h>


#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;


#include "CScript.h"
#include "CPathMgr.h"
#include "BaseDefs.h"

// TODO: reference additional headers your program requires here

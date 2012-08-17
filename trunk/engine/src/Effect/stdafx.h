// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "Base.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

using namespace std;
using namespace sqr;

#include "resource.h"



std::string LoadResString( WORD ID );

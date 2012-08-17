// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#ifdef _WIN32
//#include <windows.h>
//#endif
#include "Base.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif

#ifndef _WIN32
#define _chdir chdir
#define _mkdir mkdir
#define _getcwd getcwd
#endif

#include "CPkgFile.h"

#include "PkgStream.h"

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>
//#include <basetsd.h>
//#include <wtypes.h>

using namespace std;
using namespace sqr;


// TODO: reference additional headers your program requires here


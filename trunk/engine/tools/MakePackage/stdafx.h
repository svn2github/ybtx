// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "Base.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif

#include "BaseTypes.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>

using namespace std;
using namespace sqr;


#define  TCHAR  char

#ifndef _WIN32
#define _chdir	chdir
#endif

// TODO: reference additional headers your program requires here


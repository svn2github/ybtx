// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#include "Base.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#include <windows.h>
#endif

#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <vector>
#include <queue>
#include <list>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

#ifdef _WIN32
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

using namespace CPPUNIT_NS;
using namespace std;
using namespace sqr;


#define  NORMALSIZE  (sizeof(CNode) << 8)


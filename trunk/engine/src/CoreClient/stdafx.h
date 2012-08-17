// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#include "Base.h"

#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x0500
//#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#include <windows.h>

#include <cmath>
#include <sstream>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <string>

//#pragma warning(disable:4996)

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


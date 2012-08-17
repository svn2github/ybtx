// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#include "Base.h"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iostream>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Network.h"

#ifdef _WIN32
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

using namespace std;
using namespace sqr;
using namespace CPPUNIT_NS;


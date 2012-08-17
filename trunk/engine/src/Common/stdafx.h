#if !defined _STDAFX_H
#define _STDAFX_H

#include "Base.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <vector>
#include <stack>
#include <list>
#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

using namespace CPPUNIT_NS;
using namespace std;
using namespace sqr;

#ifdef _WIN32
#include <hash_map>
#include <hash_set>
using namespace stdext;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <dbghelp.h>

#else
#include <ext/hash_map>
#include <ext/hash_set>
using namespace __gnu_cxx;
#endif



#endif

/*
common库现在只用于game和一些工具使用，如果大家要添加基础类库，这个库
被module或者runmodule以及game都需要的话，那就把要添加的代码放到Base库里面
common库由于现在只是由game或者工具加载，所以允许出现单件形式
*/

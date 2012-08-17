// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#include "Base.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>
#include <cppunit/TestRunner.h>

using namespace CPPUNIT_NS;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <string>


namespace sqr
{
	class CError;
}


using namespace std;
using namespace sqr;


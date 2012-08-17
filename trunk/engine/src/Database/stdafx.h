// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#if !defined _MYSQL_STDAFX_H
#define _MYSQL_STDAFX_H

#include "Base.h"

#ifdef _WIN32
#include <WinSock2.h>
#endif


#include "mysql/mysql.h"

#include <iostream>
#include <list>
#include <stdexcept>
using namespace std;
using namespace sqr;

// TODO: 在此处引用程序要求的附加头文件
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

using namespace CPPUNIT_NS;

#endif



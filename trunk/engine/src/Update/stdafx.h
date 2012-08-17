// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "BaseTypes.h"

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#include <cstdlib>
#include <cstdio>

// TODO: reference additional headers your program requires here
//#include <windows.h>

//#include <sstream>
//#include <iostream>
//#include <strstream>//%%%%%
//#include <Winsock2.h>//%%%%%
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace std;
using namespace sqr;
using namespace CPPUNIT_NS;


#define NAMESPACE_BEGIN(name) \
namespace name \
{

#define NAMESPACE_END \
}


#define  MAX_PATH_SIZE  4096
#define  BUFFER_SIZE    4096


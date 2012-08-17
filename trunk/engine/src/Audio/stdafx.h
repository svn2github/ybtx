// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM
#include "Base.h"

// Windows Header Files:
#include <windows.h>
#include <MMSystem.h>


#include <BaseDefs.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include "BaseHelper.h"
#include "MathAlgorithm.h"
#include "ExpHelper.h"

using namespace std;
using namespace sqr;
using namespace CPPUNIT_NS;
 
// TODO: reference additional headers your program requires here
// XACT
#include <xact3.h>
#include <xact3d3.h>

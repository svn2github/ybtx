//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#ifndef __BLUELY_COMMON__
#define __BLUELY_COMMON__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <list>
#include <vector>
#include <string>
#include <exception>
#include <stdio.h>
#include <mmsystem.h>

#if defined(_DEBUG) && defined(_WIN32) && !defined(DEBUG_NEW)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK,THIS_FILE,__LINE__)
#endif

#pragma comment(lib,"winmm.lib")

#define BLUELY_COMMON_VERSION L"0.01"

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(p)       { if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete[] (p);   (p)=NULL; } }
#endif

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(p)      { if((p) != NULL) { (p)->Release(); (p)=NULL; } }
#endif

namespace bc
{
	namespace ut
	{
	}

	namespace io
	{
	}
}

#endif
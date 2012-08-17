//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#ifndef __BLUELY_GRAPHICS__
#define __BLUELY_GRAPHICS__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include "SmartPtr.h"
#include "RootObject.h"

#if defined(_DEBUG) && defined(_WIN32) && !defined(DEBUG_NEW)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK,THIS_FILE,__LINE__)
#endif

//using namespace std;

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")

#define BLUELY_GRAPHICS_VERSION L"0.01"
#define INVALID -1

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(p)       { if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete[] (p);   (p)=NULL; } }
#endif

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(p)      { if((p) != NULL) { (p)->Release(); (p)=NULL; } }
#endif

#if !defined(SAFE_DESTROY)
#define SAFE_DESTROY(p)      { if((p) != NULL) { (p)->Destroy(); delete (p); (p)=NULL;}}
#endif

#define GRID_BATCH_SIZE 500
#define TRIANGLE_BATCH_SIZE (GRID_BATCH_SIZE*2)
#define VERTEX_BATCH_SIZE (TRIANGLE_BATCH_SIZE*3)
#define STDFOV 27.0f
#define STDYAW 45
#define STDPITCH 45
#define STDDISTANCE 3400.0f
#define LOCAL_VERSION 0x02
#define CLIENT_VERSION 0x01
#define SERVER_VERSION 0x01
#define SPACE_GRANULARITY (4.0f)

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

namespace bg
{
	namespace cl
	{
		const DWORD GRIDHOT = 0xd0777777;
		const DWORD SEMI_BLACK = 0xd0000000;
		const DWORD BLACK = 0xff000000;
	}
	
	namespace ui
	{
    }

	namespace eg
	{
	}

	namespace rd
	{
	}

	namespace ip
	{
	}
}

namespace sqr{}

using namespace sqr;
#endif
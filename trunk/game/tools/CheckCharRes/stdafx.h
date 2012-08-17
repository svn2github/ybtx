// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>
#include <windows.h>


#include <list>
#include <vector>
#include <map>
#include <cassert>
#include <string>
#include <iostream>
#include <set>
#include <fstream>

using namespace std;
#include "Base.h"
#include "BaseTypes.h"
#include "CScript.h"
#include "direct.h"
//#pragma 
using namespace sqr;

extern CScript*	gs_pScript;
extern string g_ResPath;
extern string g_SettingCfgPath;
CScript*	GetScript();
void	Init();
//int32	InterfaceExport();
void	RunLuaFile(const TCHAR* szContext);

// TODO: reference additional headers your program requires here

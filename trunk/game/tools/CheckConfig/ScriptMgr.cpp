#include "stdafx.h"
#include "CPathMgr.h"
#include <direct.h>
#include "ExpHelper.h"
#include <fstream>
#include "Console.h"
#include "BaseHelper.h"
#include "ScriptX.h"
#include "RegistGasShell.h"
#include "RegistShellCommon.h"
#include "RegistGacShell.h"
#include "PkgStream.h"
#include "TSqrAllocator.inl"
#include "CScriptMgr.h"
#include "CScriptAppServer.h"

#define TOOLWORKDIR				

CScriptMgr::CScriptMgr()
{
#ifndef TOOLWORKDIR
	_chdir("../");
#endif
	m_ScriptApp = new CScriptAppServer("etc/gas/GasConfig.xml","etc/gas/","1");
	ConfigurePath();
	RegistShell();
	//Run();
}

CScriptMgr::~CScriptMgr()
{
	delete m_ScriptApp;
}

CScript* CScriptMgr::GetScript()
{
	if(m_ScriptApp)
		return m_ScriptApp->GetMainVM();
	return NULL;
}

void CScriptMgr::RegistShell()
{
	CScript* pScript = GetScript();
	RegistShellCommon(*pScript);
	RegistGasShell(*pScript);
	RegistGacShell(*pScript);
}

void CScriptMgr::Run()
{
	CScript* pScript = GetScript();
#ifndef TOOLWORKDIR
	pScript->RunString("require \"game/tools/CheckConfig/lua/LoadCheckModule\"",NULL);
#endif
#ifdef TOOLWORKDIR
	pScript->RunString("require \"lua/LoadCheckModule\"",NULL); 
#endif
}

void CScriptMgr::ConfigurePath()
{
#ifndef TOOLWORKDIR
	_chdir("../../programmer/trunk/");
#endif
	ShowConsole(true);
}

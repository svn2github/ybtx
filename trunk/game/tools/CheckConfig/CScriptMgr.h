#pragma once
#include "stdafx.h"

namespace sqr
{
	class CScript;
	class CScriptAppServer;
}

class CScriptMgr
{
public:
	CScriptMgr();
	~CScriptMgr();
	CScript* GetScript();
	void RegistShell();
	void Run();
	void ConfigurePath();

private:
	CScriptAppServer* m_ScriptApp;
};
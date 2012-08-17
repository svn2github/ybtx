#pragma once

namespace sqr
{
	class CScript;
}
class CDataCache;

extern CScript*	gs_pScript;
extern CDataCache* gs_pCahce;
CScript*	GetScript();
CDataCache*	GetCCache();	
void	Init();
void	Release();
int32	InterfaceExport();
void	RunLuaFile(const TCHAR* szContext);
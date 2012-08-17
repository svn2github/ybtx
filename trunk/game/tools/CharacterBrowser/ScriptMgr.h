#pragma once
namespace sqr
{
	class CScript;
}
class CDataCache;
using namespace sqr;
extern CScript*	gs_pScript;
extern CDataCache* gs_pCahce;
CScript*	GetScript();
CDataCache*	GetCCache();	
void	Init();
int32	InterfaceExport();
void	RunLuaFile(const TCHAR* szContext);
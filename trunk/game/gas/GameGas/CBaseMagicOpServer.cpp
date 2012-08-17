#include "stdafx.h"
#include "CBaseMagicOpServer.h"
#include "CCfgArg.h"

CMagicOpCalcArg* CBaseMagicOpServer::ms_pMagicOpCalcArg = NULL; 

CCfgArg* CBaseMagicOpServer::InitArg(const string& szArg)
{
	if (szArg == "")
	{
		if (!ms_pMagicOpCalcArg)
		{
			ms_pMagicOpCalcArg = new CMagicOpCalcArg();
		}
		return NULL;
	}
	CMagicOpCalcArg* pCfgArg = new CMagicOpCalcArg(szArg);
	return pCfgArg;
}

void CBaseMagicOpServer::Release()
{
	if (ms_pMagicOpCalcArg)
	{
		delete ms_pMagicOpCalcArg;
		ms_pMagicOpCalcArg = NULL;
	}
}

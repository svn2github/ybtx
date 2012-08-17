#include "stdafx.h"
#include "CCfgServantDataCheck.h"
#include "CTxtTableFile.h"

bool CCfgServantDataCheck::Check(const TCHAR* cfgFile)
{
	CTxtTableFile TableFile;
	Load(TableFile, cfgFile, "ServantProperty_Server");
	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		CheckOverlap("Name", i);

		//TODO:ÆäËû¼ì²â
	}
	return true;
}

void CCfgServantDataCheck::EndCheck()
{
	EndCheckOverlap();
}

#include "stdafx.h"
#include "CNpcResServerData.h"
#include "CAniKeyFrameCfg.h"


bool CNpcResServerData::CheckNpcAniKeyFrameCfg()
{
	bool bSuccess = CAniKeyFrameCfg::CheckNpcMapAniKeyFrameCfg();
	return bSuccess;
}

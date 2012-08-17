#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "ScriptX.h"
#include "TSqrAllocator.inl"

extern "C"
{
	#include "tg/thinkgear.h"
}
#ifdef _WIN32
#include "TGManager.h"


int32 CBaseScriptApp::RegisterThink(CScript& Script)
{
	//------------------------TGManager----------------------
	REGIST_CONSTANT( Script, TG_DATA_BATTERY);
	REGIST_CONSTANT( Script, TG_DATA_POOR_SIGNAL);
	REGIST_CONSTANT( Script, TG_DATA_ATTENTION);
	REGIST_CONSTANT( Script, TG_DATA_MEDITATION);
	REGIST_CONSTANT( Script, TG_DATA_RAW);
	REGIST_CONSTANT( Script, TG_DATA_DELTA);
	REGIST_CONSTANT( Script, TG_DATA_THETA);
	REGIST_CONSTANT( Script, TG_DATA_ALPHA1);
	REGIST_CONSTANT( Script, TG_DATA_ALPHA2);
	REGIST_CONSTANT( Script, TG_DATA_BETA1);
	REGIST_CONSTANT( Script, TG_DATA_BETA2);
	REGIST_CONSTANT( Script, TG_DATA_GAMMA1);
	REGIST_CONSTANT( Script, TG_DATA_GAMMA2);

	REGIST_B_CLASS( Script, TGManager );
	//REGIST_CLASSFUNCTION( Script, TGManager, StartThinkgear, "ii:b" );
	//REGIST_CLASSFUNCTION( Script, TGManager, EndThinkgear, ":b" );
	REGIST_CLASSFUNCTION( Script, TGManager, GetThinkgearData, "i:f" );
	//REGIST_CLASSFUNCTION( Script, TGManager, GetThinkgearDataFiltered, "i:f" );

	REGIST_CLASSFUNCTION( Script, TGManager, SetThinkgearBaudRate, "i:i" );
	REGIST_CLASSFUNCTION( Script, TGManager, SetThinkgearDataFormat, "i:i" );

	return 1;
}

#endif

#include "stdafx.h"
#include "RegistGasShell.h"
#include "ScriptX.h"
#include "CServerCheck.h"
#include "NpcCheckInfo.h"
#include "CRecordHelp.h"
#include "CheckSkillCfg.h"
#include "CheckNpcCfg.h"
#include "CheckBloodCfg.h"
#include "TSqrAllocator.inl"
#include "CCfgNpcServerBaseData.h"
#include "CCfgBaseCheck.h"
#include "CNpcEnmityTargetFilterDataCheck.h"
#include "CCfgNPCAIBaseDataCheck.h"
#include "CCfgNPCFighterBaseDataCheck.h"

void RegistGasShell(CScript& Script)
{
	REGIST_GLOBALFUNCTION( Script, ServerCheckCfg,":b" );

	REGIST_GLOBALFUNCTION( Script, CheckSkillCfg, ":b" );
	REGIST_GLOBALFUNCTION( Script, EndCheckSkillCfg, "" );
	REGIST_GLOBALFUNCTION( Script, CheckNpcCfg,":b" );
	REGIST_GLOBALFUNCTION( Script, EndCheckNpcCfg,"" );
	REGIST_GLOBALFUNCTION( Script, CppCheckNpcResCommon, "sfbs" );
	REGIST_GLOBALFUNCTION( Script, InsertTalentName, "s" );
	REGIST_GLOBALFUNCTION( Script, IsIntObjSkillNameValid, "s:b" );
	REGIST_GLOBALFUNCTION( Script, IsStateNameValid, "s:b" );
	REGIST_GLOBALFUNCTION( Script, IsPlayerSkillNameValid, "s:b" );
	REGIST_GLOBALFUNCTION( Script, IsNPCSkillNameValid, "s:b" );

	REGIST_GLOBALFUNCTION( Script, BeNeededToRun,"s:b" );
	REGIST_GLOBALFUNCTION( Script, UpdateRecord,"sb" );
	REGIST_GLOBALFUNCTION( Script, CheckBloodCfg,":b" );
	REGIST_GLOBALFUNCTION( Script, EndCheckBloodCfg,"" );

	REGIST_ABSTRACT_B_CLASS(Script, CCfgNpcServerBaseData);
	REGIST_STATICFUNCTION( Script, CCfgNpcServerBaseData, Create, "ssssIIIII");

	REGIST_ABSTRACT_B_CLASS( Script, CCfgBaseCheck);
	REGIST_ABSTRACT_D_CLASS( Script, CNpcEnmityTargetFilterDataCheck, CCfgBaseCheck);
	REGIST_STATICFUNCTION(Script, CNpcEnmityTargetFilterDataCheck, Check, ":b");
	REGIST_ABSTRACT_D_CLASS( Script, CCfgNPCAIBaseDataCheck, CCfgBaseCheck);
	REGIST_STATICFUNCTION(Script, CCfgNPCAIBaseDataCheck, GetEyeSizeByName, "s:I");
	REGIST_ABSTRACT_D_CLASS( Script, CCfgNPCFighterBaseDataCheck, CCfgBaseCheck);
	REGIST_STATICFUNCTION(Script, CCfgNPCFighterBaseDataCheck, GetAttackDistByName, "s:I");
}

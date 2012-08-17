#include "stdafx.h"
#include "RegistGacShell.h"
#include "ScriptX.h"
#include "CCustomerCheck.h"
#include "TSqrAllocator.inl"

void RegistGacShell(CScript& Script)
{
	REGIST_GLOBALFUNCTION( Script, CustomerCheckCfg,":b" );
}

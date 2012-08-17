#pragma once

#include "CStaticObject.h"
#include "CCfgBaseCheck.h"

class CNpcEnmityTargetFilterDataCheck
	: public CCfgBaseCheck
{
	typedef set<string>		SetAllNpc;
public:
	CNpcEnmityTargetFilterDataCheck();
	~CNpcEnmityTargetFilterDataCheck();

	static bool Check();

private:
	static void GetNpcArgs(SetAllNpc& lstNpcArgs, string& strNpcArgs);
};
#pragma once
#include "CConfigMallocObject.h"

class CCfgCalc;

class CCfgArg
	:public CConfigMallocObject
{
public:
	CCfgArg(){}
	virtual ~CCfgArg(){}
};

class CMagicOpCalcArg
	:public CCfgArg
{
public:
	CMagicOpCalcArg();
	CMagicOpCalcArg(const string& szArg);
	~CMagicOpCalcArg();
public:
	CCfgCalc*		m_pArg;
};

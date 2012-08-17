#include "stdafx.h"
#include "CCfgArg.h"
#include "CCfgCalc.inl"

CMagicOpCalcArg::CMagicOpCalcArg()
{
	m_pArg = new CCfgCalc();
	m_pArg->InputString("");
}

CMagicOpCalcArg::CMagicOpCalcArg(const string& szArg)
{
	m_pArg = new CCfgCalc();
	m_pArg->InputString(szArg);
}

CMagicOpCalcArg::~CMagicOpCalcArg()
{
	delete m_pArg;
}

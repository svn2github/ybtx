#include "stdafx.h"
#include "CScriptSerial.h"
#include "CScriptSerialPacker.h"
#include "CScriptSerialUnPacker.h"

namespace sqr
{
	class CScriptSerialData : public CScriptMallocObject
	{
	public:
		CScriptSerialPackerMgr		m_pPackerMgr;
		CScriptSerialUnPackerMgr	m_pUnPackerMgr;
	};
}

CScriptSerial::CScriptSerial()
{
	m_pData = new CScriptSerialData;
}

CScriptSerial::~CScriptSerial()
{
	delete m_pData;
}


void CScriptSerial::RegPackFun(CScript& Script, const string& strFunName, 
							   POST_PACKFUNCTION pPostPackFun)
{
	m_pData->m_pPackerMgr.RegPackFun(Script, strFunName, pPostPackFun);
}

void CScriptSerial::RegUnPackFun(CScript& Script, ON_UNPACKFUNCTION* pOnUnPackFun)
{
	m_pData->m_pUnPackerMgr.RegUnPackFun(Script, pOnUnPackFun);
}


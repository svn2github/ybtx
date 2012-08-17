#pragma once

#include "TConnection.h"

template<typename Traits>
template<typename CmdClass>
void TConnection<Traits>::SendCoreCmd(const CmdClass* pCmd)
{
	//cout << (uint32)(m_pPipe) << "\t" << typeid(pCmd).name() << "\t" << sizeof(CmdClass) << endl;
	SendCoreCmd(reinterpret_cast<const void*>(pCmd),sizeof(CmdClass));
}

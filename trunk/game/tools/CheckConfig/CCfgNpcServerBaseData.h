#pragma once

#include "CStaticObject.h"

class CCfgNpcServerBaseData
	: public virtual CStaticObject
{
public:
	static void Create(const TCHAR* szName, const TCHAR* szNpcType, const TCHAR* szAIType, 
		const TCHAR* szAIData, uint32 uRace, uint32 uCamp, uint32 uClass, uint32 uSex, uint32 uLevel);

	static bool FindNpc(string sNpcName);

	static set<string> m_setCfgNpcBaseData;
};
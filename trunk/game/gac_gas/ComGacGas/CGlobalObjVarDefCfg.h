#pragma once

#include <map>
#include "CObjVarDefCfg.h"
#include "TSingleton.h"

class CGlobalObjVarDefCfg 
	: public TSingleton<CGlobalObjVarDefCfg>
{
	friend class TSingleton<CGlobalObjVarDefCfg>;
	CGlobalObjVarDefCfg();
	public:
	uint32 GetIDByName(const TCHAR* szName);
	bool AddName2ID(const TCHAR* szName, uint32 uID);
	inline void SetCfg(CObjVarDefCfg* pCfg) { m_pCfg = pCfg; }
	uint32 Get(const TCHAR* szFileName) { return m_pCfg->Get(szFileName); }

	private:
	std::map<std::string, uint32> mapName2ID;
	CObjVarDefCfg* m_pCfg;
};

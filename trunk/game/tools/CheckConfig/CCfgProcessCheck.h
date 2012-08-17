#pragma once
#include "CCfgMagicEffCheck.h"

class CCfgProcessCheck
{
public:
	CCfgProcessCheck();
	~CCfgProcessCheck();
	static bool	Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckExist(const string& strName);

	string GetName() { return m_strName; }
	CCfgMagicEffCheck* GetMagicEff()			{ return m_pMagicEff; }
	CCfgMagicEffCheck* GetSelfCancelEff()		{ return m_pSelfCancelEff; }
	CCfgMagicEffCheck* GetObjCancelEff()		{ return m_pObjCancelEff; }
	bool CheckRelationAvail();

	typedef map<string, CCfgProcessCheck*> MapProcess;
	static MapProcess			ms_mapProcess;

private:
	string				m_strName;
	CCfgMagicEffCheck*	m_pMagicEff;
	CCfgMagicEffCheck*	m_pSelfCancelEff;
	CCfgMagicEffCheck*	m_pObjCancelEff;
};

#pragma once
#include "CCfgMagicEffCheck.h"

enum EBaseStateType
{
	eBST_State,
	eBST_Trigger,
	eBST_Damage,
	eBST_CumuliTrigger,
	eBST_Special,
};

class CCfgAllStateCheck
{
public:
	CCfgAllStateCheck();
	~CCfgAllStateCheck();
	static void EndCheck();
	static bool ExistDecreaseType(const string& sTypeName);
	static bool CheckExistAllState(const string& name);
	static CCfgAllStateCheck* GetStateByName(const string& name);
	static bool IsCancelEffOfState(CCfgMagicEffCheck* pCancelEff);

	string GetName() { return m_strName; }
	EBaseStateType GetType() { return m_eType; }
	bool GetPersistent() { return m_bPersistent; }
	bool GetNeedSaveToDB() { return m_bNeedSaveToDB; }
	CCfgMagicEffCheck* GetCancelEff()	{ return m_pCancelEff; }
	CCfgMagicEffCheck* GetDotEff()		{ return m_pDotEff; }
	CCfgMagicEffCheck* GetFinalEff()		{ return m_pFinalEff; }
	string GetCascadeType() { return m_strCascadeType; }
	string GetCascadeMax() { return m_strCascadeMax; }
	bool CheckRelationAvail();

	typedef map<string, CCfgAllStateCheck*>	MapState;
	static MapState				ms_mapState;

	string				m_strName;
	EBaseStateType		m_eType;
	bool				m_bPersistent;
	bool				m_bNeedSaveToDB;
	CCfgMagicEffCheck*	m_pCancelEff;
	CCfgMagicEffCheck*	m_pDotEff;
	CCfgMagicEffCheck*	m_pFinalEff;
	string				m_strCascadeType;
	string				m_strCascadeMax;
};

#define DefineCfgStateCheckClass(Name) \
class Name \
{ \
public: \
	static bool Check(const TCHAR* cfgFile); \
	static void	EndCheck(); \
	static bool CheckExist(const string& strName); \
	typedef set<string>	SetRowName; \
	static SetRowName	ms_setRowName; \
}; 

DefineCfgStateCheckClass(CCfgMagicStateCheck);
DefineCfgStateCheckClass(CCfgSpecialStateCheck);
DefineCfgStateCheckClass(CCfgTriggerStateCheck);
DefineCfgStateCheckClass(CCfgDamageChangeStateCheck);
DefineCfgStateCheckClass(CCfgCumuliTriggerStateCheck);

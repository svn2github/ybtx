#pragma once
#include "FightDef.h"
#include "CPos.h"
#include "TConfigAllocator.h"

class CFighterDictator;

class CTriggerEvent
{
	friend class CTriggerStateCfgServer;
	friend class CDamageChangeStateCfgServer;
	friend class CCumuliTriggerStateCfgServer;

public:
	static void MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, CFighterDictator* pTo, EHurtResult eArg1, bool bArg2 = false, ESkillType eArg3 = eST_None, EAttackType eArg4 = eATT_None);
	static void MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, const CFPos& pTo, EHurtResult eArg1, bool bArg2 = false, ESkillType eArg3 = eST_None, EAttackType eArg4 = eATT_None);
	static void MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, CFighterDictator* pTo, ESpecialEventType eArg1, bool bArg2 = false);
	static void MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, const CFPos& pTo, ESpecialEventType eArg1, bool bArg2 = false);
	static void MessageEvent(ETriggerStateType eType, uint32 uFromID, uint32 uToID, EHurtResult eArg1, bool bArg2 = false, ESkillType eArg3 = eST_None, EAttackType eArg4 = eATT_None);
	static void MessageEvent(ETriggerStateType eType, uint32 uFromID, const CFPos& pTo, EHurtResult eArg1, bool bArg2 = false, ESkillType eArg3 = eST_None, EAttackType eArg4 = eATT_None);
	static void MessageEvent(ETriggerStateType eType, uint32 uFromID, uint32 uToID, ESpecialEventType eArg1, bool bArg2 = false);
	static void MessageEvent(ETriggerStateType eType, uint32 uFromID, const CFPos& pTo, ESpecialEventType eArg1, bool bArg2 = false);

	static uint32	GetID(EHurtResult eArg1, bool bArg2, ESkillType eArg3, EAttackType eArg4);
	static uint32	GetID(ESpecialEventType eArg1, bool bArg2);

	static pair<bool, EHurtResult>				GetEHurtResult(const string& szName)
	{
		MapHurtResult::const_iterator itr = m_smapEHurtResult.find(szName);
		if (itr != m_smapEHurtResult.end())
		{
			return make_pair(true, itr->second);
		}
		else
		{
			return make_pair(false, EHurtResult(0));
		}
	}


	static ESkillType				GetESkillType(const string& szName)
	{
		MapSkillType::const_iterator itr = m_smapESkillType.find(szName);
		if (itr != m_smapESkillType.end())
		{
			return itr->second;
		}
		else
		{
			return ESkillType(0);
		}
	}

	static EAttackType				GetEAttackType(const string& szName)
	{
		MapAttackType::const_iterator itr = m_smapAttackType.find(szName);
		if (itr != m_smapAttackType.end())
		{
			return itr->second;
		}
		else
		{
			return EAttackType(0);
		}
	}

	static ESpecialEventType		GetESpecialEventType(const string& szName)
	{
		MapSpecialEventType::const_iterator itr = m_smapESpecialEventType.find(szName);
		if (itr != m_smapESpecialEventType.end())
		{
			return itr->second;
		}
		else
		{
			return ESpecialEventType(0);
		}
	}


private:

	static bool		BuildMap();
	static void		AddEvent(const string& strKey, EHurtResult eValue)			{m_smapEHurtResult.insert(make_pair(strKey, eValue));}
	static void		AddEvent(const string& strKey, ESkillType eValue)			{m_smapESkillType.insert(make_pair(strKey, eValue));}
	static void		AddEvent(const string& strKey, EAttackType eValue)			{m_smapAttackType.insert(make_pair(strKey, eValue));}
	static void		AddEvent(const string& strKey, ESpecialEventType eValue)	{m_smapESpecialEventType.insert(make_pair(strKey, eValue));}

	typedef map<string, EHurtResult, less<string>, TConfigAllocator<pair<string, EHurtResult > > > MapHurtResult;
	typedef map<string, ESkillType, less<string>, TConfigAllocator<pair<string, ESkillType > > > MapSkillType;
	typedef map<string, EAttackType, less<string>, TConfigAllocator<pair<string, EAttackType > > > MapAttackType;
	typedef map<string, ESpecialEventType, less<string>, TConfigAllocator<pair<string, ESpecialEventType > > > MapSpecialEventType;

	static MapHurtResult		m_smapEHurtResult;
	static MapSkillType			m_smapESkillType;
	static MapAttackType		m_smapAttackType;
	static MapSpecialEventType	m_smapESpecialEventType;

	static bool		__init;
};


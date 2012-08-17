#pragma once
#include "INpcEventHandler.h"
#include "CNpcEventMsg.h"
#include "CDistortedTick.h"
#include "PatternRef.h"
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"

enum ENpcPersonality
{
	eNP_Attract = 1,		//Œ¸“˝
	eNP_Like,				//œ≤∫√
	eNP_Abhor,				//—·∂Ò		
	eNP_Coward,				//≈≥»ı
	eNP_Sleepy,				// »ÀØ
	eNP_Greet,				//œ‡æ€
};

struct CNpcNatureData
	: public CNpcAIMallocObject 
{
	string	szNatureArg;
	uint32	uNatureExtraArg;
	string  szNatureAction;
	uint16	uColdTime;
	string	szAction;
	string	szActionArg;
	uint16	uLevel;
};

class CNpcNature;
class CNpcAI;
class NatureMgrTick;

typedef vector<string, TNpcAIAllocator<string> > VecStringNature;
typedef map<string, ENpcPersonality, less<string>, TNpcAIAllocator<pair<string, ENpcPersonality> > > MapString2Nature;
typedef map<ENpcEvent, VecStringNature , less<ENpcEvent>, TNpcAIAllocator<pair<ENpcEvent, VecStringNature> > > MapEvent2String;
typedef map<string, CNpcNatureData, less<string>, TNpcAIAllocator<pair<string, CNpcNatureData> > > MapString2Data;

class CNpcNatureMgr 
	: public INpcEventHandler
{
private:
	static MapString2Nature map_s2Nature;
	static MapEvent2String map_EventToNature;
private:
	CNpcAI* m_pAI;
	CNpcNature* m_pNature;
	CNpcNatureData m_sNatureData;
	bool m_bDisableNature;				//¿‰»¥ ±º‰
	bool m_bColdTimeEnd;
	MapString2Data m_mapNpcNatureData;
	NatureMgrTick* m_pNatureMgrTick;
		
private:
	TPtRefee<CNpcNatureMgr, CNpcAI> m_RefsByNpcAI;

public:
	CNpcNatureMgr(CNpcAI* pAI);
	virtual ~CNpcNatureMgr();

public:
	void NatureColdBegin();
	void NatureColdEnd();
	void DisableNature();
	void EnableNature();
	inline bool IsNatureActive() { return (m_bDisableNature && m_bColdTimeEnd); }
	void SetNature(CNpcNature* pNature);
	CNpcEventMsg* Dispatch(CNpcEventMsg* pEvent);
	static bool Interest(CNpcNature* pNature, uint32 uEventID);
	virtual void Handle(CNpcEventMsg* pEvent);
	virtual CNpcNature* GetNature();
	void SetNatureData(const string& szNatureName, const string& szNatureArg, uint32 uNatureExtraArg, const string& szNatureAction, 
					uint16 uColdTime, const string& szAction, const string& szActionArg, uint16 uLevel);
	string EventListenedNature(ENpcEvent e);
	bool IsEventListend(ENpcEvent e);
	bool CNpcSatisfied();
	void AfterSatisfied();
	CNpcAI* GetNpcAI() const {return m_pAI;}
	TPtRefee<CNpcNatureMgr, CNpcAI>& GetRefByNpcAI() { return m_RefsByNpcAI;}
	CNpcNature* CreateNpcNature(const TCHAR* sNatureName, const TCHAR* sNatureArg);
public:
	static void InitNatureInterest();
};

class NatureMgrTick
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	NatureMgrTick(CNpcNatureMgr* pNatureMgr):m_pNatureMgr(pNatureMgr)
	{
	};
	void OnTick()
	{
		m_pNatureMgr->NatureColdEnd();
		m_pNatureMgr->GetNpcAI()->GetCharacter()->UnRegistDistortedTick(this);
	}

private:
	CNpcNatureMgr* m_pNatureMgr;
};

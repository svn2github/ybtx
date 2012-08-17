#pragma once
#include "GameDef.h"
#include "CTalent.h"
#include "CTalentEffCfgServer.h"

class CFighterMediator;
class CSkillInstServer;
class CCfgCalc;
class CEffDataByGlobalIDMgr;

struct TalentEffData : public CFighterMallocObject
{
	TalentEffData(CSkillInstServer* pInst);
	~TalentEffData();
	CSkillInstServer* m_pInst;
	CEffDataByGlobalIDMgr*	m_pCancelEffData;
};

class CTalentHolderServer : public CTalentHolder
{
public:
	typedef	multiset<uint32,greater<uint32>, TFighterAllocator<uint32> >  MultisetTalent;
	typedef map<string,MultisetTalent,less<string>, TFighterAllocator<pair<string,MultisetTalent > > >  MapLessRuleTalent; 
	typedef	set<string,less<string>, TFighterAllocator<string> > SetSkillTalent;
	void	InsertTalent(const string& szName, uint32 uPoint);
	void	RemoveTalent(const string& szName);
	void	InsertEquipTalent(const string& szName, uint32 uPoint);
	bool	RemoveEquipTalent(const string& szName,uint32 uPoint);
	void	ClearAllSkillTalent();
private:
	void InsertDecentralTalent(const string& szName, uint32 uPoint);
	void InsertUniqueTalent(const string& szName, uint32 uPoint);
	bool RemoveDecentralTalent(const string& szName, uint32 uPoint);
	bool RemoveUniqueTalent(const string& szName, uint32 uPoint);
	MapLessRuleTalent m_mapRuleTalent;
	SetSkillTalent m_setSkillTalent;
};

class CTalentEffServer : public ITalentHandler
{
public:
	typedef pair<string,uint32> PairTalent;
	typedef	multimap< PairTalent,TalentEffData*, less<PairTalent>, TFighterAllocator<pair<PairTalent,TalentEffData* > > >  MapTalentEffData;
	typedef	set<PairTalent, less<PairTalent>, TFighterAllocator<PairTalent> >  SetUpdateTalentEff;
	CTalentEffServer(const CFighterMediator* pFighter);
	~CTalentEffServer();
	void InitClassTalentEff(EClass eClass);
	void Update();
private:
	void OnTalentChanged(const string& szName, int32 iPoint);
	void OnTalentAdded(const string& szName, uint32 uPoint);
	void OnTalentRemove(const string& szName,uint32 uPoint);
	MapTalentEffData m_mapTalentaEffData;
	SetUpdateTalentEff m_setUpdate;
	CFighterMediator*					m_pFighter;
};

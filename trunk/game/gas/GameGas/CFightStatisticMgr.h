#pragma once
#include "CFighterMallocObject.h"
#include "FightDef.h"
#include "TFighterAllocator.h"

struct FightStatisticData
{
	uint32 m_uFightHurt;
	uint32 m_uFightHeal;
	uint32 m_uBeSubHurt;
	uint32 m_uDeadTimes;
	bool m_bStatistic;
	uint64 m_uBattleBeginTime;
	uint32 m_uBattleTime;
};

class CFightStatisticMgr
	: public CFighterMallocObject
{
	typedef map<string, FightStatisticData, less<string>, TFighterAllocator<pair<string, FightStatisticData> > > MapFightStatisticData;
public:
	CFightStatisticMgr();
	~CFightStatisticMgr();

	uint32 GetStatisticFightHurt(const TCHAR* sName);
	uint32 GetStatisticFightHeal(const TCHAR* sName);
	uint32 GetStatisticBeSubHurt(const TCHAR* sName);
	uint32 GetStatisticFightTime(FightStatisticData& data);
	float GetStatisticDps(const TCHAR* sName);
	float GetStatisticHps(const TCHAR* sName);
	uint32 GetStatisticDeadTimes(const TCHAR* sName);
	bool BeBeginStatistic(FightStatisticData& data);
	void ResetData(FightStatisticData& data);
	void BeginStatisticData(FightStatisticData& data);
	void EndStatisticData(FightStatisticData& data);
	void BeginStatistic(const TCHAR* sName);
	void EndStatistic(const TCHAR* sName);
	void ClearAllStatisticData();
	
	void StatisticOnBattleStateChanged(bool bEnterBattle, FightStatisticData& data);
	void OnBattleStateChanged(bool bEnterBattle);
	void StatisticHurt(EFightInfoIndex eFightInfoIndex, uint32 uHurt, FightStatisticData& data);
	void StatisticSubHp(EFightInfoIndex eFightInfoIndex, uint32 uBeSubHp, FightStatisticData& data);
	void OnFightHurt(EFightInfoIndex eFightInfoIndex, uint32 uHurt);
	void OnBeSubHp(EFightInfoIndex eFightInfoIndex, uint32 uBeSubHp);
	void OnDead();
	static bool IsStatisticFightInfoIndex(EFightInfoIndex eFightInfoIndex);

private:
	FightStatisticData m_FightStatisticData;
	MapFightStatisticData m_mapFightStatisticData;
};


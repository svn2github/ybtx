#pragma once
#include "TEntityManager.h"
#include "TSingleton.h"
#include "TNpcAIDataAllocator.h"

class CNpcAIBaseData
	: public CNpcAIDataMallocObject
{
	friend class CNpcAIBaseDataMgr;
	typedef map<uint32, uint32, less<uint32>, TNpcAIDataAllocator<pair<uint32, uint32> > > MapAlertTime;
public:
	CNpcAIBaseData(uint32 uID, const string& sName);
	inline uint32 GetID() const { return m_uID; }
	inline const string& GetName() const { return m_sName; }
	inline uint32 GetLockEnemyDis() const {return m_uLockEnemyDis;}
	uint32 GetAlertTime(uint32 uTargetNum) const;

	//----------------- DATA MEMBERS ---------------------
	uint32 m_uMoveRange;
	float m_uWalkSpeed;
	float m_uRunSpeed;
	float m_uEyeSize;
	uint32 m_fExistTime;
	uint32 m_uChaseDistance;
	uint32 m_uLockEnemyDis;
	bool	m_bDelInBattle;
	uint32	m_uEnmityDist;
	uint32	m_uMinRandMoveTime;
	uint32	m_uMaxRandMoveTime;
	uint32	m_uEscapeTime;
	uint32	m_uEscapeDistace;
	uint32	m_uAlertDist;
	uint32	m_uPatrolPointStayTime;
	MapAlertTime m_mapAlert;
private:
	void CreateAlertMap(string strAlert);
	uint32 m_uID;
	string m_sName;
};

class CNpcAIBaseDataMgr 
	: public TEntityManager< const CNpcAIBaseData, TIDIndex< const CNpcAIBaseData >, TNameIndex< const CNpcAIBaseData > >
	, public TSingleton< CNpcAIBaseDataMgr >
	, public CNpcAIDataMallocObject
{
	friend class TSingleton< CNpcAIBaseDataMgr >;
private:
	CNpcAIBaseDataMgr();
	~CNpcAIBaseDataMgr();
public:
	bool LoadConfig( const string& sFileName );
};

#pragma once

#include "TEntityManager.h"
#include "TSingleton.h"
#include "CNpcAIDataMallocObject.h"
#include "NpcDefs.h"

class CNpcEnmityTargetFilterData
	: public CNpcAIDataMallocObject
{
	friend class CNpcEnmityTargetFilterDataMgr;
typedef set<string>		SetNpcArg;
public:
	CNpcEnmityTargetFilterData(uint32 uID, const string& sName);
	inline uint32 GetID() const { return m_uID; }
	inline const string& GetName() const { return m_sName; }
	ENpcTargetType GetTargetType() const {return m_eTargetType;}
	bool IsInTargetList(const string& strNpcName) const ;
private:
	void  InsertNpcArgs(string& strNpcArgs);
private:
	uint32		m_uID;
	string		m_sName;
	ENpcTargetType	m_eTargetType;
	SetNpcArg		m_setNpcName;
};

class CNpcEnmityTargetFilterDataMgr
	: public TEntityManager< const CNpcEnmityTargetFilterData, TIDIndex< const CNpcEnmityTargetFilterData >, TNameIndex< const CNpcEnmityTargetFilterData > >
	, public TSingleton< CNpcEnmityTargetFilterDataMgr >
	, public CNpcAIDataMallocObject
{
	friend class TSingleton< CNpcEnmityTargetFilterDataMgr >;
	typedef map<string, ENpcTargetType> MapName2Num;
private:
	CNpcEnmityTargetFilterDataMgr();
	~CNpcEnmityTargetFilterDataMgr();
public:
	bool LoadConfig( const string& sFileName );
private:
	MapName2Num m_mapName2Num;
};

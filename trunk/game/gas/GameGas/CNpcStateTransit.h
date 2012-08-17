#pragma once

#include "CNpcStateMachine.h"
#include "TEntityManager.h"
#include "TSingleton.h"
#include "CNpcAIDataMallocObject.h"

//Npc状态跳转表
class CNpcStateTransit 
	: public NpcStateTransitTable
	, public CNpcAIDataMallocObject
{
public:
	CNpcStateTransit(uint32 uID, const string& strName);
	~CNpcStateTransit();
	inline uint32 GetID() const
	{
		return m_uID;
	}
	inline void SetID(uint32 uID)
	{
		m_uID = uID;
	}
	inline const string& GetName() const
	{
		return m_strName;
	}

private:
	uint32 m_uID;
	string m_strName;
};

class CNpcSubMachineCreator;

//Npc状态跳转表管理器
class CNpcStateTransitMgr
	: public TEntityManager<const CNpcStateTransit, TIDIndex<const CNpcStateTransit>, TNameIndex<const CNpcStateTransit> >
	, public TSingleton<CNpcStateTransitMgr>
	, public CNpcAIDataMallocObject
{
	friend class TSingleton<CNpcStateTransitMgr>;

public:
	bool LoadConfig(const string& sFileName);
	void UnloadConfig();

private:
	CNpcStateTransitMgr() { }
	~CNpcStateTransitMgr()
	{
		UnloadConfig();
	}
};

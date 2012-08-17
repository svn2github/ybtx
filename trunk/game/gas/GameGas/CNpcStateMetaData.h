#pragma once

#include "TEntityManager.h"
#include "TSingleton.h"
#include "CNpcState.h"
#include "CNpcAIMallocObject.h"

class CNpcStateBase;
class CNpcAI;
/*
* =====================================================================================
*        Class:  CNpcStateMetaData
*  Description:  NpcState元数据
* =====================================================================================
*/
class CNpcStateMetaData
	: public CNpcAIMallocObject
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	CNpcStateMetaData (uint32 uID, const string& sName, CNpcStateCreatorBase* pCreator)			/* constructor */
		: m_uID(uID)
		, m_sName(sName)
		, m_uCreator(pCreator)
	{
	}

	~CNpcStateMetaData()
	{
		delete m_uCreator;
	}

	/* ====================  ACCESS        ======================================= */
	inline uint32 GetID() const						{ return m_uID; }
	inline const string& GetName() const			{ return m_sName; }
	inline CNpcStateCreatorBase* GetCreator() const { return m_uCreator; }

private:
	uint32					m_uID;
	string					m_sName;
	CNpcStateCreatorBase*	m_uCreator;

}; /* -----  end of class  CNpcStateMetaData  ----- */

/*
* =====================================================================================
*        Class:  CNpcStateMetaDataMgr
*  Description:  StateMetaData管理器
* =====================================================================================
*/
class CNpcStateMetaDataMgr : 
	public TEntityManager< CNpcStateMetaData, TIDIndex<CNpcStateMetaData>, TNameIndex<CNpcStateMetaData> >,
	public TSingleton<CNpcStateMetaDataMgr>,
	public CNpcAIMallocObject
{
	friend class TSingleton<CNpcStateMetaDataMgr>;

private:
	/* ====================  LIFECYCLE     ======================================= */
	CNpcStateMetaDataMgr()	{ }										/* constructor */
	~CNpcStateMetaDataMgr() { DeleteAll(); }						/* destructor */

}; /* -----  end of class  CNpcStateMetaDataMgr  ----- */

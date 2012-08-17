#pragma once
#include "TSingleton.h"
#include "TEntityManager.h"
#include "CNpcAIMallocObject.h"
/*
* =====================================================================================
*        Class:  CNpcEventMetaData
*  Description:  NpcEvent元数据
* =====================================================================================
*/
class CNpcEventMetaData
	: public CNpcAIMallocObject
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	CNpcEventMetaData (ENpcEvent eID, const TCHAR* szName)			/* constructor */
		: m_eID(eID)
		, m_szName(szName)
	{
	}

	/* ====================  OPERATIONS    ======================================= */
	inline ENpcEvent GetID() const			{ return m_eID; }
	inline const TCHAR* GetName() const		{ return m_szName; }

private:
	ENpcEvent		m_eID;
	const TCHAR*		m_szName;
}; /* -----  end of class  CNpcEventMetaData  ----- */

/*
* =====================================================================================
*        Class:  CNpcEventMetaDataMgr
*  Description:  EventMetaData管理器
* =====================================================================================
*/
class CNpcEventMetaDataMgr
	: public TEntityManager< CNpcEventMetaData, TEntityIndex<CNpcEventMetaData, ENpcEvent>, TNameIndex<CNpcEventMetaData> >
	, public TSingleton< CNpcEventMetaDataMgr >
	, public CNpcAIMallocObject
{
	friend class TSingleton<CNpcEventMetaDataMgr>;

private:
	/* ====================  LIFECYCLE     ======================================= */
	CNpcEventMetaDataMgr ()		{ }									/* constructor */
	~CNpcEventMetaDataMgr ()	{ DeleteAll(); }					/* destructor */

}; /* -----  end of class  CNpcEventMetaDataMgr  ----- */
